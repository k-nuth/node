/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/node/protocols/protocol_block_in.hpp>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <functional>
#include <iomanip>
#include <memory>
#include <string>
#include <boost/format.hpp>
#include <bitcoin/blockchain.hpp>
#include <bitcoin/network.hpp>
#include <bitcoin/node/define.hpp>
#include <bitcoin/node/full_node.hpp>

namespace libbitcoin {
namespace node {

#define NAME "block_in"
#define CLASS protocol_block_in

using namespace bc::blockchain;
using namespace bc::message;
using namespace bc::network;
using namespace std::chrono;
using namespace std::placeholders;

inline bool is_witness(uint64_t services)
{
#ifdef BITPRIM_CURRENCY_BCH
    return false;
#else
    return (services & version::service::node_witness) != 0;
#endif
}

protocol_block_in::protocol_block_in(full_node& node, channel::ptr channel,
    safe_chain& chain)
  : protocol_timer(node, channel, false, NAME),
    node_(node),
    chain_(chain),
    block_latency_(node.node_settings().block_latency()),

    // TODO: move send_headers to a derived class protocol_block_in_70012.
    headers_from_peer_(negotiated_version() >= version::level::bip130),

    // TODO: move send_compact to a derived class protocol_block_in_70014.
    compact_from_peer_(negotiated_version() >= version::level::bip152),
    
    compact_blocks_high_bandwidth_set_(false),

    // This patch is treated as integral to basic block handling.
    blocks_from_peer_(
        negotiated_version() > version::level::no_blocks_end ||
        negotiated_version() < version::level::no_blocks_start),

