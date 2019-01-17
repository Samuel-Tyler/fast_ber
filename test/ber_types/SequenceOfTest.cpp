#include "fast_ber/ber_types/Integer.hpp"
#include "fast_ber/ber_types/OctetString.hpp"
#include "fast_ber/ber_types/SequenceOf.hpp"

#include <catch2/catch.hpp>

#include <array>

template <typename T>
void test_sequences(const std::initializer_list<fast_ber::SequenceOf<T>>& sequences)
{
    std::array<uint8_t, 10000> buffer = {};
    for (const auto& sequence : sequences)
    {
        fast_ber::SequenceOf<T> copy;

        size_t encode_size = fast_ber::encode(absl::MakeSpan(buffer.data(), buffer.size()), sequence).length;
        fast_ber::BerViewIterator iter    = fast_ber::BerViewIterator(absl::MakeSpan(buffer.data(), encode_size));
        bool                      success = fast_ber::decode(iter, copy);
        REQUIRE(success);
        REQUIRE(copy.size() == sequence.size());
        REQUIRE(sequence == copy);
        REQUIRE(iter == fast_ber::BerViewIterator(fast_ber::End::end));
    }
}
TEST_CASE("SequenceOf: Encode decode integer")
{
    std::initializer_list<fast_ber::SequenceOf<fast_ber::Integer>> sequences{
        fast_ber::SequenceOf<fast_ber::Integer>{}, fast_ber::SequenceOf<fast_ber::Integer>{5},
        fast_ber::SequenceOf<fast_ber::Integer>{1, 2, 3},
        fast_ber::SequenceOf<fast_ber::Integer>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, -999, 999}};

    test_sequences(sequences);
}

TEST_CASE("SequenceOf: Encode decode string")
{
    std::initializer_list<fast_ber::SequenceOf<fast_ber::OctetString>> sequences = {
        {}, {"single"}, {"a series of strings"}};

    test_sequences(sequences);
}
