// Copyright (c) 2016-2022 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_NODE_SESSION_OUTBOUND_HPP
#define KTH_NODE_SESSION_OUTBOUND_HPP

#include <memory>
#include <kth/blockchain.hpp>
#include <kth/network.hpp>
#include <kth/node/define.hpp>
#include <kth/node/sessions/session.hpp>

namespace kth::node {

class full_node;

/// Outbound connections session, thread safe.
class BCN_API session_outbound : public session<network::session_outbound>, track<session_outbound> {
public:
    using ptr = std::shared_ptr<session_outbound>;

    /// Construct an instance.
    session_outbound(full_node& network, blockchain::safe_chain& chain);

protected:
    /// Overridden to attach blockchain protocols.
    void attach_protocols(network::channel::ptr channel) override;

    blockchain::safe_chain& chain_;
};

} // namespace kth::node

#endif // KTH_NODE_SESSION_OUTBOUND_HPP
