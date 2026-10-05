#include "market_data/itch/FileItchRecoverySource.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <limits>
#include <system_error>
#include <utility>

#include "market_data/itch/ItchStreamReader.h"

namespace llt::itch
{
namespace
{

constexpr std::array<char, 8> IndexMagic{
    'L', 'L', 'T', 'I', 'T', 'C', 'H', '1'};

constexpr std::uint32_t IndexVersion = 1;

template <typename T>
bool writeValue(
    std::ostream& output,
    const T& value)
{
    output.write(
        reinterpret_cast<const char*>(&value),
        sizeof(T));

    return output.good();
}

template <typename T>
bool readValue(
    std::istream& input,
    T& value)
{
    input.read(
        reinterpret_cast<char*>(&value),
        sizeof(T));

    return
        input.gcount() ==
        static_cast<std::streamsize>(
            sizeof(T));
}

} // namespace


FileItchRecoverySource::FileItchRecoverySource(
    std::string filePath,
    std::uint64_t checkpointInterval)
    : filePath_(
          std::move(filePath)),
      indexPath_(
          filePath_ + ".idx"),
      checkpointInterval_(
          checkpointInterval)
{
    if (checkpointInterval_ == 0)
    {
        return;
    }

    indexReady_ =
        initialize();
}


bool FileItchRecoverySource::initialize()
{
    std::error_code error;

    const auto fileSize =
        std::filesystem::file_size(
            filePath_,
            error);

    if (error)
    {
        return false;
    }

    sourceFileSize_ =
        static_cast<std::uint64_t>(
            fileSize);

    //
    // Fast path: load an already-built sparse index.
    //
    if (loadIndex())
    {
        indexLoadedFromDisk_ = true;

        return openRecoveryFile();
    }

    //
    // Slow path: scan once, persist, then use it.
    //
    indexLoadedFromDisk_ = false;

    if (!buildIndex())
    {
        return false;
    }

    if (!saveIndex())
    {
        return false;
    }

    return openRecoveryFile();
}


bool FileItchRecoverySource::openRecoveryFile()
{
    if (recoveryFile_.is_open())
    {
        recoveryFile_.close();
    }

    recoveryFile_.clear();

    recoveryFile_.open(
        filePath_,
        std::ios::binary);

    return recoveryFile_.is_open();
}


bool FileItchRecoverySource::buildIndex()
{
    checkpoints_.clear();
    indexedRecords_ = 0;

    std::ifstream file(
        filePath_,
        std::ios::binary);

    if (!file.is_open())
    {
        return false;
    }

    ItchStreamReader reader{
        file};

    std::uint64_t sequence{1};

    while (true)
    {
        const auto position =
            file.tellg();

        if (
            position ==
            std::streampos{-1})
        {
            checkpoints_.clear();
            indexedRecords_ = 0;

            return false;
        }

        auto record =
            reader.readNext();

        switch (record.status)
        {
        case ItchStreamReadStatus::Message:
        {
            if (
                (sequence - 1) %
                    checkpointInterval_ ==
                0)
            {
                const auto rawOffset =
                    static_cast<std::streamoff>(
                        position);

                if (rawOffset < 0)
                {
                    checkpoints_.clear();
                    indexedRecords_ = 0;

                    return false;
                }

                checkpoints_.push_back(
                    RecoveryCheckpoint{
                        sequence,
                        static_cast<std::uint64_t>(
                            rawOffset)});
            }

            ++indexedRecords_;
            ++sequence;

            break;
        }

        case ItchStreamReadStatus::EndOfSession:
        case ItchStreamReadStatus::Incomplete:
        {
            return
                !checkpoints_.empty();
        }

        case ItchStreamReadStatus::Error:
        {
            checkpoints_.clear();
            indexedRecords_ = 0;

            return false;
        }
        }
    }
}


bool FileItchRecoverySource::saveIndex() const
{
    if (
        checkpoints_.empty() ||
        indexedRecords_ == 0 ||
        sourceFileSize_ == 0)
    {
        return false;
    }

    const std::string temporaryPath =
        indexPath_ + ".tmp";

    std::ofstream output(
        temporaryPath,
        std::ios::binary |
        std::ios::trunc);

    if (!output.is_open())
    {
        return false;
    }

    output.write(
        IndexMagic.data(),
        static_cast<std::streamsize>(
            IndexMagic.size()));

    const std::uint32_t version =
        IndexVersion;

    const std::uint64_t checkpointCount =
        static_cast<std::uint64_t>(
            checkpoints_.size());

    if (
        !output.good() ||
        !writeValue(output, version) ||
        !writeValue(
            output,
            checkpointInterval_) ||
        !writeValue(
            output,
            indexedRecords_) ||
        !writeValue(
            output,
            checkpointCount) ||
        !writeValue(
            output,
            sourceFileSize_))
    {
        output.close();

        std::error_code error;
        std::filesystem::remove(
            temporaryPath,
            error);

        return false;
    }

    for (
        const auto& checkpoint :
        checkpoints_)
    {
        if (
            !writeValue(
                output,
                checkpoint.sequence) ||
            !writeValue(
                output,
                checkpoint.fileOffset))
        {
            output.close();

            std::error_code error;
            std::filesystem::remove(
                temporaryPath,
                error);

            return false;
        }
    }

    output.flush();

    if (!output.good())
    {
        output.close();

        std::error_code error;
        std::filesystem::remove(
            temporaryPath,
            error);

        return false;
    }

    output.close();

    //
    // Atomic-ish replacement: write temp completely first,
    // then rename it into place.
    //
    std::error_code error;

    std::filesystem::remove(
        indexPath_,
        error);

    error.clear();

    std::filesystem::rename(
        temporaryPath,
        indexPath_,
        error);

    if (error)
    {
        std::error_code cleanupError;

        std::filesystem::remove(
            temporaryPath,
            cleanupError);

        return false;
    }

    return true;
}


bool FileItchRecoverySource::loadIndex()
{
    checkpoints_.clear();
    indexedRecords_ = 0;

    std::ifstream input(
        indexPath_,
        std::ios::binary);

    if (!input.is_open())
    {
        return false;
    }

    std::array<char, 8> magic{};

    input.read(
        magic.data(),
        static_cast<std::streamsize>(
            magic.size()));

    if (
        input.gcount() !=
            static_cast<std::streamsize>(
                magic.size()) ||
        magic != IndexMagic)
    {
        return false;
    }

    std::uint32_t version{0};
    std::uint64_t storedInterval{0};
    std::uint64_t storedRecordCount{0};
    std::uint64_t storedCheckpointCount{0};
    std::uint64_t storedSourceFileSize{0};

    if (
        !readValue(input, version) ||
        !readValue(
            input,
            storedInterval) ||
        !readValue(
            input,
            storedRecordCount) ||
        !readValue(
            input,
            storedCheckpointCount) ||
        !readValue(
            input,
            storedSourceFileSize))
    {
        return false;
    }

    if (
        version != IndexVersion ||
        storedInterval !=
            checkpointInterval_ ||
        storedInterval == 0 ||
        storedRecordCount == 0 ||
        storedCheckpointCount == 0 ||
        storedSourceFileSize !=
            sourceFileSize_)
    {
        return false;
    }

    const auto expectedCheckpointCount =
        ((storedRecordCount - 1) /
            storedInterval) +
        1;

    if (
        storedCheckpointCount !=
            expectedCheckpointCount ||
        storedCheckpointCount >
            static_cast<std::uint64_t>(
                std::numeric_limits<
                    std::size_t>::max()))
    {
        return false;
    }

    checkpoints_.reserve(
        static_cast<std::size_t>(
            storedCheckpointCount));

    std::uint64_t previousSequence{0};
    std::uint64_t previousOffset{0};

    for (
        std::uint64_t i = 0;
        i < storedCheckpointCount;
        ++i)
    {
        RecoveryCheckpoint checkpoint{};

        if (
            !readValue(
                input,
                checkpoint.sequence) ||
            !readValue(
                input,
                checkpoint.fileOffset))
        {
            checkpoints_.clear();

            return false;
        }

        const auto expectedSequence =
            1 +
            (i * storedInterval);

        if (
            checkpoint.sequence !=
                expectedSequence ||
            checkpoint.sequence >
                storedRecordCount)
        {
            checkpoints_.clear();

            return false;
        }

        if (
            i == 0 &&
            checkpoint.fileOffset != 0)
        {
            checkpoints_.clear();

            return false;
        }

        if (
            i > 0 &&
            (checkpoint.sequence <=
                 previousSequence ||
             checkpoint.fileOffset <=
                 previousOffset))
        {
            checkpoints_.clear();

            return false;
        }

        previousSequence =
            checkpoint.sequence;

        previousOffset =
            checkpoint.fileOffset;

        checkpoints_.push_back(
            checkpoint);
    }

    //
    // No unexpected trailing bytes.
    //
    char trailingByte{0};

    input.read(
        &trailingByte,
        1);

    if (input.gcount() != 0)
    {
        checkpoints_.clear();

        return false;
    }

    indexedRecords_ =
        storedRecordCount;

    return true;
}


const FileItchRecoverySource::RecoveryCheckpoint*
FileItchRecoverySource::findCheckpoint(
    std::uint64_t sequence) const noexcept
{
    if (
        checkpoints_.empty() ||
        sequence == 0)
    {
        return nullptr;
    }

    const auto it =
        std::upper_bound(
            checkpoints_.begin(),
            checkpoints_.end(),
            sequence,
            [](
                std::uint64_t value,
                const RecoveryCheckpoint& checkpoint)
            {
                return
                    value <
                    checkpoint.sequence;
            });

    if (it == checkpoints_.begin())
    {
        return nullptr;
    }

    return &*(it - 1);
}


bool FileItchRecoverySource::recover(
    std::uint64_t fromSequence,
    std::uint64_t toSequence,
    std::vector<ItchUdpPacket>& packets)
{
    ++recoveryRequests_;

    packets.clear();

    if (
        fromSequence >= toSequence ||
        fromSequence == 0 ||
        !indexReady_)
    {
        return false;
    }

    if (
        (toSequence - 1) >
        indexedRecords_)
    {
        return false;
    }

    const auto* checkpoint =
        findCheckpoint(
            fromSequence);

    if (checkpoint == nullptr)
    {
        return false;
    }

    if (
        checkpoint->fileOffset >
        static_cast<std::uint64_t>(
            std::numeric_limits<
                std::streamoff>::max()))
    {
        return false;
    }

    //
    // Persistent handle: reset stream state and reposition.
    //
    recoveryFile_.clear();

    recoveryFile_.seekg(
        static_cast<std::streamoff>(
            checkpoint->fileOffset),
        std::ios::beg);

    if (!recoveryFile_.good())
    {
        return false;
    }

    ItchStreamReader reader{
        recoveryFile_};

    std::uint64_t currentSequence =
        checkpoint->sequence;

    //
    // Scan from sparse checkpoint to requested sequence.
    //
    while (
        currentSequence <
        fromSequence)
    {
        auto record =
            reader.readNext();

        if (
            record.status !=
            ItchStreamReadStatus::Message)
        {
            return false;
        }

        ++currentSequence;
    }

    const auto expectedCount =
        toSequence -
        fromSequence;

    packets.reserve(
        static_cast<std::size_t>(
            expectedCount));

    while (
        currentSequence <
        toSequence)
    {
        auto record =
            reader.readNext();

        if (
            record.status !=
                ItchStreamReadStatus::Message ||
            record.empty())
        {
            packets.clear();

            return false;
        }

        ItchUdpPacket packet{};

        packet.sequence =
            currentSequence;

        if (
            record.size >
            packet.payload.size())
        {
            packets.clear();

            return false;
        }

        packet.payloadSize =
            record.size;

        std::memcpy(
            packet.payload.data(),
            record.data,
            record.size);

        packets.push_back(
            std::move(packet));

        ++currentSequence;
    }

    if (
        packets.size() !=
        static_cast<std::size_t>(
            expectedCount))
    {
        packets.clear();

        return false;
    }

    for (
        std::size_t i = 0;
        i < packets.size();
        ++i)
    {
        if (
            packets[i].sequence !=
            fromSequence +
                static_cast<std::uint64_t>(
                    i))
        {
            packets.clear();

            return false;
        }
    }

    recoveredPackets_ +=
        static_cast<std::uint64_t>(
            packets.size());

    return true;
}

} // namespace llt::itch
