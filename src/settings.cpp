// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kth/node/settings.hpp>

#include <kth/domain.hpp>

namespace kth {
namespace node {

using namespace bc::asio;

settings::settings()
    : sync_peers(0)
    , sync_timeout_seconds(5)
    , block_latency_seconds(60)
    , refresh_transactions(true)
    , rpc_port(8332)
    , testnet(false)
    , subscriber_port(5556)
    , compact_blocks_high_bandwidth(true)
    , rpc_allow_all_ips(false)
#ifdef KTH_WITH_KEOKEN
    , keoken_genesis_height(kth::max_size_t)
#endif
{
    rpc_allow_ip.push_back("127.0.0.1");
}

// There are no current distinctions spanning chain contexts.
settings::settings(config::settings context)
    : settings()
{
}

duration settings::block_latency() const
{
    return seconds(block_latency_seconds);
}

} // namespace node
} // namespace kth
