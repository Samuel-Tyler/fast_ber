#include "fast_ber/ber_types/SequenceOf.hpp"

#include "fast_ber/ber_types/Boolean.hpp"
#include "fast_ber/ber_types/Integer.hpp"
#include "fast_ber/ber_types/Null.hpp"
#include "fast_ber/ber_types/OctetString.hpp"

#include <catch2/catch.hpp>

#include <array>

template <typename T, typename Identifier>
void test_sequences(const std::initializer_list<fast_ber::SequenceOf<T, Identifier>>& sequences)
{
    std::array<uint8_t, 10000> buffer = {};
    for (const auto& sequence : sequences)
    {
        fast_ber::SequenceOf<T, Identifier> copy;

        size_t                 expected_size = encoded_length(sequence);
        fast_ber::EncodeResult encode_result = fast_ber::encode(absl::MakeSpan(buffer.data(), buffer.size()), sequence);
        fast_ber::BerViewIterator iter = fast_ber::BerViewIterator(absl::MakeSpan(buffer.data(), encode_result.length));
        fast_ber::DecodeResult    decode_result = fast_ber::decode(iter, copy);

        CHECK(encode_result.success);
        CHECK(decode_result.success);
        CHECK(expected_size == encode_result.length);
        CHECK(copy.size() == sequence.size());
        CHECK(sequence == copy);
        CHECK(iter == fast_ber::BerViewIterator(fast_ber::End::end));
    }
}

TEST_CASE("SequenceOf: Encode decode integer")
{
    std::initializer_list<fast_ber::SequenceOf<fast_ber::Integer<>>> int_sequences{
        fast_ber::SequenceOf<fast_ber::Integer<>>{},
        fast_ber::SequenceOf<fast_ber::Integer<>>{5},
        fast_ber::SequenceOf<fast_ber::Integer<>>{1, 2, 3},
        fast_ber::SequenceOf<fast_ber::Integer<>>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, -999, 999},
    };

    test_sequences(int_sequences);
}

TEST_CASE("SequenceOf: Various tags")
{
    auto tagged_content =
        fast_ber::SequenceOf<fast_ber::Integer<fast_ber::Id<fast_ber::Class::context_specific, 55555555555555>>>{
            1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, -999, 999, 999999999};

    auto double_tagged_content = fast_ber::SequenceOf<fast_ber::Integer<fast_ber::DoubleId<
        fast_ber::Id<fast_ber::Class::context_specific, 10>, fast_ber::Id<fast_ber::Class::private_, 91239129321538>>>>{
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, -999, 999};

    auto tagged_sequence =
        fast_ber::SequenceOf<fast_ber::Integer<>, fast_ber::Id<fast_ber::Class::context_specific, 55555555555555>>{
            1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, -999, 999, 999999999};

    auto double_tagged_sequence =
        fast_ber::SequenceOf<fast_ber::Integer<>,
                             fast_ber::DoubleId<fast_ber::Id<fast_ber::Class::context_specific, 10>,
                                                fast_ber::Id<fast_ber::Class::private_, 91239129321538>>>{
            1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, -999, 999};

    test_sequences({tagged_content});
    test_sequences({double_tagged_content});
    test_sequences({tagged_sequence});
    test_sequences({double_tagged_sequence});
}

TEST_CASE("SequenceOf: Various types")
{
    test_sequences({fast_ber::SequenceOf<fast_ber::OctetString<>>{"", ";;", "!", "@"}});
    test_sequences({fast_ber::SequenceOf<fast_ber::Boolean<>>{true, false, true, true}});
    test_sequences({fast_ber::SequenceOf<fast_ber::Null<>>{{}, {}, {}, {}}});
}

TEST_CASE("SequenceOf: Encode decode string")
{
    std::initializer_list<fast_ber::SequenceOf<fast_ber::OctetString<>>> sequences = {
        {}, {"single"}, {"a series of strings"}};

    test_sequences(sequences);
}
