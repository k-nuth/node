/**
 * Copyright (c) 2016-2018 Bitprim Inc.
 *
 * This file is part of Bitprim.
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
#ifndef BITPRIM_NODE_KEOKEN_MANAGER_HPP_
#define BITPRIM_NODE_KEOKEN_MANAGER_HPP_

#include <functional>

#include <bitcoin/bitcoin/message/messages.hpp>
#include <bitcoin/bitcoin/wallet/payment_address.hpp>
#include <bitcoin/blockchain/interface/block_chain.hpp>
#include <bitcoin/node/define.hpp>

#include <bitprim/keoken/interpreter.hpp>
#include <bitprim/keoken/node_constants.hpp>
#include <bitprim/keoken/state_delegated.hpp>

namespace bitprim {
namespace keoken {

template <typename State>
class BCN_API base_manager {
    using payment_address = bc::wallet::payment_address;
public:
    using get_assets_by_address_list = typename State::get_assets_by_address_list;
    using get_assets_list = typename State::get_assets_list;
    using get_all_asset_addresses_list = typename State::get_all_asset_addresses_list;

    //class invariant: keoken_genesis_height > 0 

    base_manager(bc::blockchain::block_chain& chain, size_t keoken_genesis_height)
    // precondition: keoken_genesis_height > 0
        : keoken_genesis_height_(keoken_genesis_height)
        , chain_(chain)
        , interpreter_(state_, chain_)
        , initialized_(false)
        , processed_height_(keoken_genesis_height - 1)
    {}

    // non-copyable and non-movable class
    base_manager(base_manager const&) = delete;
    base_manager& operator=(base_manager const&) = delete;

    void initialize_from_blockchain() {
        using namespace std::placeholders;
        if (initialized_) return;

        initialized_ = true;

        state_.set_initial_asset_id(asset_id_initial);

        size_t top_height;
        if ( ! chain_.get_last_height(top_height)) {
            return; //TODO(fernando): qué hacemos acá?
        }

        initialize_from_blockchain(keoken_genesis_height_, top_height);
        processed_height_ = top_height;

        chain_.subscribe_blockchain(std::bind(&base_manager::handle_reorganized, this, _1, _2, _3, _4));
    }
    // Queries
    bool initialized() const {
        return initialized_;
    }

    get_assets_by_address_list get_assets_by_address(bc::wallet::payment_address const& addr) const {
        return state_.get_assets_by_address(addr);
    }

    get_assets_list get_assets() const {
        return state_.get_assets();
    }

    get_all_asset_addresses_list get_all_asset_addresses() const {
        return state_.get_all_asset_addresses();
    }

    State const& state() const {
        return state_;
    }

    State& state() {
        return state_;
    }

private:
    //TODO(fernando): change the name
    void initialize_from_blockchain(size_t from_height, size_t to_height) {
        bool witness = false;   //TODO(fernando): what to do with this...

        chain_.for_each_transaction_non_coinbase(from_height, to_height, witness, 
            [this](bc::code const& ec, size_t height, bc::chain::transaction const& tx) {
                if (ec == bc::error::success) {
                    interpreter_.process(height, tx);
                }
            }
        );
    }

    // void initialize_from_blockchain(size_t from_height);

    //TODO(fernando): move to other site
    void for_each_transaction_non_coinbase(size_t height, bc::chain::block const& block) {
        std::for_each(std::next(block.transactions().begin()), block.transactions().end(), [this, height](bc::chain::transaction const& tx) {
            interpreter_.process(height, tx);
        });
    }
    
    // A typical reorganization consists of one incoming and zero outgoing blocks.
    bool handle_reorganized(bc::code ec, size_t fork_height, bc::block_const_ptr_list_const_ptr const& incoming, bc::block_const_ptr_list_const_ptr const& outgoing) {
        if (ec == bc::error::service_stopped) {
            return false;
        }

        if (ec) {
            return false;
        }

        // Nothing to do here.
        if ( ! incoming || incoming->empty()) {
            return true;
        }
        
        if (processed_height_ + 1 < fork_height) {
            initialize_from_blockchain(processed_height_ + 1, fork_height - 1);
        }

        if ( ! outgoing || ! outgoing->empty()) {
            //Reorg
            state_.remove_up_to(fork_height);
        }

        for (auto const& block_ptr: *incoming) {
            for_each_transaction_non_coinbase(fork_height, *block_ptr);
            ++fork_height;
        }
        processed_height_ = fork_height - 1;

        // for (const auto block: *outgoing)
        //     LOG_DEBUG(LOG_NODE)
        //         << "Reorganization moved block to orphan pool ["
        //         << encode_hash(block->header().hash()) << "]";
        // const auto height = safe_add(fork_height, incoming->size());
        // set_top_block({ incoming->back()->hash(), height });

        //TODO(fernando): Important! See what to do in case of re-organizations

        return true;
    }

    size_t keoken_genesis_height_;
    size_t processed_height_;
    bool initialized_;
    State state_;
    bc::blockchain::block_chain& chain_;
    interpreter<State, libbitcoin::blockchain::block_chain> interpreter_;
};

template <typename State>
class BCN_API manager : public base_manager<State> {
    using base_manager<State>::base_manager;
};

template <>
class BCN_API manager<state_delegated> : public base_manager<state_delegated> {
public:
    using set_initial_asset_id_func = typename state_delegated::set_initial_asset_id_func;
    using reset_func = typename state_delegated::reset_func;
    using remove_up_to_func = typename state_delegated::remove_up_to_func;
    using create_asset_func = typename state_delegated::create_asset_func;
    using create_balance_entry_func = typename state_delegated::create_balance_entry_func;
    using asset_id_exists_func = typename state_delegated::asset_id_exists_func;
    using get_balance_func = typename state_delegated::get_balance_func;
    using get_assets_by_address_func = typename state_delegated::get_assets_by_address_func;
    using get_assets_func = typename state_delegated::get_assets_func;
    using get_all_asset_addresses_func = typename state_delegated::get_all_asset_addresses_func;

    using base_manager<state_delegated>::base_manager;

    void configure_state(set_initial_asset_id_func set_initial_asset_id
                    , reset_func reset
                    , remove_up_to_func remove_up_to
                    , create_asset_func create_asset
                    , create_balance_entry_func create_balance_entry
                    , asset_id_exists_func asset_id_exists
                    , get_balance_func get_balance
                    , get_assets_by_address_func get_assets_by_address
                    , get_assets_func get_assets
                    , get_all_asset_addresses_func get_all_asset_addresses) {

        state().set_initial_asset_id = set_initial_asset_id;
        state().reset = reset;
        state().remove_up_to = remove_up_to;
        state().create_asset = create_asset;
        state().create_balance_entry = create_balance_entry;
        state().asset_id_exists = asset_id_exists;
        state().get_balance = get_balance;
        state().get_assets_by_address = get_assets_by_address;
        state().get_assets = get_assets;
        state().get_all_asset_addresses = get_all_asset_addresses;
    }
};

} // namespace keoken
} // namespace bitprim

#endif //BITPRIM_NODE_KEOKEN_MANAGER_HPP_
