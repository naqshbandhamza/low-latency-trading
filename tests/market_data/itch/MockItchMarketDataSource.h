#pragma once

#include <cstddef>
#include <utility>
#include <vector>

#include "market_data/itch/IItchMarketDataSource.h"
#include "market_data/itch/ItchUdpPacket.h"

namespace llt::itch::test
{

class MockItchMarketDataSource final
    : public IItchMarketDataSource
{
public:
    explicit MockItchMarketDataSource(
        std::vector<ItchUdpPacket> packets = {}
    )
        : packets_(
            std::move(packets))
    {
    }

    [[nodiscard]]
    bool receive(
        ItchUdpPacket& packet
    ) noexcept override
    {
        if (next_ >= packets_.size())
        {
            return false;
        }

        packet =
            packets_[next_++];

        return true;
    }

    void add(
        const ItchUdpPacket& packet)
    {
        packets_.push_back(packet);
    }

    [[nodiscard]]
    std::size_t delivered() const noexcept
    {
        return next_;
    }

private:
    std::vector<ItchUdpPacket> packets_{};

    std::size_t next_{0};
};

} // namespace llt::itch::test