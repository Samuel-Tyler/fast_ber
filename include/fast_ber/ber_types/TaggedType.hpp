#pragma once

#include "fast_ber/ber_types/Choice.hpp"
#include "fast_ber/util/DecodeHelpers.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"

namespace fast_ber
{

template <typename Type, typename TagType>
class TaggedType
{
  public:
    TaggedType() : m_type() {}

    template <typename T>
    TaggedType(T t) : m_type(t)
    {
    }

    // Implicitly convert to underlying (untagged) type
    operator const Type&() const noexcept { return m_type; }
    operator Type&() noexcept { return m_type; }

    bool operator==(const TaggedType& rhs) const noexcept { return this->m_type == rhs.m_type; }
    bool operator!=(const TaggedType& rhs) const noexcept { return !(*this == rhs); }

    static TagType tag() noexcept { return TagType{}; }

    template <typename T>
    EncodeResult encode(absl::Span<uint8_t> output, T replacement_tag) const noexcept
    {
        return fast_ber::encode(output, m_type, replacement_tag);
    }
    EncodeResult encode(absl::Span<uint8_t> output) const noexcept { return fast_ber::encode(output, m_type, tag()); }

    template <typename T>
    bool decode(absl::Span<const uint8_t> input, T replacement_tag) noexcept
    {
        return fast_ber::decode(input, m_type, replacement_tag);
    }
    bool decode(absl::Span<const uint8_t> input) noexcept { return fast_ber::decode(input, m_type, tag()); }

  private:
    Type m_type;
};

template <typename Type, typename TagType>
EncodeResult encode(absl::Span<uint8_t> output, const TaggedType<Type, TagType>& tagged_type) noexcept
{
    return tagged_type.encode(output);
}

template <typename Type, typename TagType, typename ReplacementTag>
EncodeResult encode(absl::Span<uint8_t> output, const TaggedType<Type, TagType>& tagged_type,
                    const ReplacementTag& tag) noexcept
{
    return tagged_type.encode(output, tag);
}

template <typename Type, typename TagType>
bool decode(absl::Span<const uint8_t> input, TaggedType<Type, TagType>& tagged_type) noexcept
{
    return tagged_type.decode(input);
}

template <typename Type, typename TagType, typename ReplacementTag>
bool decode(absl::Span<const uint8_t> input, TaggedType<Type, TagType>& tagged_type, const ReplacementTag& tag) noexcept
{
    return tagged_type.decode(input, tag);
}

} // namespace fast_ber
