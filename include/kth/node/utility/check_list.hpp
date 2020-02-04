// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_NODE_CHECK_LIST_HPP
#define KTH_NODE_CHECK_LIST_HPP

#include <cstddef>
#include <boost/bimap.hpp>
#include <boost/bimap/set_of.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <bitcoin/database.hpp>
#include <bitcoin/node/define.hpp>

namespace kth {
namespace node {

/// A thread safe checkpoint queue.
class BCN_API check_list
{
public:

#ifdef KTH_DB_LEGACY
    using heights = database::block_database::heights;
#endif // KTH_DB_LEGACY

#ifdef KTH_DB_NEW
    using heights = std::vector<size_t>;
#endif // KTH_DB_NEW

    /// The queue contains no checkpoints.
    bool empty() const;

    /// The number of checkpoints in the queue.
    size_t size() const;

    /// Reserve the entries indicated by the given heights.
    /// Any entry not reserved here will be ignored upon enqueue.
    void reserve(const heights& heights);

    /// Place a hash on the queue at the height if it has a reservation.
    void enqueue(hash_digest&& hash, size_t height);

    /// Remove the next entry by increasing height.
    bool dequeue(hash_digest& out_hash, size_t& out_height);

private:
    // A bidirection map is used for efficient hash and height retrieval.
    typedef boost::bimaps::bimap<
        boost::bimaps::unordered_set_of<hash_digest>,
        boost::bimaps::set_of<size_t>> checks;

    checks checks_;
    mutable shared_mutex mutex_;
};

} // namespace node
} // namespace kth

#endif
