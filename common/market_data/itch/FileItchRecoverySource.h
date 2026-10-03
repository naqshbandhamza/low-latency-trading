#pragma once

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

#include "market_data/itch/IItchRecoverySource.h"
#include "market_data/itch/ItchUdpPacket.h"

namespace llt::itch
{

class FileItchRecoverySource final
    : public IItchRecoverySource
{
public:
    static constexpr std::uint64_t
        DefaultCheckpointInterval = 1000;

    explicit FileItchRecoverySource(
        std::string filePath,
        std::uint64_t checkpointInterval =
            DefaultCheckpointInterval);

    bool recover(
        std::uint64_t fromSequence,
        std::uint64_t toSequence,
        std::vector<ItchUdpPacket>& packets
    ) override;

    [[nodiscard]]
    std::uint64_t recoveryRequests() const noexcept
    {
        return recoveryRequests_;
    }

    [[nodiscard]]
    std::uint64_t recoveredPackets() const noexcept
    {
        return recoveredPackets_;
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
    std::uint64_t indexedRecords() const noexcept
    {
        return indexedRecords_;
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

    std::string filePath_;
    std::string indexPath_;

    std::uint64_t checkpointInterval_{
        DefaultCheckpointInterval};

    std::vector<RecoveryCheckpoint>
        checkpoints_{};

    std::uint64_t indexedRecords_{0};
    std::uint64_t sourceFileSize_{0};

    bool indexReady_{false};
    bool indexLoadedFromDisk_{false};

    //
    // Persistent file handle: recovery no longer opens
    // the BinaryFILE for every sequence gap.
    //
    std::ifstream recoveryFile_;

    std::uint64_t recoveryRequests_{0};
    std::uint64_t recoveredPackets_{0};
};

} // namespace llt::itch
