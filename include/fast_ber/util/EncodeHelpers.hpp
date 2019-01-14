#pragma once

#include <cstddef>

#include "absl/types/span.h"

namespace fast_ber
{

class Boolean;
class Identifier;
class Integer;
class Null;
class OctetString;

struct EncodeResult
{
    bool   success;
    size_t length;
};

inline EncodeResult wrap_with_ber_header(absl::Span<uint8_t> output, Class class_, Tag tag,
                                         size_t content_length) noexcept
{
    std::array<uint8_t, 30> buffer;
    size_t header_length = create_header(absl::MakeSpan(buffer.data(), buffer.size()), Construction::constructed,
                                         class_, tag, content_length);
    assert(header_length != 0);
    if (header_length + content_length > output.length())
    {
        return EncodeResult{false, 0};
    }

    std::memmove(output.data() + header_length, output.data(), content_length);
    std::memcpy(output.data(), buffer.data(), header_length);
    return EncodeResult{true, header_length + content_length};
}

template <typename T, UniversalTag T2>
EncodeResult encode_with_specific_id_impl(absl::Span<uint8_t> output, const T& object, const ExplicitIdentifier<T2>& id)
{
    size_t id_length = create_identifier(output, Construction::primitive, id.class_(), id.tag());
    if (id_length == 0 || id_length > output.size())
    {
        return EncodeResult{false, 0};
    }

    output.remove_prefix(id_length);

    size_t encode_length = object.encode_content_and_length(output);
    return EncodeResult{encode_length > 0, id_length + encode_length};
}

template <typename T, Class T2, Tag T3, typename T4>
EncodeResult encode_with_specific_id_impl(absl::Span<uint8_t> output, const T& object,
                                          const TaggedExplicitIdentifier<T2, T3, T4>& id)
{
    size_t encode_length = object.encode(output);
    if (encode_length == 0)
    {
        return EncodeResult{false, 0};
    }

    return wrap_with_ber_header(output, id.outer_class(), id.outer_tag(), encode_length);
}

template <typename T, Class T2, Tag T3>
EncodeResult encode_with_specific_id_impl(absl::Span<uint8_t> output, const T& object,
                                          const ImplicitIdentifier<T2, T3>& id)
{
    size_t id_length = create_identifier(output, Construction::primitive, id.class_(), id.tag());
    if (id_length == 0 || id_length > output.size())
    {
        return EncodeResult{false, 0};
    }

    output.remove_prefix(id_length);

    size_t encode_length = object.encode_content_and_length(output);
    return EncodeResult{encode_length > 0, id_length + encode_length};
}

template <typename ID>
EncodeResult encode_with_specific_id(absl::Span<uint8_t> output, const Integer& object, const ID& id)
{
    return encode_with_specific_id_impl(output, object, id);
}

template <typename ID>
EncodeResult encode_with_specific_id(absl::Span<uint8_t> output, const OctetString& object, const ID& id)
{
    return encode_with_specific_id_impl(output, object, id);
}

template <typename ID>
EncodeResult encode_with_specific_id(absl::Span<uint8_t> output, const Boolean& object, const ID& id)
{
    return encode_with_specific_id_impl(output, object, id);
}

template <typename ID>
EncodeResult encode_with_specific_id(absl::Span<uint8_t> output, const Null& object, const ID& id)
{
    return encode_with_specific_id_impl(output, object, id);
}

} // namespace fast_ber
