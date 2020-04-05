#pragma once

#include "fast_ber/util/DecodeHelpers.hpp"
#include "fast_ber/util/Definitions.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"

#include "absl/container/inlined_vector.h"

#include <iosfwd>
#include <numeric>
#include <vector>

namespace fast_ber
{

constexpr const size_t default_inlined_size = 5;

template <typename T, StorageMode storage>
struct SequenceOfImplementation
{
    using Type = absl::InlinedVector<T, default_inlined_size>;
};

template <typename T>
struct SequenceOfImplementation<T, StorageMode::small_buffer_optimised>
{
    using Type = absl::InlinedVector<T, default_inlined_size>;
};

template <typename T>
struct SequenceOfImplementation<T, StorageMode::dynamic>
{
    using Type = std::vector<T>;
};

template <typename T, typename I = ExplicitId<UniversalTag::sequence>,
          StorageMode s = StorageMode::small_buffer_optimised>
struct SequenceOf : public SequenceOfImplementation<T, s>::Type
{
    using Implementation = typename SequenceOfImplementation<T, s>::Type;

    using Implementation::Implementation;
    SequenceOf() = default;
    SequenceOf(const Implementation& t) : Implementation(t) {}
    SequenceOf(Implementation&& t) : Implementation(std::move(t)) {}
    template <typename I2, StorageMode s2>
    SequenceOf(const SequenceOf<T, I2, s2>& t) : Implementation(t.begin(), t.end())
    {
    }

    size_t       encoded_length() const noexcept;
    EncodeResult encode(absl::Span<uint8_t> buffer) const noexcept;
    DecodeResult decode(absl::Span<const uint8_t> buffer) noexcept;

    using AsnId = I;
};

template <typename T, typename I1, StorageMode s1, typename I2, StorageMode s2>
bool operator==(const SequenceOf<T, I1, s1>& lhs, const SequenceOf<T, I2, s2>& rhs) noexcept
{
    return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename T, typename I1, StorageMode s1, typename I2, StorageMode s2>
bool operator!=(const SequenceOf<T, I1, s1>& lhs, const SequenceOf<T, I2, s2>& rhs) noexcept
{
    return !(lhs == rhs);
}

template <typename T, typename I, StorageMode s>
size_t SequenceOf<T, I, s>::encoded_length() const noexcept
{
    const size_t content_length = std::accumulate(this->begin(), this->end(), size_t(0),
                                                  [](size_t count, const T& t) { return count + t.encoded_length(); });
    return fast_ber::encoded_length(content_length, I{});
}

template <typename T, typename I, StorageMode s>
EncodeResult SequenceOf<T, I, s>::encode(const absl::Span<uint8_t> buffer) const noexcept
{
    constexpr size_t header_length_guess = fast_ber::encoded_length(0, I{});
    auto             content_buffer      = buffer;
    size_t           combined_length     = 0;
    if (content_buffer.length() < header_length_guess)
    {
        return EncodeResult{false, 0};
    }
    content_buffer.remove_prefix(header_length_guess);

    for (const T& element : *this)
    {
        const auto element_encode_result = element.encode(content_buffer);
        if (!element_encode_result.success)
        {
            return {false, 0};
        }
        combined_length += element_encode_result.length;
        content_buffer.remove_prefix(element_encode_result.length);
    }

    return wrap_with_ber_header(buffer, combined_length, I{}, header_length_guess);
}

template <typename T, typename I, StorageMode s>
DecodeResult decode(BerViewIterator& input, SequenceOf<T, I, s>& output) noexcept
{
    output.clear();
    if (!has_correct_header(*input, I{}, Construction::constructed))
    {
        return DecodeResult{false};
    }

    BerViewIterator         child = (I::depth() == 1) ? input->begin() : input->begin()->begin();
    constexpr Identifier<T> child_id;

    while (child->is_valid() && child_id.check_id_match(child->class_(), child->tag()))
    {
        output.emplace_back();
        bool success = decode(child, output.back()).success;
        if (!success)
        {
            return DecodeResult{false};
        }
    }
    ++input;

    return DecodeResult{true};
}

template <typename T, typename I, StorageMode s1>
std::ostream& operator<<(std::ostream& os, const SequenceOf<T, I, s1>& sequence)
{
    bool first = true;

    os << "[";
    for (const T& member : sequence)
    {
        if (!first)
        {
            os << ", ";
        }
        os << member;
        first = false;
    }
    return os << "]";
}

} // namespace fast_ber
