// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kth/node/configuration.hpp>

#include <cstddef>

#include <kth/blockchain.hpp>
#include <kth/network.hpp>

namespace kth::node {

// Construct with defaults derived from given context.
configuration::configuration(config::settings context)
    : help(false)
#if ! defined(KTH_DB_READONLY)
    , initchain(false)
#endif  
    , settings(false)
    , version(false)
    , node(context)
    , chain(context)
    , database(context)
    , network(context)
{}

// Copy constructor.
configuration::configuration(const configuration& other)
    : help(other.help)
#if ! defined(KTH_DB_READONLY)  
    , initchain(other.initchain)
#endif  
    , settings(other.settings)
    , version(other.version)
    , file(other.file)
    , node(other.node)
    , chain(other.chain)
    , database(other.database)
    , network(other.network)
{}

} // namespace kth::node
