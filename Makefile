build/asn1_sam: build/asn1_sam.cpp
	g++ -o $@ $< -Wall -Wextra -Wpedantic -O3 -std=c++1z

build/asn1_sam.cpp: build/asn1_sam.re
	re2c -o "$@" "$<"

build/asn1_sam.re: src/asn1_sam.y build
	bison "$<" -t --report=all -o "$@"

build:
	mkdir build

clean:
	rm -rf build
