#include "fast_ber/util/BerView.hpp"
#include "fast_ber/ber_types/All.hpp"

namespace fast_ber
{
std::ostream& operator<<(std::ostream& os, BerView view) noexcept
{
    if (!view.is_valid())
    {
        os << "null";
        return os;
    }

    os << "{ \"length\": " << view.ber_length();
    os << ", \"identifier\": " << view.identifier();
    os << ", \"content\": ";
    if (view.construction() == Construction::primitive)
    {
        if (view.class_() == Class::universal)
        {
            try
            {
                switch (static_cast<UniversalTag>(view.tag()))
                {
                case UniversalTag::boolean:
                    os << Boolean<>(view);
                    break;
                case UniversalTag::integer:
                    os << Integer<>(view);
                    break;
                case UniversalTag::bit_string:
                    os << BitString<>(view);
                    break;
                case UniversalTag::octet_string:
                    os << OctetString<>(view);
                    break;
                case UniversalTag::null:
                    os << Null<>(view);
                    break;
                case UniversalTag::object_identifier:
                    os << ObjectIdentifier<>(view);
                    break;
                case UniversalTag::real:
                    // os << Real<>(view);
                    break;
                case UniversalTag::enumerated:
                    os << Integer<>(view);
                    break;
                case UniversalTag::utf8_string:
                    os << UTF8String<>(view);
                    break;
                case UniversalTag::numeric_string:
                    os << NumericString<>(view);
                    break;
                case UniversalTag::printable_string:
                    os << PrintableString<>(view);
                    break;
                case UniversalTag::teletex_string:
                    os << TeletexString<>(view);
                    break;
                case UniversalTag::videotex_string:
                    os << VideotexString<>(view);
                    break;
                case UniversalTag::ia5_string:
                    os << IA5String<>(view);
                    break;
                case UniversalTag::utc_time:
                    os << UTCTime<>(view);
                    break;
                case UniversalTag::generalized_time:
                    os << GeneralizedTime<>(view);
                    break;
                case UniversalTag::graphic_string:
                    os << GraphicString<>(view);
                    break;
                case UniversalTag::visible_string:
                    os << VisibleString<>(view);
                    break;
                case UniversalTag::general_string:
                    os << GeneralString<>(view);
                    break;
                case UniversalTag::universal_string:
                    os << UniversalString<>(view);
                    break;
                case UniversalTag::character_string:
                    os << CharacterString<>(view);
                    break;
                case UniversalTag::bmp_string:
                    os << BMPString<>(view);
                    break;

                case UniversalTag::embedded_pdv:
                case UniversalTag::relative_oid:
                case UniversalTag::sequence:
                case UniversalTag::set:
                case UniversalTag::reserved:
                case UniversalTag::object_descriptor:
                case UniversalTag::instance_of:
                default:
                    os << OctetString<>(view.content());
                }
            }
            catch (const std::exception& e)
            {
                os << OctetString<>(view.content());
            }
        }
        else
        {
            os << OctetString<>(view.content());
        }
    }
    else
    {
        bool first = true;
        os << "[";
        for (const BerView& child : view)
        {
            if (!first)
            {
                os << ", ";
            }
            os << child;
            first = false;
        }
        os << "]";
    }
    return os << " }";
}

} // namespace fast_ber
