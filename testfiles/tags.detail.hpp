
/* Functionality provided for Encoding and Decoding BER */

namespace CollectionTags {
static const auto hello = ExplicitIdentifier{UniversalTag::octet_string};
static const auto goodbye = ExplicitIdentifier{UniversalTag::octet_string};
static const auto integer = ExplicitTag{context_specific, 2, ExplicitIdentifier{UniversalTag::integer}};
static const auto bool1 = ImplicitTag{context_specific, 3};
static const auto bool2 = ExplicitTag{private, 4, ExplicitIdentifier{UniversalTag::boolean}};
static const auto bool3 = ImplicitTag{application, 5};
}

inline EncodeResult encode_with_specific_id(absl::Span<uint8_t> output, const Collection& input, const ExplicitIdentifier& id) noexcept
{
    return encode_combine(output, id,
                          input.hello, CollectionTags::hello,
                          input.goodbye, CollectionTags::goodbye,
                          input.integer, CollectionTags::integer,
                          input.bool1, CollectionTags::bool1,
                          input.bool2, CollectionTags::bool2,
                          input.bool3, CollectionTags::bool3);
}

inline EncodeResult encode(absl::Span<uint8_t> output, const Collection& input) noexcept
{
    return encode_with_specific_id(output, input, ExplicitIdentifier{UniversalTag::sequence_of});
}

constexpr const char Collection_name[] = "Collection";
inline bool decode_with_specific_id(const BerView& input, Collection& output, const ExplicitIdentifier& id) noexcept
{
    return decode_combine<Collection_name>(input, id,
                          output.hello, CollectionTags::hello,
                          output.goodbye, CollectionTags::goodbye,
                          output.integer, CollectionTags::integer,
                          output.bool1, CollectionTags::bool1,
                          output.bool2, CollectionTags::bool2,
                          output.bool3, CollectionTags::bool3);
}

inline bool decode_with_specific_id(BerViewIterator& input, Collection& output, const ExplicitIdentifier& id) noexcept
{
    bool success = decode_with_specific_id(*input, output, id) > 0;
    ++input;
    return success;
}

inline bool decode(absl::Span<const uint8_t> input, Collection& output) noexcept
{
    return decode_with_specific_id(BerView(input), output, ExplicitIdentifier{UniversalTag::sequence_of});
}


