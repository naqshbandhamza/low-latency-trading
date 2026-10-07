#pragma once

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

#include "market_data/moldudp64/IMoldItchRecoverySource.h"
#include "market_data/moldudp64/SequencedItchMessage.h"

namespace llt::moldudp64
{

class FileMoldItchRecoverySource final
    : public IMoldItchRecoverySource
{
public:
    //
    // Store one BinaryFILE checkpoint every N
    // ITCH messages.
    //
    // With interval 1000:
    //
    // sequence 1
    // sequence 1001
    // sequence 2001
    // ...
    //
    static constexpr std::uint64_t
        DefaultCheckpointInterval = 1000;

    explicit FileMoldItchRecoverySource(
        std::string filePath,
        std::uint64_t checkpointInterval =
            DefaultCheckpointInterval);

    bool recover(
        std::uint64_t fromSequence,
        std::uint64_t toSequence,
        std::vector<SequencedItchMessage>& messages
    ) override;

    [[nodiscard]]
    std::uint64_t recoveryRequests() const noexcept
    {
        return recoveryRequests_;
    }

    [[nodiscard]]
    std::uint64_t recoveredMessages() const noexcept
    {
        return recoveredMessages_;
    }

    [[nodiscard]]
    bool indexReady() const noexcept
    {
        return indexReady_;
    }

    [[nodiscard]]
    bool indexLoadedFromDisk() const noexcept
    {
        return indexLoadedFromDisk_;
    }

    [[nodiscard]]
    std::size_t checkpointCount() const noexcept
    {
        return checkpoints_.size();
    }

    [[nodiscard]]
    std::uint64_t indexedMessages() const noexcept
    {
        return indexedMessages_;
    }

    [[nodiscard]]
    std::uint64_t checkpointInterval() const noexcept
    {
        return checkpointInterval_;
    }

    [[nodiscard]]
    const std::string& indexPath() const noexcept
    {
        return indexPath_;
    }

private:
    struct RecoveryCheckpoint
    {
        std::uint64_t sequence{0};
        std::uint64_t fileOffset{0};
    };

    bool initialize();
    bool buildIndex();
    bool loadIndex();
    bool saveIndex() const;
    bool openRecoveryFile();

    [[nodiscard]]
    const RecoveryCheckpoint*
    findCheckpoint(
        std::uint64_t sequence) const noexcept;

private:
    std::string filePath_;
    std::string indexPath_;

    std::uint64_t checkpointInterval_{
        DefaultCheckpointInterval};

    std::vector<RecoveryCheckpoint>
        checkpoints_{};

    std::uint64_t indexedMessages_{0};
    std::uint64_t sourceFileSize_{0};

    bool indexReady_{false};
    bool indexLoadedFromDisk_{false};

    //
    // Persistent BinaryFILE handle.
    //
    // Recovery requests reuse this stream instead of
    // reopening the historical file for every gap.
    //
    std::ifstream recoveryFile_;

    std::uint64_t recoveryRequests_{0};
    std::uint64_t recoveredMessages_{0};
};

} // namespace llt::moldudp64