// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#include <bitcoin/node/version.hpp>

namespace kth { namespace node {

char const* version() {
    return KTH_NODE_VERSION;
}

}} // namespace kth::node

