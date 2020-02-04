// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_NODE_PROTOCOL_BLOCK_OUT_HPP
#define KTH_NODE_PROTOCOL_BLOCK_OUT_HPP

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <bitcoin/blockchain.hpp>
#include <bitcoin/network.hpp>
#include <bitcoin/node/define.hpp>

namespace libbitcoin {
namespace node {

class full_node;

class BCN_API protocol_block_out
  : public network::protocol_events, track<protocol_block_out>
{
public:
    typedef std::shared_ptr<protocol_block_out> ptr;

    /// Construct a block protocol instance.
    protocol_block_out(full_node& network, network::channel::ptr channel,
        blockchain::safe_chain& chain);

    /// Start the protocol.
    virtual void start();

private:
    size_t locator_limit();

    void send_next_data(inventory_ptr inventory);
    void send_block(const code& ec, block_const_ptr message,
        size_t height, inventory_ptr inventory);
    void send_merkle_block(const code& ec, merkle_block_const_ptr message,
        size_t height, inventory_ptr inventory);
    void send_compact_block(const code& ec, compact_block_const_ptr message,
        size_t height, inventory_ptr inventory);

    bool handle_receive_get_data(const code& ec,
        get_data_const_ptr message);
    bool handle_receive_get_blocks(const code& ec,
        get_blocks_const_ptr message);
    bool handle_receive_get_headers(const code& ec,
        get_headers_const_ptr message);
    bool handle_receive_send_headers(const code& ec,
        send_headers_const_ptr message);
    bool handle_receive_send_compact(const code& ec,
        send_compact_const_ptr message);

    bool handle_receive_get_block_transactions(const code& ec, 
        get_block_transactions_const_ptr message);

    void handle_fetch_locator_hashes(const code& ec, inventory_ptr message);
    void handle_fetch_locator_headers(const code& ec, headers_ptr message);

    void handle_stop(const code& ec);
    void handle_send_next(const code& ec, inventory_ptr inventory);
    bool handle_reorganized(code ec, size_t fork_height,
        block_const_ptr_list_const_ptr incoming,
        block_const_ptr_list_const_ptr outgoing);

    // These are thread safe.
    full_node& node_;
    blockchain::safe_chain& chain_;
    bc::atomic<hash_digest> last_locator_top_;
    std::atomic<bool> compact_to_peer_;
    std::atomic<bool> headers_to_peer_;
    const bool enable_witness_;
    std::atomic<bool> compact_high_bandwidth_;
    std::atomic<uint64_t> compact_version_;
};

} // namespace node
} // namespace kth

#endif
