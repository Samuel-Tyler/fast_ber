#pragma once

#include "fast_ber/util/BerLengthAndContentContainer.hpp"

#include "absl/strings/string_view.h"

#include <chrono>
#include <ctime>

namespace fast_ber
{
constexpr const int minimum_timestamp_length = 10;
constexpr const int max_timestamp_length     = 23;

class GeneralizedTime
{
  public:
    template <typename T>
    void set_local_time(const std::chrono::time_point<T>& time);
    template <typename T>
    void set_local_time(const std::chrono::time_point<T>& time, int utc_offset_hours, int utc_offset_minutes);
    template <typename T>
    void set_universal_time(const std::chrono::time_point<T>& time);

    bool                                                        local_time_available() const;
    bool                                                        universal_time_available() const;
    std::chrono::time_point<std::chrono::high_resolution_clock> local_time() const;
    std::chrono::time_point<std::chrono::high_resolution_clock> universal_time() const;
    std::string                                                 string() const;

    size_t       assign_ber(const BerView& view) noexcept;
    EncodeResult encode_content_and_length(absl::Span<uint8_t> buffer) const noexcept;

    template <typename T>
    GeneralizedTime(const std::chrono::time_point<T>& time)
    {
        set_universal_time(time);
    }
    GeneralizedTime() { set_universal_time(std::chrono::time_point<std::chrono::high_resolution_clock>()); }

  private:
    BerLengthAndContentContainer m_contents;
};

inline bool operator==(const GeneralizedTime& lhs, const GeneralizedTime& rhs)
{
    if (lhs.universal_time_available() && rhs.universal_time_available())
    {
        return lhs.universal_time() == rhs.universal_time();
    }
    if (lhs.local_time_available() && rhs.local_time_available())
    {
        return lhs.local_time() == rhs.local_time();
    }
    return false;
}

template <typename T>
void GeneralizedTime::set_local_time(const std::chrono::time_point<T>& time)
{
    std::array<char, max_timestamp_length + 1> buffer;
    const std::time_t                          ctime  = std::chrono::high_resolution_clock::to_time_t(time);
    const std::tm*                             ctime2 = std::gmtime(&ctime);

    strftime(buffer.data(), buffer.size(), "%Y%m%d%H%M%S", ctime2);
    m_contents.assign_content(
        absl::Span<const uint8_t>(reinterpret_cast<const uint8_t*>(buffer.data()), strlen(buffer.data())));
    assert(local_time_available());
}

template <typename T>
void GeneralizedTime::set_universal_time(const std::chrono::time_point<T>& time)
{
    std::array<char, max_timestamp_length + 1> buffer;
    const std::time_t                          ctime  = std::chrono::high_resolution_clock::to_time_t(time);
    const std::tm*                             ctime2 = std::gmtime(&ctime);
    strftime(buffer.data(), buffer.size(), "%Y%m%d%H%M%SZ", ctime2);
    m_contents.assign_content(
        absl::Span<const uint8_t>(reinterpret_cast<const uint8_t*>(buffer.data()), strlen(buffer.data())));
    assert(universal_time_available());
}

inline bool GeneralizedTime::local_time_available() const { return m_contents.content().back() != 'Z'; }

inline bool GeneralizedTime::universal_time_available() const
{
    return (m_contents.content().back() == 'Z') || (m_contents.content()[m_contents.content_length() - 5] == '-') ||
           (m_contents.content()[m_contents.content_length() - 5] == '+');
}

inline std::chrono::time_point<std::chrono::high_resolution_clock> GeneralizedTime::local_time() const
{
    assert(local_time_available());
    std::array<char, max_timestamp_length + 1> buffer = {};
    tm                                         time   = {};
    std::memcpy(buffer.data(), m_contents.content_data(), m_contents.content_length());
    strptime(buffer.data(), "%Y%m%d%H%M%S", &time);
    return std::chrono::high_resolution_clock::from_time_t(timegm(&time));
}

inline std::chrono::time_point<std::chrono::high_resolution_clock> GeneralizedTime::universal_time() const
{
    assert(universal_time_available());
    std::array<char, max_timestamp_length + 1> buffer = {};
    tm                                         time   = {};
    std::memcpy(buffer.data(), m_contents.content_data(), m_contents.content_length());
    strptime(buffer.data(), "%Y%m%d%H%M%SZ", &time);
    return std::chrono::high_resolution_clock::from_time_t(timegm(&time));
}

inline std::string GeneralizedTime::string() const
{
    return std::string(reinterpret_cast<const char*>(m_contents.content_data()), m_contents.content_length());
}

inline size_t GeneralizedTime::assign_ber(const BerView& view) noexcept
{
    if (!view.is_valid() || view.construction() != Construction::primitive)
    {
        return false;
    }
    if (view.content_length() < minimum_timestamp_length || view.content_length() > max_timestamp_length)
    {
        return false;
    }
    return m_contents.assign_ber(view);
}

inline EncodeResult GeneralizedTime::encode_content_and_length(absl::Span<uint8_t> buffer) const noexcept
{
    return m_contents.encode_content_and_length(buffer);
}

} // namespace fast_ber
