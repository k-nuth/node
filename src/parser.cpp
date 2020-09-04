// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kth/node/parser.hpp>

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <string>

#include <boost/program_options.hpp>

#include <kth/blockchain.hpp>
#include <kth/domain/multi_crypto_support.hpp>
#include <kth/network.hpp>
#include <kth/node/full_node.hpp>
#include <kth/node/settings.hpp>

//BC_DECLARE_CONFIG_DEFAULT_PATH("")
//BC_DECLARE_CONFIG_DEFAULT_PATH("kth" / "kth.cfg")

// TODO: localize descriptions.

namespace kth::node {

using namespace std::filesystem;
using namespace boost::program_options;
using namespace kth::domain;
using namespace kth::domain::config;

// Initialize configuration by copying the given instance.
parser::parser(configuration const& defaults)
    : configured(defaults)
{}

// Initialize configuration using defaults of the given context.
parser::parser(infrastructure::config::settings context)
    : configured(context)
{
    // kth_node use history
    configured.database.index_start_height = 0;
    // node doesn't use history, and history is expensive.
    // configured.database.index_start_height = kth::max_uint32;

    using serve = domain::message::version::service;

    // A node allows 8 inbound connections by default.
    configured.network.inbound_connections = 8;
    // Logs will slow things if not rotated.
    configured.network.rotation_size = 10000000;

    // With block-first sync the count should be low until complete.
    configured.network.outbound_connections = 2;

    // A node allows 1000 host names by default.
    configured.network.host_pool_capacity = 1000;
#if defined(KTH_CURRENCY_BCH)
    // Expose full node (1) services by default.
    configured.network.services = serve::node_network;
#else
    // Expose full node (1) and witness (8) network services by default.
    configured.network.services = serve::node_network | serve::node_witness;
#endif
}

options_metadata parser::load_options() {
    options_metadata description("options");
    description.add_options() (
        BN_CONFIG_VARIABLE ",c",
        value<path>(&configured.file),
        "Specify path to a configuration settings file."
    )(
        BN_HELP_VARIABLE ",h",
        value<bool>(&configured.help)->
            default_value(false)->zero_tokens(),
        "Display command line options."
    )
#if ! defined(KTH_DB_READONLY)
    (
        "initchain,i",
        value<bool>(&configured.initchain)->
            default_value(false)->zero_tokens(),
        "Initialize blockchain in the configured directory."
    ) 
#endif // ! defined(KTH_DB_READONLY)    
    (
        BN_SETTINGS_VARIABLE ",s",
        value<bool>(&configured.settings)->
            default_value(false)->zero_tokens(),
        "Display all configuration settings."
    )(
        BN_VERSION_VARIABLE ",v",
        value<bool>(&configured.version)->
            default_value(false)->zero_tokens(),
        "Display version information."
    );

    return description;
}

arguments_metadata parser::load_arguments() {
    arguments_metadata description;
    return description.add(BN_CONFIG_VARIABLE, 1);
}

options_metadata parser::load_environment() {
    options_metadata description("environment");
    description.add_options()(
        // For some reason po requires this to be a lower case name.
        // The case must match the other declarations for it to compose.
        // This composes with the cmdline options and inits to system path.
        BN_CONFIG_VARIABLE,
        value<path>(&configured.file)->composing()
            ->default_value(infrastructure::config::config_default_path()),
        "The path to the configuration settings file."
    );

    return description;
}

options_metadata parser::load_settings() {
    options_metadata description("settings");
    description.add_options()
    /* [log] */
    (
        "log.debug_file",
        value<path>(&configured.network.debug_file),
        "The debug log file path, defaults to 'debug.log'."
    )(
        "log.error_file",
        value<path>(&configured.network.error_file),
        "The error log file path, defaults to 'error.log'."
    )(
        "log.archive_directory",
        value<path>(&configured.network.archive_directory),
        "The log archive directory, defaults to 'archive'."
    )(
        "log.rotation_size",
        value<size_t>(&configured.network.rotation_size),
        "The size at which a log is archived, defaults to 10000000 (0 disables)."
    )(
        "log.minimum_free_space",
        value<size_t>(&configured.network.minimum_free_space),
        "The minimum free space required in the archive directory, defaults to 0."
    )(
        "log.maximum_archive_size",
        value<size_t>(&configured.network.maximum_archive_size),
        "The maximum combined size of archived logs, defaults to 0 (maximum)."
    )(
        "log.maximum_archive_files",
        value<size_t>(&configured.network.maximum_archive_files),
        "The maximum number of logs to archive, defaults to 0 (maximum)."
    )(
        "log.statistics_server",
        value<infrastructure::config::authority>(&configured.network.statistics_server),
        "The address of the statistics collection server, defaults to none."
    )(
        "log.verbose",
        value<bool>(&configured.network.verbose),
        "Enable verbose logging, defaults to false."
    )
    /* [network] */
    (
        "network.threads",
        value<uint32_t>(&configured.network.threads),
        "The minimum number of threads in the network threadpool, defaults to 0 (physical cores)."
    )(
        "network.protocol_maximum",
        value<uint32_t>(&configured.network.protocol_maximum),
        "The maximum network protocol version, defaults to 70013."
    )(
        "network.protocol_minimum",
        value<uint32_t>(&configured.network.protocol_minimum),
        "The minimum network protocol version, defaults to 31402."
    )(
        "network.services",
        value<uint64_t>(&configured.network.services),
        "The services exposed by network connections, defaults to 9 BTC (full node, witness). and 1 (full node BCH)"
    )(
        "network.invalid_services",
        value<uint64_t>(&configured.network.invalid_services),
        "The advertised services that cause a peer to be dropped, defaults to 176 (BTC) and 0 (BCH)."
    )(
        "network.validate_checksum",
        value<bool>(&configured.network.validate_checksum),
        "Validate the checksum of network messages, defaults to false."
    )(
        "network.identifier",
        value<uint32_t>(&configured.network.identifier),
        "The magic number for message headers, defaults to 3652501241."
    )(
        "network.inbound_port",
        value<uint16_t>(&configured.network.inbound_port),
        "The port for incoming connections, defaults to 8333."
    )(
        "network.inbound_connections",
        value<uint32_t>(&configured.network.inbound_connections),
        "The target number of incoming network connections, defaults to 0."
    )(
        "network.outbound_connections",
        value<uint32_t>(&configured.network.outbound_connections),
        "The target number of outgoing network connections, defaults to 2."
    )(
        "network.manual_attempt_limit",
        value<uint32_t>(&configured.network.manual_attempt_limit),
        "The attempt limit for manual connection establishment, defaults to 0 (forever)."
    )(
        "network.connect_batch_size",
        value<uint32_t>(&configured.network.connect_batch_size),
        "The number of concurrent attempts to establish one connection, defaults to 5."
    )(
        "network.connect_timeout_seconds",
        value<uint32_t>(&configured.network.connect_timeout_seconds),
        "The time limit for connection establishment, defaults to 5."
    )(
        "network.channel_handshake_seconds",
        value<uint32_t>(&configured.network.channel_handshake_seconds),
        "The time limit to complete the connection handshake, defaults to 30."
    )(
        "network.channel_heartbeat_minutes",
        value<uint32_t>(&configured.network.channel_heartbeat_minutes),
        "The time between ping messages, defaults to 5."
    )(
        "network.channel_inactivity_minutes",
        value<uint32_t>(&configured.network.channel_inactivity_minutes),
        "The inactivity time limit for any connection, defaults to 30."
    )(
        "network.channel_expiration_minutes",
        value<uint32_t>(&configured.network.channel_expiration_minutes),
        "The age limit for any connection, defaults to 60."
    )(
        "network.channel_germination_seconds",
        value<uint32_t>(&configured.network.channel_germination_seconds),
        "The time limit for obtaining seed addresses, defaults to 30."
    )(
        "network.host_pool_capacity",
        value<uint32_t>(&configured.network.host_pool_capacity),
        "The maximum number of peer hosts in the pool, defaults to 1000."
    )(
        "network.hosts_file",
        value<path>(&configured.network.hosts_file),
        "The peer hosts cache file path, defaults to 'hosts.cache'."
    )(
        "network.self",
        value<infrastructure::config::authority>(&configured.network.self),
        "The advertised public address of this node, defaults to none."
    )(
        "network.blacklist",
        value<infrastructure::config::authority::list>(&configured.network.blacklists),
        "IP address to disallow as a peer, multiple entries allowed."
    )(
        "network.peer",
        value<infrastructure::config::endpoint::list>(&configured.network.peers),
        "A persistent peer node, multiple entries allowed."
    )(
        "network.seed",
        value<infrastructure::config::endpoint::list>(&configured.network.seeds),
        "A seed node for initializing the host pool, multiple entries allowed."
    )(
        "network.use_ipv6",
        value<bool>(&configured.network.use_ipv6),
        "Node use ipv6."
    )(
        "network.user_agent_blacklist",
        value<std::vector<std::string>>(&configured.network.user_agent_blacklist),
        "Blacklist user-agent starting with..."
    )

    /* [database] */
    (
        "database.directory",
        value<path>(&configured.database.directory),
        "The blockchain database directory, defaults to 'blockchain'."
    )(
        "database.flush_writes",
        value<bool>(&configured.database.flush_writes),
        "Flush each write to disk, defaults to false."
    )(
        "database.file_growth_rate",
        value<uint16_t>(&configured.database.file_growth_rate),
        "Full database files increase by this percentage, defaults to 50."
    )

#ifdef KTH_DB_NEW
    (
        "database.reorg_pool_limit",
        value<uint32_t>(&configured.database.reorg_pool_limit),
        "Approximate number of blocks to store in the reorganization pool, defaults to 100."        //TODO(fernando): look for a good default
    )(
        "database.db_max_size",
        value<uint64_t>(&configured.database.db_max_size),
        "Maximum size of the database expressed in bytes, defaults to 100 GiB."                    //TODO(fernando): look for a good default
    )(
        "database.safe_mode",
        value<bool>(&configured.database.safe_mode),
        "safe mode is more secure but not the fastest, defaults to true."                  
    )
#endif // KTH_DB_NEW

#ifdef KTH_DB_LEGACY    
    (
        "database.block_table_buckets",
        value<uint32_t>(&configured.database.block_table_buckets),
        "Block hash table size, defaults to 650000."
    )(
        "database.transaction_table_buckets",
        value<uint32_t>(&configured.database.transaction_table_buckets),
        "Transaction hash table size, defaults to 110000000."
    )
#endif // KTH_DB_LEGACY
#ifdef KTH_DB_TRANSACTION_UNCONFIRMED    
    (
        "database.transaction_unconfirmed_table_buckets",
        value<uint32_t>(&configured.database.transaction_unconfirmed_table_buckets),
        "Unconfirmed Transaction hash table size, defaults to 10000."
    )
#endif // KTH_DB_TRANSACTION_UNCONFIRMED
    (
        "database.cache_capacity",
        value<uint32_t>(&configured.database.cache_capacity),
        "The maximum number of entries in the unspent outputs cache, defaults to 10000."
    )
    /* [blockchain] */
    (
        "blockchain.cores",
        value<uint32_t>(&configured.chain.cores),
        "The number of cores dedicated to block validation, defaults to 0 (physical cores)."
    )(
        "blockchain.priority",
        value<bool>(&configured.chain.priority),
        "Use high thread priority for block validation, defaults to true."
    )
    // (
    //     "blockchain.use_libconsensus",
    //     value<bool>(&configured.chain.use_libconsensus),
    //     "Use libconsensus for script validation if integrated, defaults to false."
    // )
    (
        "blockchain.reorganization_limit",
        value<uint32_t>(&configured.chain.reorganization_limit),
        "The maximum reorganization depth, defaults to 256 (0 for unlimited)."
    )(
        "blockchain.checkpoint",
        value<infrastructure::config::checkpoint::list>(&configured.chain.checkpoints),
        "A hash:height checkpoint, multiple entries allowed."
    )

    /* [fork] */
    (
        "fork.easy_blocks",
        value<bool>(&configured.chain.easy_blocks),
        "Allow minimum difficulty blocks, defaults to false."
    )(
        "fork.retarget",
        value<bool>(&configured.chain.retarget),
        "Retarget difficulty, defaults to true."
    )(
        "fork.bip16",
        value<bool>(&configured.chain.bip16),
        "Add pay-to-script-hash processing, defaults to true (soft fork)."
    )(
        "fork.bip30",
        value<bool>(&configured.chain.bip30),
        "Disallow collision of unspent transaction hashes, defaults to true (soft fork)."
    )(
        "fork.bip34",
        value<bool>(&configured.chain.bip34),
        "Require coinbase input includes block height, defaults to true (soft fork)."
    )(
        "fork.bip66",
        value<bool>(&configured.chain.bip66),
        "Require strict signature encoding, defaults to true (soft fork)."
    )(
        "fork.bip65",
        value<bool>(&configured.chain.bip65),
        "Add check-locktime-verify op code, defaults to true (soft fork)."
    )(
        "fork.bip90",
        value<bool>(&configured.chain.bip90),
        "Assume bip34, bip65, and bip66 activation if enabled, defaults to true (hard fork)."
    )(
        "fork.bip68",
        value<bool>(&configured.chain.bip68),
        "Add relative locktime enforcement, defaults to true (soft fork)."
    )(
        "fork.bip112",
        value<bool>(&configured.chain.bip112),
        "Add check-sequence-verify op code, defaults to true (soft fork)."
    )(
        "fork.bip113",
        value<bool>(&configured.chain.bip113),
        "Use median time past for locktime, defaults to true (soft fork)."
    )

#if ! defined(KTH_CURRENCY_BCH)
    // BTC only things
    (
        "fork.bip141",
        value<bool>(&configured.chain.bip141),
        "Segregated witness consensus layer, defaults to true (soft fork)."
    )(
        "fork.bip143",
        value<bool>(&configured.chain.bip143),
        "Version 0 transaction digest, defaults to true (soft fork)."
    )(
        "fork.bip147",
        value<bool>(&configured.chain.bip147),
        "Prevent dummy value malleability, defaults to true (soft fork)."
    )
#else
    // BCH only things

    // (
    //     "fork.uahf_height",
    //     value<size_t>(&configured.chain.uahf_height),
    //     "Height of the 2017-Aug-01 hard fork (UAHF), defaults to 478559 (Mainnet)."
    // )
    // (
    //     "fork.daa_height",
    //     value<size_t>(&configured.chain.daa_height),
    //     "Height of the 2017-Nov-13 hard fork (DAA), defaults to 504031 (Mainnet)."
    // )
    // (
    //     "fork.monolith_activation_time",
    //     value<uint64_t>(&configured.chain.monolith_activation_time),
    //     "Unix time used for MTP activation of 2018-May-15 hard fork, defaults to 1526400000."
    // )
    // (
    //     "fork.magnetic_anomaly_activation_time",
    //     value<uint64_t>(&configured.chain.magnetic_anomaly_activation_time),
    //     "Unix time used for MTP activation of 2018-Nov-15 hard fork, defaults to 1542300000."
    // )
    // (
    //     "fork.great_wall_activation_time",
    //     value<uint64_t>(&configured.chain.great_wall_activation_time),
    //     "Unix time used for MTP activation of 2019-May-15 hard fork, defaults to 1557921600."
    // )
    // (
    //     "fork.graviton_activation_time",
    //     value<uint64_t>(&configured.chain.graviton_activation_time),
    //     "Unix time used for MTP activation of 2019-Nov-15 hard fork, defaults to 1573819200."
    // )
    // (
    //     "fork.phonon_activation_time",
    //     value<uint64_t>(&configured.chain.phonon_activation_time),
    //     "Unix time used for MTP activation of 2020-May-15 hard fork, defaults to 1589544000."
    // )
    (
        "fork.axion_activation_time",
        value<uint64_t>(&configured.chain.axion_activation_time),
        "Unix time used for MTP activation of 2020-Nov-15 hard fork, defaults to 1605441600."
    )
    // (
    //     "fork.unnamed_activation_time",
    //     value<uint64_t>(&configured.chain.unnamed_activation_time),
    //     "Unix time used for MTP activation of 2021-May-15 hard fork, defaults to 9999999999."
    // )
    (
        "fork.asert_half_life",
        value<uint64_t>(&configured.chain.asert_half_life),
        "The half life for the ASERTi3-2d DAA. For every (asert_half_life) seconds behind schedule the blockchain gets, difficulty is cut in half. Doubled if blocks are ahead of schedule. Defaults to: 2 * 24 * 60 * 60 = 172800 (two days)."
    )

#endif //KTH_CURRENCY_BCH




    /* [node] */
    ////(
    ////    "node.sync_peers",
    ////    value<uint32_t>(&configured.node.sync_peers),
    ////    "The maximum number of initial block download peers, defaults to 0 (physical cores)."
    ////)
    ////(
    ////    "node.sync_timeout_seconds",
    ////    value<uint32_t>(&configured.node.sync_timeout_seconds),
    ////    "The time limit for block response during initial block download, defaults to 5."
    ////)
    (
        "node.block_latency_seconds",
        value<uint32_t>(&configured.node.block_latency_seconds),
        "The time to wait for a requested block, defaults to 60."
    )(
        /* Internally this is blockchain, but it is conceptually a node setting. */
        "node.notify_limit_hours",
        value<uint32_t>(&configured.chain.notify_limit_hours),
        "Disable relay when top block age exceeds, defaults to 24 (0 disables)."
    )(
        /* Internally this is blockchain, but it is conceptually a node setting. */
        "node.byte_fee_satoshis",
        value<float>(&configured.chain.byte_fee_satoshis),
        "The minimum fee per byte, cumulative for conflicts, defaults to 1."
    )(
        /* Internally this is blockchain, but it is conceptually a node setting. */
        "node.sigop_fee_satoshis",
        value<float>(&configured.chain.sigop_fee_satoshis),
        "The minimum fee per sigop, additional to byte fee, defaults to 100."
    )(
        /* Internally this is blockchain, but it is conceptually a node setting. */
        "node.minimum_output_satoshis",
        value<uint64_t>(&configured.chain.minimum_output_satoshis),
        "The minimum output value, defaults to 500."
    )(
        /* Internally this is network, but it is conceptually a node setting. */
        "node.relay_transactions",
        value<bool>(&configured.network.relay_transactions),
        "Request that peers relay transactions, defaults to true."
    )(
        "node.refresh_transactions",
        value<bool>(&configured.node.refresh_transactions),
        "Request transactions on each channel start, defaults to true."
    )
    // TODO(kth): ver como implementamos esto para diferenciar server y node
    (
        /* Internally this database, but it applies to server.*/
        "node.index_start_height",
        value<uint32_t>(&configured.database.index_start_height),
        "The lower limit of address and spend indexing, defaults to 0."
    )(
        "node.rpc_port",
        value<uint32_t>(&configured.node.rpc_port),
        "TCP port for the HTTP-JSON-RPC connection, default to 8332 (8332 mainnet, 18332 testnet)."
    )(
        "node.zmq_publisher_port",
        value<uint32_t>(&configured.node.subscriber_port),
        "ZMQ publisher port, defaults to 5556."
    )(
        "node.rpc_allow_ip",
        value<std::vector<std::string>>(&configured.node.rpc_allow_ip),
        "RPC allowed ip defaults to 127.0.0.1."
    )(
        "node.rpc_allow_all_ips",
        value<bool>(&configured.node.rpc_allow_all_ips),
        "RPC allowed ip defaults to false."
    )(
        "node.compact_blocks_high_bandwidth",
        value<bool>(&configured.node.compact_blocks_high_bandwidth),
        "Compact Blocks High-Bandwidth mode, default to true."
    )
#ifdef KTH_WITH_KEOKEN
    (
        "node.keoken_genesis_height",
        value<size_t>(&configured.node.keoken_genesis_height),
        "Block height where the first keoken transaction is stored."
    )
#endif

#if defined(KTH_WITH_MEMPOOL)
    (
        "node.mempool_max_template_size",
        value<size_t>(&configured.chain.mempool_max_template_size),
        "Max size of the template block. Default is coin dependant. (BCH: 31,980,000 bytes, BTC and LTC: 3,980,000 bytes)"
    )(
        "node.mempool_size_multiplier",
        value<size_t>(&configured.chain.mempool_size_multiplier),
        "Max mempool size is equal to MaxBlockSize multiplied by mempool_size_multiplier. Default is 10."
    )
#endif
    ;

    return description;
}

bool parser::parse(int argc, const char* argv[], std::ostream& error) {
    try {
        load_error file = load_error::non_existing_file;
        variables_map variables;
        load_command_variables(variables, argc, argv);
        load_environment_variables(variables, BN_ENVIRONMENT_VARIABLE_PREFIX);

        bool version_sett_help = true;
        // Don't load the rest if any of these options are specified.
        if ( ! get_option(variables, BN_VERSION_VARIABLE) &&
             ! get_option(variables, BN_SETTINGS_VARIABLE) &&
             ! get_option(variables, BN_HELP_VARIABLE)) {
            version_sett_help = false;
            // Returns true if the settings were loaded from a file.
            file = load_configuration_variables(variables, BN_CONFIG_VARIABLE);

            if (file == load_error::non_existing_file) {
                LOG_ERROR(LOG_NODE, "Config file provided does not exists.");
                return false;
            }
        }

        // Update bound variables in metadata.settings.
        notify(variables);
        
        if ( ! version_sett_help) {
            fix_checkpoints(configured.chain.easy_blocks, configured.chain.retarget, configured.chain.checkpoints);
        }

        // Clear the config file path if it wasn't used.
        if (file == load_error::default_config) {
            configured.file.clear();
        }
    } catch (const boost::program_options::error& e) {
        // This is obtained from boost, which circumvents our localization.
        error << format_invalid_parameter(e.what()) << std::endl;
        return false;
    }

    return true;
}

bool parser::parse_from_file(std::filesystem::path const& config_path, std::ostream& error) {
    try {
        variables_map variables;

        configured.file = config_path;
        auto file = load_configuration_variables_path(variables, config_path);

        if (file == load_error::non_existing_file) {
            // LOG_ERROR(LOG_NODE, "Config file provided does not exists.");
            error << "Config file provided does not exists." << std::endl;
            return false;
        }

        // Update bound variables in metadata.settings.
        notify(variables);

        fix_checkpoints(configured.chain.easy_blocks, configured.chain.retarget, configured.chain.checkpoints);

        // Clear the config file path if it wasn't used.
        if (file == load_error::default_config) {
            configured.file.clear();
        }

    } catch (boost::program_options::error const& e) {
        // This is obtained from boost, which circumvents our localization.
        error << format_invalid_parameter(e.what()) << std::endl;
        return false;
    }

    return true;
}

} // namespace kth::node