    // Witness must be requested if possibly enforced.
    require_witness_(is_witness(node.network_settings().services)),
    peer_witness_(is_witness(channel->peer_version()->services())),
    CONSTRUCT_TRACK(protocol_block_in)
{
}

// Start.
//-----------------------------------------------------------------------------

void protocol_block_in::start()
{
    // Use timer to drop slow peers.
    protocol_timer::start(block_latency_, BIND1(handle_timeout, _1));

    // Do not process incoming blocks if required witness is unavailable.
    // The channel will remain active outbound unless node becomes stale.
    if (require_witness_ && !peer_witness_)
        return;

    // TODO: move headers to a derived class protocol_block_in_31800.
    SUBSCRIBE2(headers, handle_receive_headers, _1, _2);

    // TODO: move not_found to a derived class protocol_block_in_70001.
    SUBSCRIBE2(not_found, handle_receive_not_found, _1, _2);
    SUBSCRIBE2(inventory, handle_receive_inventory, _1, _2);
    SUBSCRIBE2(block, handle_receive_block, _1, _2);

    SUBSCRIBE2(compact_block, handle_receive_compact_block, _1, _2);
    SUBSCRIBE2(block_transactions, handle_receive_block_transactions, _1, _2);

    // TODO: move send_headers to a derived class protocol_block_in_70012.
    if (headers_from_peer_)
    {
        // Ask peer to send headers vs. inventory block announcements.
        SEND2(send_headers{}, handle_send, _1, send_headers::command);
    }

    // TODO: move send_compact to a derived class protocol_block_in_70014.
    if (compact_from_peer_)
    {
#ifdef BITPRIM_CURRENCY_BCH
        uint64_t compact_version = 1;
#else
        uint64_t compact_version = 2;
#endif

        if (chain_.is_stale()) {
        
            //forze low bandwidth    
            LOG_INFO(LOG_NODE) << "The chain is stale, send sendcmcpt low bandwidth ["<< authority() << "]";
            SEND2((send_compact{false, compact_version}), handle_send, _1, send_compact::command);
        }
        else {
            
            LOG_INFO(LOG_NODE) << "The chain is not stale, send sendcmcpt with configured setting ["<< authority() << "]";
            SEND2((send_compact{node_.node_settings().compact_blocks_high_bandwidth, compact_version}), handle_send, _1, send_compact::command);
            compact_blocks_high_bandwidth_set_ = node_.node_settings().compact_blocks_high_bandwidth;
        } 
    }

    send_get_blocks(null_hash);
}

// Send get_[headers|blocks] sequence.
//-----------------------------------------------------------------------------

void protocol_block_in::send_get_blocks(const hash_digest& stop_hash)
{
    const auto heights = block::locator_heights(node_.top_block().height());

    chain_.fetch_block_locator(heights,
        BIND3(handle_fetch_block_locator, _1, _2, stop_hash));
}

void protocol_block_in::handle_fetch_block_locator(const code& ec,
    get_headers_ptr message, const hash_digest& stop_hash)
{
    if (stopped(ec))
        return;

    if (ec)
    {
        LOG_ERROR(LOG_NODE)
            << "Internal failure generating block locator for ["
            << authority() << "] " << ec.message();
        stop(ec);
        return;
    }

    if (message->start_hashes().empty())
        return;

    const auto& last_hash = message->start_hashes().front();

    // TODO: move get_headers to a derived class protocol_block_in_31800.
    const auto use_headers = negotiated_version() >= version::level::headers;
    const auto request_type = (use_headers ? "headers" : "inventory");

    if (stop_hash == null_hash)
    {
        LOG_DEBUG(LOG_NODE)
            << "Ask [" << authority() << "] for " << request_type << " after ["
            << encode_hash(last_hash) << "]";
    }
    else
    {
        LOG_DEBUG(LOG_NODE)
            << "Ask [" << authority() << "] for " << request_type << " from ["
            << encode_hash(last_hash) << "] through ["
            << encode_hash(stop_hash) << "]";
    }

    message->set_stop_hash(stop_hash);

    if (use_headers)
        SEND2(*message, handle_send, _1, message->command);
    else
        SEND2(static_cast<get_blocks>(*message), handle_send, _1,
            message->command);
}

// Receive headers|inventory sequence.
//-----------------------------------------------------------------------------

// TODO: move headers to a derived class protocol_block_in_31800.
// This originates from send_header->annoucements and get_headers requests, or
// from an unsolicited announcement. There is no way to distinguish.
bool protocol_block_in::handle_receive_headers(const code& ec,
    headers_const_ptr message)
{
    if (stopped(ec))
        return false;

    // We don't want to request a batch of headers out of order.
    if (!message->is_sequential())
    {
        LOG_WARNING(LOG_NODE)
            << "Block headers out of order from [" << authority() << "].";
        stop(error::channel_stopped);
        return false;
    }

    // There is no benefit to this use of headers, in fact it is suboptimal.
    // In v3 headers will be used to build block tree before getting blocks.
    const auto response = std::make_shared<get_data>();

    if (compact_from_peer_) {

         /* LOG_INFO(LOG_NODE)
            << " protocol_block_in::handle_receive_headers (compactblock) ["
            << authority() << "] ";*/

        message->to_inventory(response->inventories(), inventory::type_id::compact_block);
    } else {

        /*LOG_INFO(LOG_NODE)
            << " protocol_block_in::handle_receive_headers (block) ["
            << authority() << "] ";*/
    message->to_inventory(response->inventories(), inventory::type_id::block);
    }
   
    // Remove hashes of blocks that we already have.
    chain_.filter_blocks(response, BIND2(send_get_data, _1, response));
    return true;
}

// This originates from default annoucements and get_blocks requests, or from
// an unsolicited announcement. There is no way to distinguish.
bool protocol_block_in::handle_receive_inventory(const code& ec,
    inventory_const_ptr message)
{
    if (stopped(ec))
        return false;

    const auto response = std::make_shared<get_data>();
    
    if (compact_from_peer_) {

   /*      LOG_INFO(LOG_NODE)
            << " protocol_block_in::handle_receive_inventory (compactblock) ["
            << authority() << "] ";
*/
        message->reduce(response->inventories(), inventory::type_id::compact_block);
    } else {

          /*LOG_INFO(LOG_NODE)
            << " protocol_block_in::handle_receive_inventory (block) ["
            << authority() << "] ";*/
    message->reduce(response->inventories(), inventory::type_id::block);
    }
    
    // Remove hashes of blocks that we already have.
    chain_.filter_blocks(response, BIND2(send_get_data, _1, response));
    return true;
}

void protocol_block_in::send_get_data(const code& ec, get_data_ptr message)
{
    if (stopped(ec))
        return;

    if (ec)
    {
        LOG_ERROR(LOG_NODE)
            << "Internal failure filtering block hashes for ["
            << authority() << "] " << ec.message();
        stop(ec);
        return;
    }

    if (message->inventories().empty())
        return;


    if (compact_from_peer_) {

        if (node_.node_settings().compact_blocks_high_bandwidth) {
#ifdef BITPRIM_CURRENCY_BCH
            uint64_t compact_version = 1;
#else
            uint64_t compact_version = 2;
#endif

            if ( ! compact_blocks_high_bandwidth_set_ && ! chain_.is_stale() ) {
                LOG_INFO(LOG_NODE) << "The chain is not stale, send sendcmcpt with high bandwidth ["<< authority() << "]";
                SEND2((send_compact{true, compact_version}), handle_send, _1, send_compact::command);
                compact_blocks_high_bandwidth_set_ = true;
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    mutex.lock_upgrade();
    const auto fresh = backlog_.empty();
    mutex.unlock_upgrade_and_lock();
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // Enqueue the block inventory behind the preceding block inventory.
    for (const auto& inventory: message->inventories()){
        if (inventory.type() == inventory::type_id::block) {
            backlog_.push(inventory.hash());
        } else if (inventory.type() == inventory::type_id::compact_block) {
            backlog_.push(inventory.hash());
        }
    }

    mutex.unlock();
    ///////////////////////////////////////////////////////////////////////////

    // Convert requested message types to corresponding witness types.
    if (require_witness_)
        message->to_witness();

    // There was no backlog so the timer must be started now.
    if (fresh)
        reset_timer();

    // inventory|headers->get_data[blocks]
    SEND2(*message, handle_send, _1, message->command);
}

// Receive not_found sequence.
//-----------------------------------------------------------------------------

// TODO: move not_found to a derived class protocol_block_in_70001.
bool protocol_block_in::handle_receive_not_found(const code& ec,
    not_found_const_ptr message)
{
    if (stopped(ec))
        return false;

    if (ec)
    {
        LOG_DEBUG(LOG_NODE)
            << "Failure getting block not_found from [" << authority() << "] "
            << ec.message();
        stop(ec);
        return false;
    }

    hash_list hashes;
    message->to_hashes(hashes, inventory::type_id::block);

    for (const auto& hash: hashes)
    {
        LOG_DEBUG(LOG_NODE)
            << "Block not_found [" << encode_hash(hash) << "] from ["
            << authority() << "]";
    }

    // The peer cannot locate one or more blocks that it told us it had.
    // This only results from reorganization assuming peer is proper.
    // Drop the peer so next channgel generates a new locator and backlog.
    if (!hashes.empty())
        stop(error::channel_stopped);

    return true;
}

// Receive block sequence.
//-----------------------------------------------------------------------------

void protocol_block_in::organize_block(block_const_ptr message) {
    message->validation.originator = nonce();
    chain_.organize(message, BIND2(handle_store_block, _1, message));
}

bool protocol_block_in::handle_receive_block(const code& ec,
    block_const_ptr message)
{
    if (stopped(ec))
        return false;

    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    mutex.lock();

    auto matched = !backlog_.empty() && backlog_.front() == message->hash();

    if (matched)
        backlog_.pop();

    // Empty after pop means we need to make a new request.
    const auto cleared = backlog_.empty();

    mutex.unlock();
    ///////////////////////////////////////////////////////////////////////////

    // If a peer sends a block unannounced we drop the peer - always. However
    // it is common for block announcements to cause block requests to be sent
    // out of backlog order due to interleaving of threads. This results in
    // channel drops during initial block download but not after sync. The
    // resolution to this issue is use of headers-first sync, but short of that
    // the current implementation performs well and drops peers no more
    // frequently than block announcements occur during initial block download,
    // and not typically after it is complete.
    if (!matched)
    {
        LOG_DEBUG(LOG_NODE)
            << "Block [" << encode_hash(message->hash())
            << "] unexpected or out of order from [" << authority() << "]";
        stop(error::channel_stopped);
        return false;
    }

    if (!require_witness_ && message->is_segregated())
    {
        LOG_DEBUG(LOG_NODE)
            << "Block [" << encode_hash(message->hash())
            << "] contains unrequested witness from [" << authority() << "]";
        stop(error::channel_stopped);
        return false;
    }

    // message->validation.originator = nonce();
    // chain_.organize(message, BIND2(handle_store_block, _1, message));
    organize_block(message);

    // Sending a new request will reset the timer upon inventory->get_data, but
    // we need to time out the lack of response to those requests when stale.
    // So we rest the timer in case of cleared and for not cleared.
    reset_timer();

    if (cleared)
        send_get_blocks(null_hash);

    return true;
}

bool protocol_block_in::handle_receive_block_transactions(const code& ec, block_transactions_const_ptr message)
{
    if (stopped(ec))
        return false;
      
    auto it = compact_blocks_map_.find(message->block_hash());
    if (it == compact_blocks_map_.end()) {
       
        LOG_DEBUG(LOG_NODE)
            << "Compact Block [" << encode_hash(message->block_hash())
            << "] The blocktxn received doesn't match with any temporal compact block [" << authority() << "]";
        stop(error::channel_stopped);
        return false;
    }

    auto temp_compact_block_ = it->second;

    auto const& vtx_missing = message->transactions();

    auto& txn_available = temp_compact_block_.transactions;
    auto const& header_temp = temp_compact_block_.header;

    size_t tx_missing_offset = 0;

    for (size_t i = 0; i < txn_available.size(); i++) {
        
        if ( ! txn_available[i].is_valid()) {
            if (vtx_missing.size() <= tx_missing_offset) {
                
                LOG_DEBUG(LOG_NODE)
                    << "Compact Block [" << encode_hash(message->block_hash())
                    << "] The offset " << tx_missing_offset << " is invalid [" << authority() << "]";
                stop(error::channel_stopped);

                //TODO(Mario) verify if necesary mutual exclusion
                compact_blocks_map_.erase(it);
                return false;
            }

            txn_available[i] = std::move(vtx_missing[tx_missing_offset]);
            ++tx_missing_offset;
        } 
    }
  
    if (vtx_missing.size() != tx_missing_offset) {
       LOG_DEBUG(LOG_NODE)
            << "Compact Block [" << encode_hash(message->block_hash())
            << "] The offset " << tx_missing_offset << " is invalid [" << authority() << "]";
        stop(error::channel_stopped);

        //TODO(Mario) verify if necesary mutual exclusion
        compact_blocks_map_.erase(it);
        return false;
    }

    auto const tempblock = std::make_shared<message::block>(std::move(header_temp), std::move(txn_available));
        
    organize_block(tempblock);

    //TODO(Mario) verify if necesary mutual exclusion
    compact_blocks_map_.erase(it);

    return true;
}

void protocol_block_in::handle_fetch_block_locator_compact_block(const code& ec, get_headers_ptr message, const hash_digest& stop_hash) {
    
    if (stopped(ec))
        return;

    if (ec)
    {
        LOG_ERROR(LOG_NODE)
        << "Internal failure generating block locator (compact block) for ["
        << authority() << "] " << ec.message();
        stop(ec);
        return;
    }

    if (message->start_hashes().empty()) {
        return;
    }
    
    message->set_stop_hash(stop_hash);  
    SEND2(*message, handle_send, _1, message->command);


    LOG_DEBUG(LOG_NODE)
        << "Sended get header message compact blocks"
        << authority() << "] ";

}

bool protocol_block_in::handle_receive_compact_block(code const& ec, compact_block_const_ptr message) {
    
    if (stopped(ec)) {
        return false;
    }

    //TODO(Mario): purge old compact blocks

    //the header of the compact block is the header of the block
    auto const& header_temp = message->header();    

    if (!header_temp.is_valid()) {
        LOG_DEBUG(LOG_NODE)
            << "Compact Block [" << encode_hash(header_temp.hash())
            << "] The compact block header is invalid [" << authority() << "]";
        stop(error::channel_stopped);
        return false;
    }

    //if the compact block exists in the map, is already in process
    if (compact_blocks_map_.count(header_temp.hash()) > 0) {
        return true;
    }

    //if we haven't the parent block already, send a get_header message
    // and return
    if ( ! chain_.get_block_exists_safe(header_temp.previous_block_hash() ) ) {
        
        LOG_DEBUG(LOG_NODE)
            << "Compact Block parent block not exists [ " << encode_hash(header_temp.previous_block_hash())
            << " [" << authority() << "]";
        
        if ( ! chain_.is_stale() ) {
            
            LOG_DEBUG(LOG_NODE)
            << "The chain isn't stale sending getheaders message [" << authority() << "]";
            
            const auto heights = block::locator_heights(node_.top_block().height());
            chain_.fetch_block_locator(heights,BIND3(handle_fetch_block_locator_compact_block, _1, _2, null_hash));
        } 
        return true;
    }
    //  else {
    //     LOG_INFO(LOG_NODE)
    //         << "Compact Block parent block EXISTS [ " << encode_hash(header_temp.previous_block_hash())
    //         << " [" << authority() << "]";
    // }
   
    //the nonce used to calculate the short id
    auto const nonce = message->nonce();

    auto const& prefiled_txs = message->transactions();
    auto const& short_ids = message->short_ids();
        
    std::vector<chain::transaction> txs_available(short_ids.size() + prefiled_txs.size());
    int32_t lastprefilledindex = -1;
    
    for (size_t i = 0; i < prefiled_txs.size(); ++i) {
       
        if (!prefiled_txs[i].is_valid()) {
            
            LOG_DEBUG(LOG_NODE)
            << "Compact Block [" << encode_hash(header_temp.hash())
            << "] The prefilled transaction is invalid [" << authority() << "]";
            stop(error::channel_stopped);
            return false;
        }

        //encoded = (current_index - prev_index) - 1
        // current = +1 + prev
        //       prev            current       
        lastprefilledindex += prefiled_txs[i].index() + 1;

        
        if (lastprefilledindex > std::numeric_limits<uint16_t>::max()) {
            LOG_DEBUG(LOG_NODE)
            << "Compact Block [" << encode_hash(header_temp.hash())
            << "] The prefilled index " << lastprefilledindex << " is out of range [" << authority() << "]";
            stop(error::channel_stopped);
            return false;
        }
          
        if ((uint32_t)lastprefilledindex > short_ids.size() + i) {
            // If we are inserting a tx at an index greater than our full list
            // of shorttxids plus the number of prefilled txn we've inserted,
            // then we have txn for which we have neither a prefilled txn or a
            // shorttxid!

            LOG_DEBUG(LOG_NODE)
            << "Compact Block [" << encode_hash(header_temp.hash())
            << "] The prefilled index " << lastprefilledindex << " is out of range [" << authority() << "]";
            stop(error::channel_stopped);
            return false;
        }

        txs_available[lastprefilledindex] = prefiled_txs[i].transaction();
    }
 
    // Calculate map of txids -> positions and check mempool to see what we have
    // (or don't). Because well-formed cmpctblock messages will have a
    // (relatively) uniform distribution of short IDs, any highly-uneven
    // distribution of elements can be safely treated as a READ_STATUS_FAILED.
    std::unordered_map<uint64_t, uint16_t> shorttxids(short_ids.size());
    uint16_t index_offset = 0;
    
    for (size_t i = 0; i < short_ids.size(); ++i) {
                
        while (txs_available[i + index_offset].is_valid()) {
            ++index_offset;
        }
        shorttxids[short_ids[i]] = i + index_offset;
        // To determine the chance that the number of entries in a bucket
        // exceeds N, we use the fact that the number of elements in a single
        // bucket is binomially distributed (with n = the number of shorttxids
        // S, and p = 1 / the number of buckets), that in the worst case the
        // number of buckets is equal to S (due to std::unordered_map having a
        // default load factor of 1.0), and that the chance for any bucket to
        // exceed N elements is at most buckets * (the chance that any given
        // bucket is above N elements). Thus: P(max_elements_per_bucket > N) <=
        // S * (1 - cdf(binomial(n=S,p=1/S), N)). If we assume blocks of up to
        // 16000, allowing 12 elements per bucket should only fail once per ~1
        // million block transfers (per peer and connection).
        
        if (shorttxids.bucket_size(shorttxids.bucket(short_ids[i])) > 12) {
            // Duplicate txindexes, the block is now in-flight, so
            // just request it.
            
            LOG_INFO(LOG_NODE) << "Compact Block, sendening getdata for hash (" << encode_hash(header_temp.hash()) << ") to [" << authority() << "]";
            send_get_data_compact_block(ec, header_temp.hash());
            
            return true;
        }
    }
   
    
    // TODO: in the shortid-collision case, we should instead request both
    // transactions which collided. Falling back to full-block-request here is
    // overkill.
    if (shorttxids.size() != short_ids.size()) {
        // Short ID collision
        LOG_INFO(LOG_NODE) << "Compact Block, sendening getdata for hash (" << encode_hash(header_temp.hash()) << ") to [" << authority() << "]";
        send_get_data_compact_block(ec, header_temp.hash());
        return true;
    }
        
    size_t mempool_count = 0;
    chain_.fill_tx_list_from_mempool(*message, mempool_count, txs_available, shorttxids);

    std::vector<uint64_t> txs;
    size_t prev_idx = 0;

    for (size_t i = 0; i < txs_available.size(); ++i) {
        if ( ! txs_available[i].is_valid()) {
            //diff_enc = (current_index - prev_index) - 1
            size_t diff_enc = i - prev_idx - (txs.size() > 0 ? 1 : 0);
            prev_idx = i;
            txs.push_back(diff_enc);
        }
    }

    if (txs.empty()) {

        auto const tempblock = std::make_shared<message::block>(std::move(header_temp), std::move(txs_available)); 
        organize_block(tempblock);
        return true;

    } else {
       
        compact_blocks_map_.emplace(header_temp.hash(), temp_compact_block{std::move(header_temp), std::move(txs_available)});

        auto req_tx = get_block_transactions(header_temp.hash(),txs);
        SEND2(req_tx, handle_send, _1, get_block_transactions::command);
        return true;
    } 
}

void protocol_block_in::send_get_data_compact_block(const code& ec, const hash_digest& hash) {

    hash_list hashes;
    hashes.push_back(hash);

    get_data_ptr request;
    request = std::make_shared<get_data>(hashes, inventory::type_id::block);

    send_get_data(ec,request);
}

// The block has been saved to the block chain (or not).
// This will be picked up by subscription in block_out and will cause the block
// to be announced to non-originating peers.
void protocol_block_in::handle_store_block(const code& ec, block_const_ptr message) {
    if (stopped(ec)) {
        return;
    }

    const auto hash = message->header().hash();

    // Ask the peer for blocks from the chain top up to this orphan.
    if (ec == error::orphan_block) {
        send_get_blocks(hash);
    }

    const auto encoded = encode_hash(hash);

    if (ec == error::orphan_block ||
        ec == error::duplicate_block ||
        ec == error::insufficient_work)
    {
        LOG_DEBUG(LOG_NODE)
            << "Captured block [" << encoded << "] from [" << authority()
            << "] " << ec.message();
        return;
    }

    // TODO: send reject as applicable.
    if (ec)
    {
        LOG_DEBUG(LOG_NODE)
            << "Rejected block [" << encoded << "] from [" << authority()
            << "] " << ec.message();
        stop(ec);
        return;
    }

    const auto state = message->validation.state;
    BITCOIN_ASSERT(state);

    // Show that diplayed forks may be missing activations due to checkpoints.
    const auto checked = state->is_under_checkpoint() ? "*" : "";

#ifdef WITH_MINING
    chain_.remove_mined_txs_from_chosen_list(message);
#endif // WITH_MINING

    LOG_DEBUG(LOG_NODE)
        << "Connected block [" << encoded << "] at height [" << state->height()
        << "] from [" << authority() << "] (" << state->enabled_forks()
        << checked << ", " << state->minimum_version() << ").";

    report(*message);
}

// Subscription.
//-----------------------------------------------------------------------------

// This is fired by the callback (i.e. base timer and stop handler).
void protocol_block_in::handle_timeout(const code& ec)
{
    if (stopped(ec))
    {
        // This may get called more than once per stop.
        handle_stop(ec);
        return;
    }

    // Since we need blocks do not stay connected to peer in bad version range.
    if (!blocks_from_peer_)
    {
        stop(error::channel_stopped);
        return;
    }

    if (ec && ec != error::channel_timeout)
    {
        LOG_DEBUG(LOG_NODE)
            << "Failure in block timer for [" << authority() << "] "
            << ec.message();
        stop(ec);
        return;
    }

    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    mutex.lock_shared();
    const auto backlog_empty = backlog_.empty();
    mutex.unlock_shared();
    ///////////////////////////////////////////////////////////////////////////

    // Can only end up here if time was not extended.
    if (!backlog_empty)
    {
        LOG_DEBUG(LOG_NODE)
            << "Peer [" << authority()
            << "] exceeded configured block latency.";
        stop(ec);
    }

    // Can only end up here if peer did not respond to inventory or get_data.
    // At this point we are caught up with an honest peer. But if we are stale
    // we should try another peer and not just keep pounding this one.
    if (chain_.is_stale())
        stop(error::channel_stopped);

    // If we are not stale then we are either good or stalled until peer sends
    // an announcement. There is no sense pinging a broken peer, so we either
    // drop the peer after a certain mount of time (above 10 minutes) or rely
    // on other peers to keep us moving and periodically age out connections.
    // Note that this allows a non-witness peer to hang on indefinately to our
    // witness-requiring node until the node becomes stale. Allowing this then
    // depends on requiring witness peers for explicitly outbound connections.
}

void protocol_block_in::handle_stop(const code&)
{
    LOG_DEBUG(LOG_NETWORK)
        << "Stopped block_in protocol for [" << authority() << "].";
}

// Block reporting.
//-----------------------------------------------------------------------------

inline bool enabled(size_t height)
{
    // Vary the reporting performance reporting interval by height.
    const auto modulus =
        (height < 100000 ? 100 :
        (height < 200000 ? 10 : 1));

    return height % modulus == 0;
}

inline float difference(const asio::time_point& start,
    const asio::time_point& end)
{
    const auto elapsed = duration_cast<asio::microseconds>(end - start);
    return static_cast<float>(elapsed.count());
}

inline size_t unit_cost(const asio::time_point& start,
    const asio::time_point& end, size_t value)
{
    return static_cast<size_t>(std::round(difference(start, end) / value));
}

inline size_t total_cost_ms(const asio::time_point& start,
    const asio::time_point& end)
{
    static constexpr size_t microseconds_per_millisecond = 1000;
    return unit_cost(start, end, microseconds_per_millisecond);
}

void protocol_block_in::report(const chain::block& block)
{
    BITCOIN_ASSERT(block.validation.state);
    const auto height = block.validation.state->height();

    if (enabled(height))
    {
        const auto& times = block.validation;
        const auto now = asio::steady_clock::now();
        const auto transactions = block.transactions().size();
        const auto inputs = std::max(block.total_inputs(), size_t(1));

        // Subtract total deserialization time from start of validation because
        // the wait time is between end_deserialize and start_check. This lets
        // us simulate block announcement validation time as there is no wait.
        const auto start_validate = times.start_check -
            (times.end_deserialize - times.start_deserialize);

        boost::format format("Block [%|i|] %|4i| txs %|4i| ins "
            "%|4i| wms %|4i| vms %|4i| vus %|4i| rus %|4i| cus %|4i| pus "
            "%|4i| aus %|4i| sus %|4i| dus %|f|");

        LOG_INFO(LOG_BLOCKCHAIN)
            << (format % height % transactions % inputs %

            // wait total (ms)
            total_cost_ms(times.end_deserialize, times.start_check) %

            // validation total (ms)
            total_cost_ms(start_validate, times.start_notify) %

            // validation per input (µs)
            unit_cost(start_validate, times.start_notify, inputs) %

            // deserialization (read) per input (µs)
            unit_cost(times.start_deserialize, times.end_deserialize, inputs) %

            // check per input (µs)
            unit_cost(times.start_check, times.start_populate, inputs) %

            // population per input (µs)
            unit_cost(times.start_populate, times.start_accept, inputs) %

            // accept per input (µs)
            unit_cost(times.start_accept, times.start_connect, inputs) %

            // connect (script) per input (µs)
            unit_cost(times.start_connect, times.start_notify, inputs) %

            // deposit per input (µs)
            unit_cost(times.start_push, times.end_push, inputs) %

            // this block transaction cache efficiency (hits/queries)
            block.validation.cache_efficiency);
    }
}

} // namespace node
} // namespace libbitcoin
