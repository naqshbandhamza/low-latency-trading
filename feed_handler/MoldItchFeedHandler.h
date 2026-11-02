#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>
#include <thread>

#include "FeedHandlerState.h"
#include "types/SequenceCheckResult.h"
#include "market_data/moldudp64/MoldDatagramQueue.h"
#include "market_data/itch/ItchMarketState.h"
#include "market_data/itch/ItchDispatcher.h"
#include "market_data/moldudp64/IMoldItchSequenceRecovery.h"
#include "market_data/moldudp64/IMoldMarketDataSource.h"
#include "market_data/moldudp64/MoldUdp64.h"
#include "market_data/moldudp64/MoldUdp64Codec.h"
#include "market_data/moldudp64/SequencedItchMessage.h"
#include "market_data/moldudp64/MoldUdpReceiver.h"

#ifdef LLT_ENABLE_LATENCY_TRACE
#include "market_data/utility/LatencyRecorder.h"
#include "market_data/utility/LatencyClock.h"
#endif

namespace llt::itch
{
    template <typename BboHandler = NullBboHandler, 
              typename TradeHandler = NullTradeHandler>
    class MoldItchFeedHandler
    {
    public:
#ifdef LLT_ENABLE_LATENCY_TRACE

        struct ProcessingLatencyReport
        {
            latency::LatencyRecorder<>::Summary
                datagramQueue;

            latency::LatencyRecorder<>::Summary
                liveMessageProcessing;

            latency::LatencyRecorder<>::Summary
                recoveredMessageProcessing;

            llt::latency::LatencyRecorder<>::Summary
                datagramProcessing{};

            llt::latency::LatencyRecorder<>::Summary
                consumerPopInterval{};
        };

        [[nodiscard]]
        ProcessingLatencyReport latencyReport()
        {
            return {
                datagramQueueLatency_.summarize(),
                messageProcessingLatency_.summarize(),
                recoveredMessageProcessingLatency_.summarize(),
                datagramProcessingLatency_.summarize(),
                consumerPopIntervalLatency_.summarize(),
            };
        }

#endif

        MoldItchFeedHandler(
            llt::moldudp64::IMoldMarketDataSource &source,
            llt::moldudp64::IMoldItchSequenceRecovery &recovery,
            ItchMarketState<BboHandler, TradeHandler> &marketState) noexcept
            : source_(source),
              recovery_(recovery),
              marketState_(marketState)
        {
        }

        //
        // Deterministic mode.
        //
        void start(std::size_t datagramCount)
        {
            startLifecycle();

            std::size_t acceptedLiveDatagrams{0};
            llt::moldudp64::ReceivedMoldDatagram datagram{};

            while (
                running_.load(std::memory_order_acquire) &&
                acceptedLiveDatagrams < datagramCount)
            {
                if (!source_.receive(datagram))
                {
                    continue;
                }

                if (!processDatagram(datagram))
                {
                    ++malformedDatagrams_;
                    state_.store(llt::FeedHandlerState::Failed, std::memory_order_release);
                    break;
                }

                ++processedDatagrams_;
                ++acceptedLiveDatagrams;
            }

            finishLifecycle();
        }

        //
        // Continuous live mode.
        //
        void run()
        {
            startLifecycle();
            llt::moldudp64::ReceivedMoldDatagram datagram{};

            while (running_.load(std::memory_order_acquire))
            {
                if (!source_.receive(datagram))
                {
                    continue;
                }

                if (!processDatagram(datagram))
                {
                    ++malformedDatagrams_;
                    state_.store(llt::FeedHandlerState::Failed, std::memory_order_release);
                    break;
                }

                ++processedDatagrams_;
            }

            finishLifecycle();
        }

