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
    exports_sources = "include*", "generate.cmake"
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = {"shared": False}
    generators = "cmake"
    requires = "abseil/20191010@conan/testing"

    def source(self):
        self.run(f"git clone {self.url} --single-branch --branch conan")

    def build(self):
        cmake = CMake(self)
        cmake.definitions["SKIP_TESTING"] = True
        cmake.definitions["SKIP_AUTO_GENERATION"] = True
        cmake.configure(source_folder=self.name)
        cmake.build()

    def package(self):
        self.copy("*.hpp", dst="include", src="include")
        self.copy("*.lib", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)
        self.copy("fast_ber_compiler", dst="bin", src="bin", keep_path=False)
        self.copy("generate.cmake", dst="", src="", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)
        self.cpp_info.build_modules.append("generate.cmake")
        self.env_info.PATH = os.path.join(self.package_folder, "bin")

