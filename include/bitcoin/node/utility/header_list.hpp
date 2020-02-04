// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_NODE_HEADER_LIST_HPP
#define KTH_NODE_HEADER_LIST_HPP

#include <cstddef>
#include <memory>
#include <kth/domain.hpp>
#include <bitcoin/node/define.hpp>
#include <bitcoin/node/utility/check_list.hpp>

namespace libbitcoin {
namespace node {

/// A smart queue for chaining blockchain headers, thread safe.
/// The peer should be stopped if merge fails.
class BCN_API header_list
{
public:
    typedef std::shared_ptr<header_list> ptr;

    /// Construct a list to fill the specified range of headers.
    header_list(size_t slot, const config::checkpoint& start,
        const config::checkpoint& stop);

    /// The list is fully populated.
    bool complete() const;

    /// The slot id of this instance.
    size_t slot() const;

    /// The height of the first header in the list.
    size_t first_height() const;

    /// The height of the last header in the list (or the start height).
    size_t previous_height() const;

    /// The hash of the last header in the list (or the start hash).
    hash_digest previous_hash() const;

    /// The hash of the stop checkpoint.
    const hash_digest& stop_hash() const;

    /// The ordered list of headers.
    /// This is not thread safe, call only after complete.
    const chain::header::list& headers() const;

    /////// Generate a check list from a complete list of headers.
    ////config::checkpoint::list to_checkpoints() const;

    /// Merge the hashes in the message with those in the queue.
    /// Return true if linked all headers or complete.
    bool merge(headers_const_ptr message);

private:
    // The number of headers remaining until complete.
    size_t remaining() const;

    /// The hash of the last header in the list (or the start hash).
    const hash_digest& last() const;

    // Determine if the hash is linked to the preceding header.
    bool link(const chain::header& header) const;

    // Determine if the header is valid (context free).
    bool check(const chain::header& header) const;

    // Determine if the header is acceptable for the current height.
    bool accept(const chain::header& header) const;

    // This is protected by mutex.
    chain::header::list list_;
    mutable upgrade_mutex mutex_;

    const size_t height_;
    const config::checkpoint start_;
    const config::checkpoint stop_;
    const size_t slot_;
};

} // namespace node
} // namespace kth

#endif

