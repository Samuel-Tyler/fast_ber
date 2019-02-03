#pragma once

#include "fast_ber/util/BerLengthAndContentContainer.hpp"
#include "fast_ber/util/DecodeHelpers.hpp"
#include "fast_ber/util/EncodeHelpers.hpp"

#include "absl/container/inlined_vector.h"
#include "absl/types/span.h"

namespace fast_ber
{

using ObjectIdentifierComponents = absl::InlinedVector<int64_t, 10>;

inline size_t       encoded_object_id_length(const ObjectIdentifierComponents& input) noexcept;
inline EncodeResult encode_object_id(absl::Span<uint8_t> output, const ObjectIdentifierComponents& input,
                                     size_t encoded_length = 0) noexcept;
inline DecodeResult decode_object_id(absl::Span<const uint8_t> input, ObjectIdentifierComponents& output) noexcept;
// Return -1 on fail
inline int64_t get_component_number(absl::Span<const uint8_t> input, size_t component_number) noexcept;
inline size_t  get_number_of_components(absl::Span<const uint8_t> input) noexcept;

class ObjectIdentifier
{
  public:
    ObjectIdentifier() noexcept                            = default;
    ObjectIdentifier(const ObjectIdentifier& rhs) noexcept = default;
    ObjectIdentifier(ObjectIdentifier&& rhs) noexcept      = default;

    explicit ObjectIdentifier(const ObjectIdentifierComponents& oid) noexcept { assign(oid); }

    ObjectIdentifier& operator=(const BerView& view) noexcept;
    ObjectIdentifier& operator=(const ObjectIdentifierComponents& rhs) noexcept;

    ObjectIdentifier& operator=(absl::Span<const uint8_t> buffer) noexcept;
    bool operator==(const ObjectIdentifier& rhs) const noexcept { return this->m_contents == rhs.m_contents; }
    bool operator!=(const ObjectIdentifier& rhs) const noexcept { return !(*this == rhs); }
    bool operator==(const ObjectIdentifierComponents& rhs) const noexcept { return this->value() == rhs; }
    bool operator!=(const ObjectIdentifierComponents& rhs) const noexcept { return !(*this == rhs); }
    friend std::ostream& operator<<(std::ostream& os, const OctetString& str) noexcept;

    size_t  number_of_components() const noexcept;
    int64_t component_number(int64_t component_number) const noexcept;

    ObjectIdentifierComponents value() const noexcept;

    void   assign(const ObjectIdentifierComponents& oid) noexcept;
    size_t assign_ber(const BerView& view) noexcept;
    size_t assign_ber(const BerContainer& container) noexcept;
    size_t assign_ber(absl::Span<const uint8_t> buffer) noexcept;

    EncodeResult encode_content_and_length(absl::Span<uint8_t> buffer) const noexcept;

