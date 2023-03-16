# Copyright (c) 2016-2023 Knuth Project developers.
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

import os
<<<<<<< Updated upstream
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout
=======
from conan import ConanFile
from conan.tools.build.cppstd import check_min_cppstd
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout
from conan.tools.files import copy #, apply_conandata_patches, export_conandata_patches, get, rm, rmdir
>>>>>>> Stashed changes
from kthbuild import option_on_off, march_conan_manip, pass_march_to_compiler
from kthbuild import KnuthConanFileV2

required_conan_version = ">=2.0"
class KnuthNodeConan(KnuthConanFileV2):
    name = "node"
    license = "http://www.boost.org/users/license.html"
    url = "https://github.com/k-nuth/node"
    description = "Crypto full node"
    settings = "os", "compiler", "build_type", "arch"

    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "tests": [True, False],
        "currency": ['BCH', 'BTC', 'LTC'],

        "verbose": [True, False],
        "mempool": [True, False],
        "db": ['legacy', 'legacy_full', 'pruned', 'default', 'full'],
        "db_readonly": [True, False],

        "march_id": ["ANY"],
        "march_strategy": ["download_if_possible", "optimized", "download_or_fail"],

        "cxxflags": ["ANY"],
        "cflags": ["ANY"],
        "cmake_export_compile_commands": [True, False],
        "log": ["boost", "spdlog", "binlog"],
        "use_libmdbx": [True, False],
        "statistics": [True, False],
    }
    # "with_console": [True, False],

    default_options = {
        "shared": False,
        "fPIC": True,
        "tests": False,
        "currency": "BCH",

        "march_strategy": "download_if_possible",

        "verbose": False,
        "mempool": False,
        "db": "default",
        "db_readonly": False,

        "cmake_export_compile_commands": False,
        "log": "spdlog",
        "use_libmdbx": False,
        "statistics": False,
    }
    # "with_console=True",

    # generators = "cmake"
<<<<<<< Updated upstream
    exports = "conan_*", "ci_utils/*"
    exports_sources = "src/*", "CMakeLists.txt", "cmake/*", "kth-nodeConfig.cmake.in", "knuthbuildinfo.cmake", "include/*", "test/*", "console/*"
    package_files = "build/lkth-node.a"
=======
    # exports = "conan_*", "ci_utils/*"
    exports_sources = "src/*", "CMakeLists.txt", "ci_utils/cmake/*", "cmake/*", "kth-nodeConfig.cmake.in", "knuthbuildinfo.cmake", "include/*", "test/*", "console/*"
    # package_files = "build/lkth-node.a"
>>>>>>> Stashed changes
    # build_policy = "missing"


    def build_requirements(self):
        if self.options.tests:
            self.test_requires("catch2/3.3.1")

    def requirements(self):
        self.requires("blockchain/0.X@%s/%s" % (self.user, self.channel))
        self.requires("network/0.X@%s/%s" % (self.user, self.channel))

        if self.options.statistics:
            self.requires("tabulate/1.0@")

    def validate(self):
<<<<<<< Updated upstream
        KnuthConanFile.validate(self)
        if self.info.settings.compiler.cppstd:
            check_min_cppstd(self, "20")
=======
        KnuthConanFileV2.validate(self)
>>>>>>> Stashed changes

    def config_options(self):
        KnuthConanFileV2.config_options(self)

    def configure(self):
        KnuthConanFileV2.configure(self)

        self.options["*"].mempool = self.options.mempool

        self.options["*"].db_readonly = self.options.db_readonly
        self.output.info("Compiling with read-only DB: %s" % (self.options.db_readonly,))

        self.output.info("Compiling for currency: %s" % (self.options.currency,))
        self.output.info("Compiling with mempool: %s" % (self.options.mempool,))

        #TODO(fernando): move to kthbuild
        self.options["*"].log = self.options.log
        self.output.info("Compiling with log: %s" % (self.options.log,))

        self.options["*"].use_libmdbx = self.options.use_libmdbx
        self.output.info("Compiling with use_libmdbx: %s" % (self.options.use_libmdbx,))

    def package_id(self):
        KnuthConanFileV2.package_id(self)

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = self.cmake_toolchain_basis()
        # tc.variables["CMAKE_VERBOSE_MAKEFILE"] = True
        # tc.variables["WITH_CONSOLE"] = option_on_off(self.with_console)
        tc.variables["WITH_KEOKEN"] = option_on_off(False)
        # tc.variables["WITH_KEOKEN"] = option_on_off(self.is_keoken)

        tc.variables["WITH_MEMPOOL"] = option_on_off(self.options.mempool)
        tc.variables["DB_READONLY_MODE"] = option_on_off(self.options.db_readonly)
        tc.variables["LOG_LIBRARY"] = self.options.log
        tc.variables["USE_LIBMDBX"] = option_on_off(self.options.use_libmdbx)
        tc.variables["STATISTICS"] = option_on_off(self.options.statistics)
        tc.variables["CONAN_DISABLE_CHECK_COMPILER"] = option_on_off(True)

        tc.generate()
        tc = CMakeDeps(self)
        tc.generate()

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = self.cmake_toolchain_basis()
        # tc.variables["CMAKE_VERBOSE_MAKEFILE"] = True
        # tc.variables["WITH_CONSOLE"] = option_on_off(self.with_console)
        tc.variables["WITH_MEMPOOL"] = option_on_off(self.options.mempool)
        tc.variables["DB_READONLY_MODE"] = option_on_off(self.options.db_readonly)
        tc.variables["LOG_LIBRARY"] = self.options.log
        tc.variables["USE_LIBMDBX"] = option_on_off(self.options.use_libmdbx)
        tc.variables["STATISTICS"] = option_on_off(self.options.statistics)
        tc.variables["CONAN_DISABLE_CHECK_COMPILER"] = option_on_off(True)
        tc.generate()
        tc = CMakeDeps(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
<<<<<<< Updated upstream
=======

>>>>>>> Stashed changes
        if not self.options.cmake_export_compile_commands:
            cmake.build()
            if self.options.tests:
                cmake.test()

    def imports(self):
        self.copy("*.h", "", "include")

    def package(self):
        cmake = CMake(self)
        cmake.install()
        # rmdir(self, os.path.join(self.package_folder, "lib", "cmake"))
        # rmdir(self, os.path.join(self.package_folder, "lib", "pkgconfig"))
        # rmdir(self, os.path.join(self.package_folder, "res"))
        # rmdir(self, os.path.join(self.package_folder, "share"))

    def package_info(self):
        self.cpp_info.includedirs = ['include']
        self.cpp_info.libs = ["kth-node"]
