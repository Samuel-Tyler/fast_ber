#include "autogen/all.hpp"

#include "catch2/catch.hpp"

#include <vector>

struct IntegerDefault
{
    constexpr static int get_value() noexcept { return 100; }
};

struct StringDefault
{
    constexpr static const char* get_value() noexcept { return "Test string!"; }
};

const int iterations = 1000000;

template <typename T>
void component_benchmark_encode(const T& type, const std::string& type_name)
{
    std::array<uint8_t, 1000> buffer{};
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
    std::array<uint8_t, 1000> buffer{};
    fast_ber::EncodeResult    encode_result = fast_ber::encode(absl::Span<uint8_t>(buffer), type);

    T decoded_copy;

    fast_ber::DecodeResult res = {false};
    BENCHMARK("fast_ber        - decode " + type_name)
    {
        for (int i = 0; i < iterations; i++)
        {
            res = fast_ber::decode(absl::Span<uint8_t>(buffer.data(), encode_result.length), decoded_copy);
        }
    }
    REQUIRE(res.success);
}

template <typename T1, typename T2>
void component_benchmark_construct(const T2& initial_value, const std::string& type_name)
{
    BENCHMARK("fast_ber        - construct " + type_name)
    {
        for (int i = 0; i < iterations; i++)
        {
            T1 t1(initial_value);
        }
    }
}

template <typename T1>
void component_benchmark_default_construct(const std::string& type_name)
{
    BENCHMARK("fast_ber        - dflt construct " + type_name)
    {
        for (int i = 0; i < iterations; i++)
        {
            T1 t1{};
        }
    }
}

TEST_CASE("Component Performance: Encode")
{
    component_benchmark_encode(fast_ber::Integer<>(-99999999), "Integer");
    component_benchmark_encode(fast_ber::Default<fast_ber::Integer<>, IntegerDefault>(-99999999), "Default Integer");
    component_benchmark_encode(fast_ber::Boolean<>(true), "Boolean");
    component_benchmark_encode(fast_ber::OctetString<>("Test string!"), "OctetString");
    component_benchmark_encode(fast_ber::Default<fast_ber::OctetString<>, StringDefault>("Test string!"),
                               "Default OctetString");
    component_benchmark_encode(fast_ber::Null<>(), "Null");
    component_benchmark_encode(fast_ber::Real<>(0.333333333333333314829616256247390992939472198486328125), "Real");
    component_benchmark_encode(fast_ber::ObjectIdentifier<>(fast_ber::ObjectIdentifierComponents{1, 2, 840, 113549}),
                               "ObjectIdentifier");
    component_benchmark_encode(fast_ber::Optional<fast_ber::OctetString<>>("hello!"), "Optional (String)");
    component_benchmark_encode(fast_ber::Optional<fast_ber::Integer<>>(500), "Optional (Integer)");
    component_benchmark_encode(fast_ber::Optional<fast_ber::Integer<>>(absl::nullopt), "Optional (Empty)");
    component_benchmark_encode(fast_ber::All::The_Choice<>(fast_ber::OctetString<>("hello!")), "Choice (String)");
    component_benchmark_encode(fast_ber::All::The_Choice<>(5), "Choice (Integer)");
}

TEST_CASE("Component Performance: Decode")
{
    component_benchmark_decode(fast_ber::Integer<>(-99999999), "Integer");
    component_benchmark_decode(fast_ber::Default<fast_ber::Integer<>, IntegerDefault>(-99999999), "Default Integer");
    component_benchmark_decode(fast_ber::Boolean<>(true), "Boolean");
    component_benchmark_decode(fast_ber::OctetString<>("Test string!"), "OctetString");
    component_benchmark_decode(fast_ber::Default<fast_ber::OctetString<>, StringDefault>("Test string!"),
                               "Default OctetString");
    component_benchmark_decode(fast_ber::Null<>(), "Null");
    component_benchmark_decode(fast_ber::Real<>(0.333333333333333314829616256247390992939472198486328125), "Real");
    component_benchmark_decode(fast_ber::ObjectIdentifier<>(fast_ber::ObjectIdentifierComponents{1, 2, 840, 113549}),
                               "ObjectIdentifier");
    component_benchmark_decode(fast_ber::Optional<fast_ber::OctetString<>>("hello!"), "Optional (String)");
    component_benchmark_decode(fast_ber::Optional<fast_ber::Integer<>>(500), "Optional (Integer)");
    component_benchmark_decode(fast_ber::Optional<fast_ber::Integer<>>(absl::nullopt), "Optional (Empty)");
    component_benchmark_decode(fast_ber::All::The_Choice<>(fast_ber::OctetString<>("hello!")), "Choice (String)");
    component_benchmark_decode(fast_ber::All::The_Choice<>(5), "Choice (Integer)");
}

