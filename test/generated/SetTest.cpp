#include "autogen/set.hpp"

#include "catch2/catch.hpp"

#include <array>

TEST_CASE("Set: Empty")
{
    std::array<uint8_t, 100> buffer = {};
    fast_ber::Set_::Empty    set    = {};

    fast_ber::EncodeResult encode_result = fast_ber::encode(absl::MakeSpan(buffer.data(), buffer.size()), set);
    fast_ber::DecodeResult decode_result = fast_ber::decode(absl::MakeSpan(buffer.data(), buffer.size()), set);

    REQUIRE(encode_result.success);
    REQUIRE(decode_result.success);
}

TEST_CASE("Set: Round Trip")
{
    std::array<uint8_t, 100> buffer = {};
    fast_ber::Set_::Set_     set    = {{}, 5, false, "set"};
    fast_ber::Set_::Set_     copy   = {{}, -5, true, "myset"};

    REQUIRE(set != copy);

    fast_ber::EncodeResult encode_result = fast_ber::encode(absl::MakeSpan(buffer.data(), buffer.size()), set);
    fast_ber::DecodeResult decode_result = fast_ber::decode(absl::MakeSpan(buffer.data(), buffer.size()), copy);

    REQUIRE(encode_result.success);
    REQUIRE(decode_result.success);
    REQUIRE(set == copy);
}

TEST_CASE("Set: Round Trip Duplicate Tag Okay")
{
    // Check that set can be decoded with two members with match tags but different classes
    std::array<uint8_t, 100>                          buffer = {};
    fast_ber::Set_::SetWithDuplicateTagDifferentClass set    = {1, 2};
    fast_ber::Set_::SetWithDuplicateTagDifferentClass copy   = {0, 0};

    REQUIRE(set != copy);

    fast_ber::EncodeResult encode_result = fast_ber::encode(absl::MakeSpan(buffer.data(), buffer.size()), set);
    fast_ber::DecodeResult decode_result = fast_ber::decode(absl::MakeSpan(buffer.data(), buffer.size()), copy);

    REQUIRE(encode_result.success);
    REQUIRE(decode_result.success);
    REQUIRE(set == copy);
}

TEST_CASE("Set: Decode Ordered")
{
    // Check that set can be decoded with encoded members matching order of definition
    std::array<uint8_t, 100> buffer = {};
    fast_ber::Set_::Set_     set    = {};
    fast_ber::Null<>         null;
    fast_ber::Integer<>      integer    = 50;
    fast_ber::Boolean<>      boolean    = true;
    fast_ber::UTF8String<>   utf_string = "Decode Ordered";

    auto inner_buffer = absl::Span<uint8_t>(buffer);

    inner_buffer.remove_prefix(null.encode(inner_buffer).length);
    inner_buffer.remove_prefix(integer.encode(inner_buffer).length);
    inner_buffer.remove_prefix(boolean.encode(inner_buffer).length);
    inner_buffer.remove_prefix(utf_string.encode(inner_buffer).length);

    size_t content_length = buffer.size() - inner_buffer.size();
    fast_ber::wrap_with_ber_header(absl::Span<uint8_t>(buffer), content_length,
                                   fast_ber::ExplicitId<fast_ber::UniversalTag::set>());

    fast_ber::DecodeResult decode_result = fast_ber::decode(absl::MakeSpan(buffer.data(), buffer.size()), set);
    REQUIRE(decode_result.success);

    REQUIRE(set.null == null);
    REQUIRE(set.integer == integer);
    REQUIRE(set.boolean == boolean);
    REQUIRE(set.string == utf_string);
}

