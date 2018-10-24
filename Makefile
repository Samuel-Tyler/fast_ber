parser: test/ParseTest.cpp build/clock.hpp
	g++ -o $@ $< -Wall -Wextra -Wpedantic -O3 -std=c++1z -I. -I3rd_party/Catch2/single_include -I3rd_party/abseil-cpp -I3rd_party/GSL/include -Isrc -Iinclude

build/clock.hpp: build/asn1_sam
	./build/asn1_sam "testfiles/simple3.asn" > build/clock.hpp

build/asn1_sam: src/compiler/CompilerMain.cpp build/asn1_sam.hpp
	g++ -o $@ $< -Wall -Wextra -Wpedantic -O3 -std=c++1z  -Ibuild -I3rd_party/abseil-cpp -I3rd_party/GSL/include -Isrc -Iinclude

build/asn1_sam.cpp: build/asn1_sam.re
	re2c -o "$@" "$<"

build/asn1_sam.re: src/asn1_sam.y build
	bison "$<" -t --report=all -o "$@"

build:
	mkdir build

clean:
	rm -rf build
