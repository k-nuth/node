// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_NODE_VERSION_HPP_
#define KTH_NODE_VERSION_HPP_

#ifdef KTH_PROJECT_VERSION
#define KTH_NODE_VERSION KTH_PROJECT_VERSION
#else
#define KTH_NODE_VERSION "0.0.0"
#endif

namespace kth::node {

char const* version();

} // namespace kth::node

#endif //KTH_NODE_VERSION_HPP_
