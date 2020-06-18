// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/test/unit_test.hpp>
#include <kth/node.hpp>

using namespace kth;
using namespace kth::node;

BOOST_AUTO_TEST_SUITE(performance_tests)

// normal
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(performance__normal__21_over_42__0_point_5)
{
    performance instance;
    instance.idle = true;
    instance.events = 21;
    instance.window = 84;
    instance.database = 42;
    BOOST_REQUIRE_EQUAL(instance.normal(), 0.5);
}

BOOST_AUTO_TEST_CASE(performance__normal__1_over_negative_1__negative_1_point_0)
{
    performance instance;
    instance.events = 1;
    instance.window = 0;
    instance.database = 1;
    BOOST_REQUIRE_EQUAL(instance.normal(), -1.0);
}

BOOST_AUTO_TEST_CASE(performance__normal__0_over_0__0_point_0)
{
    performance instance;
    instance.events = 0;
    instance.window = 1;
    instance.database = 1;
    BOOST_REQUIRE_EQUAL(instance.normal(), 0.0);
}

BOOST_AUTO_TEST_CASE(performance__normal__0_over_1__0_point_0)
{
    performance instance;
    instance.events = 0;
    instance.window = 1;
    instance.database = 0;
    BOOST_REQUIRE_EQUAL(instance.normal(), 0.0);
}

BOOST_AUTO_TEST_CASE(performance__normal__1_over_0__0_point_0)
{
    performance instance;
    instance.events = 1;
    instance.window = 2;
    instance.database = 2;
    BOOST_REQUIRE_EQUAL(instance.normal(), 0.0);
}

BOOST_AUTO_TEST_CASE(performance__normal__1_over_1__1_point_0)
{
    performance instance;
    instance.events = 1;
    instance.window = 2;
    instance.database = 1;
    BOOST_REQUIRE_EQUAL(instance.normal(), 1.0);
}

BOOST_AUTO_TEST_CASE(performance__normal__2_over_1__2_point_0)
{
    performance instance;
    instance.events = 2;
    instance.window = 2;
    instance.database = 1;
    BOOST_REQUIRE_EQUAL(instance.normal(), 2.0);
}

BOOST_AUTO_TEST_CASE(performance__normal__1_over_2__0_point_5)
{
    performance instance;
    instance.events = 1;
    instance.window = 4;
    instance.database = 2;
    BOOST_REQUIRE_EQUAL(instance.normal(), 0.5);
}

// ratio
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(performance__ratio__21_over_42__0_point_5)
{
    performance instance;
    instance.idle = true;
    instance.events = 1;
    instance.database = 21;
    instance.window = 42;
    BOOST_REQUIRE_EQUAL(instance.ratio(), 0.5);
}

BOOST_AUTO_TEST_CASE(performance__ratio__0_over_0__0_point_0)
{
    performance instance;
    instance.database = 0;
    instance.window = 0;
    BOOST_REQUIRE_EQUAL(instance.ratio(), 0.0);
}

BOOST_AUTO_TEST_CASE(performance__ratio__0_over_1__0_point_0)
{
    performance instance;
    instance.database = 0;
    instance.window = 1;
    BOOST_REQUIRE_EQUAL(instance.ratio(), 0.0);
}

BOOST_AUTO_TEST_CASE(performance__ratio__1_over_0__0_point_0)
{
    performance instance;
    instance.database = 1;
    instance.window = 0;
    BOOST_REQUIRE_EQUAL(instance.ratio(), 0.0);
}

BOOST_AUTO_TEST_CASE(performance__ratio__1_over_1__1_point_0)
{
    performance instance;
    instance.database = 1;
    instance.window = 1;
    BOOST_REQUIRE_EQUAL(instance.ratio(), 1.0);
}

BOOST_AUTO_TEST_CASE(performance__ratio__2_over_1__2_point_0)
{
    performance instance;
    instance.database = 2;
    instance.window = 1;
    BOOST_REQUIRE_EQUAL(instance.ratio(), 2.0);
}

BOOST_AUTO_TEST_CASE(performance__ratio__1_over_2__0_point_5)
{
    performance instance;
    instance.database = 1;
    instance.window = 2;
    BOOST_REQUIRE_EQUAL(instance.ratio(), 0.5);
}

// total
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(performance__total__21_over_42__0_point_5)
{
    performance instance;
    instance.idle = true;
    instance.database = 1;
    instance.events = 21;
    instance.window = 42;
    BOOST_REQUIRE_EQUAL(instance.total(), 0.5);
}

BOOST_AUTO_TEST_CASE(performance__total__0_over_0__0_point_0)
{
    performance instance;
    instance.events = 0;
    instance.window = 0;
    BOOST_REQUIRE_EQUAL(instance.total(), 0.0);
}

BOOST_AUTO_TEST_CASE(performance__total__0_over_1__0_point_0)
{
    performance instance;
    instance.events = 0;
    instance.window = 1;
    BOOST_REQUIRE_EQUAL(instance.total(), 0.0);
}

BOOST_AUTO_TEST_CASE(performance__total__1_over_0__0_point_0)
{
    performance instance;
    instance.events = 1;
    instance.window = 0;
    BOOST_REQUIRE_EQUAL(instance.total(), 0.0);
}

BOOST_AUTO_TEST_CASE(performance__total__1_over_1__1_point_0)
{
    performance instance;
    instance.events = 1;
    instance.window = 1;
    BOOST_REQUIRE_EQUAL(instance.total(), 1.0);
}

BOOST_AUTO_TEST_CASE(performance__total__2_over_1__2_point_0)
{
    performance instance;
    instance.events = 2;
    instance.window = 1;
    BOOST_REQUIRE_EQUAL(instance.total(), 2.0);
}

BOOST_AUTO_TEST_CASE(performance__total__1_over_2__0_point_5)
{
    performance instance;
    instance.events = 1;
    instance.window = 2;
    BOOST_REQUIRE_EQUAL(instance.total(), 0.5);
}

BOOST_AUTO_TEST_SUITE_END()