  private:
    BerLengthAndContentContainer m_contents;
};

inline size_t encoded_object_id_length(const ObjectIdentifierComponents& input) noexcept
{
    if (input.size() < 2)
    {
        return 0;
    }

    size_t output_size = 1;
    for (size_t i = 2; i < input.size(); i++)
    {
        int64_t component = input[i];
        do
        {
            output_size++;
            component /= 128;
        } while (component > 0);
    }
    return output_size;
}

inline EncodeResult encode_object_id(absl::Span<uint8_t> output, const ObjectIdentifierComponents& input,
                                     size_t encoded_length) noexcept
{
    if (encoded_length == 0)
    {
        encoded_length = encoded_object_id_length(input);
    }

    if (output.empty() || output.size() < encoded_length || input.size() < 2 || input[0] > 2 || input[1] > 39)
    {
        return EncodeResult{false, 0};
    }

    output[0] = 40 * input[0] + input[1];

    size_t output_write_pos = encoded_length - 1;
    for (size_t i = input.size() - 1; i > 1; i--)
    {
        int64_t component = input[i];
        if (component < 0)
        {
            return EncodeResult{false, 0};
        }

        bool first = true;
        do
        {
            output[output_write_pos] = component % 128;
            if (first)
            {
                first = false;
            }
            else
            {
                output[output_write_pos] |= 0x80;
            }

            output_write_pos--;
            component /= 128;
        } while (component > 0);
    }

    return EncodeResult{true, encoded_length};
}

inline DecodeResult decode_object_id(absl::Span<const uint8_t> input, absl::InlinedVector<int64_t, 10>& output) noexcept
{
    output.clear();

    if (input.empty())
    {
        return DecodeResult{false};
    }

    output.push_back(input[0] / 40);
    output.push_back(input[0] % 40);

    for (size_t i = 1, current_num = 0; i < input.length(); i++)
    {
        current_num += 0x7F & input[i];
        if (0x80 & input[i])
        {
            current_num *= 0x80;
        }
        else
        {
            output.push_back(current_num);
            current_num = 0;
        }
    }

    return DecodeResult{true};
}

inline int64_t get_component_number(absl::Span<const uint8_t> input, size_t component_number) noexcept
{
    if (input.empty())
    {
        return -1;
    }

    if (component_number == 0)
    {
        return input[0] / 40;
    }
    if (component_number == 1)
    {
        return input[1] % 40;
    }

    for (size_t i = 1, current_num = 0, current_component_number = 2; i < input.length(); i++)
    {
        current_num += 0x7F & input[i];
        if (0x80 & input[i])
        {
            current_num *= 0x80;
        }
        else
        {
            current_num = 0;
            if (current_component_number == component_number)
            {
                return current_num;
            }
            current_component_number++;
        }
    }
}

inline size_t get_number_of_components(absl::Span<const uint8_t> input) noexcept { return 0; }

inline ObjectIdentifier& ObjectIdentifier::operator=(const BerView& view) noexcept
{
    assign_ber(view);
    return *this;
}
inline ObjectIdentifier& ObjectIdentifier::operator=(const ObjectIdentifierComponents& rhs) noexcept
{
    assign(rhs);
    return *this;
}

inline ObjectIdentifier& ObjectIdentifier::operator=(absl::Span<const uint8_t> buffer) noexcept
{
    assign_ber(buffer);
    return *this;
}

inline std::ostream& operator<<(std::ostream& os, const ObjectIdentifier& oid) noexcept
{
    os << "{ ";
    const auto oid_components = oid.value();
    for (size_t i = 0; i < oid_components.size(); i++)
    {
        os << oid_components[i];
        if (i < oid_components.size() - 1)
        {
            os << ", ";
        }
    }
    os << " }";
    return os;
}

inline size_t ObjectIdentifier::number_of_components() const noexcept
{
    return fast_ber::get_number_of_components(m_contents.content());
}

inline int64_t ObjectIdentifier::component_number(int64_t component_number) const noexcept
{
    return fast_ber::get_component_number(m_contents.content(), component_number);
}

inline ObjectIdentifierComponents ObjectIdentifier::value() const noexcept
{
    ObjectIdentifierComponents output;
    decode_object_id(m_contents.content(), output);
    return output;
}

inline void ObjectIdentifier::assign(const ObjectIdentifierComponents& oid) noexcept
{
    const size_t encoded_length = encoded_object_id_length(oid);
    m_contents.resize_content(encoded_length);
    EncodeResult res = encode_object_id(absl::Span<uint8_t>(m_contents.content()), oid, encoded_length);
    assert(res.success);
    assert(res.length == m_contents.size());
    (void)res;
}
inline size_t ObjectIdentifier::assign_ber(const BerView& view) noexcept { return m_contents.assign_ber(view); }
inline size_t ObjectIdentifier::assign_ber(const BerContainer& container) noexcept
{
    return m_contents.assign_ber(container);
}
inline size_t ObjectIdentifier::assign_ber(absl::Span<const uint8_t> buffer) noexcept
{
    return m_contents.assign_ber(buffer);
}

inline EncodeResult ObjectIdentifier::encode_content_and_length(absl::Span<uint8_t> buffer) const noexcept
{
    return m_contents.encode_content_and_length(buffer);
}

} // namespace fast_ber
