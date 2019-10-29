#pragma once

#include "absl/memory/memory.h"
#include "absl/types/optional.h"
#include "absl/types/span.h"

#include "fast_ber/ber_types/Class.hpp"
#include "fast_ber/ber_types/Identifier.hpp"
#include "fast_ber/ber_types/Tag.hpp"
#include "fast_ber/util/DecodeHelpers.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"

namespace fast_ber
{

using Empty                 = absl::nullopt_t;
constexpr static auto empty = absl::nullopt;

template <typename T>
using StaticOptional = absl::optional<T>;

template <typename T>
class DynamicOptional
{
  public:
    DynamicOptional() = default;
    DynamicOptional(const DynamicOptional& rhs) : m_val(rhs.m_val ? absl::make_unique<T>(*rhs.m_val) : nullptr) {}
    DynamicOptional(DynamicOptional&&) = default;
    DynamicOptional(Empty) {}
    DynamicOptional(const T& t) : m_val(absl::make_unique<T>(t)) {}
    template <typename First, typename... Args>
    DynamicOptional(First first, Args&&... args) : m_val(absl::make_unique<T>(first, args...))
    {
    }

    template <typename T2>
    DynamicOptional& operator=(const T2& t2)
    {
        m_val = absl::make_unique<T>(t2);
        return *this;
    }
    DynamicOptional& operator=(const T& t)
    {
        m_val.reset(new T(t));
        return *this;
    }
    DynamicOptional& operator=(const DynamicOptional& rhs)
    {
        m_val = (rhs.m_val ? absl::make_unique<T>(*rhs.m_val) : nullptr);
        return *this;
    }
    DynamicOptional& operator=(Empty)
    {
        m_val = nullptr;
        return *this;
    }
    DynamicOptional& operator=(DynamicOptional&&) = default;

    T&       operator*() { return *m_val; }
    T*       operator->() { return m_val.get(); }
    const T& operator*() const { return *m_val; }
    const T* operator->() const { return m_val.get(); }

    template <typename... Args>
    T& emplace(Args&&... args)
    {
        m_val = absl::make_unique<T>(args...);
        return *m_val;
    }
    bool has_value() const noexcept { return bool(m_val); }

    bool operator==(const DynamicOptional& rhs) const noexcept
    {
        if (this->has_value() != rhs.has_value())
        {
            return false;
        }

        if (!this->has_value())
        {
            return true;
        }

        return *m_val == *rhs.m_val;
    }
    bool operator==(const T& rhs) const noexcept
    {
        if (!this->has_value())
        {
            return false;
        }

        return *m_val == *rhs;
    }
    bool operator!=(const DynamicOptional& rhs) const noexcept { return !(*this == rhs); }
    bool operator!=(const T& rhs) const noexcept { return !(*this == rhs); }

  private:
    std::unique_ptr<T> m_val;
};

enum class StorageMode
{
    static_,
    small_buffer_optimised,
    dynamic,
};

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
    using Type = absl::optional<T>;
};

template <typename T, StorageMode storage = StorageMode::static_>
struct Optional : public OptionalImplementation<T, storage>::Type
{
    using Implementation = typename OptionalImplementation<T, storage>::Type;
    using Implementation::Implementation;
    using Implementation::operator=;
    const Implementation& base() const { return *static_cast<const Implementation*>(this); }

    Optional() : Implementation() {}
    Optional(const Implementation& rhs) : Implementation(rhs) {}
    Optional(Implementation&& rhs) : Implementation(rhs) {}
};

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

template <typename T, typename ID, StorageMode s1>
EncodeResult encode(absl::Span<uint8_t> buffer, const Optional<T, s1>& optional_type, const ID& id) noexcept
{
    if (optional_type.has_value())
    {
        return encode(buffer, *optional_type, id);
    }
    else
    {
        return {true, 0};
    }
}

template <typename T, StorageMode s1>
EncodeResult encode(absl::Span<uint8_t> buffer, const Optional<T, s1>& optional_type) noexcept
{
    constexpr auto id = identifier(static_cast<T*>(nullptr));
    return encode(buffer, optional_type, id);
}

template <typename T, typename ID, StorageMode s1>
DecodeResult decode(BerViewIterator& input, Optional<T, s1>& output, const ID& id) noexcept
{
    if (input->is_valid() && input->tag() == val(id.tag()))
    {
        output.emplace();
        return decode(input, *output, id);
    }
    else
    {
        output = empty;
        return DecodeResult{true};
    }
}

template <typename T, StorageMode s1>
DecodeResult decode(BerViewIterator& input, Optional<T, s1>& output) noexcept
{
    constexpr auto id = identifier(static_cast<T*>(nullptr));
    return decode(input, output, id);
}

template <typename T, StorageMode s1>
constexpr auto identifier(const Optional<T, s1>*, IdentifierAdlToken = IdentifierAdlToken{}) noexcept
    -> decltype(identifier(static_cast<T*>(nullptr)))
{
    return {};
}

template <typename T, StorageMode s1>
constexpr auto explicit_identifier(const Optional<T, s1>*, IdentifierAdlToken = IdentifierAdlToken{}) noexcept
    -> decltype(explicit_identifier(static_cast<T*>(nullptr)))
{
    return {};
}

} // namespace fast_ber
