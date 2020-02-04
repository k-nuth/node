// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/test/unit_test.hpp>
#include <bitcoin/node.hpp>

using namespace bc;

BOOST_AUTO_TEST_SUITE(configuration_tests)

// constructors
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(configuration__construct1__none_context__expected)
{
    node::configuration instance(config::settings::none);
    BOOST_REQUIRE(!instance.help);
    BOOST_REQUIRE(!instance.initchain);
    BOOST_REQUIRE(!instance.settings);
    BOOST_REQUIRE(!instance.version);
    BOOST_REQUIRE_EQUAL(instance.node.sync_peers, 0u);
    BOOST_REQUIRE_EQUAL(instance.node.sync_timeout_seconds, 5u);
}

BOOST_AUTO_TEST_CASE(configuration__construct1__mainnet_context__expected)
{
    node::configuration instance(config::settings::mainnet);
    BOOST_REQUIRE(!instance.help);
    BOOST_REQUIRE(!instance.initchain);
    BOOST_REQUIRE(!instance.settings);
    BOOST_REQUIRE(!instance.version);
    BOOST_REQUIRE_EQUAL(instance.node.sync_peers, 0u);
    BOOST_REQUIRE_EQUAL(instance.node.sync_timeout_seconds, 5u);
}

BOOST_AUTO_TEST_CASE(configuration__construct1__testnet_context__expected)
{
    node::configuration instance(config::settings::testnet);
    BOOST_REQUIRE(!instance.help);
    BOOST_REQUIRE(!instance.initchain);
    BOOST_REQUIRE(!instance.settings);
    BOOST_REQUIRE(!instance.version);
    BOOST_REQUIRE_EQUAL(instance.node.sync_peers, 0u);
    BOOST_REQUIRE_EQUAL(instance.node.sync_timeout_seconds, 5u);
}

BOOST_AUTO_TEST_CASE(configuration__construct2__none_context__expected)
{
    node::configuration instance1(config::settings::none);
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
