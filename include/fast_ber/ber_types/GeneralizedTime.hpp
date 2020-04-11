#pragma once

#include "fast_ber/util/DecodeHelpers.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"
#include "fast_ber/util/FixedIdBerContainer.hpp"

#include "absl/strings/string_view.h"
#include "absl/time/time.h"

namespace fast_ber
{
constexpr const int minimum_timestamp_length = 10;
constexpr const int max_timestamp_length     = 23;

const std::string g_local_time_format                    = "%E4Y%m%d%H%M%S";
const std::string g_universal_time_format                = "%E4Y%m%d%H%M%SZ";
const std::string g_universal_time_with_time_zone_format = "%E4Y%m%d%H%M%S";

template <typename Identifier = ExplicitId<UniversalTag::generalized_time>>
class GeneralizedTime
{
  public:
    enum class TimeFormat
    {
        universal,
        universal_with_timezone,
        local
    };

    void set_time(const absl::CivilSecond& cs);
    void set_time(const absl::Time& time);
    void set_time(const absl::Time& time, int timezone_offset_minutes);

    absl::Time  time() const;
    std::string string() const;
    TimeFormat  format() const;

    GeneralizedTime() noexcept { set_time(absl::Time()); }
    GeneralizedTime(const GeneralizedTime&)     = default;
    GeneralizedTime(GeneralizedTime&&) noexcept = default;
    GeneralizedTime(const absl::Time& time) { set_time(time); }
    explicit GeneralizedTime(BerView view) { decode(view); }
    ~GeneralizedTime() noexcept = default;

    GeneralizedTime& operator=(const GeneralizedTime&) = default;
    GeneralizedTime& operator=(GeneralizedTime&&) noexcept = default;

    size_t       encoded_length() const noexcept;
    EncodeResult encode(absl::Span<uint8_t> buffer) const noexcept;
    DecodeResult decode(BerView buffer) noexcept;

    using AsnId = Identifier;

  private:
    FixedIdBerContainer<Identifier> m_contents;
};

template <typename Identifier>
bool operator==(const GeneralizedTime<Identifier>& lhs, const GeneralizedTime<Identifier>& rhs)
{
    return lhs.time() == rhs.time();
}

template <typename Identifier>
bool operator!=(const GeneralizedTime<Identifier>& lhs, const GeneralizedTime<Identifier>& rhs)
{
    return !(lhs == rhs);
}

template <typename Identifier>
void GeneralizedTime<Identifier>::set_time(const absl::Time& time)
{
    std::string time_str = absl::FormatTime(g_universal_time_format, time, absl::UTCTimeZone());

    m_contents.assign_content(
        absl::Span<const uint8_t>(reinterpret_cast<const uint8_t*>(time_str.data()), time_str.length()));
}

template <typename Identifier>
void GeneralizedTime<Identifier>::set_time(const absl::CivilSecond& cs)
{
    std::string time_str =
        absl::FormatTime(g_local_time_format, absl::FromCivil(cs, absl::LocalTimeZone()), absl::LocalTimeZone());

    m_contents.assign_content(
        absl::Span<const uint8_t>(reinterpret_cast<const uint8_t*>(time_str.data()), time_str.length()));
}

template <typename Identifier>
void GeneralizedTime<Identifier>::set_time(const absl::Time& time, int timezone_offset_minutes)
{
    std::string time_str = absl::FormatTime(g_universal_time_with_time_zone_format, time, absl::UTCTimeZone());

    std::string timezone_extension = std::string(5, '\0');
    snprintf(&timezone_extension[0], timezone_extension.length() + 1, "%c%2.2d%2.2d", // NOLINT(cppcoreguidelines-pro-type-vararg)
             (timezone_offset_minutes >= 0) ? '+' : '-', std::abs(timezone_offset_minutes) / 60,
             std::abs(timezone_offset_minutes) % 60);
    time_str += timezone_extension;

    m_contents.assign_content(
        absl::Span<const uint8_t>(reinterpret_cast<const uint8_t*>(time_str.data()), time_str.length()));
}

template <typename Identifier>
typename GeneralizedTime<Identifier>::TimeFormat GeneralizedTime<Identifier>::format() const
{
    if (m_contents.content().back() == 'Z')
    {
        return TimeFormat::universal;
    }
    else if ((m_contents.content()[m_contents.content_length() - 5] == '-') ||
             (m_contents.content()[m_contents.content_length() - 5] == '+'))
    {
        return TimeFormat::universal_with_timezone;
    }
    else
    {
        return TimeFormat::local;
    }
}

template <typename Identifier>
absl::Time GeneralizedTime<Identifier>::time() const
{
    thread_local std::string s_error_string;
    absl::Time               time;
    const TimeFormat         frmt = format();

    if (frmt == TimeFormat::universal)
    {
        if (!absl::ParseTime(g_universal_time_format, this->string(), absl::UTCTimeZone(), &time, &s_error_string))
        {
            throw std::runtime_error("Failed to parse time: " + s_error_string);
        }
    }
    else if (frmt == TimeFormat::universal_with_timezone)
    {
        const auto time_str = this->string();
        if (!absl::ParseTime(g_universal_time_with_time_zone_format, time_str.substr(.0, time_str.length() - 5),
                             absl::UTCTimeZone(), &time, &s_error_string))
        {
            throw std::runtime_error("Failed to parse time: " + s_error_string);
        }
    }
    else if (frmt == TimeFormat::local)
    {
        if (!absl::ParseTime(g_local_time_format, this->string(), absl::UTCTimeZone(), &time, &s_error_string))
        {
            throw std::runtime_error("Failed to parse time: " + s_error_string);
        }
    }
    return time;
}

template <typename Identifier>
std::string GeneralizedTime<Identifier>::string() const
{
    return std::string(reinterpret_cast<const char*>(m_contents.content_data()), m_contents.content_length());
}

template <typename Identifier>
size_t GeneralizedTime<Identifier>::encoded_length() const noexcept
{
    return this->m_contents.ber().length();
}

template <typename Identifier>
EncodeResult GeneralizedTime<Identifier>::encode(absl::Span<uint8_t> output) const noexcept
{
    return this->m_contents.encode(output);
}

template <typename Identifier>
DecodeResult GeneralizedTime<Identifier>::decode(BerView view) noexcept
{
    DecodeResult res = m_contents.decode(view);
    if (m_contents.content_length() < minimum_timestamp_length || m_contents.content_length() > max_timestamp_length)
    {
        return DecodeResult{false};
    }
    return res;
}

template <typename Identifier>
std::ostream& operator<<(std::ostream& os, const GeneralizedTime<Identifier>& time)
{
    return os << time.time();
}

} // namespace fast_ber
