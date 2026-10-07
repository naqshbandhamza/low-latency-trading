#include "market_data/moldudp64/FileMoldItchRecoverySource.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <limits>
#include <system_error>
#include <utility>

#include "market_data/itch/ItchStreamReader.h"

namespace llt::moldudp64
{

namespace
{

//
// Keep this distinct from the old custom-UDP recovery
// index format.
//
// Both indexes contain similar checkpoint information,
// but they belong to different recovery implementations.
//
constexpr std::array<char, 8> IndexMagic{
    'L', 'L', 'T', 'M', 'O', 'L', 'D', '1'};

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


FileMoldItchRecoverySource::
FileMoldItchRecoverySource(
    std::string filePath,
    std::uint64_t checkpointInterval)
    : filePath_(
          std::move(filePath)),
      indexPath_(
          filePath_ + ".mold.idx"),
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


bool
FileMoldItchRecoverySource::initialize()
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
    // Fast path:
    //
    // Reuse an existing validated persistent index.
    //
    if (loadIndex())
    {
        indexLoadedFromDisk_ = true;

        return openRecoveryFile();
    }


    //
    // Slow path:
    //
    // No usable index exists.
    //
    // Scan the BinaryFILE once, construct the sparse
    // checkpoints, persist them, and then open the
    // recovery stream.
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


bool
FileMoldItchRecoverySource::openRecoveryFile()
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


bool
FileMoldItchRecoverySource::buildIndex()
{
    checkpoints_.clear();
    indexedMessages_ = 0;

    std::ifstream file(
        filePath_,
        std::ios::binary);

    if (!file.is_open())
    {
        return false;
    }

    llt::itch::ItchStreamReader reader{
        file};

    std::uint64_t sequence{1};

    while (true)
    {
        //
        // Capture the position of the BinaryFILE length
        // prefix before reading the record.
        //
        const auto position =
            file.tellg();

        if (
            position ==
            std::streampos{-1})
        {
            checkpoints_.clear();
            indexedMessages_ = 0;

            return false;
        }

        auto record =
            reader.readNext();

        switch (record.status)
        {
        case llt::itch::ItchStreamReadStatus::Message:
        {
            //
            // Sequence 1 is always a checkpoint.
            //
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
                    indexedMessages_ = 0;

                    return false;
                }

                checkpoints_.push_back(
                    RecoveryCheckpoint{
                        sequence,
                        static_cast<std::uint64_t>(
                            rawOffset)});
            }

            ++indexedMessages_;
            ++sequence;

            break;
        }

        case llt::itch::ItchStreamReadStatus::EndOfSession:
        case llt::itch::ItchStreamReadStatus::Incomplete:
        {
            //
            // Preserve the existing BinaryFILE semantics:
            // physical EOF is currently represented by
            // Incomplete.
            //
            return
                !checkpoints_.empty();
        }

        case llt::itch::ItchStreamReadStatus::Error:
        {
            checkpoints_.clear();
            indexedMessages_ = 0;

            return false;
        }
        }
    }
}