TEST_CASE("Component Performance: Object Construction")
{
    component_benchmark_construct<fast_ber::Integer<>>(-99999999, "Integer");
    component_benchmark_construct<fast_ber::Default<fast_ber::Integer<>, IntegerDefault>>(-99999999, "Default Integer");
    component_benchmark_construct<fast_ber::Boolean<>>(true, "Boolean");
    component_benchmark_construct<fast_ber::OctetString<>>("Test string!", "OctetString");
    component_benchmark_construct<fast_ber::Default<fast_ber::OctetString<>, StringDefault>>("Test string!",
                                                                                             "Default String");
    component_benchmark_construct<fast_ber::Null<>>(fast_ber::Null<>{}, "Null");
    component_benchmark_construct<fast_ber::Real<>>(0.333333333333333314829616256247390992939472198486328125, "Real");
    component_benchmark_construct<fast_ber::ObjectIdentifier<>>(fast_ber::ObjectIdentifierComponents{1, 2, 840, 113549},
                                                                "ObjectIdentifier");
    component_benchmark_construct<fast_ber::Optional<fast_ber::OctetString<>>>("hello!", "Optional (String)");
    component_benchmark_construct<fast_ber::Optional<fast_ber::Integer<>>>(500, "Optional (Integer)");
    component_benchmark_construct<fast_ber::Optional<fast_ber::Integer<>>>(absl::nullopt, "Optional (Empty)");
    component_benchmark_construct<fast_ber::Choice<fast_ber::Choices<fast_ber::Integer<>, fast_ber::OctetString<>>>>(
        fast_ber::OctetString<>("hello!"), "Choice (String)");
    component_benchmark_construct<fast_ber::Choice<
        fast_ber::Choices<fast_ber::Integer<>, fast_ber::OctetString<>>,
        fast_ber::ChoiceId<fast_ber::Identifier<fast_ber::Integer<>>, fast_ber::Identifier<fast_ber::OctetString<>>>,
        fast_ber::StorageMode::dynamic>>(fast_ber::OctetString<>("hello!"), "Choice (String, D)");
    component_benchmark_construct<fast_ber::Choice<fast_ber::Choices<fast_ber::Integer<>, fast_ber::OctetString<>>>>(
        fast_ber::Integer<>(5), "Choice (Integer)");
}

TEST_CASE("Component Performance: Default Construction")
{
    component_benchmark_default_construct<fast_ber::Integer<>>("Integer");
    component_benchmark_default_construct<fast_ber::Default<fast_ber::Integer<>, IntegerDefault>>("Default Int");
    component_benchmark_default_construct<fast_ber::Boolean<>>("Boolean");
    component_benchmark_default_construct<fast_ber::OctetString<>>("OctetString");
    component_benchmark_default_construct<fast_ber::Default<fast_ber::OctetString<>, StringDefault>>("Default Str");
    component_benchmark_default_construct<fast_ber::Null<>>("Null");
    component_benchmark_default_construct<fast_ber::Real<>>("Real");
    component_benchmark_default_construct<fast_ber::ObjectIdentifier<>>("ObjectId");
    component_benchmark_default_construct<fast_ber::Optional<fast_ber::Integer<>>>("Optional");
    component_benchmark_default_construct<
        fast_ber::Choice<fast_ber::Choices<fast_ber::Integer<>, fast_ber::OctetString<>>>>("Choice");
}
