#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "market_data/normalized/Instrument.h"

namespace llt::market_data
{

class InstrumentStore
{
public:
    static constexpr std::size_t Capacity =
        1ULL << 16;

    bool add(
        const Instrument& instrument
    ) noexcept
    {
        const auto index =
            static_cast<std::size_t>(
                instrument.id
            );

        if (present_[index])
        {
            return false;
        }

        instruments_[index] =
            instrument;

        present_[index] = true;

        ++size_;

        return true;
    }

    [[nodiscard]]
    Instrument* find(
        InstrumentId id
    ) noexcept
    {
        const auto index =
            static_cast<std::size_t>(id);

        if (!present_[index])
        {
            return nullptr;
        }

        return &instruments_[index];
    }

    [[nodiscard]]
    const Instrument* find(
        InstrumentId id
    ) const noexcept
    {
        const auto index =
            static_cast<std::size_t>(id);

        if (!present_[index])
        {
            return nullptr;
        }

        return &instruments_[index];
    }

    [[nodiscard]]
    bool contains(
        InstrumentId id
    ) const noexcept
    {
        return present_[
            static_cast<std::size_t>(id)
        ];
    }

    [[nodiscard]]
    std::size_t size() const noexcept
    {
        return size_;
    }

private:
    std::array<
        Instrument,
        Capacity
    > instruments_{};

    std::array<
        bool,
        Capacity
    > present_{};

    std::size_t size_{0};
};

} // namespace llt::market_data