// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

////#include <algorithm>
////#include <memory>
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
////BOOST_AUTO_TEST_SUITE(reservations_tests)
////
////// max_request
//////-----------------------------------------------------------------------------
////
////BOOST_AUTO_TEST_CASE(reservations__max_request__default__50000)
////{
////    DECLARE_RESERVATIONS(reserves, true);
////    BOOST_REQUIRE_EQUAL(reserves.max_request(), 50000u);
////}
////
////BOOST_AUTO_TEST_CASE(reservations__set_max_request__42__42)
////{
////    DECLARE_RESERVATIONS(reserves, true);
////    reserves.set_max_request(42);
////    BOOST_REQUIRE_EQUAL(reserves.max_request(), 42u);
////}
////
////// import
//////-----------------------------------------------------------------------------
////
////BOOST_AUTO_TEST_CASE(reservations__import__true__true)
////{
////    auto const block_ptr = std::make_shared<const block>();
////    DECLARE_RESERVATIONS(reserves, true);
////    BOOST_REQUIRE(reserves.import(block_ptr, 42));
////}
////
////BOOST_AUTO_TEST_CASE(reservations__import__false__false)
////{
////    auto const block_ptr = std::make_shared<const block>();
////    DECLARE_RESERVATIONS(reserves, false);
////    BOOST_REQUIRE(!reserves.import(block_ptr, 42));
////}
////
////// table
//////-----------------------------------------------------------------------------
////
////BOOST_AUTO_TEST_CASE(reservations__table__default__empty)
////{
////    DECLARE_RESERVATIONS(reserves, true);
////    BOOST_REQUIRE(reserves.table().empty());
////}
////
////BOOST_AUTO_TEST_CASE(reservations__table__hash_1__size_1_by_1_hashes_empty)
////{
////    node::settings settings;
////    blockchain_fixture blockchain;
////    header_queue hashes(no_checks);
////    hashes.initialize(check42);
////    reservations reserves(hashes, blockchain, settings);
////    auto const table = reserves.table();
////    BOOST_REQUIRE_EQUAL(table.size(), 1u);
////    BOOST_REQUIRE_EQUAL(table[0]->size(), 1u);
////    BOOST_REQUIRE_EQUAL(table[0]->slot(), 0u);
////    BOOST_REQUIRE(hashes.empty());
////}
////
////BOOST_AUTO_TEST_CASE(reservations__table__hash_4__size_4_by_1_hashes_empty)
////{
////    node::settings settings;
////    blockchain_fixture blockchain;
////    header_queue hashes(no_checks);
////    auto const message = message_factory(3, check42.hash());
////    hashes.initialize(check42);
////    BOOST_REQUIRE(hashes.enqueue(message));
////
////    reservations reserves(hashes, blockchain, settings);
////    auto const table = reserves.table();
////    BOOST_REQUIRE_EQUAL(table.size(), 4u);
////    BOOST_REQUIRE_EQUAL(table[0]->size(), 1u);
////    BOOST_REQUIRE_EQUAL(table[1]->size(), 1u);
////    BOOST_REQUIRE_EQUAL(table[2]->size(), 1u);
////    BOOST_REQUIRE_EQUAL(table[3]->size(), 1u);
////    BOOST_REQUIRE_EQUAL(table[0]->slot(), 0u);
////    BOOST_REQUIRE_EQUAL(table[1]->slot(), 1u);
////    BOOST_REQUIRE_EQUAL(table[2]->slot(), 2u);
////    BOOST_REQUIRE_EQUAL(table[3]->slot(), 3u);
////    BOOST_REQUIRE(hashes.empty());
////}
////
////BOOST_AUTO_TEST_CASE(reservations__table__connections_5_hash_46__size_5_by_9_hashes_1)
////{
////    node::settings settings;
////    settings.sync_peers = 5;
////    blockchain_fixture blockchain;
////    header_queue hashes(no_checks);
////    auto const message = message_factory(45, check42.hash());
////    hashes.initialize(check42);
////    BOOST_REQUIRE(hashes.enqueue(message));
////
////    reservations reserves(hashes, blockchain, settings);
////    auto const table = reserves.table();
////    BOOST_REQUIRE_EQUAL(table.size(), 5u);
////    BOOST_REQUIRE_EQUAL(table[0]->size(), 9u);
////    BOOST_REQUIRE_EQUAL(table[1]->size(), 9u);
////    BOOST_REQUIRE_EQUAL(table[2]->size(), 9u);
////    BOOST_REQUIRE_EQUAL(table[3]->size(), 9u);
////    BOOST_REQUIRE_EQUAL(table[4]->size(), 9u);
////    BOOST_REQUIRE_EQUAL(table[0]->slot(), 0u);
////    BOOST_REQUIRE_EQUAL(table[1]->slot(), 1u);
////    BOOST_REQUIRE_EQUAL(table[2]->slot(), 2u);
////    BOOST_REQUIRE_EQUAL(table[3]->slot(), 3u);
////    BOOST_REQUIRE_EQUAL(table[4]->slot(), 4u);
////    BOOST_REQUIRE_EQUAL(hashes.size(), 1u);
////}
////
////BOOST_AUTO_TEST_CASE(reservations__table__hash_42__size_8_by_5_hashes_2)
////{
////    node::settings settings;
////    blockchain_fixture blockchain;
////    header_queue hashes(no_checks);
////    auto const message = message_factory(41, check42.hash());
////    hashes.initialize(check42);
////    BOOST_REQUIRE(hashes.enqueue(message));
////
////    reservations reserves(hashes, blockchain, settings);
////    auto const table = reserves.table();
////    BOOST_REQUIRE_EQUAL(table.size(), 8u);
////    BOOST_REQUIRE_EQUAL(table[0]->size(), 5u);
////    BOOST_REQUIRE_EQUAL(table[1]->size(), 5u);
////    BOOST_REQUIRE_EQUAL(table[2]->size(), 5u);
////    BOOST_REQUIRE_EQUAL(table[3]->size(), 5u);
////    BOOST_REQUIRE_EQUAL(table[4]->size(), 5u);
////    BOOST_REQUIRE_EQUAL(table[5]->size(), 5u);
////    BOOST_REQUIRE_EQUAL(table[6]->size(), 5u);
////    BOOST_REQUIRE_EQUAL(table[7]->size(), 5u);
////    BOOST_REQUIRE_EQUAL(table[0]->slot(), 0u);
////    BOOST_REQUIRE_EQUAL(table[1]->slot(), 1u);
////    BOOST_REQUIRE_EQUAL(table[2]->slot(), 2u);
////    BOOST_REQUIRE_EQUAL(table[3]->slot(), 3u);
////    BOOST_REQUIRE_EQUAL(table[4]->slot(), 4u);
////    BOOST_REQUIRE_EQUAL(table[5]->slot(), 5u);
////    BOOST_REQUIRE_EQUAL(table[6]->slot(), 6u);
////    BOOST_REQUIRE_EQUAL(table[7]->slot(), 7u);
////    BOOST_REQUIRE_EQUAL(hashes.size(), 2u);
////}
////
////// remove
//////-----------------------------------------------------------------------------
////
////BOOST_AUTO_TEST_CASE(reservations__remove__empty__does_not_throw)
////{
////    node::settings settings;
////    blockchain_fixture blockchain;
////    header_queue hashes(no_checks);
////    hashes.initialize(check42);
////    reservations reserves(hashes, blockchain, settings);
////    auto const table = reserves.table();
////    BOOST_REQUIRE_EQUAL(table.size(), 1u);
////
////    auto const row = table[0];
////    reserves.remove(row);
////    BOOST_REQUIRE(reserves.table().empty());
////    BOOST_REQUIRE_NO_THROW(reserves.remove(row));
////}
////
////BOOST_AUTO_TEST_CASE(reservations__remove__hash_4__size_3)
////{
////    node::settings settings;
////    blockchain_fixture blockchain;
////    header_queue hashes(no_checks);
////    auto const message = message_factory(3, check42.hash());
////    hashes.initialize(check42);
////    BOOST_REQUIRE(hashes.enqueue(message));
////
////    reservations reserves(hashes, blockchain, settings);
////    auto const table1 = reserves.table();
////    BOOST_REQUIRE_EQUAL(table1.size(), 4u);
////    BOOST_REQUIRE(hashes.empty());
////
////    auto const row = table1[2];
////    BOOST_REQUIRE_EQUAL(row->slot(), 2u);
////
////    reserves.remove(row);
////    auto const table2 = reserves.table();
////    BOOST_REQUIRE_EQUAL(table2.size(), 3u);
////    BOOST_REQUIRE_EQUAL(table2[0]->slot(), 0u);
////    BOOST_REQUIRE_EQUAL(table2[1]->slot(), 1u);
////    BOOST_REQUIRE_EQUAL(table2[2]->slot(), 3u);
////}
////
////// populate
//////-----------------------------------------------------------------------------
////
////BOOST_AUTO_TEST_CASE(reservations__populate__hashes_not_empty_row_not_empty__no_population)
////{
////    node::settings settings;
////    settings.sync_peers = 3;
////    blockchain_fixture blockchain;
////    header_queue hashes(no_checks);
////    auto const message = message_factory(9, check42.hash());
////    hashes.initialize(check42);
////    BOOST_REQUIRE(hashes.enqueue(message));
////
////    reservations reserves(hashes, blockchain, settings);
////    auto const table = reserves.table();
////    BOOST_REQUIRE_EQUAL(table.size(), 3u);
////
////    // All rows have three hashes.
////    BOOST_REQUIRE_EQUAL(table[0]->size(), 3u);
////    BOOST_REQUIRE_EQUAL(table[1]->size(), 3u);
////    BOOST_REQUIRE_EQUAL(table[2]->size(), 3u);
////
////    // The row is not empty so must not cause a reserve.
////    // The row is not empty so must not cause a repartitioning.
////    BOOST_REQUIRE_EQUAL(hashes.size(), 1u);
////    BOOST_REQUIRE(reserves.populate(table[1]));
////    BOOST_REQUIRE_EQUAL(hashes.size(), 1u);
////
////    // All rows still have three hashes.
////    BOOST_REQUIRE_EQUAL(table[0]->size(), 3u);
////    BOOST_REQUIRE_EQUAL(table[1]->size(), 3u);
////    BOOST_REQUIRE_EQUAL(table[2]->size(), 3u);
////}
////
////BOOST_AUTO_TEST_CASE(reservations__populate__hashes_empty_row_not_empty__no_population)
////{
////    node::settings settings;
////    blockchain_fixture blockchain;
////    header_queue hashes(no_checks);
////    auto const message = message_factory(3, check42.hash());
////    hashes.initialize(check42);
////    BOOST_REQUIRE(hashes.enqueue(message));
////
////    reservations reserves(hashes, blockchain, settings);
////    auto const table = reserves.table();
////    BOOST_REQUIRE_EQUAL(table.size(), 4u);
////
////    // All rows have one hash.
////    BOOST_REQUIRE_EQUAL(table[0]->size(), 1u);
////    BOOST_REQUIRE_EQUAL(table[1]->size(), 1u);
////    BOOST_REQUIRE_EQUAL(table[2]->size(), 1u);
////    BOOST_REQUIRE_EQUAL(table[3]->size(), 1u);
////
////    // There are no hashes in reserve.
////    BOOST_REQUIRE(hashes.empty());
////
////    // The row is not empty so must not cause a repartitioning.
////    BOOST_REQUIRE(reserves.populate(table[0]));
////
////    // Partitions remain unchanged.
////    BOOST_REQUIRE_EQUAL(table[0]->size(), 1u);
////    BOOST_REQUIRE_EQUAL(table[1]->size(), 1u);
////    BOOST_REQUIRE_EQUAL(table[2]->size(), 1u);
////    BOOST_REQUIRE_EQUAL(table[3]->size(), 1u);
////}
////
////BOOST_AUTO_TEST_CASE(reservations__populate__hashes_empty_table_empty__no_population)
////{
////    node::settings settings;
////    settings.sync_peers = 3;
////    blockchain_fixture blockchain;
////    header_queue hashes(no_checks);
////
////    // Initialize with a known header so we can import its block later.
////    auto const message = message_factory(3, null_hash);
////    auto& elements = message->elements();
////    auto const genesis_header = elements[0];
////    hashes.initialize(genesis_header.hash(), 0);
////    elements.erase(std::find(elements.begin(), elements.end(), elements[0]));
////    BOOST_REQUIRE_EQUAL(elements.size(), 2u);
////    BOOST_REQUIRE(hashes.enqueue(message));
////
////    reservations reserves(hashes, blockchain, settings);
////    auto const table = reserves.table();
////    BOOST_REQUIRE_EQUAL(table.size(), 3u);
////
////    // There are no hashes in reserve.
////    BOOST_REQUIRE(hashes.empty());
////
////    // Declare blocks that hash to the allocated headers.
////    // Blocks are evenly distrubuted (every third to each row).
////    auto const block0 = std::make_shared<const block>(block{ genesis_header, {} });
////    auto const block1 = std::make_shared<const block>(block{ elements[0], {} });
////    auto const block2 = std::make_shared<const block>(block{ elements[1], {} });
////
////    // All rows have one hash.
////    BOOST_REQUIRE_EQUAL(table[0]->size(), 1u); // 0
////    BOOST_REQUIRE_EQUAL(table[1]->size(), 1u); // 1
////    BOOST_REQUIRE_EQUAL(table[2]->size(), 1u); // 2
////
////    // Remove all rows from the member table.
////    reserves.remove(table[0]);
////    reserves.remove(table[1]);
////    reserves.remove(table[2]);
////    BOOST_REQUIRE(reserves.table().empty());
////
////    // Removing a block from the first row of the cached table must result in
////    // one less hash in that row and no partitioning of other rows, since they
////    // are no longer accessible from the member table.
////    table[0]->import(block0);
////    BOOST_REQUIRE_EQUAL(table[0]->size(), 0u); //
////    BOOST_REQUIRE_EQUAL(table[1]->size(), 1u); // 1
////    BOOST_REQUIRE_EQUAL(table[2]->size(), 1u); // 2
////}
////
////BOOST_AUTO_TEST_CASE(reservations__populate__hashes_not_empty_row_emptied__uncapped_reserve)
////{
////    node::settings settings;
////    settings.sync_peers = 3;
////    blockchain_fixture blockchain;
////    header_queue hashes(no_checks);
////    auto const message = message_factory(7, check42.hash());
////    hashes.initialize(check42);
////    BOOST_REQUIRE(hashes.enqueue(message));
////
////    reservations reserves(hashes, blockchain, settings);
////    auto const table = reserves.table();
////    BOOST_REQUIRE_EQUAL(table.size(), 3u);
////
////    BOOST_REQUIRE_EQUAL(hashes.size(), 2u);
////    BOOST_REQUIRE_EQUAL(table[0]->size(), 2u);
////    BOOST_REQUIRE_EQUAL(table[1]->size(), 2u);
////
////    auto const block1 = std::make_shared<block>(block{ message->elements()[0], {} });
////    auto const block4 = std::make_shared<block>(block{ message->elements()[3], {} });
////
////    // The import of two blocks from same row will cause populate to invoke reservation.
////    table[1]->import(block1);
////    BOOST_REQUIRE_EQUAL(hashes.size(), 2u);
////    BOOST_REQUIRE_EQUAL(table[0]->size(), 2u);
////    BOOST_REQUIRE_EQUAL(table[1]->size(), 1u);
////
////    // The reserve is reduced from 2 to 0 (unlimited by max_request default of 50000).
////    table[1]->import(block4);
////    BOOST_REQUIRE_EQUAL(hashes.size(), 0u);
////    BOOST_REQUIRE_EQUAL(table[0]->size(), 2u);
////    BOOST_REQUIRE_EQUAL(table[1]->size(), 2u);
////}
////
////BOOST_AUTO_TEST_CASE(reservations__populate__hashes_not_empty_row_emptied__capped_reserve)
////{
////    node::settings settings;
////    settings.sync_peers = 3;
////    blockchain_fixture blockchain;
////    header_queue hashes(no_checks);
////    auto const message = message_factory(7, check42.hash());
////    hashes.initialize(check42);
////    BOOST_REQUIRE(hashes.enqueue(message));
////
////    reservations reserves(hashes, blockchain, settings);
////    auto const table = reserves.table();
////    BOOST_REQUIRE_EQUAL(table.size(), 3u);
////
////    BOOST_REQUIRE_EQUAL(hashes.size(), 2u);
////    BOOST_REQUIRE_EQUAL(table[0]->size(), 2u);
////    BOOST_REQUIRE_EQUAL(table[1]->size(), 2u);
////
////    // Cap reserve at 1 block.
////    reserves.set_max_request(1);
////
////    auto const block1 = std::make_shared<const block>(block{ message->elements()[0], {} });
////    auto const block4 = std::make_shared<const block>(block{ message->elements()[3], {} });
////
////    // The import of two blocks from same row will cause populate to invoke reservation.
////    table[1]->import(block1);
////    BOOST_REQUIRE_EQUAL(hashes.size(), 2u);
////    BOOST_REQUIRE_EQUAL(table[0]->size(), 2u);
////    BOOST_REQUIRE_EQUAL(table[1]->size(), 1u);
////
////    // The reserve is reduced from 2 to 1 (limited by max_request of 1).
////    table[1]->import(block4);
////    BOOST_REQUIRE_EQUAL(hashes.size(), 1u);
////    BOOST_REQUIRE_EQUAL(table[0]->size(), 2u);
////    BOOST_REQUIRE_EQUAL(table[1]->size(), 1u);
////}
////
////BOOST_AUTO_TEST_CASE(reservations__populate__hashes_empty_rows_emptied__partition)
////{
////    node::settings settings;
////    settings.sync_peers = 3;
////    blockchain_fixture blockchain;
////    header_queue hashes(no_checks);
////
////    // Initialize with a known header so we can import its block later.
////    auto const message = message_factory(9, null_hash);
////    auto& elements = message->elements();
////    auto const genesis_header = elements[0];
////    hashes.initialize(genesis_header.hash(), 0);
////    elements.erase(std::find(elements.begin(), elements.end(), elements[0]));
////    BOOST_REQUIRE_EQUAL(elements.size(), 8u);
////    BOOST_REQUIRE(hashes.enqueue(message));
////
////    reservations reserves(hashes, blockchain, settings);
////    auto const table = reserves.table();
////    BOOST_REQUIRE_EQUAL(table.size(), 3u);
////
////    // There are no hashes in reserve.
////    BOOST_REQUIRE(hashes.empty());
////
////    // Declare blocks that hash to the allocated headers.
////    // Blocks are evenly distrubuted (every third to each row).
////    auto const block0 = std::make_shared<const block>(block{ genesis_header, {} });
////    auto const block1 = std::make_shared<const block>(block{ elements[0], {} });
////    auto const block2 = std::make_shared<const block>(block{ elements[1], {} });
////    auto const block3 = std::make_shared<const block>(block{ elements[2], {} });
////    auto const block4 = std::make_shared<const block>(block{ elements[3], {} });
////    auto const block5 = std::make_shared<const block>(block{ elements[4], {} });
////    auto const block6 = std::make_shared<const block>(block{ elements[5], {} });
////    auto const block7 = std::make_shared<const block>(block{ elements[6], {} });
////    auto const block8 = std::make_shared<const block>(block{ elements[7], {} });
////
////    // This will reset pending on all rows.
////    BOOST_REQUIRE_EQUAL(table[0]->request(false).inventories().size(), 3u);
////    BOOST_REQUIRE_EQUAL(table[1]->request(false).inventories().size(), 3u);
////    BOOST_REQUIRE_EQUAL(table[2]->request(false).inventories().size(), 3u);
////
////    // A row becomes stopped once empty.
////    BOOST_REQUIRE(!table[0]->stopped());
////    BOOST_REQUIRE(!table[1]->stopped());
////    BOOST_REQUIRE(!table[2]->stopped());
////
////    // All rows have three hashes.
////    BOOST_REQUIRE_EQUAL(table[0]->size(), 3u); // 0/3/6
////    BOOST_REQUIRE_EQUAL(table[1]->size(), 3u); // 1/4/7
////    BOOST_REQUIRE_EQUAL(table[2]->size(), 3u); // 2/5/8
////
////    // Remove a block from the first row.
////    table[0]->import(block0);
////    BOOST_REQUIRE_EQUAL(table[0]->size(), 2u); // 3/6
////    BOOST_REQUIRE_EQUAL(table[1]->size(), 3u); // 1/4/7
////    BOOST_REQUIRE_EQUAL(table[2]->size(), 3u); // 2/5/8
////
////    // Remove another block from the first row.
////    table[0]->import(block3);
////    BOOST_REQUIRE_EQUAL(table[0]->size(), 1u); // 6
////    BOOST_REQUIRE_EQUAL(table[1]->size(), 3u); // 1/4/7
////    BOOST_REQUIRE_EQUAL(table[2]->size(), 3u); // 2/5/8
////
////    // Removing the last block from the first row results in partitioning of
////    // of the highst row (row 1 winds the tie with row 2 due to ordering).
////    // Half of the row 1 allocation is moved to row 0, rounded up to 2 hashes.
////    table[0]->import(block6);
////    BOOST_REQUIRE_EQUAL(table[0]->size(), 2u); // 1/4
////    BOOST_REQUIRE_EQUAL(table[1]->size(), 1u); // 7
////    BOOST_REQUIRE_EQUAL(table[2]->size(), 3u); // 2/5/8
////    BOOST_REQUIRE(table[1]->toggle_partitioned());
////    BOOST_REQUIRE(!table[1]->toggle_partitioned());
////
////    // The last row has not been modified.
////    BOOST_REQUIRE_EQUAL(table[0]->request(false).inventories().size(), 2u);
////    BOOST_REQUIRE_EQUAL(table[1]->request(false).inventories().size(), 1u);
////    BOOST_REQUIRE_EQUAL(table[2]->request(false).inventories().size(), 0u);
////
////    // The rows are no longer pending.
////    BOOST_REQUIRE(table[0]->request(false).inventories().empty());
////    BOOST_REQUIRE(table[1]->request(false).inventories().empty());
////    BOOST_REQUIRE(table[2]->request(false).inventories().empty());
////
////    // Remove another block from the first row (originally from the second).
////    table[0]->import(block1);
////    BOOST_REQUIRE_EQUAL(table[0]->size(), 1u); // 4
////    BOOST_REQUIRE_EQUAL(table[1]->size(), 1u); // 7
////    BOOST_REQUIRE_EQUAL(table[2]->size(), 3u); // 2/5/8
////
////    // Remove another block from the first row (originally from the second).
////    table[0]->import(block4);
////    BOOST_REQUIRE_EQUAL(table[0]->size(), 2u); // 2/5
////    BOOST_REQUIRE_EQUAL(table[1]->size(), 1u); // 7
////    BOOST_REQUIRE_EQUAL(table[2]->size(), 1u); // 8
////    BOOST_REQUIRE(table[2]->toggle_partitioned());
////    BOOST_REQUIRE(!table[2]->toggle_partitioned());
////
////    // The second row has not been modified.
////    BOOST_REQUIRE_EQUAL(table[0]->request(false).inventories().size(), 2u);
////    BOOST_REQUIRE_EQUAL(table[1]->request(false).inventories().size(), 0u);
////    BOOST_REQUIRE_EQUAL(table[2]->request(false).inventories().size(), 1u);
////
////    // The rows are no longer pending.
////    BOOST_REQUIRE(table[0]->request(false).inventories().empty());
////    BOOST_REQUIRE(table[1]->request(false).inventories().empty());
////    BOOST_REQUIRE(table[2]->request(false).inventories().empty());
////
////    // Remove another block from the first row (originally from the third).
////    table[0]->import(block2);
////    BOOST_REQUIRE_EQUAL(table[0]->size(), 1u); // 5
////    BOOST_REQUIRE_EQUAL(table[1]->size(), 1u); // 7
////    BOOST_REQUIRE_EQUAL(table[2]->size(), 1u); // 8
////
////    // Remove another block from the first row (originally from the third).
////    table[0]->import(block5);
////    BOOST_REQUIRE_EQUAL(table[0]->size(), 1u); // 7
////    BOOST_REQUIRE_EQUAL(table[1]->size(), 0u); //
////    BOOST_REQUIRE_EQUAL(table[2]->size(), 1u); // 8
////    BOOST_REQUIRE(table[1]->stopped());
////    BOOST_REQUIRE(!table[1]->toggle_partitioned());
////
////    // The third row has not been modified and the second row is empty.
////    BOOST_REQUIRE_EQUAL(table[0]->request(false).inventories().size(), 1u);
////    BOOST_REQUIRE_EQUAL(table[1]->request(false).inventories().size(), 0u);
////    BOOST_REQUIRE_EQUAL(table[2]->request(false).inventories().size(), 0u);
////
////    // The rows are no longer pending.
////    BOOST_REQUIRE(table[0]->request(false).inventories().empty());
////    BOOST_REQUIRE(table[1]->request(false).inventories().empty());
////    BOOST_REQUIRE(table[2]->request(false).inventories().empty());
////
////    // Remove another block from the first row (originally from the second).
////    table[0]->import(block7);
////    BOOST_REQUIRE_EQUAL(table[0]->size(), 1u); // 8
////    BOOST_REQUIRE_EQUAL(table[1]->size(), 0u); //
////    BOOST_REQUIRE_EQUAL(table[2]->size(), 0u); //
////    BOOST_REQUIRE(table[2]->stopped());
////    BOOST_REQUIRE(!table[2]->toggle_partitioned());
////
////    // The second row has not been modified and the third row is empty.
////    BOOST_REQUIRE_EQUAL(table[0]->request(false).inventories().size(), 1u);
////    BOOST_REQUIRE_EQUAL(table[1]->request(false).inventories().size(), 0u);
////    BOOST_REQUIRE_EQUAL(table[2]->request(false).inventories().size(), 0u);
////
////    // The rows are no longer pending.
////    BOOST_REQUIRE(table[0]->request(false).inventories().empty());
////    BOOST_REQUIRE(table[1]->request(false).inventories().empty());
////    BOOST_REQUIRE(table[2]->request(false).inventories().empty());
////
////    // Remove another block from the first row (originally from the third).
////    table[0]->import(block8);
////    BOOST_REQUIRE_EQUAL(table[0]->size(), 0u); //
////    BOOST_REQUIRE_EQUAL(table[1]->size(), 0u); //
////    BOOST_REQUIRE_EQUAL(table[2]->size(), 0u); //
////    BOOST_REQUIRE(table[0]->stopped());
////
////    // The second and third rows have not been modified and the first is empty.
////    BOOST_REQUIRE_EQUAL(table[0]->request(false).inventories().size(), 0u);
////    BOOST_REQUIRE_EQUAL(table[1]->request(false).inventories().size(), 0u);
////    BOOST_REQUIRE_EQUAL(table[2]->request(false).inventories().size(), 0u);
////
////    // The rows are no longer pending.
////    BOOST_REQUIRE(table[0]->request(false).inventories().empty());
////    BOOST_REQUIRE(table[1]->request(false).inventories().empty());
////    BOOST_REQUIRE(table[2]->request(false).inventories().empty());
////
////    // We can't test the partition aspect of population directly
////    // because there is no way to reduce the row count to empty.
////    ////BOOST_REQUIRE(reserves.populate(table[0]));
////}
////
////// rates
//////-----------------------------------------------------------------------------
////
////BOOST_AUTO_TEST_CASE(reservations__rates__default__zeros)
////{
////    DECLARE_RESERVATIONS(reserves, true);
////    auto const rates = reserves.rates();
////    BOOST_REQUIRE_EQUAL(rates.active_count, 0u);
////    BOOST_REQUIRE_EQUAL(rates.arithmentic_mean, 0.0);
////    BOOST_REQUIRE_EQUAL(rates.standard_deviation, 0.0);
////}
////
////BOOST_AUTO_TEST_CASE(reservations__rates__three_reservations_same_rates__no_deviation)
////{
////    node::settings settings;
////    settings.sync_peers = 3;
////    blockchain_fixture blockchain;
////    header_queue hashes(no_checks);
////    auto const message = message_factory(2, check42.hash());
////    hashes.initialize(check42);
////    BOOST_REQUIRE(hashes.enqueue(message));
////
////    reservations reserves(hashes, blockchain, settings);
////    auto const table = reserves.table();
////    BOOST_REQUIRE_EQUAL(table.size(), 3u);
////
////    auto const rates1 = reserves.rates();
////    BOOST_REQUIRE_EQUAL(rates1.active_count, 0u);
////    BOOST_REQUIRE_EQUAL(rates1.arithmentic_mean, 0.0);
////    BOOST_REQUIRE_EQUAL(rates1.standard_deviation, 0.0);
////
////    // normalized rates: 5 / (2 - 1) = 5
////    performance rate0;
////    rate0.idle = false;
////    rate0.events = 5;
////    rate0.database = 1;
////    rate0.window = 2;
////    performance rate1 = rate0;
////    performance rate2 = rate0;
////
////    // Simulate the rate summary on each channel by setting it directly.
////    table[0]->set_rate(std::move(rate0));
////    table[1]->set_rate(std::move(rate1));
////    table[2]->set_rate(std::move(rate2));
////
////    auto const rates2 = reserves.rates();
////
////    // There are three active (non-idle) rows.
////    BOOST_REQUIRE_EQUAL(rates2.active_count, 3u);
////
////    // mean: (5 + 5 + 5) / 3 = 5
////    BOOST_REQUIRE_EQUAL(rates2.arithmentic_mean, 5.0);
////
////    // deviations: { 5-5=0, 5-5=0, 5-5=0 }
////    // variance: (0^2 + 0^2 + 0^2) / 3 = 0
////    // standard deviation: sqrt(0)
////    BOOST_REQUIRE_EQUAL(rates2.standard_deviation, 0.0);
////}
////
////BOOST_AUTO_TEST_CASE(reservations__rates__five_reservations_one_idle__idle_excluded)
////{
////    node::settings settings;
////    settings.sync_peers = 5;
////    blockchain_fixture blockchain;
////    header_queue hashes(no_checks);
////    auto const message = message_factory(4, check42.hash());
////    hashes.initialize(check42);
////    BOOST_REQUIRE(hashes.enqueue(message));
////
////    reservations reserves(hashes, blockchain, settings);
////    auto const table = reserves.table();
////
////    // normalized rate: 5 / (2 - 1) = 5
////    performance rate0;
////    rate0.idle = false;
////    rate0.events = 5;
////    rate0.database = 1;
////    rate0.window = 2;
////
////    // This rate is idle, so values must be excluded in rates computation.
////    performance rate1;
////    rate1.idle = true;
////    rate1.events = 42;
////    rate1.database = 42;
////    rate1.window = 42;
////
////    // normalized rate: 10 / (6 - 1) = 2
////    performance rate2;
////    rate2.idle = false;
////    rate2.events = 10;
////    rate2.database = 1;
////    rate2.window = 6;
////
////    // normalized rate: 3 / (6 - 3) = 1
////    performance rate3;
////    rate3.idle = false;
////    rate3.events = 3;
////    rate3.database = 3;
////    rate3.window = 6;
////
////    // normalized rate: 8 / (5 - 3) = 4
////    performance rate4;
////    rate4.idle = false;
////    rate4.events = 8;
////    rate4.database = 3;
////    rate4.window = 5;
////
////    // Simulate the rate summary on each channel by setting it directly.
////    table[0]->set_rate(std::move(rate0));
////    table[1]->set_rate(std::move(rate1));
////    table[2]->set_rate(std::move(rate2));
////    table[3]->set_rate(std::move(rate3));
////    table[4]->set_rate(std::move(rate4));
////
////    auto const rates2 = reserves.rates();
////
////    // There are three active (non-idle) rows.
////    BOOST_REQUIRE_EQUAL(rates2.active_count, 4u);
////
////    // mean: (5 + 2 + 1 + 4) / 4 = 3
////    BOOST_REQUIRE_EQUAL(rates2.arithmentic_mean, 3.0);
////
////    // deviations: { 3-5=-2, 3-2=1, 3-1=-2, 3-4=-1 }
////    // variance: ((-2)^2 + 1^2 + 2^2 + (-1)^2) / 4 = 2.5
////    // standard deviation: sqrt(2.5)
////    BOOST_REQUIRE_EQUAL(rates2.standard_deviation, std::sqrt(2.5));
////}
////
////BOOST_AUTO_TEST_SUITE_END()
