// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/test/unit_test.hpp>
#include <kth/node.hpp>

using namespace bc;

BOOST_AUTO_TEST_SUITE(settings_tests)

// constructors
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(settings__construct__default_context__expected)
{
    node::settings configuration;
    BOOST_REQUIRE_EQUAL(configuration.sync_peers, 0u);
    BOOST_REQUIRE_EQUAL(configuration.sync_timeout_seconds, 5u);
    BOOST_REQUIRE_EQUAL(configuration.refresh_transactions, true);
}

BOOST_AUTO_TEST_CASE(settings__construct__none_context__expected)
{
    node::settings configuration(config::settings::none);
    BOOST_REQUIRE_EQUAL(configuration.sync_peers, 0u);
    BOOST_REQUIRE_EQUAL(configuration.sync_timeout_seconds, 5u);
    BOOST_REQUIRE_EQUAL(configuration.refresh_transactions, true);
}

BOOST_AUTO_TEST_CASE(settings__construct__mainnet_context__expected)
{
    node::settings configuration(config::settings::mainnet);
    BOOST_REQUIRE_EQUAL(configuration.sync_peers, 0u);
    BOOST_REQUIRE_EQUAL(configuration.sync_timeout_seconds, 5u);
    BOOST_REQUIRE_EQUAL(configuration.refresh_transactions, true);
}

BOOST_AUTO_TEST_CASE(settings__construct__testnet_context__expected)
{
    node::settings configuration(config::settings::testnet);
    BOOST_REQUIRE_EQUAL(configuration.sync_peers, 0u);
    BOOST_REQUIRE_EQUAL(configuration.sync_timeout_seconds, 5u);
    BOOST_REQUIRE_EQUAL(configuration.refresh_transactions, true);
}

BOOST_AUTO_TEST_SUITE_END()
