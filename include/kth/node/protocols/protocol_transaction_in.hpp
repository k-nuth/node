// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_NODE_PROTOCOL_TRANSACTION_IN_HPP
#define KTH_NODE_PROTOCOL_TRANSACTION_IN_HPP

#include <cstdint>
#include <memory>
#include <kth/blockchain.hpp>
#include <kth/network.hpp>
#include <kth/node/define.hpp>

namespace kth {
namespace node {

class full_node;

class BCN_API protocol_transaction_in
  : public network::protocol_events, track<protocol_transaction_in>
{
public:
    typedef std::shared_ptr<protocol_transaction_in> ptr;

    /// Construct a transaction protocol instance.
    protocol_transaction_in(full_node& network, network::channel::ptr channel,
        blockchain::safe_chain& chain);

    /// Start the protocol.
    virtual void start();

private:
    void send_get_transactions(transaction_const_ptr message);
    void send_get_data(const code& ec, get_data_ptr message);

    bool handle_receive_inventory(const code& ec, inventory_const_ptr message);
    bool handle_receive_transaction(const code& ec,
        transaction_const_ptr message);
    void handle_store_transaction(const code& ec,
        transaction_const_ptr message);

    void handle_stop(const code&);

    // These are thread safe.
    blockchain::safe_chain& chain_;
    const uint64_t minimum_relay_fee_;
    const bool relay_from_peer_;
    const bool refresh_pool_;
    const bool require_witness_;
    const bool peer_witness_;
};

} // namespace node
} // namespace kth

#endif
