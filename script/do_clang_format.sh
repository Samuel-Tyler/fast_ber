find ../include ../src/compiler/include -name '*hpp' -exec clang-format -i {} \;
find ../src ../include ../test ../sample ../benchmarks -name '*cpp' -exec clang-format -i {} \;

python ../3rd_party/run-clang-format/run-clang-format.py -r ../include ../src ../test ../sample ../benchmarks --exclude '*asn_compiler.hpp'
