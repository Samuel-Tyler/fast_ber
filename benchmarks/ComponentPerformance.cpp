#include "fast_ber/ber_types/All.hpp"

#include "catch2/catch.hpp"

#include <vector>

const int iterations = 1000000;

template <typename T>
void component_benchmark_encode(const T& type, const std::string& type_name)
{
    std::array<uint8_t, 1000> buffer;
    fast_ber::EncodeResult    res = {};
    BENCHMARK("fast_ber        - encode " + type_name)
    {
        for (int i = 0; i < iterations; i++)
        {
            res = fast_ber::encode(absl::Span<uint8_t>(buffer), type);
        }
    }
    REQUIRE(res.success);
}

template <typename T>
void component_benchmark_decode(const T& type, const std::string& type_name)
{
    std::array<uint8_t, 1000> buffer;
    fast_ber::encode(absl::Span<uint8_t>(buffer), type);

    T decoded_copy;

    bool res = false;
    BENCHMARK("fast_ber        - decode " + type_name)
    {
        for (int i = 0; i < iterations; i++)
        {
            res = fast_ber::decode(absl::Span<uint8_t>(buffer), decoded_copy);
        }
    }
    REQUIRE(res);
}

TEST_CASE("Component Performance: Encode")
{
    component_benchmark_encode(fast_ber::Integer(-99999999), "Integer");
    component_benchmark_encode(fast_ber::Boolean(true), "Boolean");
    component_benchmark_encode(fast_ber::OctetString("Test string!"), "OctetString");
    component_benchmark_encode(fast_ber::Null(), "Null");
    component_benchmark_encode(fast_ber::Optional<fast_ber::OctetString>("hello!"), "Optional (String)");
    component_benchmark_encode(fast_ber::Optional<fast_ber::OctetString>("hello!"), "Optional (Integer)");
    component_benchmark_encode(
        fast_ber::Choice<fast_ber::Integer, fast_ber::OctetString>(fast_ber::OctetString("hello!")), "Choice (String)");
}

TEST_CASE("Component Performance: Decode")
{
    component_benchmark_decode(fast_ber::Integer(-99999999), "Integer");
    component_benchmark_decode(fast_ber::Boolean(true), "Boolean");
    component_benchmark_decode(fast_ber::OctetString("Test string!"), "OctetString");
    component_benchmark_decode(fast_ber::Null(), "Null");
    component_benchmark_decode(fast_ber::Optional<fast_ber::OctetString>("hello!"), "Optional (String)");
    component_benchmark_decode(fast_ber::Optional<fast_ber::OctetString>("hello!"), "Optional (Integer)");
    component_benchmark_decode(
        fast_ber::Choice<fast_ber::Integer, fast_ber::OctetString>(fast_ber::OctetString("hello!")), "Choice (String)");
}
