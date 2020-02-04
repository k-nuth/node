// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <bitcoin/node/full_node.hpp>

#include <cstddef>
#include <cstdint>
#include <functional>
#include <utility>
#include <bitcoin/blockchain.hpp>
#include <bitcoin/node/configuration.hpp>
#include <bitcoin/node/define.hpp>
#include <bitcoin/node/sessions/session_block_sync.hpp>
#include <bitcoin/node/sessions/session_header_sync.hpp>
#include <bitcoin/node/sessions/session_inbound.hpp>
#include <bitcoin/node/sessions/session_manual.hpp>
#include <bitcoin/node/sessions/session_outbound.hpp>

namespace libbitcoin {
namespace node {

using namespace bc::blockchain;
using namespace bc::chain;
using namespace bc::config;
using namespace bc::network;
using namespace std::placeholders;

full_node::full_node(const configuration& configuration)
    : multi_crypto_setter(configuration.network)
    , p2p(configuration.network)
    , chain_(thread_pool(), configuration.chain, configuration.database, configuration.network.relay_transactions)
    , protocol_maximum_(configuration.network.protocol_maximum)
    , chain_settings_(configuration.chain)
    , node_settings_(configuration.node)
// #ifdef WITH_KEOKEN
//     , keoken_manager_(chain_, node_settings().keoken_genesis_height)
// #endif
{}

full_node::~full_node()
{
    // LOG_INFO(LOG_NODE) << "full_node::~full_node()";
    // std::cout << "full_node::~full_node() -- std::this_thread::get_id(): " << std::this_thread::get_id() << std::endl;
    full_node::close();
}

// Start.
// ----------------------------------------------------------------------------

void full_node::start(result_handler handler)
{
    if (!stopped())
    {
        handler(error::operation_failed);
        return;
    }

    if (!chain_.start())
    {
        LOG_ERROR(LOG_NODE) << "Failure starting blockchain.";
        handler(error::operation_failed);
        return;
    }

    // This is invoked on the same thread.
    // Stopped is true and no network threads until after this call.
    p2p::start(handler);
}

// Run sequence.
// ----------------------------------------------------------------------------

void full_node::run(result_handler handler)
{
    if (stopped())
    {
        handler(error::service_stopped);
        return;
    }

    // Skip sync sessions.
    handle_running(error::success, handler);
    return;

    // TODO: make this safe by requiring sync if gaps found.
    ////// By setting no download connections checkpoints can be used without sync.
    ////// This also allows the maximum protocol version to be set below headers.
    ////if (settings_.sync_peers == 0)
    ////{
    ////    // This will spawn a new thread before returning.
    ////    handle_running(error::success, handler);
    ////    return;
    ////}

    ////// The instance is retained by the stop handler (i.e. until shutdown).
    ////auto const header_sync = attach_header_sync_session();

    ////// This is invoked on a new thread.
    ////header_sync->start(
    ////    std::bind(&full_node::handle_headers_synchronized,
    ////        this, _1, handler));
}

void full_node::handle_headers_synchronized(const code& ec,
    result_handler handler)
{
    ////if (stopped())
    ////{
    ////    handler(error::service_stopped);
    ////    return;
    ////}

    ////if (ec)
    ////{
    ////    LOG_ERROR(LOG_NODE)
    ////        << "Failure synchronizing headers: " << ec.message();
    ////    handler(ec);
    ////    return;
    ////}

    ////// The instance is retained by the stop handler (i.e. until shutdown).
    ////auto const block_sync = attach_block_sync_session();

    ////// This is invoked on a new thread.
    ////block_sync->start(
    ////    std::bind(&full_node::handle_running,
    ////        this, _1, handler));
}

void full_node::handle_running(const code& ec, result_handler handler) {
    if (stopped()) {
        handler(error::service_stopped);
        return;
    }

    if (ec) {
        LOG_ERROR(LOG_NODE)
            << "Failure synchronizing blocks: " << ec.message();
        handler(ec);
        return;
    }

    size_t top_height;
    hash_digest top_hash;

    if ( ! chain_.get_last_height(top_height) ||
         ! chain_.get_block_hash(top_hash, top_height)) {
        LOG_ERROR(LOG_NODE) << "The blockchain is corrupt.";
        handler(error::operation_failed);
        return;
    }

    set_top_block({ std::move(top_hash), top_height });

    LOG_INFO(LOG_NODE) << "Node start height is (" << top_height << ").";

    subscribe_blockchain(
        std::bind(&full_node::handle_reorganized, this, _1, _2, _3, _4));

    // This is invoked on a new thread.
    // This is the end of the derived run startup sequence.
    p2p::run(handler);
}

// A typical reorganization consists of one incoming and zero outgoing blocks.
bool full_node::handle_reorganized(code ec, size_t fork_height,
    block_const_ptr_list_const_ptr incoming,
    block_const_ptr_list_const_ptr outgoing)
{
    if (stopped() || ec == error::service_stopped)
        return false;

    if (ec)
    {
        LOG_ERROR(LOG_NODE)
            << "Failure handling reorganization: " << ec.message();
        stop();
        return false;
    }

    // Nothing to do here.
    if (!incoming || incoming->empty())
        return true;

    for (auto const block: *outgoing)
        LOG_DEBUG(LOG_NODE)
            << "Reorganization moved block to orphan pool ["
            << encode_hash(block->header().hash()) << "]";

    auto const height = safe_add(fork_height, incoming->size());

    set_top_block({ incoming->back()->hash(), height });
    return true;
}

// Specializations.
// ----------------------------------------------------------------------------
// Create derived sessions and override these to inject from derived node.

// Must not connect until running, otherwise imports may conflict with sync.
// But we establish the session in network so caller doesn't need to run.
network::session_manual::ptr full_node::attach_manual_session()
{
    return attach<node::session_manual>(chain_);
}

network::session_inbound::ptr full_node::attach_inbound_session()
{
    return attach<node::session_inbound>(chain_);
}

network::session_outbound::ptr full_node::attach_outbound_session()
{
    return attach<node::session_outbound>(chain_);
}

session_header_sync::ptr full_node::attach_header_sync_session()
{
    return attach<session_header_sync>(hashes_, chain_,
        chain_.chain_settings().checkpoints);
}

session_block_sync::ptr full_node::attach_block_sync_session()
{
    return attach<session_block_sync>(hashes_, chain_, node_settings_);
}

// Shutdown
// ----------------------------------------------------------------------------

bool full_node::stop()
{
    // Suspend new work last so we can use work to clear subscribers.
    auto const p2p_stop = p2p::stop();
    auto const chain_stop = chain_.stop();

    if (!p2p_stop)
        LOG_ERROR(LOG_NODE)
            << "Failed to stop network.";

    if (!chain_stop)
        LOG_ERROR(LOG_NODE)
            << "Failed to stop blockchain.";

    return p2p_stop && chain_stop;
}

// This must be called from the thread that constructed this class (see join).
bool full_node::close()
{
    // LOG_INFO(LOG_NODE) << "full_node::close()";
    // std::cout << "full_node::close() -- std::this_thread::get_id(): " << std::this_thread::get_id() << std::endl;
    
    // Invoke own stop to signal work suspension.
    if (!full_node::stop())
        return false;

    auto const p2p_close = p2p::close();
    auto const chain_close = chain_.close();

    if (!p2p_close)
        LOG_ERROR(LOG_NODE)
            << "Failed to close network.";

    if (!chain_close)
        LOG_ERROR(LOG_NODE)
            << "Failed to close blockchain.";

    return p2p_close && chain_close;
}

// Properties.
// ----------------------------------------------------------------------------

const node::settings& full_node::node_settings() const
{
    return node_settings_;
}

const blockchain::settings& full_node::chain_settings() const
{
    return chain_settings_;
}

safe_chain& full_node::chain()
{
    return chain_;
}

//TODO: remove this function and use safe_chain in the rpc lib
block_chain& full_node::chain_bitprim()
{
    return chain_;
}

// #ifdef WITH_KEOKEN
// knuth::keoken::manager<knuth::keoken::state_delegated>& full_node::keoken_manager() {
//     return keoken_manager_;
// }
// #endif

// Subscriptions.
// ----------------------------------------------------------------------------

void full_node::subscribe_blockchain(reorganize_handler&& handler)
{
    chain().subscribe_blockchain(std::move(handler));
}

void full_node::subscribe_transaction(transaction_handler&& handler)
{
    chain().subscribe_transaction(std::move(handler));
}

// Init node utils.
// ------------------------------------------------------------------------
chain::block full_node::get_genesis_block(blockchain::settings const& settings) {
//    bool const testnet = libbitcoin::get_network(metadata_.configured.network.identifier) == libbitcoin::config::settings::testnet;

    bool const testnet_blocks = settings.easy_blocks;
    bool const retarget = settings.retarget;
    auto const mainnet = retarget && !testnet_blocks;

    if (!mainnet && !testnet_blocks) {
        ////NOTE: To be on regtest, retarget and easy_blocks options must be set to false
        return chain::block::genesis_regtest();
    }

    return testnet_blocks ? chain::block::genesis_testnet() : chain::block::genesis_mainnet();

}

} // namespace node
} // namespace kth
