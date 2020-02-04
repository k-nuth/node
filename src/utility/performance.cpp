// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <bitcoin/node/utility/performance.hpp>

#include <kth/domain.hpp>

namespace libbitcoin {
namespace node {

double performance::normal() const
{
    // If numerator is small we can overflow (infinity).
    return divide<double>(events, static_cast<double>(window) - database);
}

double performance::total() const
{
    return divide<double>(events, window);
}

double performance::ratio() const
{
    return divide<double>(database, window);
}

} // namespace node
} // namespace kth
