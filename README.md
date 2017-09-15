# Bitprim Node <a target="_blank" href="https://gitter.im/bitprim/Lobby">![Gitter Chat][badge.Gitter]</a>

*Bitcoin full node based on libbitcoin-blockchain*

| **master(linux/osx)** | **dev(linux/osx)**   | **master(windows)**   | **dev(windows)** |
|:------:|:-:|:-:|:-:|
| [![Build Status](https://travis-ci.org/bitprim/bitprim-node.svg)](https://travis-ci.org/bitprim/bitprim-node)       | [![Build StatusB](https://travis-ci.org/bitprim/bitprim-node.svg?branch=dev)](https://travis-ci.org/bitprim/bitprim-node?branch=dev)  | [![Appveyor Status](https://ci.appveyor.com/api/projects/status/github/bitprim/bitprim-node?svg=true)](https://ci.appveyor.com/project/bitprim/bitprim-node)  | [![Appveyor StatusB](https://ci.appveyor.com/api/projects/status/github/bitprim/bitprim-node?branch=dev&svg=true)](https://ci.appveyor.com/project/bitprim/bitprim-node?branch=dev)  |

Make sure you have installed [bitprim-core](https://github.com/bitprim/bitprim-core), [bitprim-database](https://github.com/bitprim/bitprim-database), [bitprim-blockchain](https://github.com/bitprim/bitprim-blockchain), [bitprim-consensus](https://github.com/bitprim/bitprim-consensus) (optional) and [bitprim-network](https://github.com/bitprim/bitprim-network) beforehand according to its build instructions.

```
$ git clone https://github.com/bitprim/bitprim-node.git
$ cd bitprim-node
$ mkdir build
$ cd build
$ cmake .. -DENABLE_TESTS=OFF -DWITH_TESTS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-std=c++11" 
$ make -j2
$ sudo make install
```

bitprim-node is now installed in `/usr/local/`.

In version2 the `bitcoin-node` console app is for demonstration purposes only. See [bitprim-server](https://github.com/bitprim/bitprim-server) for release quality full node functionality.

[badge.Gitter]: https://img.shields.io/badge/gitter-join%20chat-blue.svg
