// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <bitcoin/node/configuration.hpp>

#include <cstddef>
#include <bitcoin/blockchain.hpp>
#include <bitcoin/network.hpp>

namespace libbitcoin {
namespace node {

// Construct with defaults derived from given context.
configuration::configuration(config::settings context)
  : help(false),

#if !defined(WITH_REMOTE_BLOCKCHAIN) && !defined(WITH_REMOTE_DATABASE)
    initchain(false),
#endif // !defined(WITH_REMOTE_BLOCKCHAIN) && !defined(WITH_REMOTE_DATABASE)    

    settings(false),
    version(false),
    node(context),
    chain(context),
    database(context),
    network(context)
{
}

// Copy constructor.
configuration::configuration(const configuration& other)
  : help(other.help),

#if !defined(WITH_REMOTE_BLOCKCHAIN) && !defined(WITH_REMOTE_DATABASE)
    initchain(other.initchain),
#endif // !defined(WITH_REMOTE_BLOCKCHAIN) && !defined(WITH_REMOTE_DATABASE)

    settings(other.settings),
    version(other.version),
    file(other.file),
    node(other.node),
    chain(other.chain),
    database(other.database),
    network(other.network)
{
}

} // namespace node
} // namespace kth
