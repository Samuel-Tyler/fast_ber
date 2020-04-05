#pragma once

#include "fast_ber/util/DecodeHelpers.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"

#include "absl/types/optional.h"

#include <type_traits>

namespace fast_ber
{

template <typename T, typename DefaultValue>
struct Default
{
    static_assert(std::is_convertible<decltype(DefaultValue::value), T>::value, "Must be convertible");

    Default()                   = default;
    Default(const Default& rhs) = default;
    Default(Default&& rhs)      = default;
    Default(BerView view) { decode(view); }
    template <typename T2>
    Default(const T2& val) : m_item((val == DefaultValue::value) ? absl::optional<T>() : absl::optional<T>(val))
    {
    }

    Default& operator=(const Default&) = default;
    Default& operator=(Default&&) = default;
    template <typename T2>
    Default& operator=(const T2& val);

    T                     get() const noexcept { return is_default() ? T(DefaultValue::value) : *m_item; }
    decltype(T{}.value()) value() const noexcept(T{}.value()) { return get()->value(); }
    bool                  is_default() const noexcept { return !m_item; }
    void                  set_to_default() noexcept { m_item = absl::nullopt; }

    size_t       encoded_length() const noexcept;
    EncodeResult encode(absl::Span<uint8_t> buffer) const noexcept;
    DecodeResult decode(BerView input) noexcept;

  private:
    absl::optional<T> m_item;
};

template <typename T, typename DefaultValue>
struct IdentifierType<Default<T, DefaultValue>>
{
    using type = Identifier<T>;
};

template <typename T, typename DefaultValue>
template <typename T2>
Default<T, DefaultValue>& Default<T, DefaultValue>::operator=(const T2& val)
{
    if (val == DefaultValue::value)
    {
        m_item = absl::nullopt;
    }
    else
    {
        m_item = val;
    }
    return *this;
}

template <typename T, typename DefaultValue>
size_t Default<T, DefaultValue>::encoded_length() const noexcept
{
    if (is_default())
    {
        return 0;
    }
    else
    {
        return m_item->encoded_length();
    }
}

template <typename T, typename DefaultValue>
EncodeResult Default<T, DefaultValue>::encode(absl::Span<uint8_t> buffer) const noexcept
{
    if (is_default())
    {
        return {true, 0};
    }
    else
    {
        return m_item->encode(buffer);
    }
}

template <typename T, typename DefaultValue>
DecodeResult Default<T, DefaultValue>::decode(BerView input) noexcept
{
    if (input.is_valid() && Identifier<T>::check_id_match(input.class_(), input.tag()))
    {
        m_item = T();
        return m_item->decode(input);
    }
    else
    {
        return DecodeResult{false};
    }
}

template <typename T, typename DefaultValue>
size_t encoded_length(const Default<T, DefaultValue>& default_type) noexcept
{
    return default_type.encoded_length();
}

template <typename T, typename DefaultValue>
EncodeResult encode(absl::Span<uint8_t> buffer, const Default<T, DefaultValue>& default_type) noexcept
{
    return default_type.encode(buffer);
}

template <typename T, typename DefaultValue>
DecodeResult decode(BerViewIterator& input, Default<T, DefaultValue>& output) noexcept
{
    if (input->is_valid() && Identifier<T>::check_id_match(input->class_(), input->tag()))
    {
        return output.decode(*input);
    }
    else
    {
        output.set_to_default();
        return DecodeResult{true};
    }
}

template <typename T, typename DefaultValue>
std::ostream& operator<<(std::ostream& os, const Default<T, DefaultValue>& default_type)
{
    return os << default_type.get();
}

template <typename T, typename T2, typename DefaultValue>
bool operator==(const Default<T, DefaultValue>& lhs, const T2& rhs)
{
    return lhs.get() == rhs;
}

template <typename T, typename T2, typename DefaultValue>
bool operator==(const T& lhs, const Default<T2, DefaultValue>& rhs)
{
    return lhs == rhs.get();
}

template <typename T, typename DefaultValue, typename T2, typename DefaultValue2>
bool operator==(const Default<T, DefaultValue>& lhs, const Default<T2, DefaultValue2>& rhs)
{
    return lhs.get() == rhs.get();
}

template <typename T, typename T2, typename DefaultValue>
bool operator!=(const Default<T, DefaultValue>& lhs, const T2& rhs)
{
    return lhs.get() != rhs;
}

template <typename T, typename T2, typename DefaultValue>
bool operator!=(const T& lhs, const Default<T2, DefaultValue>& rhs)
{
    return lhs != rhs.get();
}

template <typename T, typename DefaultValue, typename T2, typename DefaultValue2>
bool operator!=(const Default<T, DefaultValue>& lhs, const Default<T2, DefaultValue2>& rhs)
{
    return lhs.get() != rhs.get();
}
  /*
template <typename T, typename U>
EncodeResult encode(absl::Span<uint8_t> output, const Default<T, U>& object) noexcept
{
    return object.encode(output);
}

template <typename T, typename U>
DecodeResult decode(BerViewIterator& input, Default<T, U>& output) noexcept
{
    DecodeResult res = output.decode(*input);
    ++input;
    return res;
}*/
} // namespace fast_ber
