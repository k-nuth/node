// Copyright (c) 2016-2021 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kth/node/protocols/protocol_double_spend_proofs_in.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <utility>

#include <kth/network.hpp>
#include <kth/node/define.hpp>
#include <kth/node/full_node.hpp>

namespace kth::node {

#define NAME "double_spend_proofs_in"
#define CLASS protocol_double_spend_proofs_in

using namespace kth::blockchain;
using namespace kth::domain::message;
using namespace kth::network;
using namespace std::placeholders;

protocol_double_spend_proofs_in::protocol_double_spend_proofs_in(full_node& node, channel::ptr channel, safe_chain& chain)
    : protocol_events(node, channel, NAME)
    , chain_(chain)
    , ds_proofs_enabled_(node.node_settings().ds_proofs_enabled)
    , CONSTRUCT_TRACK(protocol_double_spend_proofs_in)
{}

// Start.
//-----------------------------------------------------------------------------

void protocol_double_spend_proofs_in::start() {
    protocol_events::start(BIND1(handle_stop, _1));

    SUBSCRIBE2(inventory, handle_receive_inventory, _1, _2);
    SUBSCRIBE2(double_spend_proofs, handle_receive_ds_proof_data, _1, _2);
}

// Receive inventory sequence.
//-----------------------------------------------------------------------------

bool protocol_double_spend_proofs_in::handle_receive_inventory(code const& ec, inventory_const_ptr message) {
    if (stopped(ec)) {
        return false;
    }

    // Do not process DSProof inventory while chain is stale.
    if (chain_.is_stale()) {
        return true;
    }

    auto const response = std::make_shared<get_data>();

    // Copy the DSProof inventories into a get_data instance.
    message->reduce(response->inventories(), inventory::type_id::double_spend_proofs);

    if (message->inventories().empty()) {
        return true;
    }

    if ( ! ds_proofs_enabled_) {
        LOG_DEBUG(LOG_NODE, "Got DSProof INV but node.ds_proofs is disabled (settings) from [", authority(), "]");
        return true;
    }
    // BCHN: inv.hash.ToString()

    LOG_DEBUG(LOG_NODE, "Got DSProof INV from [", authority(), "]");

    // if (DoubleSpendProof::IsEnabled()) {
    //     // dsproof subsystem enabled, ask peer for the proof
    //     LogPrint(BCLog::DSPROOF, "Got DSProof INV %s\n", inv.hash.ToString());
    //     connman->PushMessage(pfrom,
    //                             msgMaker.Make(NetMsgType::GETDATA, std::vector<CInv>{inv}));
    // } else {
    //     // dsproof subsystem disabled, ignore this inv
    //     LogPrint(BCLog::DSPROOF, "Got DSProof INV %s (ignored, -doublespendproof=0)\n",
    //                 inv.hash.ToString());
    // }

    send_get_data(ec, response);
    return true;
}

void protocol_double_spend_proofs_in::send_get_data(code const& ec, get_data_ptr message) {
    if (stopped(ec) || message->inventories().empty()) {
        return;
    }

    // if (ec) {
    //     LOG_ERROR(LOG_NODE, "Internal failure filtering transaction hashes for [", authority(), "] ", ec.message());
    //     stop(ec);
    //     return;
    // }

    // inventory->get_data[transaction]
    SEND2(*message, handle_send, _1, message->command);
}

// Receive DSProof data.
//-----------------------------------------------------------------------------

// A transaction is acceptable whether solicited or broadcast.
bool protocol_double_spend_proofs_in::handle_receive_ds_proof_data(code const& ec, double_spend_proofs_const_ptr message) {

    if (stopped(ec)) {
        return false;
    }

    // TODO: manage channel relay at the service layer.
    // Do not process transactions while chain is stale.
    if (chain_.is_stale()) {
        return true;
    }

    // chain_.organize(message, BIND2(handle_store_ds_proof_data, _1, message));
    return true;
}

// The transaction has been saved to the memory pool (or not).
// This will be picked up by subscription in transaction_out and will cause
// the transaction to be announced to non-originating relay-accepting peers.
void protocol_double_spend_proofs_in::handle_store_ds_proof_data(code const& ec, double_spend_proofs_const_ptr message) {
    if (stopped(ec)) {
        return;
    }

    auto const encoded = encode_hash(message->hash());

    if (ec) {
        // This should not happen with a single peer since we filter inventory.
        // However it will happen when a block or another peer's tx intervenes.
        LOG_DEBUG(LOG_NODE, "Dropped transaction [", encoded, "] from [", authority(), "] ", ec.message());
        return;
    }

    LOG_DEBUG(LOG_NODE, "Stored transaction [", encoded, "] from [", authority(), "].");
}

// Stop.
//-----------------------------------------------------------------------------

void protocol_double_spend_proofs_in::handle_stop(code const&) {
    LOG_DEBUG(LOG_NETWORK, "Stopped double_spend_proofs_in protocol for [", authority(), "].");
}

} // namespace kth::node