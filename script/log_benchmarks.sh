set -e

TAG=$(git describe --tags)

mkdir -p ../build_bench
cd ../build_bench
cmake --verbose -D CMAKE_CXX_COMPILER=clang++-7 CMAKE_C_COMPILER=clang-7 -DCMAKE_BUILD_TYPE=Release ..
make -j10
./benchmarks/fast_ber_benchmarks > ../benchmarks/history/results_${TAG}.txt
