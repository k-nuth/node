// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_NODE_FULL_NODE_HPP
#define KTH_NODE_FULL_NODE_HPP

#include <cstdint>
#include <memory>
#include <bitcoin/blockchain.hpp>
#include <bitcoin/network.hpp>
#include <bitcoin/bitcoin/multi_crypto_support.hpp>
#include <bitcoin/node/configuration.hpp>
#include <bitcoin/node/define.hpp>
#include <bitcoin/node/sessions/session_block_sync.hpp>
#include <bitcoin/node/sessions/session_header_sync.hpp>
#include <bitcoin/node/utility/check_list.hpp>

// #ifdef WITH_KEOKEN
// #include <knuth/keoken/manager.hpp>
// #endif

namespace libbitcoin {
namespace node {

struct multi_crypto_setter {
    multi_crypto_setter(network::settings const& net_settings) {
#if defined(KTH_CURRENCY_BCH)
        switch (net_settings.identifier) {
            case netmagic::bch_mainnet:
                set_cashaddr_prefix("bitcoincash");
                break;
            case netmagic::bch_testnet: 
                set_cashaddr_prefix("bchtest");
                break;
            case netmagic::bch_regtest: 
                set_cashaddr_prefix("bchreg");
                break;
            default: 
                set_cashaddr_prefix("");
        }


#elif defined(KTH_CURRENCY_BTC)
        // set_network(net_settings.identifier);
        // set_cashaddr_prefix("");
#else //KTH_CURRENCY_BCH
        // set_network(net_settings.identifier);
        // set_cashaddr_prefix("");
#endif //KTH_CURRENCY_BCH
    }
};

/// A full node on the Bitcoin P2P network.
class BCN_API full_node
    : public multi_crypto_setter
    , public network::p2p
{
public:
    typedef std::shared_ptr<full_node> ptr;
    typedef blockchain::block_chain::reorganize_handler reorganize_handler;
    typedef blockchain::block_chain::transaction_handler transaction_handler;

    /// Construct the full node.
    full_node(const configuration& configuration);

    /// Ensure all threads are coalesced.
    virtual ~full_node();

    // Start/Run sequences.
    // ------------------------------------------------------------------------

    /// Invoke startup and seeding sequence, call from constructing thread.
    void start(result_handler handler) override;

    /// Synchronize the blockchain and then begin long running sessions,
    /// call from start result handler. Call base method to skip sync.
    void run(result_handler handler) override;

    // Shutdown.
    // ------------------------------------------------------------------------

    /// Idempotent call to signal work stop, start may be reinvoked after.
    /// Returns the result of file save operation.
    bool stop() override;

    /// Blocking call to coalesce all work and then terminate all threads.
    /// Call from thread that constructed this class, or don't call at all.
    /// This calls stop, and start may be reinvoked after calling this.
    bool close() override;

    // Properties.
    // ------------------------------------------------------------------------

    /// Node configuration settings.
    virtual const node::settings& node_settings() const;

    /// Node configuration settings.
    virtual const blockchain::settings& chain_settings() const;

    /// Blockchain query interface.
    virtual blockchain::safe_chain& chain();

    /// Blockchain.
    //TODO: remove this function and use safe_chain in the rpc lib
    virtual blockchain::block_chain& chain_kth();

// #ifdef WITH_KEOKEN
//     knuth::keoken::manager<knuth::keoken::state_delegated>& keoken_manager();
// #endif

    // Subscriptions.
    // ------------------------------------------------------------------------

    /// Subscribe to blockchain reorganization and stop events.
    virtual void subscribe_blockchain(reorganize_handler&& handler);

    /// Subscribe to transaction pool acceptance and stop events.
    virtual void subscribe_transaction(transaction_handler&& handler);

    // Init node utils.
    // ------------------------------------------------------------------------
    static chain::block get_genesis_block(blockchain::settings const& settings);


protected:
    /// Attach a node::session to the network, caller must start the session.
    template <class Session, typename... Args>
    typename Session::ptr attach(Args&&... args)
    {
        return std::make_shared<Session>(*this, std::forward<Args>(args)...);
    }

    /// Override to attach specialized p2p sessions.
    ////network::session_seed::ptr attach_seed_session() override;
    network::session_manual::ptr attach_manual_session() override;
    network::session_inbound::ptr attach_inbound_session() override;
    network::session_outbound::ptr attach_outbound_session() override;

    /// Override to attach specialized node sessions.
    virtual session_header_sync::ptr attach_header_sync_session();
    virtual session_block_sync::ptr attach_block_sync_session();

    ///For mining
    blockchain::block_chain chain_;

private:
    typedef message::block::ptr_list block_ptr_list;

    bool handle_reorganized(code ec, size_t fork_height,
        block_const_ptr_list_const_ptr incoming,
        block_const_ptr_list_const_ptr outgoing);

    void handle_headers_synchronized(const code& ec, result_handler handler);
    void handle_network_stopped(const code& ec, result_handler handler);

    void handle_started(const code& ec, result_handler handler);
    void handle_running(const code& ec, result_handler handler);

    // These are thread safe.
    check_list hashes_;
    //blockchain::block_chain chain_;
    const uint32_t protocol_maximum_;
    const node::settings& node_settings_;
    const blockchain::settings& chain_settings_;

// #ifdef WITH_KEOKEN
//     knuth::keoken::manager<knuth::keoken::state_delegated> keoken_manager_;
// #endif
};

} // namespace node
} // namespace kth

#endif
