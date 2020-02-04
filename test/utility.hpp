// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_NODE_TEST_RESERVATIONS_HPP
#define KTH_NODE_TEST_RESERVATIONS_HPP

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <kth/node.hpp>

namespace kth {
namespace node {
namespace test {

#define DECLARE_RESERVATIONS(name, import) \
config::checkpoint::list checkpoints; \
header_queue hashes(checkpoints); \
blockchain_fixture blockchain(import); \
node::settings config; \
reservations name(hashes, blockchain, config)

extern const config::checkpoint check0;
extern const config::checkpoint check42;
extern const config::checkpoint::list no_checks;
extern const config::checkpoint::list one_check;

// Create a headers message of specified size, using specified previous hash.
extern message::headers::ptr message_factory(size_t count);
extern message::headers::ptr message_factory(size_t count,
    const hash_digest& previous);

class reservation_fixture
  : public reservation
{
public:
    typedef std::chrono::high_resolution_clock clock;
    reservation_fixture(reservations& reservations, size_t slot,
        uint32_t sync_timeout_seconds, clock::time_point now = clock::now());
    std::chrono::microseconds rate_window() const;
    clock::time_point now() const override;
    bool pending() const;
    void set_pending(bool value);

private:
    clock::time_point now_;
};

class blockchain_fixture
  : public blockchain::fast_chain
{
public:
    blockchain_fixture(bool import_result=true, size_t gap_trigger=max_size_t,
        size_t gap_height=max_size_t);

    // Getters.
    // ------------------------------------------------------------------------
    bool get_gap_range(size_t& out_first, size_t& out_last) const;
    bool get_next_gap(size_t& out_height, size_t start_height) const;
    bool get_block_exists(const hash_digest& block_hash) const;
    bool get_fork_work(uint256_t& out_difficulty, size_t height) const;
    bool get_header(chain::header& out_header, size_t height) const;
    bool get_height(size_t& out_height, const hash_digest& block_hash) const;
    bool get_bits(uint32_t& out_bits, const size_t& height) const;
    bool get_timestamp(uint32_t& out_timestamp, const size_t& height) const;
    bool get_version(uint32_t& out_version, const size_t& height) const;
    bool get_last_height(size_t& out_height) const;
    bool get_output(chain::output& out_output, size_t& out_height,
        size_t& out_position, const chain::output_point& outpoint) const;
    bool get_spender_hash(hash_digest& out_hash,
        const chain::output_point& outpoint) const;
    bool get_is_unspent_transaction(const hash_digest& transaction_hash) const;
    bool get_transaction_height(size_t& out_block_height,
        const hash_digest& transaction_hash) const;
    transaction_ptr get_transaction(size_t& out_block_height,
        const hash_digest& transaction_hash) const;

    // Setters.
    // ------------------------------------------------------------------------
    bool stub(header_const_ptr header, size_t height);
    bool fill(block_const_ptr block, size_t height);
    bool push(const block_const_ptr_list& blocks, size_t height);
    bool pop(block_const_ptr_list& out_blocks, const hash_digest& fork_hash);

private:
    bool import_result_;
    size_t gap_trigger_;
    size_t gap_height_;
};

} // namespace test
} // namespace node
} // namespace kth

#endif
