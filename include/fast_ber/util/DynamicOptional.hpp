#pragma once

#include "absl/memory/memory.h"
#include "absl/types/optional.h"

namespace fast_ber
{

using Empty                 = absl::nullopt_t;
constexpr static auto empty = absl::nullopt;

template <typename T>
class DynamicOptional
{
  public:
    DynamicOptional() = default;
    DynamicOptional(const DynamicOptional& rhs) noexcept : m_val(rhs.m_val ? absl::make_unique<T>(*rhs.m_val) : nullptr)
    {
    }
    DynamicOptional(DynamicOptional&&) = default;
    DynamicOptional(Empty) {}
    template <typename U = T,
              typename std::enable_if<
                  absl::conjunction<absl::negation<std::is_same<absl::in_place_t, typename std::decay<U>::type>>,
                                    absl::negation<std::is_same<DynamicOptional<T>, typename std::decay<U>::type>>,
                                    std::is_convertible<U&&, T>, std::is_constructible<T, U&&>>::value,
                  bool>::type = false>
    DynamicOptional(const U& t) : m_val(absl::make_unique<T>(t))
    {
    }

    template <typename T2>
    DynamicOptional& operator=(const T2& t2)
    {
        m_val = absl::make_unique<T>(t2);
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
             operator bool() const { return bool(m_val); }

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

} // namespace fast_ber
