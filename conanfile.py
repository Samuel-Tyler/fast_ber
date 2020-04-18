from conans import ConanFile, CMake, tools
import os

class FastberConan(ConanFile):
    name = "fast_ber"
    version = "0.4"
    license = "Boost Software License 1.0"
    author = "Samuel Tyler"
    url = "https://github.com/Samuel-Tyler/fast_ber"
    description = "A performant ASN.1 BER encoding and decoding library written in C++11"
    topics = ("ASN", "ASN1", "ASN.1", "BER", "Serialization")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = {"shared": False}
    generators = ("cmake", "cmake_find_package_multi")
    requires = "abseil/20200205"
    scm = {"type": "git", "url": url, "revision": "devel", "subfolder": name}

    def build(self):
        cmake = CMake(self)
        cmake.definitions["SKIP_TESTING"] = True
        cmake.definitions["SKIP_AUTO_GENERATION"] = True
        cmake.configure(source_folder=self.name)
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)
        self.env_info.PATH = os.path.join(self.package_folder, "bin")

