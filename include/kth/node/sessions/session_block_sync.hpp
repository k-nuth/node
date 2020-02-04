// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_NODE_SESSION_BLOCK_SYNC_HPP
#define KTH_NODE_SESSION_BLOCK_SYNC_HPP

#include <cstddef>
#include <cstdint>
#include <memory>
#include <kth/blockchain.hpp>
#include <kth/network.hpp>
#include <kth/node/define.hpp>
#include <kth/node/sessions/session.hpp>
#include <kth/node/settings.hpp>
#include <kth/node/utility/check_list.hpp>
#include <kth/node/utility/reservation.hpp>
#include <kth/node/utility/reservations.hpp>

namespace kth {
namespace node {

class full_node;

/// Class to manage initial block download connections, thread safe.
class BCN_API session_block_sync
  : public session<network::session_outbound>, track<session_block_sync>
{
public:
    typedef std::shared_ptr<session_block_sync> ptr;

    session_block_sync(full_node& network, check_list& hashes,
        blockchain::fast_chain& chain, const settings& settings);

    void start(result_handler handler) override;

protected:
    /// Overridden to attach and start specialized handshake.
    void attach_handshake_protocols(network::channel::ptr channel,
        result_handler handle_started) override;

    /// Override to attach and start specialized protocols after handshake.
    virtual void attach_protocols(network::channel::ptr channel,
        reservation::ptr row, result_handler handler);

private:
    void handle_started(const code& ec, result_handler handler);
    void new_connection(reservation::ptr row, result_handler handler);

    // Sequence.
    void handle_connect(const code& ec, network::channel::ptr channel,
        reservation::ptr row, result_handler handler);
    void handle_channel_start(const code& ec, network::channel::ptr channel,
        reservation::ptr row, result_handler handler);
    void handle_channel_complete(const code& ec, reservation::ptr row,
        result_handler handler);
    void handle_channel_stop(const code& ec, reservation::ptr row);
    void handle_complete(const code& ec, result_handler handler);

    // Timers.
    void reset_timer();
    void handle_timer(const code& ec);

    // These are thread safe.
    blockchain::fast_chain& chain_;
    reservations reservations_;
    deadline::ptr timer_;
};

} // namespace node
} // namespace kth

#endif