TEST_CASE("Set: Decode Unordered")
{
    // Check that set can be decoded with encoded members not matching order of definition
    std::array<uint8_t, 100> buffer = {};
    fast_ber::Set_::Set_     set    = {};
    fast_ber::Null<>         null;
    fast_ber::Integer<>      integer    = 50;
    fast_ber::Boolean<>      boolean    = true;
    fast_ber::UTF8String<>   utf_string = "Decode Ordererd";

    auto inner_buffer = absl::Span<uint8_t>(buffer);

    inner_buffer.remove_prefix(utf_string.encode(inner_buffer).length);
    inner_buffer.remove_prefix(null.encode(inner_buffer).length);
    inner_buffer.remove_prefix(integer.encode(inner_buffer).length);
    inner_buffer.remove_prefix(boolean.encode(inner_buffer).length);

    size_t content_length = buffer.size() - inner_buffer.size();
    fast_ber::wrap_with_ber_header(absl::Span<uint8_t>(buffer), content_length,
                                   fast_ber::ExplicitId<fast_ber::UniversalTag::set>());

    fast_ber::DecodeResult decode_result = fast_ber::decode(absl::MakeSpan(buffer.data(), buffer.size()), set);
    REQUIRE(decode_result.success);

    REQUIRE(set.null == null);
    REQUIRE(set.integer == integer);
    REQUIRE(set.boolean == boolean);
    REQUIRE(set.string == utf_string);
}

TEST_CASE("Set: Decode Optional Members")
{
    // Check that set can be decoded with optional members not set
    std::array<uint8_t, 100> buffer = {};
    fast_ber::Set_::Set_     set    = {};
    fast_ber::Null<>         null;
    fast_ber::Integer<>      integer    = 50;
    fast_ber::UTF8String<>   utf_string = "Decode Ordererd";

    auto inner_buffer = absl::Span<uint8_t>(buffer);

    inner_buffer.remove_prefix(null.encode(inner_buffer).length);
    inner_buffer.remove_prefix(integer.encode(inner_buffer).length);
    inner_buffer.remove_prefix(utf_string.encode(inner_buffer).length);

    size_t content_length = buffer.size() - inner_buffer.size();
    fast_ber::wrap_with_ber_header(absl::Span<uint8_t>(buffer), content_length,
                                   fast_ber::ExplicitId<fast_ber::UniversalTag::set>());

    fast_ber::DecodeResult decode_result = fast_ber::decode(absl::MakeSpan(buffer.data(), buffer.size()), set);
    REQUIRE(decode_result.success);

    REQUIRE(set.null == null);
    REQUIRE(set.integer == integer);
    REQUIRE(set.boolean == fast_ber::empty);
    REQUIRE(set.string == utf_string);
}

TEST_CASE("Set: Decode Automatic Tags")
{
    // Check that set can be decoded with generated tag numbers
    std::array<uint8_t, 100>                                                 buffer = {};
    fast_ber::SetAutomatic::Set_                                             set    = {};
    fast_ber::Null<fast_ber::Id<fast_ber::Class::context_specific, 0>>       null;
    fast_ber::Integer<fast_ber::Id<fast_ber::Class::context_specific, 1>>    integer    = 50;
    fast_ber::Boolean<fast_ber::Id<fast_ber::Class::context_specific, 2>>    boolean    = 50;
    fast_ber::UTF8String<fast_ber::Id<fast_ber::Class::context_specific, 3>> utf_string = "Decode Ordererd";

    auto inner_buffer = absl::Span<uint8_t>(buffer);

    inner_buffer.remove_prefix(null.encode(inner_buffer).length);
    inner_buffer.remove_prefix(integer.encode(inner_buffer).length);
    inner_buffer.remove_prefix(boolean.encode(inner_buffer).length);
    inner_buffer.remove_prefix(utf_string.encode(inner_buffer).length);

    size_t content_length = buffer.size() - inner_buffer.size();
    fast_ber::wrap_with_ber_header(absl::Span<uint8_t>(buffer), content_length,
                                   fast_ber::ExplicitId<fast_ber::UniversalTag::set>());

    fast_ber::DecodeResult decode_result = fast_ber::decode(absl::MakeSpan(buffer.data(), buffer.size()), set);
    REQUIRE(decode_result.success);

    REQUIRE(set.null == null);
    REQUIRE(set.integer == integer);
    REQUIRE(set.boolean == boolean);
    REQUIRE(set.string == utf_string);
}

