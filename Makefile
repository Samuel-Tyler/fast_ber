parser: test/parse_test.cpp clock.hpp
	g++ -o $@ $< -Wall -Wextra -Wpedantic -O3 -std=c++1z -I. -I3rd_party/abseil-cpp -I3rd_party/GSL/include -Isrc -Iinclude

clock.hpp: build/asn1_sam
	./build/asn1_sam "testfiles/simple3.asn" > clock.hpp

build/asn1_sam: build/asn1_sam.cpp
	g++ -o $@ $< -Wall -Wextra -Wpedantic -O3 -std=c++1z -I3rd_party/abseil-cpp -I3rd_party/GSL/include -Isrc -Iinclude

build/asn1_sam.cpp: build/asn1_sam.re
	re2c -o "$@" "$<"

build/asn1_sam.re: src/asn1_sam.y build
	bison "$<" -t --report=all -o "$@"

build:
	mkdir build

clean:
	rm -rf build
