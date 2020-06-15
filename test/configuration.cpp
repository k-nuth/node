// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <test_helpers.hpp>
#include <kth/node.hpp>

using namespace kth;

// Start Boost Suite: configuration tests

// constructors
//-----------------------------------------------------------------------------

TEST_CASE("configuration  construct1  none context  expected", "[configuration tests]") {
    node::configuration instance(infrastructure::config::settings::none);
    REQUIRE(!instance.help);
    REQUIRE(!instance.initchain);
    REQUIRE(!instance.settings);
    REQUIRE(!instance.version);
    REQUIRE(instance.node.sync_peers == 0u);
    REQUIRE(instance.node.sync_timeout_seconds == 5u);
}

TEST_CASE("configuration  construct1  mainnet context  expected", "[configuration tests]") {
    node::configuration instance(infrastructure::config::settings::mainnet);
    REQUIRE(!instance.help);
    REQUIRE(!instance.initchain);
    REQUIRE(!instance.settings);
    REQUIRE(!instance.version);
    REQUIRE(instance.node.sync_peers == 0u);
    REQUIRE(instance.node.sync_timeout_seconds == 5u);
}

TEST_CASE("configuration  construct1  testnet context  expected", "[configuration tests]") {
    node::configuration instance(infrastructure::config::settings::testnet);
    REQUIRE(!instance.help);
    REQUIRE(!instance.initchain);
    REQUIRE(!instance.settings);
    REQUIRE(!instance.version);
    REQUIRE(instance.node.sync_peers == 0u);
    REQUIRE(instance.node.sync_timeout_seconds == 5u);
}

TEST_CASE("configuration  construct2  none context  expected", "[configuration tests]") {
    node::configuration instance1(infrastructure::config::settings::none);
    instance1.help = true;
    instance1.initchain = true;
    instance1.settings = true;
    instance1.version = true;
    instance1.node.sync_peers = 42;
    instance1.node.sync_timeout_seconds = 24;

    node::configuration instance2(instance1);

    BOOST_REQUIRE(instance2.help);
    BOOST_REQUIRE(instance2.initchain);
    BOOST_REQUIRE(instance2.settings);
    BOOST_REQUIRE(instance2.version);
    BOOST_REQUIRE_EQUAL(instance2.node.sync_peers, 42u);
    BOOST_REQUIRE_EQUAL(instance2.node.sync_timeout_seconds, 24u);
}

BOOST_AUTO_TEST_SUITE_END()