TEST_CASE("Set: Decode Extension")
{
    // Check that unknown tags are ignored when decoding set with extensions
    std::array<uint8_t, 100>          buffer = {};
    fast_ber::Set_::SetWithExtensions set    = {};
    fast_ber::Null<>                  null;
    fast_ber::Integer<>               integer    = 50;
    fast_ber::Boolean<>               boolean    = true;
    fast_ber::UTF8String<>            utf_string = "Decode Ordered";
    fast_ber::Real<>                  extension  = 55;

    auto inner_buffer = absl::Span<uint8_t>(buffer);

    inner_buffer.remove_prefix(null.encode(inner_buffer).length);
    inner_buffer.remove_prefix(integer.encode(inner_buffer).length);
    inner_buffer.remove_prefix(boolean.encode(inner_buffer).length);
    inner_buffer.remove_prefix(utf_string.encode(inner_buffer).length);
    inner_buffer.remove_prefix(extension.encode(inner_buffer).length);

    size_t content_length = buffer.size() - inner_buffer.size();
    fast_ber::wrap_with_ber_header(absl::Span<uint8_t>(buffer), content_length,
                                   fast_ber::ExplicitId<fast_ber::UniversalTag::set>());

    fast_ber::DecodeResult decode_result = fast_ber::decode(absl::MakeSpan(buffer.data(), buffer.size()), set);
    REQUIRE(decode_result.success);

    REQUIRE(set.null == null);
    REQUIRE(set.integer == integer);
    REQUIRE(set.boolean == boolean);
    REQUIRE(set.string == utf_string);
}

TEST_CASE("Set: Decode Fail - Missing Members")
{
    // Check that set fails to decode if non optional members are missing
    std::array<uint8_t, 100> buffer = {};
    fast_ber::Set_::Set_     set    = {};
    fast_ber::Null<>         null;
    fast_ber::Boolean<>      boolean    = false;
    fast_ber::UTF8String<>   utf_string = "Decode Ordererd";

    auto inner_buffer = absl::Span<uint8_t>(buffer);

    inner_buffer.remove_prefix(null.encode(inner_buffer).length);
    inner_buffer.remove_prefix(boolean.encode(inner_buffer).length);
    inner_buffer.remove_prefix(utf_string.encode(inner_buffer).length);

    size_t content_length = buffer.size() - inner_buffer.size();
    fast_ber::wrap_with_ber_header(absl::Span<uint8_t>(buffer), content_length,
                                   fast_ber::ExplicitId<fast_ber::UniversalTag::set>());

    fast_ber::DecodeResult decode_result = fast_ber::decode(absl::MakeSpan(buffer.data(), buffer.size()), set);
    REQUIRE(!decode_result.success);
}

TEST_CASE("Set: Decode Fail - Duplicate Members")
{
    // Check that set fails to decode if members are duplicated
    std::array<uint8_t, 100> buffer = {};
    fast_ber::Set_::Set_     set    = {};
    fast_ber::Null<>         null;
    fast_ber::Integer<>      integer    = 50;
    fast_ber::Boolean<>      boolean    = false;
    fast_ber::UTF8String<>   utf_string = "Decode Ordererd";

    auto inner_buffer = absl::Span<uint8_t>(buffer);

    inner_buffer.remove_prefix(null.encode(inner_buffer).length);
    inner_buffer.remove_prefix(integer.encode(inner_buffer).length);
    inner_buffer.remove_prefix(integer.encode(inner_buffer).length);
    inner_buffer.remove_prefix(boolean.encode(inner_buffer).length);
    inner_buffer.remove_prefix(utf_string.encode(inner_buffer).length);

    size_t content_length = buffer.size() - inner_buffer.size();
    fast_ber::wrap_with_ber_header(absl::Span<uint8_t>(buffer), content_length,
                                   fast_ber::ExplicitId<fast_ber::UniversalTag::set>());

    fast_ber::DecodeResult decode_result = fast_ber::decode(absl::MakeSpan(buffer.data(), buffer.size()), set);
    REQUIRE(!decode_result.success);
}