        // Live mode using a dedicated UDP receiver.
        void runQueued(
            llt::moldudp64::MoldDatagramQueue &queue,
            const llt::moldudp64::MoldUdpReceiver &receiver)
        {
            startLifecycle();

#ifdef LLT_ENABLE_LATENCY_TRACE
            std::uint64_t previousPopNs = 0;
#endif

            while (true)
            {
                bool processed = true;

                const bool consumed = queue.try_consume(
                    [&](const llt::moldudp64::ReceivedMoldDatagram& datagram)
                    {
#ifdef LLT_ENABLE_LATENCY_TRACE
                        const auto dequeuedAtNs = llt::latency::nowNs();

                        if (datagram.queuedAtNs != 0 && dequeuedAtNs >= datagram.queuedAtNs)
                        {
                            datagramQueueLatency_.record(dequeuedAtNs - datagram.queuedAtNs);
                        }

                        if (previousPopNs != 0 && dequeuedAtNs >= previousPopNs)
                        {
                            consumerPopIntervalLatency_.record(dequeuedAtNs - previousPopNs);
                        }

                        previousPopNs = dequeuedAtNs;

                        const auto processingStartNs = llt::latency::nowNs();
#endif

                        processed = processDatagram(datagram);

#ifdef LLT_ENABLE_LATENCY_TRACE
                        const auto processingEndNs = llt::latency::nowNs();
                        if (processingEndNs >= processingStartNs)
                        {
                            datagramProcessingLatency_.record(processingEndNs - processingStartNs);
                        }
#endif
                    });

                if (consumed)
                {
                    if (!processed)
                    {
                        ++malformedDatagrams_;
                        state_.store(llt::FeedHandlerState::Failed, std::memory_order_release);
                        break;
                    }

                    ++processedDatagrams_;
                    continue;
                }

                if (!receiver.running())
                {
                    if (queue.empty())
                    {
                        break;
                    }
                    continue;
                }

                if (!running_.load(std::memory_order_acquire))
                {
                    break;
                }

                std::this_thread::yield();
            }

            finishLifecycle();
        }

        void stop() noexcept
        {
            running_.store(false, std::memory_order_release);
        }

        [[nodiscard]]
        llt::FeedHandlerState state() const noexcept
        {
            return state_.load(std::memory_order_acquire);
        }

        [[nodiscard]] std::uint64_t processedDatagrams() const noexcept { return processedDatagrams_; }
        [[nodiscard]] std::uint64_t processedMessages() const noexcept { return processedMessages_; }
        [[nodiscard]] std::uint64_t recoveredMessages() const noexcept { return recoveredMessages_; }
        [[nodiscard]] std::uint64_t ignoredMessages() const noexcept { return ignoredMessages_; }
        [[nodiscard]] std::uint64_t gapsDetected() const noexcept { return gapsDetected_; }
        [[nodiscard]] std::uint64_t malformedDatagrams() const noexcept { return malformedDatagrams_; }
        [[nodiscard]] std::uint64_t expectedSequence() const noexcept { return expectedSequence_; }

    private:
#ifdef LLT_ENABLE_LATENCY_TRACE
        latency::LatencyRecorder<> datagramQueueLatency_;
        latency::LatencyRecorder<> messageProcessingLatency_;
        latency::LatencyRecorder<> recoveredMessageProcessingLatency_;
        llt::latency::LatencyRecorder<> datagramProcessingLatency_;
        llt::latency::LatencyRecorder<> consumerPopIntervalLatency_;
#endif

        [[nodiscard]]
        llt::SequenceCheckResult checkSequence(std::uint64_t sequence)
        {
            if (!hasSequence_)
            {
                expectedSequence_ = sequence + 1;
                hasSequence_ = true;
                return llt::SequenceCheckResult::Process;
            }

            if (sequence == expectedSequence_)
            {
                expectedSequence_ = sequence + 1;
                return llt::SequenceCheckResult::Process;
            }

            if (sequence < expectedSequence_)
            {
                ++ignoredMessages_;
                return llt::SequenceCheckResult::Ignore;
            }

            ++gapsDetected_;

            std::vector<llt::moldudp64::SequencedItchMessage> recoveredMessages;
            const auto recoveryResult = recovery_.recover(
                expectedSequence_,
                sequence,
                recoveredMessages);

            if (recoveryResult != llt::SequenceCheckResult::Process)
            {
                return recoveryResult;
            }

            for (const auto& recoveredMessage : recoveredMessages)
            {
                if (!processRecoveredMessage(recoveredMessage))
                {
                    return llt::SequenceCheckResult::Stop;
                }
            }

            expectedSequence_ = sequence + 1;
            return llt::SequenceCheckResult::Process;
        }

