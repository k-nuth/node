#
# Copyright (c) 2017-2018 Bitprim Inc.
#
# This file is part of Bitprim.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

import os
from conans import CMake
from ci_utils import option_on_off, get_version, get_conan_req_version, march_conan_manip, pass_march_to_compiler
from ci_utils import BitprimConanFile

class BitprimNodeConan(BitprimConanFile):
    name = "bitprim-node"
    # version = get_version()
    license = "http://www.boost.org/users/license.html"
    url = "https://github.com/bitprim/bitprim-node"
    description = "Bitcoin full node"
    settings = "os", "compiler", "build_type", "arch"

    # if Version(conan_version) < Version(get_conan_req_version()):
    #     raise Exception ("Conan version should be greater or equal than %s. Detected: %s." % (get_conan_req_version(), conan_version))

    options = {"shared": [True, False],
               "fPIC": [True, False],
               "with_tests": [True, False],
               "currency": ['BCH', 'BTC', 'LTC'],
               "microarchitecture": "ANY", #["x86_64", "haswell", "ivybridge", "sandybridge", "bulldozer", ...]
               "fix_march": [True, False],
               "verbose": [True, False]

    }
    # "with_remote_blockchain": [True, False],
    # "with_remote_database": [True, False],
    # "with_console": [True, False],

    default_options = "shared=False", \
        "fPIC=True", \
        "with_tests=False", \
        "currency=BCH", \
        "microarchitecture=_DUMMY_",  \
        "fix_march=False", \
        "verbose=True"

    # "with_remote_blockchain=False", \
    # "with_remote_database=False", \
    # "with_console=True", \


    with_remote_blockchain = False
    with_remote_database = False
    # with_console = False

    generators = "cmake"
    exports = "conan_*", "ci_utils/*"
    exports_sources = "src/*", "CMakeLists.txt", "cmake/*", "bitprim-nodeConfig.cmake.in", "bitprimbuildinfo.cmake", "include/*", "test/*", "console/*"
    package_files = "build/lbitprim-node.a"
    build_policy = "missing"


    def requirements(self):
        self.requires("boost/1.66.0@bitprim/stable")
        self.requires("bitprim-blockchain/0.X@%s/%s" % (self.user, self.channel))
        self.requires("bitprim-network/0.X@%s/%s" % (self.user, self.channel))

    def config_options(self):
        if self.settings.arch != "x86_64":
            self.output.info("microarchitecture is disabled for architectures other than x86_64, your architecture: %s" % (self.settings.arch,))
            self.options.remove("microarchitecture")
            self.options.remove("fix_march")

        if self.settings.compiler == "Visual Studio":
            self.options.remove("fPIC")
            if self.options.shared and self.msvc_mt_build:
                self.options.remove("shared")

    def configure(self):
        if self.settings.arch == "x86_64" and self.options.microarchitecture == "_DUMMY_":
            del self.options.fix_march
            # self.options.remove("fix_march")
            # raise Exception ("fix_march option is for using together with microarchitecture option.")

        if self.settings.arch == "x86_64":
            march_conan_manip(self)
            self.options["*"].microarchitecture = self.options.microarchitecture

        self.options["*"].currency = self.options.currency
        self.output.info("Compiling for currency: %s" % (self.options.currency,))

    def package_id(self):
        self.info.options.with_tests = "ANY"
        self.info.options.verbose = "ANY"
        self.info.options.fix_march = "ANY"

        #For Bitprim Packages libstdc++ and libstdc++11 are the same
        if self.settings.compiler == "gcc" or self.settings.compiler == "clang":
            if str(self.settings.compiler.libcxx) == "libstdc++" or str(self.settings.compiler.libcxx) == "libstdc++11":
                self.info.settings.compiler.libcxx = "ANY"

    def build(self):
        cmake = CMake(self)
        cmake.definitions["USE_CONAN"] = option_on_off(True)
        cmake.definitions["NO_CONAN_AT_ALL"] = option_on_off(False)
        cmake.verbose = self.options.verbose
        cmake.definitions["ENABLE_SHARED"] = option_on_off(self.is_shared)
        cmake.definitions["ENABLE_POSITION_INDEPENDENT_CODE"] = option_on_off(self.fPIC_enabled)
        cmake.definitions["WITH_REMOTE_BLOCKCHAIN"] = option_on_off(self.with_remote_blockchain)
        cmake.definitions["WITH_REMOTE_DATABASE"] = option_on_off(self.with_remote_database)
        cmake.definitions["WITH_TESTS"] = option_on_off(self.options.with_tests)
        # cmake.definitions["WITH_CONSOLE"] = option_on_off(self.with_console)

        cmake.definitions["CURRENCY"] = self.options.currency

        if self.settings.compiler != "Visual Studio":
            # cmake.definitions["CONAN_CXX_FLAGS"] += " -Wno-deprecated-declarations"
            cmake.definitions["CONAN_CXX_FLAGS"] = cmake.definitions.get("CONAN_CXX_FLAGS", "") + " -Wno-deprecated-declarations"

        if self.settings.compiler == "Visual Studio":
            cmake.definitions["CONAN_CXX_FLAGS"] = cmake.definitions.get("CONAN_CXX_FLAGS", "") + " /DBOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE"

        cmake.definitions["MICROARCHITECTURE"] = self.options.microarchitecture
        cmake.definitions["BITPRIM_PROJECT_VERSION"] = self.version

        #TODO(bitprim): compare with the other project to see if this could be deleted!
        if self.settings.compiler == "gcc":
            if float(str(self.settings.compiler.version)) >= 5:
                cmake.definitions["NOT_USE_CPP11_ABI"] = option_on_off(False)
            else:
                cmake.definitions["NOT_USE_CPP11_ABI"] = option_on_off(True)
        elif self.settings.compiler == "clang":
            if str(self.settings.compiler.libcxx) == "libstdc++" or str(self.settings.compiler.libcxx) == "libstdc++11":
                cmake.definitions["NOT_USE_CPP11_ABI"] = option_on_off(False)

        pass_march_to_compiler(self, cmake)

        cmake.configure(source_dir=self.source_folder)
        cmake.build()

        if self.options.with_tests:
            cmake.test()

    def imports(self):
        self.copy("*.h", "", "include")

    def package(self):
        self.copy("*.h", dst="include", src="include")
        self.copy("*.hpp", dst="include", src="include")
        self.copy("*.ipp", dst="include", src="include")
        self.copy("*.lib", dst="lib", keep_path=False)
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.dylib*", dst="lib", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.includedirs = ['include']
        self.cpp_info.libs = ["bitprim-node"]