bool
FileMoldItchRecoverySource::saveIndex() const
{
    if (
        checkpoints_.empty() ||
        indexedMessages_ == 0 ||
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


    //
    // Header
    //
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
        !writeValue(
            output,
            version) ||
        !writeValue(
            output,
            checkpointInterval_) ||
        !writeValue(
            output,
            indexedMessages_) ||
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


    //
    // Sparse checkpoints
    //
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
    // Write the new index completely before replacing
    // the existing one.
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


bool
FileMoldItchRecoverySource::loadIndex()
{
    checkpoints_.clear();
    indexedMessages_ = 0;

    std::ifstream input(
        indexPath_,
        std::ios::binary);

    if (!input.is_open())
    {
        return false;
    }


    //
    // Validate magic.
    //
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


    //
    // Read metadata.
    //
    std::uint32_t version{0};

    std::uint64_t storedInterval{0};
    std::uint64_t storedMessageCount{0};
    std::uint64_t storedCheckpointCount{0};
    std::uint64_t storedSourceFileSize{0};


    if (
        !readValue(
            input,
            version) ||
        !readValue(
            input,
            storedInterval) ||
        !readValue(
            input,
            storedMessageCount) ||
        !readValue(
            input,
            storedCheckpointCount) ||
        !readValue(
            input,
            storedSourceFileSize))
    {
        return false;
    }


    //
    // Validate metadata against the current source.
    //
    if (
        version != IndexVersion ||
        storedInterval !=
            checkpointInterval_ ||
        storedInterval == 0 ||
        storedMessageCount == 0 ||
        storedCheckpointCount == 0 ||
        storedSourceFileSize !=
            sourceFileSize_)
    {
        return false;
    }


    const auto expectedCheckpointCount =
        ((storedMessageCount - 1) /
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


    //
    // Load and validate every sparse checkpoint.
    //
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
                storedMessageCount)
        {
            checkpoints_.clear();

            return false;
        }


        //
        // The first ITCH BinaryFILE record begins at
        // file offset zero.
        //
        if (
            i == 0 &&
            checkpoint.fileOffset != 0)
        {
            checkpoints_.clear();

            return false;
        }


        //
        // Both sequence numbers and file offsets must
        // move strictly forward.
        //
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
    // Reject unexpected trailing data.
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


    indexedMessages_ =
        storedMessageCount;

    return true;
}


const FileMoldItchRecoverySource::RecoveryCheckpoint*
FileMoldItchRecoverySource::findCheckpoint(
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


    if (
        it ==
        checkpoints_.begin())
    {
        return nullptr;
    }


    return
        &*(it - 1);
}


bool
FileMoldItchRecoverySource::recover(
    std::uint64_t fromSequence,
    std::uint64_t toSequence,
    std::vector<SequencedItchMessage>& messages)
{
    ++recoveryRequests_;

    messages.clear();


    //
    // Recovery interval:
    //
    //      [fromSequence, toSequence)
    //
    if (
        fromSequence >=
            toSequence ||
        fromSequence == 0 ||
        !indexReady_)
    {
        return false;
    }


    //
    // toSequence is exclusive.
    //
    // Example:
    //
    //      [5, 6)
    //
    // requires only message sequence 5.
    //
    if (
        (toSequence - 1) >
            indexedMessages_)
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
    // Persistent file handle:
    //
    // Reset any EOF/failure state left by a previous
    // recovery and seek directly to the nearest sparse
    // checkpoint.
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


    llt::itch::ItchStreamReader reader{
        recoveryFile_};


    std::uint64_t currentSequence =
        checkpoint->sequence;


    //
    // Scan from the nearest checkpoint to the first
    // requested sequence.
    //
    while (
        currentSequence <
            fromSequence)
    {
        auto record =
            reader.readNext();


        if (
            record.status !=
            llt::itch::ItchStreamReadStatus::Message)
        {
            return false;
        }


        ++currentSequence;
    }


    const auto expectedCount =
        toSequence -
        fromSequence;


    if (
        expectedCount >
        static_cast<std::uint64_t>(
            std::numeric_limits<
                std::size_t>::max()))
    {
        return false;
    }


    messages.reserve(
        static_cast<std::size_t>(
            expectedCount));


    //
    // Recover exactly:
    //
    //      [fromSequence, toSequence)
    //
    while (
        currentSequence <
            toSequence)
    {
        auto record =
            reader.readNext();


        if (
            record.status !=
                llt::itch::ItchStreamReadStatus::Message ||
            record.empty())
        {
            messages.clear();

            return false;
        }


        SequencedItchMessage message{};

        message.sequence =
            currentSequence;


        if (
            record.size >
            message.payload.size())
        {
            messages.clear();

            return false;
        }


        message.payloadSize =
            static_cast<std::uint16_t>(
                record.size);


        std::memcpy(
            message.payload.data(),
            record.data,
            record.size);


        messages.push_back(
            std::move(message));


        ++currentSequence;
    }


    //
    // Defensive count verification.
    //
    if (
        messages.size() !=
        static_cast<std::size_t>(
            expectedCount))
    {
        messages.clear();

        return false;
    }


    //
    // Defensive sequence verification.
    //
    for (
        std::size_t i = 0;
        i < messages.size();
        ++i)
    {
        const auto expectedSequence =
            fromSequence +
            static_cast<std::uint64_t>(
                i);


        if (
            messages[i].sequence !=
                expectedSequence)
        {
            messages.clear();

            return false;
        }
    }


    recoveredMessages_ +=
        static_cast<std::uint64_t>(
            messages.size());


    return true;
}

} // namespace llt::moldudp64