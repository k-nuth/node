#
# Copyright (c) 2017 Bitprim developers (see AUTHORS)
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
from conans import ConanFile, CMake
from conans import __version__ as conan_version
from conans.model.version import Version

def option_on_off(option):
    return "ON" if option else "OFF"


def get_content(file_name):
    file_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), file_name)
    with open(file_path, 'r') as f:
        return f.read().replace('\n', '').replace('\r', '')

def get_version():
    return get_content('conan_version')

def get_channel():
    return get_content('conan_channel')

def get_conan_req_version():
    return get_content('conan_req_version')


class BitprimNodeConan(ConanFile):
    name = "bitprim-node"
    version = get_version()
    license = "http://www.boost.org/users/license.html"
    url = "https://github.com/bitprim/bitprim-node"
    description = "Bitcoin full node"
    settings = "os", "compiler", "build_type", "arch"

    if conan_version < Version(get_conan_req_version()):
        raise Exception ("Conan version should be greater or equal than %s" % (get_conan_req_version(), ))

    options = {"shared": [True, False],
               "fPIC": [True, False],
               "with_tests": [True, False],
               "currency": ['BCH', 'BTC', 'LTC'],
               "verbose": [True, False],
    }
    #    "with_litecoin": [True, False],

    # "with_remote_blockchain": [True, False],
    # "with_remote_database": [True, False],
    # "with_console": [True, False],

    default_options = "shared=False", \
        "fPIC=True", \
        "with_tests=False", \
        "currency=BCH", \
        "verbose=False"
        

    # "with_litecoin=False", \

    # "with_remote_blockchain=False", \
    # "with_remote_database=False", \
    # "with_console=True", \


    with_remote_blockchain = False
    with_remote_database = False
    # with_console = False

    generators = "cmake"
    exports = "conan_channel", "conan_version", "conan_req_version"
    exports_sources = "src/*", "CMakeLists.txt", "cmake/*", "bitprim-nodeConfig.cmake.in", "bitprimbuildinfo.cmake", "include/*", "test/*", "console/*"
    package_files = "build/lbitprim-node.a"
    build_policy = "missing"

    requires = (("boost/1.66.0@bitprim/stable"),
                ("bitprim-blockchain/0.11.0@bitprim/%s" % get_channel()),
                ("bitprim-network/0.11.0@bitprim/%s" % get_channel()))

    @property
    def msvc_mt_build(self):
        return "MT" in str(self.settings.compiler.runtime)

    @property
    def fPIC_enabled(self):
        if self.settings.compiler == "Visual Studio":
            return False
        else:
            return self.options.fPIC

    @property
    def is_shared(self):
        if self.options.shared and self.msvc_mt_build:
            return False
        else:
            return self.options.shared


    def config_options(self):
        self.output.info('def config_options(self):')
        if self.settings.compiler == "Visual Studio":
            self.options.remove("fPIC")

            if self.options.shared and self.msvc_mt_build:
                self.options.remove("shared")

    def package_id(self):
        self.info.options.with_tests = "ANY"
        self.info.options.verbose = "ANY"

        #For Bitprim Packages libstdc++ and libstdc++11 are the same
        if self.settings.compiler == "gcc" or self.settings.compiler == "clang":
            if str(self.settings.compiler.libcxx) == "libstdc++" or str(self.settings.compiler.libcxx) == "libstdc++11":
                self.info.settings.compiler.libcxx = "ANY"

    def build(self):
        cmake = CMake(self)
        
        cmake.definitions["USE_CONAN"] = option_on_off(True)
        cmake.definitions["NO_CONAN_AT_ALL"] = option_on_off(False)

        # cmake.definitions["CMAKE_VERBOSE_MAKEFILE"] = option_on_off(False)
        # cmake.verbose = False
        cmake.verbose = self.options.verbose
        

        cmake.definitions["ENABLE_SHARED"] = option_on_off(self.is_shared)
        cmake.definitions["ENABLE_POSITION_INDEPENDENT_CODE"] = option_on_off(self.fPIC_enabled)

        cmake.definitions["WITH_REMOTE_BLOCKCHAIN"] = option_on_off(self.with_remote_blockchain)
        cmake.definitions["WITH_REMOTE_DATABASE"] = option_on_off(self.with_remote_database)

        cmake.definitions["WITH_TESTS"] = option_on_off(self.options.with_tests)
        # cmake.definitions["WITH_CONSOLE"] = option_on_off(self.with_console)

        # cmake.definitions["WITH_TESTS"] = option_on_off(self.with_tests)
        # cmake.definitions["WITH_CONSOLE"] = option_on_off(self.options.with_console)

        # cmake.definitions["WITH_LITECOIN"] = option_on_off(self.options.with_litecoin)

        cmake.definitions["CURRENCY"] = self.options.currency


        if self.settings.compiler != "Visual Studio":
            # cmake.definitions["CONAN_CXX_FLAGS"] += " -Wno-deprecated-declarations"
            cmake.definitions["CONAN_CXX_FLAGS"] = cmake.definitions.get("CONAN_CXX_FLAGS", "") + " -Wno-deprecated-declarations"

        if self.settings.compiler == "Visual Studio":
            cmake.definitions["CONAN_CXX_FLAGS"] = cmake.definitions.get("CONAN_CXX_FLAGS", "") + " /DBOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE"


        #TODO(bitprim): compare with the other project to see if this could be deleted!
        if self.settings.compiler == "gcc":
            if float(str(self.settings.compiler.version)) >= 5:
                cmake.definitions["NOT_USE_CPP11_ABI"] = option_on_off(False)
            else:
                cmake.definitions["NOT_USE_CPP11_ABI"] = option_on_off(True)
        elif self.settings.compiler == "clang":
            if str(self.settings.compiler.libcxx) == "libstdc++" or str(self.settings.compiler.libcxx) == "libstdc++11":
                cmake.definitions["NOT_USE_CPP11_ABI"] = option_on_off(False)


        cmake.definitions["BITPRIM_BUILD_NUMBER"] = os.getenv('BITPRIM_BUILD_NUMBER', '-')
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

        # self.copy("bn.exe", dst="bin", keep_path=False) # Windows
        # self.copy("bn", dst="bin", keep_path=False) # Linux / Macos

        # self.copy("bn.exe", dst="bin", src="bin") # Windows
        # self.copy("bn", dst="bin", src="bin") # Linux / Macos

        self.copy("*.lib", dst="lib", keep_path=False)
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.dylib*", dst="lib", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.includedirs = ['include']
        self.cpp_info.libs = ["bitprim-node"]
