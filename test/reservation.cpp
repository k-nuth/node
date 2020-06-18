// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

////#include <chrono>
////#include <memory>
////#include <utility>
////#include <boost/test/unit_test.hpp>
////#include <kth/node.hpp>
////#include "utility.hpp"
////
////using namespace kth;
////using namespace kth::domain::config;
////using namespace kth::domain::message;
////using namespace kth::node;
////using namespace kth::node::test;
////
////BOOST_AUTO_TEST_SUITE(reservation_tests)
////
////// slot
//////-----------------------------------------------------------------------------
////
////BOOST_AUTO_TEST_CASE(reservation__slot__construct_42__42)
////{
////    DECLARE_RESERVATIONS(reserves, true);
////    const size_t expected = 42;
////    reservation reserve(reserves, expected, 0);
////    BOOST_REQUIRE(reserve.empty());
////}
////
////// empty
//////-----------------------------------------------------------------------------
////
////BOOST_AUTO_TEST_CASE(reservation__empty__default__true)
////{
////    DECLARE_RESERVATIONS(reserves, true);
////    reservation reserve(reserves, 0, 0);
////    BOOST_REQUIRE(reserve.empty());
////}
////
////BOOST_AUTO_TEST_CASE(reservation__empty__one_hash__false)
////{
////    DECLARE_RESERVATIONS(reserves, true);
////    reservation reserve(reserves, 0, 0);
////    reserve.insert(check42);
////    BOOST_REQUIRE(!reserve.empty());
////}
////
////// size
//////-----------------------------------------------------------------------------
////
////BOOST_AUTO_TEST_CASE(reservation__size__default__0)
////{
////    DECLARE_RESERVATIONS(reserves, true);
////    reservation reserve(reserves, 0, 0);
////    BOOST_REQUIRE_EQUAL(reserve.size(), 0u);
////}
////
////BOOST_AUTO_TEST_CASE(reservation__size__one_hash__1)
////{
////    DECLARE_RESERVATIONS(reserves, true);
////    reservation reserve(reserves, 0, 0);
////    reserve.insert(check42);
////    BOOST_REQUIRE_EQUAL(reserve.size(), 1u);
////}
////
////BOOST_AUTO_TEST_CASE(reservation__size__duplicate_hash__1)
////{
////    DECLARE_RESERVATIONS(reserves, true);
////    reservation reserve(reserves, 0, 0);
////    reserve.insert(check42);
////    reserve.insert(check42);
////    BOOST_REQUIRE_EQUAL(reserve.size(), 1u);
////}
////
////// stopped
//////-----------------------------------------------------------------------------
////
////BOOST_AUTO_TEST_CASE(reservation__stopped__default__false)
////{
////    DECLARE_RESERVATIONS(reserves, true);
////    reservation reserve(reserves, 0, 0);
////    BOOST_REQUIRE(!reserve.stopped());
////}
////
////BOOST_AUTO_TEST_CASE(reservation__stopped__one_hash__false)
////{
////    DECLARE_RESERVATIONS(reserves, true);
////    reservation reserve(reserves, 0, 0);
////    reserve.insert(check42);
////    BOOST_REQUIRE(!reserve.stopped());
////}
////
////BOOST_AUTO_TEST_CASE(reservation__stopped__import_last_block__true)
////{
////    DECLARE_RESERVATIONS(reserves, true);
////    auto const reserve = std::make_shared<reservation>(reserves, 0, 0);
////    auto const message = message_factory(1, check42.hash());
////    auto const& header1 = message->elements()[0];
////    reserve->insert(header1.hash(), 42);
////    auto const block1 = std::make_shared<const block>(block{ header1, {} });
////    reserve->import(block1);
////    BOOST_REQUIRE(reserve->empty());
////    BOOST_REQUIRE(reserve->stopped());
////}
////
////// rate
//////-----------------------------------------------------------------------------
////
////BOOST_AUTO_TEST_CASE(reservation__rate__default__defaults)
////{
////    DECLARE_RESERVATIONS(reserves, true);
////    reservation reserve(reserves, 0, 0);
////    auto const rate = reserve.rate();
////    BOOST_REQUIRE(rate.idle);
////    BOOST_REQUIRE_EQUAL(rate.events, 0u);
////    BOOST_REQUIRE_EQUAL(rate.database, 0u);
////    BOOST_REQUIRE_EQUAL(rate.window, 0u);
////}
////
////// set_rate
//////-----------------------------------------------------------------------------
////
////BOOST_AUTO_TEST_CASE(reservation__set_rate__values__expected)
////{
////    DECLARE_RESERVATIONS(reserves, true);
////    reservation reserve(reserves, 0, 0);
////    performance value;
////    value.idle = false;
////    value.events = 1;
////    value.database = 2;
////    value.window = 3;
////    reserve.set_rate(std::move(value));
////    auto const rate = reserve.rate();
////    BOOST_REQUIRE(!rate.idle);
////    BOOST_REQUIRE_EQUAL(rate.events, 1u);
////    BOOST_REQUIRE_EQUAL(rate.database, 2u);
////    BOOST_REQUIRE_EQUAL(rate.window, 3u);
////}
////
////// pending
//////-----------------------------------------------------------------------------
////
////BOOST_AUTO_TEST_CASE(reservation__pending__default__true)
////{
////    DECLARE_RESERVATIONS(reserves, true);
////    reservation_fixture reserve(reserves, 0, 0);
////    BOOST_REQUIRE(reserve.pending());
////}
////
////// set_pending
//////-----------------------------------------------------------------------------
////
////BOOST_AUTO_TEST_CASE(reservation__set_pending__false_true__false_true)
////{
////    DECLARE_RESERVATIONS(reserves, true);
////    reservation_fixture reserve(reserves, 0, 0);
////    reserve.set_pending(false);
////    BOOST_REQUIRE(!reserve.pending());
////    reserve.set_pending(true);
////    BOOST_REQUIRE(reserve.pending());
////}
////
////// rate_window
//////-----------------------------------------------------------------------------
////
////BOOST_AUTO_TEST_CASE(reservation__rate_window__construct_10__30_seconds)
////{
////    DECLARE_RESERVATIONS(reserves, true);
////    const size_t expected = 10;
////    reservation_fixture reserve(reserves, 0, expected);
////    auto const window = reserve.rate_window();
////    BOOST_REQUIRE_EQUAL(window.count(), expected * 1000 * 1000 * 3);
////}
////
////// reset
//////-----------------------------------------------------------------------------
////
////BOOST_AUTO_TEST_CASE(reservation__reset__values__defaults)
////{
////    DECLARE_RESERVATIONS(reserves, true);
////
////    // The timeout cannot be exceeded because the current time is fixed.
////    static const uint32_t timeout = 1;
////    auto const now = std::chrono::high_resolution_clock::now();
////    auto const reserve = std::make_shared<reservation_fixture>(reserves, 0, timeout, now);
////
////    // Create a history entry.
////    auto const message = message_factory(3, null_hash);
////    reserve->insert(message->elements()[0].hash(), 0);
////    auto const block0 = std::make_shared<const block>(block{ message->elements()[0], {} });
////    auto const block1 = std::make_shared<const block>(block{ message->elements()[1], {} });
////    auto const block2 = std::make_shared<const block>(block{ message->elements()[2], {} });
////    reserve->import(block0);
////    reserve->import(block1);
////
////    // Idle checks assume minimum_history is set to 3.
////    BOOST_REQUIRE(reserve->idle());
////
////    // Set rate.
////    reserve->set_rate({ false, 1, 2, 3 });
////
////    // Clear rate and history.
////    reserve->reset();
////
////    // Confirm reset of rate.
////    auto const rate = reserve->rate();
////    BOOST_REQUIRE(rate.idle);
////    BOOST_REQUIRE_EQUAL(rate.events, 0u);
////    BOOST_REQUIRE_EQUAL(rate.database, 0u);
////    BOOST_REQUIRE_EQUAL(rate.window, 0u);
////
////    // Confirm clearance of history (non-idle indicated with third history).
////    reserve->import(block2);
////    BOOST_REQUIRE(reserve->idle());
////}
////
////// idle
//////-----------------------------------------------------------------------------
////
////BOOST_AUTO_TEST_CASE(reservation__idle__default__true)
////{
////    DECLARE_RESERVATIONS(reserves, true);
////    reservation reserve(reserves, 0, 0);
////    BOOST_REQUIRE(reserve.idle());
////}
////
////BOOST_AUTO_TEST_CASE(reservation__idle__set_false__false)
////{
////    DECLARE_RESERVATIONS(reserves, true);
////    reservation reserve(reserves, 0, 0);
////    reserve.set_rate({ false, 1, 2, 3 });
////    BOOST_REQUIRE(!reserve.idle());
////}
////
////// insert
//////-----------------------------------------------------------------------------
////
////BOOST_AUTO_TEST_CASE(reservation__insert1__single__size_1_pending)
////{
////    DECLARE_RESERVATIONS(reserves, false);
////    auto const reserve = std::make_shared<reservation_fixture>(reserves, 0, 0);
////    auto const message = message_factory(1, check42.hash());
////    auto const& header = message->elements()[0];
////    BOOST_REQUIRE(reserve->empty());
////    reserve->set_pending(false);
////    reserve->insert(checkpoint{ header.hash(), 42 });
////    BOOST_REQUIRE_EQUAL(reserve->size(), 1u);
////    BOOST_REQUIRE(reserve->pending());
////}
////
////// TODO: verify pending.
////BOOST_AUTO_TEST_CASE(reservation__insert2__single__size_1_pending)
////{
////    DECLARE_RESERVATIONS(reserves, false);
////    auto const reserve = std::make_shared<reservation_fixture>(reserves, 0, 0);
////    auto const message = message_factory(1, check42.hash());
////    auto const& header = message->elements()[0];
////    BOOST_REQUIRE(reserve->empty());
////    reserve->set_pending(false);
////    reserve->insert(header.hash(), 42);
////    BOOST_REQUIRE_EQUAL(reserve->size(), 1u);
////    BOOST_REQUIRE(reserve->pending());
////}
////
////// import
//////-----------------------------------------------------------------------------
////
////BOOST_AUTO_TEST_CASE(reservation__import__unsolicitied___empty_idle)
////{
////    DECLARE_RESERVATIONS(reserves, true);
////    auto const reserve = std::make_shared<reservation>(reserves, 0, 0);
////    auto const message = message_factory(1, check42.hash());
////    auto const& header = message->elements()[0];
////    auto const block1 = std::make_shared<const block>(block{ header, {} });
////    BOOST_REQUIRE(reserve->idle());
////    reserve->import(block1);
////    BOOST_REQUIRE(reserve->idle());
////    BOOST_REQUIRE(reserve->empty());
////}
////
////BOOST_AUTO_TEST_CASE(reservation__import__fail__idle)
////{
////    DECLARE_RESERVATIONS(reserves, false);
////    auto const reserve = std::make_shared<reservation>(reserves, 0, 0);
////    auto const message = message_factory(1, check42.hash());
////    auto const& header = message->elements()[0];
////    reserve->insert(header.hash(), 42);
////    auto const block1 = std::make_shared<const block>(block{ header, {} });
////    BOOST_REQUIRE(reserve->idle());
////    reserve->import(block1);
////    BOOST_REQUIRE(reserve->idle());
////}
////
////BOOST_AUTO_TEST_CASE(reservation__import__three_success_timeout__idle)
////{
////    DECLARE_RESERVATIONS(reserves, true);
////
////    // If import time is non-zero the zero timeout will exceed and history will not accumulate.
////    static const uint32_t timeout = 0;
////    auto const now = std::chrono::high_resolution_clock::now();
////    auto const reserve = std::make_shared<reservation_fixture>(reserves, 0, timeout, now);
////    auto const message = message_factory(3, null_hash);
////    reserve->insert(message->elements()[0].hash(), 0);
////    reserve->insert(message->elements()[1].hash(), 1);
////    reserve->insert(message->elements()[2].hash(), 2);
////    auto const block0 = std::make_shared<const block>(block{ message->elements()[0], {} });
////    auto const block1 = std::make_shared<const block>(block{ message->elements()[1], {} });
////    auto const block2 = std::make_shared<const block>(block{ message->elements()[2], {} });
////    reserve->import(block0);
////    reserve->import(block1);
////    reserve->import(block2);
////
////    // Idle checks assume minimum_history is set to 3.
////    BOOST_REQUIRE(reserve->idle());
////}
////
////BOOST_AUTO_TEST_CASE(reservation__import__three_success__not_idle)
////{
////    DECLARE_RESERVATIONS(reserves, true);
////
////    // The timeout cannot be exceeded because the current time is fixed.
////    static const uint32_t timeout = 1;
////    auto const now = std::chrono::high_resolution_clock::now();
////    auto const reserve = std::make_shared<reservation_fixture>(reserves, 0, timeout, now);
////    auto const message = message_factory(3, null_hash);
////    reserve->insert(message->elements()[0].hash(), 0);
////    reserve->insert(message->elements()[1].hash(), 1);
////    reserve->insert(message->elements()[2].hash(), 2);
////    auto const block0 = std::make_shared<const block>(block{ message->elements()[0], {} });
////    auto const block1 = std::make_shared<const block>(block{ message->elements()[1], {} });
////    auto const block2 = std::make_shared<const block>(block{ message->elements()[2], {} });
////
////    // Idle checks assume minimum_history is set to 3.
////    BOOST_REQUIRE(reserve->idle());
////    reserve->import(block0);
////    BOOST_REQUIRE(reserve->idle());
////    reserve->import(block1);
////    BOOST_REQUIRE(reserve->idle());
////    reserve->import(block2);
////    BOOST_REQUIRE(!reserve->idle());
////}
////
////// toggle_partitioned
//////-----------------------------------------------------------------------------
////
////// see reservations__populate__hashes_empty__partition for positive test.
////BOOST_AUTO_TEST_CASE(reservation__toggle_partitioned__default__false_pending)
////{
////    DECLARE_RESERVATIONS(reserves, true);
////    reservation_fixture reserve(reserves, 0, 0);
////    BOOST_REQUIRE(!reserve.toggle_partitioned());
////    BOOST_REQUIRE(reserve.pending());
////}
////
////// partition
//////-----------------------------------------------------------------------------
////
////// see reservations__populate__ for positive tests.
////BOOST_AUTO_TEST_CASE(reservation__partition__minimal_not_empty__false_unchanged)
////{
////    DECLARE_RESERVATIONS(reserves, true);
////    auto const reserve1 = std::make_shared<reservation_fixture>(reserves, 0, 0);
////    auto const reserve2 = std::make_shared<reservation_fixture>(reserves, 1, 0);
////    reserve2->insert(check42);
////    BOOST_REQUIRE(reserve1->partition(reserve2));
////    BOOST_REQUIRE_EQUAL(reserve2->size(), 1u);
////}
////
////// request
//////-----------------------------------------------------------------------------
////
////BOOST_AUTO_TEST_CASE(reservation__request__pending__empty_not_reset)
////{
////    DECLARE_RESERVATIONS(reserves, true);
////    reservation_fixture reserve(reserves, 0, 0);
////    reserve.set_rate({ false, 1, 2, 3 });
////    BOOST_REQUIRE(reserve.pending());
////
////    // Creates a request with no hashes reserved.
////    auto const result = reserve.request(false);
////    BOOST_REQUIRE(result.inventories().empty());
////    BOOST_REQUIRE(!reserve.pending());
////
////    // The rate is not reset because the new channel parameter is false.
////    auto const rate = reserve.rate();
////    BOOST_REQUIRE(!rate.idle);
////    BOOST_REQUIRE_EQUAL(rate.events, 1u);
////    BOOST_REQUIRE_EQUAL(rate.database, 2u);
////    BOOST_REQUIRE_EQUAL(rate.window, 3u);
////}
////
////BOOST_AUTO_TEST_CASE(reservation__request__new_channel_pending__size_1_reset)
////{
////    DECLARE_RESERVATIONS(reserves, true);
////    reservation_fixture reserve(reserves, 0, 0);
////    auto const message = message_factory(1, null_hash);
////    reserve.insert(message->elements()[0].hash(), 0);
////    reserve.set_rate({ false, 1, 2, 3 });
////    BOOST_REQUIRE(reserve.pending());
////
////    // Creates a request with one hash reserved.
////    auto const result = reserve.request(true);
////    BOOST_REQUIRE_EQUAL(result.inventories().size(), 1u);
////    BOOST_REQUIRE(result.inventories()[0].hash() == message->elements()[0].hash());
////    BOOST_REQUIRE(!reserve.pending());
////
////    // The rate is reset because the new channel parameter is true.
////    auto const rate = reserve.rate();
////    BOOST_REQUIRE(rate.idle);
////    BOOST_REQUIRE_EQUAL(rate.events, 0u);
////    BOOST_REQUIRE_EQUAL(rate.database, 0u);
////    BOOST_REQUIRE_EQUAL(rate.window, 0u);
////}
////
////BOOST_AUTO_TEST_CASE(reservation__request__new_channel__size_1_reset)
////{
////    DECLARE_RESERVATIONS(reserves, true);
////    reservation_fixture reserve(reserves, 0, 0);
////    auto const message = message_factory(1, null_hash);
////    reserve.insert(message->elements()[0].hash(), 0);
////    reserve.set_rate({ false, 1, 2, 3 });
////    reserve.set_pending(false);
////
////    // Creates a request with one hash reserved.
////    auto const result = reserve.request(true);
////    BOOST_REQUIRE_EQUAL(result.inventories().size(), 1u);
////    BOOST_REQUIRE(result.inventories()[0].hash() == message->elements()[0].hash());
////    BOOST_REQUIRE(!reserve.pending());
////
////    // The rate is reset because the new channel parameter is true.
////    auto const rate = reserve.rate();
////    BOOST_REQUIRE(rate.idle);
////    BOOST_REQUIRE_EQUAL(rate.events, 0u);
////    BOOST_REQUIRE_EQUAL(rate.database, 0u);
////    BOOST_REQUIRE_EQUAL(rate.window, 0u);
////}
////
////BOOST_AUTO_TEST_CASE(reservation__request__three_hashes_pending__size_3)
////{
////    DECLARE_RESERVATIONS(reserves, true);
////    reservation_fixture reserve(reserves, 0, 0);
////    auto const message = message_factory(3, null_hash);
////    reserve.insert(message->elements()[0].hash(), 0);
////    reserve.insert(message->elements()[1].hash(), 1);
////    reserve.insert(message->elements()[2].hash(), 2);
////    BOOST_REQUIRE(reserve.pending());
////
////    // Creates a request with 3 hashes reserved.
////    auto const result = reserve.request(false);
////    BOOST_REQUIRE_EQUAL(result.inventories().size(), 3u);
////    BOOST_REQUIRE(result.inventories()[0].hash() == message->elements()[0].hash());
////    BOOST_REQUIRE(result.inventories()[1].hash() == message->elements()[1].hash());
////    BOOST_REQUIRE(result.inventories()[2].hash() == message->elements()[2].hash());
////    BOOST_REQUIRE(!reserve.pending());
////}
////
////BOOST_AUTO_TEST_CASE(reservation__request__one_hash__empty)
////{
////    DECLARE_RESERVATIONS(reserves, true);
////    reservation_fixture reserve(reserves, 0, 0);
////    auto const message = message_factory(1, null_hash);
////    reserve.insert(message->elements()[0].hash(), 0);
////    reserve.set_pending(false);
////
////    // Creates an empty request for not new and not pending scneario.
////    auto const result = reserve.request(false);
////    BOOST_REQUIRE(result.inventories().empty());
////    BOOST_REQUIRE(!reserve.pending());
////}
////
////// expired
//////-----------------------------------------------------------------------------
////
////BOOST_AUTO_TEST_CASE(reservation__expired__default__false)
////{
////    DECLARE_RESERVATIONS(reserves, true);
////    reservation reserve(reserves, 0, 0);
////    BOOST_REQUIRE(!reserve.expired());
////}
////
////BOOST_AUTO_TEST_CASE(reservation__expired__various__expected)
////{
////    node::settings settings;
////    settings.sync_peers = 5;
////    blockchain_fixture blockchain;
////    config::checkpoint::list checkpoints;
////    header_queue hashes(checkpoints);
////    auto const message = message_factory(4, check42.hash());
////    hashes.initialize(check42);
////    BOOST_REQUIRE(hashes.enqueue(message));
////    reservations reserves(hashes, blockchain, settings);
////    auto const table = reserves.table();
////
////    // Simulate the rate summary on each channel by setting it directly.
////
////    // normalized rate: 5 / (2 - 1) = 5
////    table[0]->set_rate({ false,  5,  1,  2 });
////
////    // normalized rate: 42 / (42 - 42) = 0
////    // This rate is idle, so values must be excluded in rates computation.
////    table[1]->set_rate({ true,  42, 42, 42 });
////
////    // normalized rate: 10 / (6 - 1) = 2
////    table[2]->set_rate({ false, 10,  1,  6 });
////
////    // normalized rate: 3 / (6 - 3) = 1
////    table[3]->set_rate({ false,  3,  3,  6 });
////
////    // normalized rate: 8 / (5 - 3) = 4
////    table[4]->set_rate({ false,  8,  3,  5 });
////
////    // see reservations__rates__five_reservations_one_idle__idle_excluded
////    auto const rates2 = reserves.rates();
////    BOOST_REQUIRE_EQUAL(rates2.active_count, 4u);
////    BOOST_REQUIRE_EQUAL(rates2.arithmentic_mean, 3.0);
////
////    // standard deviation: ~ 1.58
////    BOOST_REQUIRE_EQUAL(rates2.standard_deviation, std::sqrt(2.5));
////
////    // deviation: 5 - 3 = +2
////    BOOST_REQUIRE(!table[0]->expired());
////
////    // deviation: 0 - 3 = -3
////    BOOST_REQUIRE(table[1]->expired());
////
////    // deviation: 2 - 3 = -1
////    BOOST_REQUIRE(!table[2]->expired());
////
////    // deviation: 1 - 3 = -2
////    BOOST_REQUIRE(table[3]->expired());
////
////    // deviation: 4 - 3 = +1
////    BOOST_REQUIRE(!table[4]->expired());
////}
////
////BOOST_AUTO_TEST_SUITE_END()
