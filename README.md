# Bitprim Node <a target="_blank" href="https://gitter.im/bitprim/Lobby">![Gitter Chat][badge.Gitter]</a>

*Bitcoin full node based on libbitcoin-blockchain*

| **master(linux/osx)** | **dev(linux/osx)**   | **master(windows)**   | **dev(windows)** |
|:------:|:-:|:-:|:-:|
| [![Build Status](https://travis-ci.org/k-nuth/node.svg)](https://travis-ci.org/k-nuth/node)       | [![Build StatusB](https://travis-ci.org/k-nuth/node.svg?branch=dev)](https://travis-ci.org/k-nuth/node?branch=dev)  | [![Appveyor Status](https://ci.appveyor.com/api/projects/status/github/k-nuth/node?svg=true)](https://ci.appveyor.com/project/k-nuth/node)  | [![Appveyor StatusB](https://ci.appveyor.com/api/projects/status/github/k-nuth/node?branch=dev&svg=true)](https://ci.appveyor.com/project/k-nuth/node?branch=dev)  |

Make sure you have installed [kth-domain](https://github.com/k-nuth/core), [kth-database](https://github.com/k-nuth/database), [kth-blockchain](https://github.com/k-nuth/blockchain), [kth-consensus](https://github.com/k-nuth/consensus) (optional) and [kth-network](https://github.com/k-nuth/network) beforehand according to its build instructions.

```
$ git clone https://github.com/k-nuth/node.git
$ cd kth-node
$ mkdir build
$ cd build
$ cmake .. -DENABLE_TESTS=OFF -DWITH_TESTS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-std=c++11" 
$ make -j2
$ sudo make install
```

kth-node is now installed in `/usr/local/`.

In version2 the `bitcoin-node` console app is for demonstration purposes only. See [bitprim-server](https://github.com/k-nuth/server) for release quality full node functionality.

[badge.Gitter]: https://img.shields.io/badge/gitter-join%20chat-blue.svg