        [[nodiscard]]
        bool processDatagram(const llt::moldudp64::ReceivedMoldDatagram &datagram)
        {
            llt::moldudp64::Header header{};

            if (!llt::moldudp64::MoldUdp64Codec::decodeHeader(
                    datagram.bytes.data(),
                    datagram.size,
                    header))
            {
                return false;
            }

            if (header.messageCount == 0)
            {
                return false;
            }

            std::size_t validationOffset = llt::moldudp64::HeaderSize;
            std::uint64_t validationSequence = header.sequenceNumber;

            for (std::uint16_t i = 0; i < header.messageCount; ++i)
            {
                llt::moldudp64::MessageView validationMessage{};
                if (!llt::moldudp64::MoldUdp64Codec::nextMessage(
                        datagram.bytes.data(),
                        datagram.size,
                        validationOffset,
                        validationSequence,
                        validationMessage))
                {
                    return false;
                }
                ++validationSequence;
            }

            if (validationOffset != datagram.size)
            {
                return false;
            }

            std::size_t offset = llt::moldudp64::HeaderSize;
            std::uint64_t sequence = header.sequenceNumber;

            for (std::uint16_t i = 0; i < header.messageCount; ++i)
            {
                llt::moldudp64::MessageView message{};
                if (!llt::moldudp64::MoldUdp64Codec::nextMessage(
                        datagram.bytes.data(),
                        datagram.size,
                        offset,
                        sequence,
                        message))
                {
                    return false;
                }

                const auto sequenceResult = checkSequence(message.sequenceNumber);

                if (sequenceResult == llt::SequenceCheckResult::Stop)
                {
                    return false;
                }

                if (sequenceResult == llt::SequenceCheckResult::Ignore)
                {
                    ++sequence;
                    continue;
                }

                if (!processMessage(message))
                {
                    return false;
                }

                ++sequence;
            }

            return true;
        }

        [[nodiscard]]
        bool processMessage(const llt::moldudp64::MessageView &message)
        {
            if (message.data == nullptr || message.size == 0)
            {
                return false;
            }

#ifdef LLT_ENABLE_LATENCY_TRACE
            const auto processingStartedNs = llt::latency::nowNs();
#endif

            const auto decoded = ItchDispatcher::dispatch(message.data, message.size);

            if (!decoded.has_value())
            {
                return false;
            }

            marketState_.onMessage(*decoded);

#ifdef LLT_ENABLE_LATENCY_TRACE
            const auto processingCompletedNs = llt::latency::nowNs();
            messageProcessingLatency_.record(processingCompletedNs - processingStartedNs);
#endif

            ++processedMessages_;
            return true;
        }

        [[nodiscard]]
        bool processRecoveredMessage(const llt::moldudp64::SequencedItchMessage &message)
        {
            if (message.payloadSize == 0 ||
                message.payloadSize > llt::moldudp64::SequencedItchMessage::MaxPayloadSize)
            {
                return false;
            }

#ifdef LLT_ENABLE_LATENCY_TRACE
            const auto processingStartedNs = llt::latency::nowNs();
#endif

            const auto decoded = ItchDispatcher::dispatch(
                message.payload.data(),
                static_cast<std::size_t>(message.payloadSize));

            if (!decoded.has_value())
            {
                return false;
            }

            marketState_.onMessage(*decoded);

#ifdef LLT_ENABLE_LATENCY_TRACE
            const auto processingCompletedNs = llt::latency::nowNs();
            recoveredMessageProcessingLatency_.record(processingCompletedNs - processingStartedNs);
#endif

            ++recoveredMessages_;
            return true;
        }

        void startLifecycle() noexcept
        {
            expectedSequence_ = 0;
            hasSequence_ = false;
            processedDatagrams_ = 0;
            processedMessages_ = 0;
            recoveredMessages_ = 0;
            ignoredMessages_ = 0;
            gapsDetected_ = 0;
            malformedDatagrams_ = 0;

            state_.store(llt::FeedHandlerState::Running, std::memory_order_release);
            running_.store(true, std::memory_order_release);
        }

        void finishLifecycle() noexcept
        {
            running_.store(false, std::memory_order_release);

            if (state_.load(std::memory_order_acquire) != llt::FeedHandlerState::Failed)
            {
                state_.store(llt::FeedHandlerState::Stopped, std::memory_order_release);
            }
        }

    private:
        llt::moldudp64::IMoldMarketDataSource &source_;
        llt::moldudp64::IMoldItchSequenceRecovery &recovery_;
        ItchMarketState<BboHandler, TradeHandler> &marketState_;

        std::uint64_t expectedSequence_{0};
        bool hasSequence_{false};

        std::uint64_t processedDatagrams_{0};
        std::uint64_t processedMessages_{0};
        std::uint64_t recoveredMessages_{0};
        std::uint64_t ignoredMessages_{0};
        std::uint64_t gapsDetected_{0};
        std::uint64_t malformedDatagrams_{0};

        std::atomic<bool> running_{false};
        std::atomic<llt::FeedHandlerState> state_{llt::FeedHandlerState::Stopped};
    };

} // namespace llt::itch