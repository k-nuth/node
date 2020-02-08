<!-- <a target="_blank" href="http://semver.org">![Version][badge.version]</a> -->
<!-- <a target="_blank" href="https://cirrus-ci.com/github/k-nuth/node">![Build Status][badge.Cirrus]</a> -->

# Knuth Node <a target="_blank" href="https://github.com/k-nuth/node/releases">![Github Releases][badge.release]</a> <a target="_blank" href="https://travis-ci.org/k-nuth/node">![Build status][badge.Travis]</a> <a target="_blank" href="https://ci.appveyor.com/projects/k-nuth/node">![Build Status][badge.Appveyor]</a> <a target="_blank" href="https://t.me/knuth_cash">![Telegram][badge.telegram]</a> <a target="_blank" href="https://k-nuth.slack.com/">![Slack][badge.slack]</a>

> Bitcoin full node as a C++17 library

Build steps
-----------

The *Knuth* libraries can be installed on Linux, macOS, FreeBSD, Windows and others. These binaries are pre-built for the most usual operating system/compiler combinations and hosted in an online repository. If there are no pre-built binaries for your platform, a build from source will be attempted.

So, for any platform, an installation can be performed in 2 simple steps:

1. Install the Knuth build helper:
```
pip install kthbuild --user --upgrade
```

2. Configure the Conan remote:
```
conan remote add kth https://api.bintray.com/conan/k-nuth/kth
```

3. Install the appropriate library:

```
conan install node/0.X@kth/stable 
```

In you want to tune the installation for better performance, please refer to the [documentation](https://kth.github.io/docfx/content/user_guide/installation.html#advanced-installation).


<!-- Links -->
[badge.Travis]: https://travis-ci.org/k-nuth/node.svg?branch=master
[badge.Appveyor]: https://ci.appveyor.com/api/projects/status/github/k-nuth/node?svg=true&branch=master
[badge.Cirrus]: https://api.cirrus-ci.com/github/k-nuth/node.svg?branch=master
[badge.version]: https://badge.fury.io/gh/k-nuth%2Fnode.svg
[badge.release]: https://img.shields.io/github/release/k-nuth/node.svg

[badge.telegram]: https://img.shields.io/badge/telegram-badge-blue.svg?logo=telegram
[badge.slack]: https://img.shields.io/badge/slack-badge-orange.svg?logo=slack

<!-- [badge.Gitter]: https://img.shields.io/badge/gitter-join%20chat-blue.svg -->
