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

    EncodeResult encode(absl::Span<uint8_t> output) const noexcept { return fast_ber::encode(output, m_type, tag()); }

    bool decode(absl::Span<const uint8_t> input) noexcept { return fast_ber::decode(input, m_type, tag()); }

  private:
    Type m_type;
};

template <typename Type, typename TagType, typename ReplacementTag = TagType>
EncodeResult encode(absl::Span<uint8_t> output, const TaggedType<Type, TagType>& tagged_type,
                    const ReplacementTag& tag = ReplacementTag()) noexcept
{
    return tagged_type.encode(output);
}

template <typename Type, typename TagType>
bool decode(absl::Span<const uint8_t> input, TaggedType<Type, TagType>& tagged_type) noexcept
{
    return tagged_type.decode(input);
}

} // namespace fast_ber
