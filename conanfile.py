from conans import ConanFile, CMake

def option_on_off(option):
    return "ON" if option else "OFF"

class BitprimNodeConan(ConanFile):
    name = "bitprim-node"
    version = "0.1"
    license = "http://www.boost.org/users/license.html"
    url = "https://github.com/bitprim/bitprim-node"
    description = "Bitcoin full node"
    settings = "os", "compiler", "build_type", "arch"

    # options = {"shared": [True, False]}
    # default_options = "shared=False"

    options = {"shared": [True, False],
               "fPIC": [True, False],
               "with_remote_blockchain": [True, False],
               "with_remote_database": [True, False],
               "with_tests": [True, False],
               "with_console": [True, False],
               "with_litecoin": [True, False],
    }

    default_options = "shared=False", \
        "fPIC=True", \
        "with_remote_blockchain=False", \
        "with_remote_database=False", \
        "with_tests=True", \
        "with_console=True", \
        "with_litecoin=False"

    generators = "cmake"
    exports_sources = "src/*", "CMakeLists.txt", "cmake/*", "bitprim-nodeConfig.cmake.in", "include/*", "test/*"
    package_files = "build/lbitprim-node.a"
    build_policy = "missing"

    requires = (("bitprim-conan-boost/1.64.0@bitprim/stable"),
                ("bitprim-blockchain/0.1@bitprim/testing"),
                ("bitprim-network/0.1@bitprim/testing"))

    def build(self):
        cmake = CMake(self)
        
        cmake.definitions["USE_CONAN"] = "ON"
        cmake.definitions["CMAKE_VERBOSE_MAKEFILE"] = "ON"
        cmake.definitions["ENABLE_SHARED"] = option_on_off(self.options.shared)
        cmake.definitions["ENABLE_POSITION_INDEPENDENT_CODE"] = option_on_off(self.options.fPIC)
        cmake.definitions["WITH_REMOTE_BLOCKCHAIN"] = option_on_off(self.options.with_remote_blockchain)
        cmake.definitions["WITH_REMOTE_DATABASE"] = option_on_off(self.options.with_remote_database)
        cmake.definitions["WITH_TESTS"] = option_on_off(self.options.with_tests)
        cmake.definitions["WITH_CONSOLE"] = option_on_off(self.options.with_console)
        cmake.definitions["WITH_LITECOIN"] = option_on_off(self.options.with_litecoin)

        cmake.configure(source_dir=self.conanfile_directory)
        cmake.build()

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
