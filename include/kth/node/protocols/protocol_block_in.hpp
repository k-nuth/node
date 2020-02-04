// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_NODE_PROTOCOL_BLOCK_IN_HPP
#define KTH_NODE_PROTOCOL_BLOCK_IN_HPP

#include <cstddef>
#include <cstdint>
#include <memory>
#include <queue>
#include <bitcoin/blockchain.hpp>
#include <bitcoin/network.hpp>
#include <bitcoin/node/define.hpp>

namespace kth {
namespace node {

struct temp_compact_block {
     chain::header header;
     std::vector<chain::transaction> transactions;
 };

class full_node;

class BCN_API protocol_block_in
  : public network::protocol_timer, track<protocol_block_in>
{
public:
    typedef std::shared_ptr<protocol_block_in> ptr;

    using compact_block_map = std::unordered_map<hash_digest,temp_compact_block>;
    /// Construct a block protocol instance.
    protocol_block_in(full_node& network, network::channel::ptr channel,
        blockchain::safe_chain& chain);

    /// Start the protocol.
    virtual void start();

private:
    typedef std::queue<hash_digest> hash_queue;

    static void report(const chain::block& block);

    void send_get_blocks(const hash_digest& stop_hash);
    void send_get_data(const code& ec, get_data_ptr message);

    bool handle_receive_block(const code& ec, block_const_ptr message);
    bool handle_receive_compact_block(const code& ec, compact_block_const_ptr message);
    bool handle_receive_block_transactions(const code& ec,block_transactions_const_ptr message);
    bool handle_receive_headers(const code& ec, headers_const_ptr message);
    bool handle_receive_inventory(const code& ec, inventory_const_ptr message);
    bool handle_receive_not_found(const code& ec, not_found_const_ptr message);
    void handle_store_block(const code& ec, block_const_ptr message);
    void handle_fetch_block_locator(const code& ec, get_headers_ptr message, const hash_digest& stop_hash);
    void handle_fetch_block_locator_compact_block(const code& ec, get_headers_ptr message, const hash_digest& stop_hash);

    void send_get_data_compact_block(const code& ec, const hash_digest& hash);

    void handle_timeout(const code& ec);
    void handle_stop(const code& ec);

    void organize_block(block_const_ptr message);

    // These are thread safe.
    full_node& node_;
    blockchain::safe_chain& chain_;
    const asio::duration block_latency_;
    const bool headers_from_peer_;
    const bool compact_from_peer_;
    const bool blocks_from_peer_;
    const bool require_witness_;
    const bool peer_witness_;

    // This is protected by mutex.
    hash_queue backlog_;
    mutable upgrade_mutex mutex;

    compact_block_map compact_blocks_map_;

    bool compact_blocks_high_bandwidth_set_;

    // TODO(Mario) compact blocks version 1 hardcoded, change to 2 when segwit is implemented
    
};


} // namespace node
} // namespace kth

#endif
