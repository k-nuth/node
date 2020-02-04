// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_NODE_PROTOCOL_TRANSACTION_OUT_HPP
#define KTH_NODE_PROTOCOL_TRANSACTION_OUT_HPP

#include <atomic>
#include <cstdint>
#include <memory>
#include <bitcoin/blockchain.hpp>
#include <bitcoin/network.hpp>
#include <bitcoin/node/define.hpp>

namespace libbitcoin {
namespace node {

class full_node;

class BCN_API protocol_transaction_out
  : public network::protocol_events, track<protocol_transaction_out>
{
public:
    typedef std::shared_ptr<protocol_transaction_out> ptr;

    /// Construct a transaction protocol instance.
    protocol_transaction_out(full_node& network, network::channel::ptr channel,
        blockchain::safe_chain& chain);

    /// Start the protocol.
    virtual void start();

private:
    void send_next_data(inventory_ptr inventory);
    void send_transaction(const code& ec, transaction_const_ptr message,
        size_t position, size_t height, inventory_ptr inventory);

    bool handle_receive_get_data(const code& ec,
        get_data_const_ptr message);
    bool handle_receive_fee_filter(const code& ec,
        fee_filter_const_ptr message);
    bool handle_receive_memory_pool(const code& ec,
        memory_pool_const_ptr message);

    void handle_fetch_mempool(const code& ec, inventory_ptr message);

    void handle_stop(const code& ec);
    void handle_send_next(const code& ec, inventory_ptr inventory);
    bool handle_transaction_pool(const code& ec,
        transaction_const_ptr message);

    // These are thread safe.
    blockchain::safe_chain& chain_;
    std::atomic<uint64_t> minimum_peer_fee_;
    ////std::atomic<bool> compact_to_peer_;
    const bool relay_to_peer_;
    const bool enable_witness_;
};

} // namespace node
} // namespace kth

#endif
