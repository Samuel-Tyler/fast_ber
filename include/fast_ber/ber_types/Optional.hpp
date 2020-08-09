#pragma once

#include "absl/memory/memory.h"
#include "absl/types/optional.h"
#include "absl/types/span.h"

#include "fast_ber/ber_types/Class.hpp"
#include "fast_ber/ber_types/Identifier.hpp"
#include "fast_ber/ber_types/Tag.hpp"
#include "fast_ber/util/DecodeHelpers.hpp"
#include "fast_ber/util/DynamicOptional.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"

namespace fast_ber
{

template <typename T>
using StaticOptional = absl::optional<T>;

template <typename T, StorageMode storage>
struct OptionalImplementation
{
    using Type = absl::optional<T>;
};

template <typename T>
struct OptionalImplementation<T, StorageMode::static_>
{
    using Type = absl::optional<T>;
};

template <typename T>
struct OptionalImplementation<T, StorageMode::dynamic>
{
    using Type = DynamicOptional<T>;
};

template <typename T, StorageMode storage = StorageMode::static_>
struct Optional : public OptionalImplementation<T, storage>::Type
{
    using Implementation = typename OptionalImplementation<T, storage>::Type;
    using Implementation::Implementation;
    using Implementation::operator=;

    const Implementation& base() const { return *static_cast<const Implementation*>(this); }

    Optional() noexcept           = default;
    Optional(const Optional& rhs) = default;
    Optional(Optional&& rhs) noexcept;
    Optional& operator=(const Optional& rhs) = default;
    Optional& operator                       =(Optional&& rhs) noexcept;
    ~Optional() noexcept                     = default;

    size_t       encoded_length() const noexcept;
    EncodeResult encode(absl::Span<uint8_t> buffer) const noexcept;
    DecodeResult decode(BerView input) noexcept;

    template <typename Identifier>
    size_t encoded_length_with_id() const noexcept;
    template <typename Identifier>
    EncodeResult encode_with_id(absl::Span<uint8_t> buffer) const noexcept;
    template <typename Identifier>
    DecodeResult decode_with_id(BerView input) noexcept;
};

template <typename T, StorageMode s1>
struct IdentifierType<Optional<T, s1>>
{
    using type = Identifier<T>;
};

template <typename T, StorageMode s1>
Optional<T, s1>::Optional(Optional<T, s1>&& rhs) noexcept : Implementation(std::move(rhs))
{
}

template <typename T, StorageMode s1>
Optional<T, s1>& Optional<T, s1>::operator=(Optional<T, s1>&& rhs) noexcept
{
    this->Implementation::operator=(std::move(rhs));
    return *this;
}

template <typename T, StorageMode s1>
size_t Optional<T, s1>::encoded_length() const noexcept
{
    if (this->has_value())
    {
        return (*this)->encoded_length();
    }
    else
    {
        return 0;
    }
}

template <typename T, StorageMode s1>
EncodeResult Optional<T, s1>::encode(absl::Span<uint8_t> buffer) const noexcept
{
    if (this->has_value())
    {
        return (*this)->encode(buffer);
    }
    else
    {
        return {true, 0};
    }
}

template <typename T, StorageMode s1>
DecodeResult decode(BerViewIterator& input, Optional<T, s1>& output) noexcept
{
    if (input->is_valid() && Identifier<T>::check_id_match(input->class_(), input->tag()))
    {
        output.emplace();
        return decode(input, *output);
    }
    else
    {
        output = empty;
        return DecodeResult{true};
    }
}

template <typename T, StorageMode s1>
DecodeResult Optional<T, s1>::decode(BerView input) noexcept
{
    if (input.is_valid() && Identifier<T>::check_id_match(input.class_(), input.tag()))
    {
        this->emplace();
        return (*this)->decode(input);
    }
    else if (!input.is_valid())
    {
        *this = empty;
        return DecodeResult{true};
    }
    else
    {
        return DecodeResult{false};
    }
}

template <typename T, StorageMode s1>
template <typename Identifier>
size_t Optional<T, s1>::encoded_length_with_id() const noexcept
{
    if (this->has_value())
    {
        return (*this)->template encoded_length_with_id<Identifier>();
    }
    else
    {
        return 0;
    }
}

template <typename T, StorageMode s1>
template <typename Identifier>
EncodeResult Optional<T, s1>::encode_with_id(absl::Span<uint8_t> buffer) const noexcept
{
    if (this->has_value())
    {
        return (*this)->template encode_with_id<Identifier>(buffer);
    }
    else
    {
        return {true, 0};
    }
}

template <typename T, StorageMode s1>
template <typename Identifier>
DecodeResult Optional<T, s1>::decode_with_id(BerView input) noexcept
{
    if (input.is_valid() && Identifier::check_id_match(input.class_(), input.tag()))
    {
        this->emplace();
        return (*this)->template decode_with_id<Identifier>(input);
    }
    else if (!input.is_valid())
    {
        *this = empty;
        return DecodeResult{true};
    }
    else
    {
        return DecodeResult{false};
    }
}

template <typename T, StorageMode s1>
std::ostream& operator<<(std::ostream& os, const Optional<T, s1>& optional)
{
    if (!optional)
    {
        return os << "null";
    }

    return os << *optional;
}

template <typename T, typename T2, StorageMode s1>
bool operator==(const Optional<T, s1>& lhs, const T2& rhs)
{
    return lhs.base() == rhs;
}

template <typename T, typename T2, StorageMode s1>
bool operator==(const T& lhs, const Optional<T2, s1>& rhs)
{
    return lhs == rhs.base();
}

template <typename T, typename T2, StorageMode s1, StorageMode s2>
bool operator==(const Optional<T, s1>& lhs, const Optional<T2, s2>& rhs)
{
    return lhs.base() == rhs.base();
}

template <typename T, typename T2, StorageMode s1>
bool operator!=(const Optional<T, s1>& lhs, const T2& rhs)
{
    return lhs.base() != rhs;
}

template <typename T, typename T2, StorageMode s1>
bool operator!=(const T& lhs, const Optional<T2, s1>& rhs)
{
    return lhs != rhs.base();
}

template <typename T, typename T2, StorageMode s1, StorageMode s2>
bool operator!=(const Optional<T, s1>& lhs, const Optional<T2, s2>& rhs)
{
    return lhs.base() != rhs.base();
}

} // namespace fast_ber
