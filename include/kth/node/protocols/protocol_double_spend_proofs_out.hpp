// Copyright (c) 2016-2021 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_NODE_PROTOCOL_DOUBLE_SPEND_PROOFS_OUT_HPP
#define KTH_NODE_PROTOCOL_DOUBLE_SPEND_PROOFS_OUT_HPP

#include <atomic>
#include <cstdint>
#include <memory>
#include <kth/blockchain.hpp>
#include <kth/network.hpp>
#include <kth/node/define.hpp>

namespace kth::node {

class full_node;

class BCN_API protocol_double_spend_proofs_out : public network::protocol_events, track<protocol_double_spend_proofs_out> {
public:
    using ptr = std::shared_ptr<protocol_double_spend_proofs_out>;

    /// Construct a transaction protocol instance.
    protocol_double_spend_proofs_out(full_node& network, network::channel::ptr channel, blockchain::safe_chain& chain);

    /// Start the protocol.
    virtual void start();

private:
    // void send_next_data(inventory_ptr inventory);
    // void send_transaction(code const& ec, transaction_const_ptr message, size_t position, size_t height, inventory_ptr inventory);

    bool handle_receive_get_data(code const& ec, get_data_const_ptr message);
    void handle_stop(code const& ec);
    // void handle_send_next(code const& ec, inventory_ptr inventory);
    // bool handle_transaction_pool(code const& ec, transaction_const_ptr message);

    // These are thread safe.
    blockchain::safe_chain& chain_;
    bool const ds_proofs_enabled_;
};

} // namespace kth::node

#endif // KTH_NODE_PROTOCOL_DOUBLE_SPEND_PROOFS_OUT_HPP
