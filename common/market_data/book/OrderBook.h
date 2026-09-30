#pragma once

#include "market_data/book/BookSide.h"
#include "market_data/normalized/MarketTypes.h"

namespace llt::market_data
{

struct Bbo
{
    bool hasBid{false};
    Price bidPrice{0};
    Quantity bidQuantity{0};

    bool hasAsk{false};
    Price askPrice{0};
    Quantity askQuantity{0};

    [[nodiscard]]
    bool operator==(
        const Bbo& other
    ) const noexcept
    {
        // Bid presence changed.
        if (hasBid != other.hasBid)
        {
            return false;
        }

        // Ask presence changed.
        if (hasAsk != other.hasAsk)
        {
            return false;
        }

        // Only compare bid values when a bid exists.
        if (
            hasBid &&
            (
                bidPrice != other.bidPrice ||
                bidQuantity != other.bidQuantity
            )
        )
        {
            return false;
        }

        // Only compare ask values when an ask exists.
        if (
            hasAsk &&
            (
                askPrice != other.askPrice ||
                askQuantity != other.askQuantity
            )
        )
        {
            return false;
        }

        return true;
    }

    [[nodiscard]]
    bool operator!=(
        const Bbo& other
    ) const noexcept
    {
        return !(*this == other);
    }
};


class OrderBook
{
public:
    OrderBook() noexcept
        : bids_(Side::Buy),
          asks_(Side::Sell)
    {
    }

    [[nodiscard]]
    BookSide& bids() noexcept
    {
        return bids_;
    }

    [[nodiscard]]
    const BookSide& bids() const noexcept
    {
        return bids_;
    }

    [[nodiscard]]
    BookSide& asks() noexcept
    {
        return asks_;
    }

    [[nodiscard]]
    const BookSide& asks() const noexcept
    {
        return asks_;
    }

    [[nodiscard]]
    BookSide& side(
        Side side
    ) noexcept
    {
        return side == Side::Buy
            ? bids_
            : asks_;
    }

    [[nodiscard]]
    const BookSide& side(
        Side side
    ) const noexcept
    {
        return side == Side::Buy
            ? bids_
            : asks_;
    }

    [[nodiscard]]
    Bbo bbo() const noexcept
    {
        Bbo result{};

        if (const auto* bid = bids_.best())
        {
            result.hasBid = true;
            result.bidPrice = bid->price;
            result.bidQuantity = bid->quantity;
        }

        if (const auto* ask = asks_.best())
        {
            result.hasAsk = true;
            result.askPrice = ask->price;
            result.askQuantity = ask->quantity;
        }

        return result;
    }

    [[nodiscard]]
    bool empty() const noexcept
    {
        return bids_.empty() &&
               asks_.empty();
    }

private:
    BookSide bids_;
    BookSide asks_;
};

} // namespace llt::market_data