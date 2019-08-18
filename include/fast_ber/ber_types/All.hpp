#pragma once

#include "BitString.hpp"
#include "Boolean.hpp"
#include "CharacterString.hpp"
#include "Choice.hpp"
#include "Date.hpp"
#include "DateTime.hpp"
#include "Duration.hpp"
#include "EmbeddedPDV.hpp"
#include "Enumerated.hpp"
#include "External.hpp"
#include "GeneralizedTime.hpp"
#include "IRI.hpp"
#include "InstanceOf.hpp"
#include "Integer.hpp"
#include "Null.hpp"
#include "ObjectIdentifier.hpp"
#include "OctetString.hpp"
#include "Optional.hpp"
#include "Prefixed.hpp"
#include "Real.hpp"
#include "RelativeIRI.hpp"
#include "RelativeOID.hpp"
#include "Sequence.hpp"
#include "SequenceOf.hpp"
#include "Set.hpp"
#include "SetOf.hpp"
#include "TaggedType.hpp"
#include "Time.hpp"
#include "TimeOfDay.hpp"
#include "UTCTime.hpp"
#include "VisibleString.hpp"

namespace fast_ber
{
using Any = fast_ber::Choice<BitString, Boolean, CharacterString, Date, DateTime, Duration, GeneralizedTime, Integer,
                             Null, ObjectIdentifier, OctetString, Real, Time, TimeOfDay, UTCTime, VisibleString>;
}
