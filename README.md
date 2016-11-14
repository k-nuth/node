[![Build Status](https://travis-ci.org/bitprim/bitprim-node.svg?branch=master)](https://travis-ci.org/bitprim/bitprim-node)

# Bitprim Node

*Bitcoin full node based on libbitcoin-blockchain*

Make sure you have installed [bitprim-blockchain](https://github.com/bitprim/bitprim-blockchain) beforehand according to its build instructions.

```sh
$ ./autogen.sh
$ ./configure
$ make
$ sudo make install
$ sudo ldconfig
```

bitprim-node is now installed in `/usr/local/`.

In version2 the `bitcoin-node` console app is for demonstration purposes only. See [bitprim-server](https://github.com/bitprim/bitprim-server) for release quality full node functionality.
