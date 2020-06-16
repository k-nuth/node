// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_NODE_SETTINGS_HPP
#define KTH_NODE_SETTINGS_HPP

#include <cstdint>
#include <kth/domain.hpp>
#include <kth/node/define.hpp>

namespace kth::node {

/// Common database configuration settings, properties not thread safe.
class BCN_API settings {
public:
    settings();
    settings(infrastructure::config::settings context);

    /// Properties.
    uint32_t sync_peers;
    uint32_t sync_timeout_seconds;
    uint32_t block_latency_seconds;
    bool refresh_transactions;

    /// Mining
    uint32_t rpc_port;
    bool testnet;
    uint32_t subscriber_port;
    std::vector<std::string> rpc_allow_ip;
    bool rpc_allow_all_ips;


    //Compact Blocks
    bool compact_blocks_high_bandwidth;

#ifdef KTH_WITH_KEOKEN
    size_t keoken_genesis_height;
#endif

    /// Helpers.
    asio::duration block_latency() const;
};

} // namespace kth::node

#endif
