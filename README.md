[![Build Status](https://travis-ci.org/bitprim/bitprim-node.svg?branch=c-api)](https://travis-ci.org/bitprim/bitprim-node)

# Bitprim Node

*Bitcoin full node based on libbitcoin-blockchain*

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
