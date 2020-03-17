// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_NODE_CONFIGURATION_HPP
#define KTH_NODE_CONFIGURATION_HPP

#include <boost/filesystem.hpp>
#include <kth/blockchain.hpp>
#include <kth/network.hpp>
#include <kth/node/define.hpp>
#include <kth/node/settings.hpp>

// Not localizable.
#define BN_HELP_VARIABLE "help"
#define BN_SETTINGS_VARIABLE "settings"
#define BN_VERSION_VARIABLE "version"

// This must be lower case but the env var part can be any case.
#define BN_CONFIG_VARIABLE "config"

// This must match the case of the env var.
#define BN_ENVIRONMENT_VARIABLE_PREFIX "BN_"


namespace kth::node {

/// Full node configuration, thread safe.
class BCN_API configuration {
public:
    configuration(config::settings context);
    configuration(const configuration& other);

    /// Options.
    bool help;

#if ! defined(KTH_DB_READONLY)
    bool initchain;
#endif
    
    bool settings;
    bool version;

    /// Options and environment vars.
    boost::filesystem::path file;

    /// Settings.
    node::settings node;
    blockchain::settings chain;
    database::settings database;
    network::settings network;
};

} // namespace kth::node

#endif
