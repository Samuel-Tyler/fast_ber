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

template <typename T, StorageMode storage = StorageMode::dynamic>
struct Optional : public OptionalImplementation<T, storage>::Type
{
    using Implementation = typename OptionalImplementation<T, storage>::Type;
    using Implementation::Implementation;

    const Implementation& base() const { return *static_cast<const Implementation*>(this); }

    Optional() : Implementation() {}
    Optional(const Implementation& rhs) : Implementation(rhs) {}
    Optional(Implementation&& rhs) : Implementation(rhs) {}
};

template <typename T>
struct IdentifierType<Optional<T>>
{
    using type = Identifier<T>;
};

template <typename T, StorageMode s1>
EncodeResult encode(absl::Span<uint8_t> buffer, const Optional<T, s1>& optional_type) noexcept
{
    if (optional_type.has_value())
    {
        return encode(buffer, *optional_type);
    }
    else
    {
        return {true, 0};
    }
}

template <typename T, StorageMode s1>
DecodeResult decode(BerViewIterator& input, Optional<T, s1>& output) noexcept
{
    if (input->is_valid() && input->tag() == Identifier<T>::tag() && input->class_() == Identifier<T>::class_())
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
std::ostream& operator<<(std::ostream& os, const Optional<T, s1>& optional)
{
    if (!optional)
    {
        return os << "null";
    }

    return os << *optional;
}
} // namespace fast_ber
