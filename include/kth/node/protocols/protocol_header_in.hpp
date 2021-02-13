// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_NODE_PROTOCOL_HEADER_IN_HPP_
#define KTH_NODE_PROTOCOL_HEADER_IN_HPP_

#include <cstddef>
#include <cstdint>
#include <memory>
#include <queue>

#include <kth/blockchain.hpp>
#include <kth/network.hpp>
#include <kth/node/define.hpp>

namespace kth::node {

class full_node;

class BCN_API protocol_header_in : public network::protocol_timer, track<protocol_header_in> {
public:
    using ptr = std::shared_ptr<protocol_header_in>;

    /// Construct a header protocol instance.
    protocol_header_in(full_node& network, network::channel::ptr channel, blockchain::safe_chain& chain);

    /// Start the protocol.
    void start() override;

private:
    using hash_queue = std::queue<hash_digest>;

#if defined(KTH_STATISTICS_ENABLED)
    static 
    void report(domain::chain::header const& block, full_node& node);
#else
    static 
    void report(domain::chain::header const& header);
#endif

    void send_get_blocks(hash_digest const& stop_hash);
    void send_get_data(code const& ec, get_data_ptr message);

    bool handle_receive_headers(code const& ec, headers_const_ptr message);
    bool handle_receive_not_found(code const& ec, not_found_const_ptr message);
    void handle_store_block(code const& ec, block_const_ptr message);
    void handle_fetch_block_locator(code const& ec, get_headers_ptr message, hash_digest const& stop_hash);

    void handle_timeout(code const& ec);
    void handle_stop(code const& ec);

    void organize_headers(headers_const_ptr message);

    // These are thread safe.
    full_node& node_;
    blockchain::safe_chain& chain_;
    asio::duration const header_latency_;
    bool const send_headers_;

    // This is protected by mutex.
    hash_queue backlog_;
    upgrade_mutex mutable mutex;
};

} // namespace kth::node

#endif // KTH_NODE_PROTOCOL_HEADER_IN_HPP_
