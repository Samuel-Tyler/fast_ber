%skeleton "lalr1.cc"
%define parser_class_name {asn1_parser}
%define api.token.constructor
%define api.value.type variant
%define parse.assert
%define parse.error verbose
%locations   // <--

%code requires
{
    #include <map>
    #include <list>
    #include <vector>
    #include <string>
    #include <iostream>
    #include <algorithm>
    #include <fstream>

    struct Context;
}

%code
{
    struct Context
    {
        const char* cursor;
        yy::location location;
    };

    void yy::asn1_parser::error(const location_type& l, const std::string& m)
    {
        std::cerr << (l.begin.filename ? l.begin.filename->c_str() : "(undefined)");
        std::cerr << ':' << l.begin.line << ':' << l.begin.column << '-' << l.end.column << ": " << m << '\n';
    }

    namespace yy { asn1_parser::symbol_type yylex(Context& c); }
} // End code


%param { Context& context }

%token END_OF_FILE 0

%token comment
%token number
%token realnumber
%token bstring
%token xmlbstring
%token hstring
%token xmlhstring
%token cstring
%token xmlcstring
%token simplestring
%token xmltstring
%token psname
%token encodingreference
%token integerUnicodeLabel
%token non-integerUnicodeLabel
%token extended-true
%token tstring
%token extended-false
%token objectclassreference
%token objectreference
%token objectsetreference
%token typefieldreference
%token valuefieldreference
%token valuesetfieldreference
%token objectfieldreference
%token objectsetfieldreference
%token word

%token ABSENT
%token ABSTRACT_SYNTAX
%token ALL
%token APPLICATION
%token ASN_NULL
%token AUTOMATIC
%token BEGIN
%token BIT
%token BMPString
%token BOOLEAN
%token BY
%token CHARACTER
%token CHOICE
%token CLASS
%token COMPONENT
%token COMPONENTS
%token CONSTRAINED
%token CONTAINING
%token DATE
%token DATE_TIME
%token DEFAULT
%token DEFINITIONS
%token DURATION
%token EMBEDDED
%token ENCODED
%token ENCODING_CONTROL
%token END
%token ENUMERATED
%token EXCEPT
%token EXPLICIT
%token EXPORTS
%token EXTENSIBILITY
%token EXTERNAL
%token FALSE
%token FROM
%token GeneralizedTime
%token GeneralString
%token GraphicString
%token IA5String
%token IDENTIFIER
%token IMPLICIT
%token IMPLIED
%token IMPORTS
%token INCLUDES
%token INSTANCE
%token INSTRUCTIONS
%token INTEGER
%token INTERSECTION
%token ISO646String
%token MAX
%token MIN
%token MINUS_INFINITY
%token NOT_A_NUMBER
%token NumericString
%token OBJECT
%token ObjectDescriptor
%token OCTET
%token OF
%token OID_IRI
%token OPTIONAL
%token PATTERN
%token PDV
%token PLUS_INFINITY
%token PRESENT
%token PrintableString
%token PRIVATE
%token REAL
%token RELATIVE_OID
%token RELATIVE_OID_IRI
%token SEQUENCE
%token SET
%token SETTINGS
%token SIZE
%token STRING
%token SYNTAX
%token T61String
%token TAGS
%token TeletexString
%token TIME
%token TIME_OF_DAY
%token TRUE
%token TYPE_IDENTIFIER
%token UNION
%token UNIQUE
%token UNIVERSAL
%token UniversalString
%token UTCTime
%token UTF8String
%token VideotexString
%token VisibleString
%token WITH

%token OPEN_BRACE "{"
%token CLOSE_BRACE "}"
%token DEFINED_AS "::="
%token COLON " : "

%token NO_WS
%token PLUS
%token STAR
%token QUESTION_MARK
%token GENERIC_IDENTIFIER

%type<std::string> GENERIC_IDENTIFIER
%%

ModuleDefinition:
    ModuleIdentifier
    DEFINITIONS
    EncodingReferenceDefault
    TagDefault
    ExtensionDefault
    DEFINED_AS
    BEGIN
    ModuleBody
    EncodingControlSections
    END;

SyntaxList:
   "{" TokenOrGroupSpec "}";

EncodingInstruction:
    %empty;

EncodingInstructionAssignmentList:
    %empty;

ExensionEndMarker:
    %empty;

DefinedObjectClass:
    ExternalObjectClassReference
|   objectclassreference
|   UsefulObjectClassReference;

ExternalObjectClassReference:
    modulereference "." objectclassreference;

UsefulObjectClassReference:
    TYPE_IDENTIFIER
|   ABSTRACT_SYNTAX;

ObjectClassAssignment:
    objectclassreference DEFINED_AS ObjectClass;

ObjectClass:
    DefinedObjectClass
|   ObjectClassDefn
|   ParameterizedObjectClass;

ObjectClassDefn:
    CLASS "{" FieldSpec "," PLUS "}" WithSyntaxSpec QUESTION_MARK;

FieldSpec:
    TypeFieldSpec
|   FixedTypeValueFieldSpec
|   VariableTypeValueFieldSpec
|   FixedTypeValueSetFieldSpec
|   VariableTypeValueSetFieldSpec
|   ObjectFieldSpec
|   ObjectSetFieldSpec;

PrimitiveFieldName:
    typefieldreference
|   valuefieldreference
|   valuesetfieldreference
|   objectfieldreference
|   objectsetfieldreference;

FieldName:
    PrimitiveFieldName "." PLUS;

TypeFieldSpec:
    typefieldreference TypeOptionalitySpec QUESTION_MARK;

TypeOptionalitySpec:
    OPTIONAL
|   DEFAULT Type;

FixedTypeValueFieldSpec:
    valuefieldreference Type UNIQUE QUESTION_MARK ValueOptionalitySpec QUESTION_MARK;

ValueOptionalitySpec:
    OPTIONAL
|   DEFAULT Value;

VariableTypeValueFieldSpec:
    valuefieldreference FieldName ValueOptionalitySpec QUESTION_MARK;

FixedTypeValueSetFieldSpec:
    valuesetfieldreference Type ValueSetOptionalitySpec QUESTION_MARK;

ValueSetOptionalitySpec:
    OPTIONAL
|   DEFAULT ValueSet;

VariableTypeValueSetFieldSpec:
    valuesetfieldreference FieldName ValueSetOptionalitySpec QUESTION_MARK;

ObjectFieldSpec:
    objectfieldreference DefinedObjectClass ObjectOptionalitySpec QUESTION_MARK;

ObjectOptionalitySpec:
    OPTIONAL
|   DEFAULT Object;

ObjectSetFieldSpec:
    objectsetfieldreference DefinedObjectClass ObjectSetOptionalitySpec QUESTION_MARK;

ObjectSetOptionalitySpec:
    OPTIONAL
|   DEFAULT;

ObjectSeWithSyntaxSpec:
    WITH SYNTAX SyntaxList;

WithSyntaxSpec:
    WITH SYNTAX SyntaxList;

TokenOrGroupSpec:
    RequiredToken
|   OptionalGroup;

OptionalGroup:
    "[" TokenOrGroupSpec PLUS "]";

RequiredToken:
    Literal
|   PrimitiveFieldName;

Literal:
    word
|   ",";

DefinedObject:
    ExternalObjectReference
|   objectreference;

ExternalObjectReference:
    modulereference "." objectreference;

ObjectAssignment:
    objectreference DefinedObjectClass DEFINED_AS Object;

Object:
    DefinedObject
|   ObjectDefn
|   ObjectFromObject
|   ParameterizedObject;

ObjectDefn:
    DefaultSyntax
|   DefinedSyntax;

DefaultSyntax:
    "{" FieldSetting "," STAR "}";

FieldSetting:
     PrimitiveFieldName Setting;

DefinedSyntax:
    "{" DefinedSyntaxToken STAR "}";

DefinedSyntaxToken:
    Literal
|   Setting;

Setting:
    Type
|   Value
|   ValueSet
|   Object
|   ObjectSet;

DefinedObjectSet:
    ExternalObjectSetReference
|   objectsetreference;

ExternalObjectSetReference:
    modulereference "." objectsetreference;

ObjectSetAssignment:
    objectsetreference DefinedObjectClass DEFINED_AS ObjectSet;

ObjectSet:
    "{" ObjectSetSpec "}";

ObjectSetSpec:
    RootElementSetSpec
|   RootElementSetSpec "," "..."
|   "..."
|   "..." "," AdditionalElementSetSpec
|   RootElementSetSpec "," "..." "," AdditionalElementSetSpec;

ObjectSetElements:
    Object
|   DefinedObjectSet
|   ObjectSetFromObjects
|   ParameterizedObjectSet;

ObjectClassFieldType:
    DefinedObjectClass "." FieldName;

ObjectClassFieldValue:
    OpenTypeFieldVal
|   FixedTypeFieldVal;

OpenTypeFieldVal:
    Type ":" Value;

FixedTypeFieldVal:
    BuiltinValue
|   ReferencedValue;

XMLObjectClassFieldValue:
    XMLOpenTypeFieldVal
|   XMLFixedTypeFieldVal;

XMLOpenTypeFieldVal:
    XMLTypedValue
|   xmlhstring;

XMLFixedTypeFieldVal:
    XMLBuiltinValue;

InformationFromObjects:
    ValueFromObject
|   ValueSetFromObjects
|   TypeFromObject
|   ObjectFromObject
|   ObjectSetFromObjects;

ReferencedObjects:
    DefinedObject
|   ParameterizedObject
|   DefinedObjectSet
|   ParameterizedObjectSet;

ValueFromObject:
    ReferencedObjects "." FieldName;

ValueSetFromObjects:
    ReferencedObjects "." FieldName;

TypeFromObject:
    ReferencedObjects "." FieldName;

ObjectFromObject:
    ReferencedObjects "." FieldName;

ObjectSetFromObjects:
    ReferencedObjects "." FieldName;

InstanceOfType:
    INSTANCE OF DefinedObjectClass;

InstanceOfValue:
    Value;

XMLInstanceOfValue:
    XMLValue;

ParameterizedAssignment:
    ParameterizedTypeAssignment
|   ParameterizedValueAssignment
|   ParameterizedValueSetTypeAssignment
|   ParameterizedObjectClassAssignment
|   ParameterizedObjectAssignment
|   ParameterizedObjectSetAssignment;

ParameterizedTypeAssignment:
    typereference;

ParameterList:
    Type;

ParameterizedValueAssignment:
    valuereference ParameterList Type DEFINED_AS Value;

ParameterizedValueSetTypeAssignment:
    typereference ParameterList Type DEFINED_AS ValueSet;

ParameterizedObjectClassAssignment:
    objectclassreference ParameterList: ObjectClass;

ParameterizedObjectAssignment:
    objectreference ParameterList DefinedObjectClass DEFINED_AS Object;

ParameterizedObjectSetAssignment:
    objectsetreference ParameterList DefinedObjectClass DEFINED_AS ObjectSet;

ParameterList:
   "{" Parameter "," "}";

Parameter:
    ParamGovernor ":" DummyReference
|   DummyReference;

ParamGovernor:
    Governor
|   DummyGovernor;

Governor:
    Type
|   DefinedObjectClass;

DummyGovernor:
    DummyReference;

DummyReference:
    Reference;

ParameterizedReference:
    Reference
|   Reference "{" "}";

SimpleDefinedType:
    ExternalTypeReference
|   typereference;

SimpleDefinedValue:
    ExternalValueReference
|   valuereference;

ParameterizedType:
    SimpleDefinedType
    ActualParameterList;

ParameterizedValue:
    SimpleDefinedValue
    ActualParameterList;

ParameterizedValueSetType:
    SimpleDefinedType
    ActualParameterList;

ParameterizedObjectClass:
    DefinedObjectClass
    ActualParameterList;

ParameterizedObjectSet:
    DefinedObjectSet
    ActualParameterList;

ParameterizedObject:
    DefinedObject
    ActualParameterList;

ActualParameterList:
    "{" ActualParameter "," "}"

ActualParameter:
    Type
|   Value
|   ValueSet
|   DefinedObjectClass
|   Object
|   ObjectSet;

GeneralConstraint:
    UserDefinedConstraint
|   TableConstraint
|   ContentsConstraint;

UserDefinedConstraint:
    CONSTRAINED BY "{" UserDefinedConstraintParameter "," STAR "}";

UserDefinedConstraintParameter:
    Governor ":" Value
|   Governor ":" Object
|   DefinedObjectSet
|   Type
|   DefinedObjectClass;

TableConstraint:
    SimpleTableConstraint
|   ComponentRelationConstraint;

SimpleTableConstraint:
    ObjectSet;

ComponentRelationConstraint:
    "{" DefinedObjectSet "}" "{" AtNotation "," "}";

AtNotation:
    ComponentIdList
|   "@." Level ComponentIdList;

Level:
    "." Level
|   %empty;

ComponentIdList:
    identifier ".";

ContentsConstraint:
    CONTAINING Type
|   ENCODED BY Value
|   CONTAINING Type ENCODED BY Value;

EncodingPrefixedType:
    EncodingPrefix Type;

EncodingPrefix:
    "[" EncodingReference EncodingInstruction "]";

EncodingControlSections:
    EncodingControlSection EncodingControlSections
|   %empty;

EncodingControlSection:
    ENCODING_CONTROL
    encodingreference
    EncodingInstructionAssignmentList;

ModuleIdentifier:
    modulereference
    DefinitiveIdentification;

DefinitiveIdentification:
    DefinitiveOID
|   DefinitiveOIDandIRI
|   %empty;

DefinitiveOID:
    "{" DefinitiveObjIdComponentList "}";

DefinitiveOIDandIRI:
    DefinitiveOID
    IRIValue;

DefinitiveObjIdComponentList:
    DefinitiveObjIdComponent
|   DefinitiveObjIdComponent
    DefinitiveObjIdComponentList;

DefinitiveObjIdComponent:
    NameForm
|   DefinitiveNumberForm
|   DefinitiveNameAndNumberForm;

DefinitiveNumberForm:
    number;

DefinitiveNameAndNumberForm:
    identifier
    "(" DefinitiveNumberForm ")";

EncodingReferenceDefault:
    encodingreference INSTRUCTIONS
|   %empty;

TagDefault:
    EXPLICIT TAGS
|   IMPLICIT TAGS
|   AUTOMATIC TAGS
|   %empty;

ExtensionDefault:
    EXTENSIBILITY IMPLIED
|   %empty;

ModuleBody:
    Exports Imports AssignmentList
|   %empty;

Exports:
    EXPORTS SymbolsExported ";"
|   EXPORTS ALL ";"
|   %empty;

SymbolsExported:
    SymbolList
|   %empty;

Imports:
    IMPORTS
    SymbolsImported ";"
|   %empty;

SymbolsImported:
    SymbolsFromModuleList
|   %empty;

SymbolsFromModuleList:
    SymbolsFromModule
|   SymbolsFromModuleList SymbolsFromModule;

SymbolsFromModule:
    SymbolList
    FROM
    GlobalModuleReference;

GlobalModuleReference:
    modulereference
    AssignedIdentifier;

AssignedIdentifier:
    ObjectIdentifierValue
|   DefinedValue
|   %empty;

SymbolList:
    Symbol
|   SymbolList
    "," Symbol;

Symbol:
    Reference
|   ParameterizedReference;

Reference:
    typereference
|   valuereference
|   objectclassreference
|   objectreference
|   objectsetreference;

AssignmentList:
    Assignment
|   AssignmentList
    Assignment

Assignment:
    TypeAssignment
|   ValueAssignment
|   XMLValueAssignment
|   ValueSetTypeAssignment
|   ObjectClassAssignment
|   ObjectAssignment
|   ObjectSetAssignment
|   ParameterizedAssignment;

DefinedType:
    ExternalTypeReference
|   typereference
|   ParameterizedType
|   ParameterizedValueSetType;

DefinedValue:
    ExternalValueReference
|   valuereference
|   ParameterizedValue;

NonParameterizedTypeName:
    ExternalTypeReference
|   typereference
|   xmlasn1typename;

ExternalTypeReference:
    modulereference
    "."
    typereference;

ExternalValueReference:
    modulereference
    "."
    valuereference;

AbsoluteReference:
    "@"
    ModuleIdentifier
    "."
    ItemSpec;

ItemSpec:
    typereference
|   ItemId " . " ComponentId;

ItemId:
    ItemSpec;

ComponentId:
    identifier
|   number
|   "*";

TypeAssignment:
    typereference
    DEFINED_AS
    Type;

ValueAssignment:
    valuereference
    Type
    DEFINED_AS
    Value;

XMLValueAssignment:
    valuereference
    DEFINED_AS
    XMLTypedValue;

XMLTypedValue:
    "<" NO_WS NonParameterizedTypeName ">";

XMLValue:
    "</" NO_WS NonParameterizedTypeName ">"
|   "<" NO_WS NonParameterizedTypeName "/>";

ValueSetTypeAssignment:
    typereference
    Type
    DEFINED_AS
    ValueSet;

ValueSet:
    "{" ElementSetSpecs "}";

Type:
    BuiltinType
|   ReferencedType
|   ConstrainedType;

BuiltinType:
    BitStringType
|   BooleanType
|   CharacterStringType
|   ChoiceType
|   DateType
|   DateTimeType
|   DurationType
|   EmbeddedPDVType
|   EnumeratedType
|   ExternalType
|   InstanceOfType
|   IntegerType
|   IRIType
|   NullType
|   ObjectClassFieldType
|   ObjectIdentifierType
|   OctetStringType
|   RealType
|   RelativeIRIType
|   RelativeOIDType
|   SequenceType
|   SequenceOfType
|   SetType
|   SetOfType
|   PrefixedType
|   TimeType
|   TimeOfDayType;

ReferencedType:
    DefinedType
|   UsefulType
|   SelectionType
|   TypeFromObject
|   ValueSetFromObjects;

NamedType:
    identifier
    Type;

Value:
    BuiltinValue
|   ReferencedValue
|   ObjectClassFieldValue;

XMLValue:
    XMLBuiltinValue
|   XMLObjectClassFieldValue;

BuiltinValue:
    BitStringValue
|   BooleanValue
|   CharacterStringValue
|   ChoiceValue
|   EmbeddedPDVValue
|   EnumeratedValue
|   ExternalValue
|   InstanceOfValue
|   IntegerValue
|   IRIValue
|   NullValue
|   ObjectIdentifierValue
|   OctetStringValue
|   RealValue
|   RelativeIRIValue
|   RelativeOIDValue
|   SequenceValue
|   SequenceOfValue
|   SetValue
|   SetOfValue
|   PrefixedValue
|   TimeValue;

XMLBuiltinValue:
    XMLBitStringValue
|   XMLBooleanValue
|   XMLCharacterStringValue
|   XMLChoiceValue
|   XMLEmbeddedPDVValue
|   XMLEnumeratedValue
|   XMLExternalValue
|   XMLInstanceOfValue
|   XMLIntegerValue
|   XMLIRIValue
|   XMLNullValue
|   XMLObjectIdentifierValue
|   XMLOctetStringValue
|   XMLRealValue
|   XMLRelativeIRIValue
|   XMLRelativeOIDValue
|   XMLSequenceValue
|   XMLSequenceOfValue
|   XMLSetValue
|   XMLSetOfValue
|   XMLPrefixedValue
|   XMLTimeValue;

ReferencedValue:
    DefinedValue
|   ValueFromObject;

NamedValue:
    identifier
    Value;

XMLNamedValue:
    "<" NO_WS identifier ">"
    XMLValue
    "</" NO_WS identifier ">";

BooleanType:
    BOOLEAN;

BooleanValue:
    TRUE
|   FALSE;

XMLBooleanValue:
    EmptyElementBoolean
|   TextBoolean;

EmptyElementBoolean:
    "<" NO_WS "true" "/>"
|   "<" NO_WS "false" "/>";

TextBoolean:
    extended-true
|   extended-false;

IntegerType:
    INTEGER
|   INTEGER "{" NamedNumberList "}";

NamedNumberList:
    NamedNumber
|   NamedNumberList "," NamedNumber;

NamedNumber:
    identifier "(" SignedNumber ")"
|   identifier "(" DefinedValue ")";

SignedNumber:
    number
|   "-" number;

IntegerValue:
    SignedNumber
|   identifier;

XMLIntegerValue:
    XMLSignedNumber
|   EmptyElementInteger
|   TextInteger;

XMLSignedNumber:
    number
|   "-" NO_WS number;

EmptyElementInteger:
    "<" NO_WS identifier "/>";

TextInteger:
    identifier;

EnumeratedType:
    ENUMERATED
    "{" Enumerations "}";

Enumerations:
    RootEnumeration
|   RootEnumeration "," " ... " ExceptionSpec
|   RootEnumeration "," " ... " ExceptionSpec "," AdditionalEnumeration;

RootEnumeration:
    Enumeration;

AdditionalEnumeration:
    Enumeration;

Enumeration:
    EnumerationItem
|   EnumerationItem "," Enumeration;

EnumerationItem:
    identifier
|   NamedNumber;

EnumeratedValue:
    identifier;

XMLEnumeratedValue:
    EmptyElementEnumerated
|   TextEnumerated;

EmptyElementEnumerated:
    "<" NO_WS identifier "/>";

TextEnumerated:
    identifier;

RealType:
    REAL;

RealValue:
    NumericRealValue
|   SpecialRealValue;

NumericRealValue:
    realnumber
|   "-" realnumber
|   SequenceValue;

SpecialRealValue:
    PLUS_INFINITY
|   MINUS_INFINITY
|   NOT_A_NUMBER;

XMLRealValue:
    XMLNumericRealValue
|   XMLSpecialRealValue;

XMLNumericRealValue:
    realnumber
|   "-" NO_WS realnumber;

XMLSpecialRealValue:
    EmptyElementReal
|   TextReal;

EmptyElementReal:
    "<" NO_WS PLUS_INFINITY "/>"
|   "<" NO_WS MINUS_INFINITY "/>"
|   "<" NO_WS NOT_A_NUMBER "/>";

TextReal:
    " INF "
|   "-" NO_WS " INF "
|   " NaN ";

BitStringType:
    BIT STRING
|   BIT STRING "{" NamedBitList "}";

NamedBitList:
    NamedBit
|   NamedBitList "," NamedBit;

NamedBit:
    identifier "(" number ")"
|   identifier "(" DefinedValue ")";

BitStringValue:
    bstring
|   hstring
|   "{" IdentifierList "}"
|   "{" "}"
|   CONTAINING Value;

IdentifierList:
    identifier
|   IdentifierList "," identifier;

XMLBitStringValue:
    XMLTypedValue
|   xmlbstring
|   XMLIdentifierList
|   %empty;

XMLIdentifierList:
    EmptyElementList
|   TextList;

EmptyElementList:
"<" NO_WS identifier "/>"
|   EmptyElementList "<" NO_WS identifier "/>";

TextList:
    identifier
|   TextList identifier;

OctetStringType:
    OCTET STRING;

OctetStringValue:
    bstring
|   hstring
|   CONTAINING Value;

XMLOctetStringValue:
    XMLTypedValue
|   xmlhstring;

NullType:
    ASN_NULL;

NullValue:
    ASN_NULL;

XMLNullValue:
    %empty;

SequenceType:
    SEQUENCE "{" "}"
|   SEQUENCE "{" ExtensionAndException OptionalExtensionMarker "}"
|   SEQUENCE "{" ComponentTypeLists "}";

ExtensionAndException:
    " ... "
|   " ... " ExceptionSpec;

OptionalExtensionMarker:
    "," " ... "
|   %empty;

ComponentTypeLists:
    RootComponentTypeList
|   RootComponentTypeList "," ExtensionAndException ExtensionAdditions
|   OptionalExtensionMarker RootComponentTypeList "," ExtensionAndException ExtensionAdditions
|   ExtensionEndMarker "," RootComponentTypeList ExtensionAndException ExtensionAdditions ExensionEndMarker ","
|   RootComponentTypeList ExtensionAndException ExtensionAdditions OptionalExtensionMarker

RootComponentTypeList:
    ComponentTypeList;

ExtensionEndMarker:
    "," " ... "

ExtensionAdditions:
    "," ExtensionAdditionList
|   %empty

ExtensionAdditionList:
    ExtensionAddition
|   ExtensionAdditionList "," ExtensionAddition;

ExtensionAddition:
    ComponentType
|   ExtensionAdditionGroup;

ExtensionAdditionGroup:
    "[[" VersionNumber ComponentTypeList "]]";

VersionNumber:
    %empty
|   number COLON;

ComponentTypeList:
    ComponentType
|   ComponentTypeList "," ComponentType;

ComponentType:
    NamedType
|   NamedType OPTIONAL
|   NamedType DEFAULT Value
|   COMPONENTS OF Type;

SequenceValue:
    "{" ComponentValueList "}"
|   "{" "}";

ComponentValueList:
    NamedValue
|   ComponentValueList "," NamedValue;

XMLSequenceValue:
    XMLComponentValueList
|   %empty;

XMLComponentValueList:
    XMLNamedValue
|   XMLComponentValueList XMLNamedValue;

SequenceOfType:
    SEQUENCE OF Type
|   SEQUENCE OF NamedType;

SequenceOfValue:
    "{" ValueList "}"
|   "{" NamedValueList "}"
|   "{" "}";

ValueList:
    Value
|   ValueList "," Value;

NamedValueList:
    NamedValue
|   NamedValueList "," NamedValue;

XMLSequenceOfValue:
    XMLValueList
|   XMLDelimitedItemList
|   %empty;

XMLValueList:
    XMLValueOrEmpty
|   XMLValueOrEmpty XMLValueList;

XMLValueOrEmpty:
    XMLValue
|   "<" NO_WS NonParameterizedTypeName "/>";

XMLDelimitedItemList:
    XMLDelimitedItem
|   XMLDelimitedItem XMLDelimitedItemList;

XMLDelimitedItem:
    "<" NO_WS NonParameterizedTypeName ">"
    XMLValue
    "</" NO_WS NonParameterizedTypeName ">"
|   "<" NO_WS identifier ">"
    XMLValue
    "</" NO_WS identifier ">";

SetType:
    SET "{" "}"
|   SET "{" ExtensionAndException OptionalExtensionMarker "}"
|   SET "{" ComponentTypeLists "}";

SetValue:
    "{" ComponentValueList "}"
|   "{" "}";

XMLSetValue:
    XMLComponentValueList
|   %empty;

SetOfType:
    SET OF Type
|   SET OF NamedType;

SetOfValue:
    "{" ValueList "}"
|   "{" NamedValueList "}"
|   "{" "}";

XMLSetOfValue:
    XMLValueList
|   XMLDelimitedItemList
|   %empty;

ChoiceType:
    CHOICE "{" AlternativeTypeLists "}";

AlternativeTypeLists:
    RootAlternativeTypeList
|   RootAlternativeTypeList
    ","
    ExtensionAndException
    ExtensionAdditionAlternatives
    OptionalExtensionMarker;

RootAlternativeTypeList:
    AlternativeTypeList;

ExtensionAdditionAlternatives:
    "," ExtensionAdditionAlternativesList
|   %empty;

ExtensionAdditionAlternativesList:
    ExtensionAdditionAlternative
|   ExtensionAdditionAlternativesList "," ExtensionAdditionAlternative

ExtensionAdditionAlternative:
    ExtensionAdditionAlternativesGroup
|   NamedType;

ExtensionAdditionAlternativesGroup:
    "[[" VersionNumber AlternativeTypeList "]]";

AlternativeTypeList:
    NamedType
|   AlternativeTypeList "," NamedType;

ChoiceValue:
    identifier DEFINED_AS Value;

XMLChoiceValue:
    "<" NO_WS identifier ">"
    XMLValue
    "</" NO_WS identifier ">";

SelectionType:
    identifier "<" Type;

PrefixedType:
    TaggedType
|   EncodingPrefixedType;

PrefixedValue:
    Value;

XMLPrefixedValue:
    XMLValue;

EncodingPrefixedType:
    EncodingPrefix Type

EncodingPrefix:
    "[" EncodingReference EncodingInstruction "]";

TaggedType:
    Tag Type
|   Tag IMPLICIT Type
|   Tag EXPLICIT Type;

Tag:
    "[" EncodingReference Class ClassNumber "]";

EncodingReference:
    encodingreference ":"
|   %empty;

ClassNumber:
    number
|   DefinedValue;

Class:
    UNIVERSAL
|   APPLICATION
|   PRIVATE
|   %empty;

EncodingPrefixedType:
    EncodingPrefix Type;

EncodingPrefix:
    "[" EncodingReference EncodingInstruction "]";

ObjectIdentifierType:
    OBJECT IDENTIFIER;

ObjectIdentifierValue:
    "{" ObjIdComponentsList "}"
|   "{" DefinedValue ObjIdComponentsList "}";

ObjIdComponentsList:
    ObjIdComponents
|   ObjIdComponents ObjIdComponentsList;

ObjIdComponents:
    NameForm
|   NumberForm
|   NameAndNumberForm
|   DefinedValue;

NameForm:
    identifier;

NumberForm:
    number
|   DefinedValue;

NameAndNumberForm:
    identifier "(" NumberForm ")";

XMLObjectIdentifierValue:
    XMLObjIdComponentList;

XMLObjIdComponentList:
    XMLObjIdComponent
|   XMLObjIdComponent NO_WS " . " NO_WS XMLObjIdComponentList;

XMLObjIdComponent:
    NameForm
|   XMLNumberForm
|   XMLNameAndNumberForm;

XMLNumberForm:
    number;

XMLNameAndNumberForm:
    identifier NO_WS
    "(" NO_WS XMLNumberForm NO_WS ")";

RelativeOIDType:
    RELATIVE_OID;

RelativeOIDValue:
    "{" RelativeOIDComponentsList "}";

RelativeOIDComponentsList:
    RelativeOIDComponents
|   RelativeOIDComponents RelativeOIDComponentsList;

RelativeOIDComponents:
    NumberForm
|   NameAndNumberForm
|   DefinedValue;

XMLRelativeOIDValue:
    XMLRelativeOIDComponentList;

XMLRelativeOIDComponentList:
    XMLRelativeOIDComponent
|   XMLRelativeOIDComponent NO_WS " . " NO_WS XMLRelativeOIDComponentList;

XMLRelativeOIDComponent:
    XMLNumberForm
|   XMLNameAndNumberForm;

IRIType:
    OID_IRI;

IRIValue:
    "\""
    FirstArcIdentifier
    SubsequentArcIdentifier
    "\"";

FirstArcIdentifier:
    "/" ArcIdentifier;

SubsequentArcIdentifier:
    "/" ArcIdentifier SubsequentArcIdentifier
|   %empty;

ArcIdentifier :
    integerUnicodeLabel
|   non-integerUnicodeLabel

XMLIRIValue :
    FirstArcIdentifier
    SubsequentArcIdentifier

RelativeIRIType:
    RELATIVE_OID_IRI;

RelativeIRIValue:
    "\""
    FirstRelativeArcIdentifier
    SubsequentArcIdentifier
    "\"";

FirstRelativeArcIdentifier:
    ArcIdentifier;

XMLRelativeIRIValue:
    FirstRelativeArcIdentifier
    SubsequentArcIdentifier

EmbeddedPDVType:
    EMBEDDED
    PDV;

EmbeddedPDVValue:
    SequenceValue

XMLEmbeddedPDVValue:
    XMLSequenceValue;

ExternalType:
    EXTERNAL;

ExternalValue:
    SequenceValue;

XMLExternalValue:
    XMLSequenceValue;

TimeType:
    TIME;

TimeValue:
    tstring;

XMLTimeValue:
    xmltstring;

DateType:
    DATE;

TimeOfDayType:
    TIME_OF_DAY;

DateTimeType:
    DATE_TIME;

DurationType:
    DURATION;

CharacterStringType:
    RestrictedCharacterStringType
|   UnrestrictedCharacterStringType;

CharacterStringValue:
    RestrictedCharacterStringValue
|   UnrestrictedCharacterStringValue;

XMLCharacterStringValue:
    XMLRestrictedCharacterStringValue
|   XMLUnrestrictedCharacterStringValue;

RestrictedCharacterStringType:
    BMPString
|   GeneralString
|   GraphicString
|   IA5String
|   ISO646String
|   NumericString
|   PrintableString
|   TeletexString
|   T61String
|   UniversalString
|   UTF8String
|   VideotexString
|   VisibleString;

RestrictedCharacterStringValue:
    cstring
|   CharacterStringList
|   Quadruple
|   Tuple;

CharacterStringList:
    "{" CharSyms "}";

CharSyms:
    CharsDefn
|   CharSyms "," CharsDefn;

CharsDefn:
    cstring
|   Quadruple
|   Tuple
|   DefinedValue;

Quadruple:
    "{" Group "," Plane "," Row "," Cell "}";

Group:
    number;

Plane:
    number;

Row:
    number;

Cell:
    number;

Tuple:
    "{" TableColumn "," TableRow "}";

TableColumn:
    number;

TableRow:
    number;

XMLRestrictedCharacterStringValue:
    xmlcstring;

UnrestrictedCharacterStringType:
    CHARACTER STRING;

UnrestrictedCharacterStringValue:
    SequenceValue;

XMLUnrestrictedCharacterStringValue:
    XMLSequenceValue;

UsefulType:
    typereference
    UTF8String
    GraphicString
    NumericString
    VisibleString
    PrintableString
    TeletexString
    ISO646String
    GeneralString
    T61String
    VideotexString
    UniversalString
    BMPString
    IA5String
    GeneralizedTime
    UTCTime
    ObjectDescriptor;

ConstrainedType:
    Type Constraint
|   TypeWithConstraint;

TypeWithConstraint:
    SET Constraint OF Type
|   SET SizeConstraint OF Type
|   SEQUENCE Constraint OF Type
|   SEQUENCE SizeConstraint OF Type
|   SET Constraint OF NamedType
|   SET SizeConstraint OF NamedType
|   SEQUENCE Constraint OF NamedType
|   SEQUENCE SizeConstraint OF NamedType;

Constraint:
    "(" ConstraintSpec ExceptionSpec ")";

ConstraintSpec:
    SubtypeConstraint
|   GeneralConstraint;

SubtypeConstraint:
    ElementSetSpecs;

ElementSetSpecs:
    RootElementSetSpec
|   RootElementSetSpec "," " ... "
|   RootElementSetSpec "," " ... " "," AdditionalElementSetSpec;

RootElementSetSpec:
    ElementSetSpec;

AdditionalElementSetSpec:
    ElementSetSpec;

ElementSetSpec:
    Unions
|   ALL Exclusions;

Unions:
    Intersections
|   UElems UnionMark Intersections;

UElems:
    Unions;

Intersections:
    IntersectionElements
|   IElems IntersectionMark IntersectionElements;

IElems:
    Intersections;

IntersectionElements:
    Elements
|   Elems Exclusions;

Elems:
    Elements;

Exclusions:
    EXCEPT Elements;

UnionMark:
    "|"
|   UNION;

IntersectionMark:
    " ^ "
|   INTERSECTION;

Elements:
    SubtypeElements
|   ObjectSetElements
|   "(" ElementSetSpec ")";

SubtypeElements:
    SingleValue
|   ContainedSubtype
|   ValueRange
|   PermittedAlphabet
|   SizeConstraint
|   TypeConstraint
|   InnerTypeConstraints
|   PatternConstraint
|   PropertySettings
|   DurationRange
|   TimePointRange
|   RecurrenceRange;

SingleValue:
    Value;

ContainedSubtype:
    Includes Type;

Includes:
    INCLUDES
|   %empty;

ValueRange:
    LowerEndpoint " .. " UpperEndpoint;

LowerEndpoint:
    LowerEndValue
|   LowerEndValue "<";

UpperEndpoint:
    UpperEndValue
|   "<" UpperEndValue;

LowerEndValue:
    Value
|   MIN;

UpperEndValue:
    Value
|   MAX;

SizeConstraint:
    SIZE Constraint;

TypeConstraint:
    Type;

PermittedAlphabet:
    FROM Constraint;

InnerTypeConstraints:
    WITH COMPONENT SingleTypeConstraint
|   WITH COMPONENTS MultipleTypeConstraints;

SingleTypeConstraint:
    Constraint;

MultipleTypeConstraints:
    FullSpecification
|   PartialSpecification;

FullSpecification:
    "{" TypeConstraints "}";

PartialSpecification:
    "{" " ... " "," TypeConstraints "}";

TypeConstraints:
    NamedConstraint
|   NamedConstraint "," TypeConstraints;

NamedConstraint:
    identifier ComponentConstraint;

ComponentConstraint:
    ValueConstraint PresenceConstraint;

ValueConstraint:
    Constraint
|   %empty;

PresenceConstraint:
    PRESENT
|   ABSENT
|   OPTIONAL
|   %empty;

PatternConstraint:
    PATTERN Value;

PropertySettings:
    SETTINGS simplestring;

PropertySettingsList:
    PropertyAndSettingPair
|   PropertySettingsList PropertyAndSettingPair;

PropertyAndSettingPair:
    PropertyName "=" SettingName;

PropertyName:
    psname;

SettingName:
    psname;

DurationRange:
    ValueRange;

TimePointRange:
    ValueRange;

RecurrenceRange:
    ValueRange;

ExceptionSpec:
    "!" ExceptionIdentification
|   %empty;

ExceptionIdentification:
    SignedNumber
|   DefinedValue
|   Type DEFINED_AS Value;

typereference:
	GENERIC_IDENTIFIER;

xmlasn1typename:
	GENERIC_IDENTIFIER;

identifier:
	GENERIC_IDENTIFIER;

valuereference:
	GENERIC_IDENTIFIER;

modulereference:
	GENERIC_IDENTIFIER;

%%


namespace yy {

    asn1_parser::symbol_type yylex(Context& context)
    {
        const char* start = context.cursor;
        const char* YYMARKER = nullptr;
        context.location.step();

        // Lexer
        %{
			re2c:yyfill:enable   = 0;
			re2c:define:YYCTYPE  = "char";
			re2c:define:YYCURSOR = "context.cursor";

			// Keywords
			"ABSENT"                { context.location.columns(context.cursor - start); return asn1_parser::make_ABSENT (context.location); }
			"ABSTRACT_SYNTAX"       { context.location.columns(context.cursor - start); return asn1_parser::make_ABSTRACT_SYNTAX (context.location); }
			"ALL"                   { context.location.columns(context.cursor - start); return asn1_parser::make_ALL (context.location); }
			"APPLICATION"           { context.location.columns(context.cursor - start); return asn1_parser::make_APPLICATION (context.location); }
			"ASN_NULL"              { context.location.columns(context.cursor - start); return asn1_parser::make_ASN_NULL (context.location); }
			"AUTOMATIC"             { context.location.columns(context.cursor - start); return asn1_parser::make_AUTOMATIC (context.location); }
			"BEGIN"                 { context.location.columns(context.cursor - start); return asn1_parser::make_BEGIN (context.location); }
			"BIT"                   { context.location.columns(context.cursor - start); return asn1_parser::make_BIT (context.location); }
			"BMPString"             { context.location.columns(context.cursor - start); return asn1_parser::make_BMPString (context.location); }
			"BOOLEAN"               { context.location.columns(context.cursor - start); return asn1_parser::make_BOOLEAN (context.location); }
			"BY"                    { context.location.columns(context.cursor - start); return asn1_parser::make_BY (context.location); }
			"CHARACTER"             { context.location.columns(context.cursor - start); return asn1_parser::make_CHARACTER (context.location); }
			"CHOICE"                { context.location.columns(context.cursor - start); return asn1_parser::make_CHOICE (context.location); }
			"CLASS"                 { context.location.columns(context.cursor - start); return asn1_parser::make_CLASS (context.location); }
			"COMPONENT"             { context.location.columns(context.cursor - start); return asn1_parser::make_COMPONENT (context.location); }
			"COMPONENTS"            { context.location.columns(context.cursor - start); return asn1_parser::make_COMPONENTS (context.location); }
			"CONSTRAINED"           { context.location.columns(context.cursor - start); return asn1_parser::make_CONSTRAINED (context.location); }
			"CONTAINING"            { context.location.columns(context.cursor - start); return asn1_parser::make_CONTAINING (context.location); }
			"DATE"                  { context.location.columns(context.cursor - start); return asn1_parser::make_DATE (context.location); }
			"DATE_TIME"             { context.location.columns(context.cursor - start); return asn1_parser::make_DATE_TIME (context.location); }
            "DEFAULT"               { context.location.columns(context.cursor - start); return asn1_parser::make_DEFAULT (context.location); }
            "DEFINITIONS"           { context.location.columns(context.cursor - start); return asn1_parser::make_DEFINITIONS (context.location); }
            "DURATION"              { context.location.columns(context.cursor - start); return asn1_parser::make_DURATION (context.location); }
            "EMBEDDED"              { context.location.columns(context.cursor - start); return asn1_parser::make_EMBEDDED (context.location); }
            "ENCODED"               { context.location.columns(context.cursor - start); return asn1_parser::make_ENCODED (context.location); }
            "ENCODING_CONTROL"      { context.location.columns(context.cursor - start); return asn1_parser::make_ENCODING_CONTROL (context.location); }
            "END"                   { context.location.columns(context.cursor - start); return asn1_parser::make_END (context.location); }
            "ENUMERATED"            { context.location.columns(context.cursor - start); return asn1_parser::make_ENUMERATED (context.location); }
            "EXCEPT"                { context.location.columns(context.cursor - start); return asn1_parser::make_EXCEPT (context.location); }
            "EXPLICIT"              { context.location.columns(context.cursor - start); return asn1_parser::make_EXPLICIT (context.location); }
            "EXPORTS"               { context.location.columns(context.cursor - start); return asn1_parser::make_EXPORTS (context.location); }
            "EXTENSIBILITY"         { context.location.columns(context.cursor - start); return asn1_parser::make_EXTENSIBILITY (context.location); }
            "EXTERNAL"              { context.location.columns(context.cursor - start); return asn1_parser::make_EXTERNAL (context.location); }
            "FALSE"                 { context.location.columns(context.cursor - start); return asn1_parser::make_FALSE (context.location); }
            "FROM"                  { context.location.columns(context.cursor - start); return asn1_parser::make_FROM (context.location); }
            "GeneralizedTime"       { context.location.columns(context.cursor - start); return asn1_parser::make_GeneralizedTime (context.location); }
            "GeneralString"         { context.location.columns(context.cursor - start); return asn1_parser::make_GeneralString (context.location); }
            "GraphicString"         { context.location.columns(context.cursor - start); return asn1_parser::make_GraphicString (context.location); }
            "IA5String"             { context.location.columns(context.cursor - start); return asn1_parser::make_IA5String (context.location); }
            "IDENTIFIER"            { context.location.columns(context.cursor - start); return asn1_parser::make_IDENTIFIER (context.location); }
            "IMPLICIT"              { context.location.columns(context.cursor - start); return asn1_parser::make_IMPLICIT (context.location); }
            "IMPLIED"               { context.location.columns(context.cursor - start); return asn1_parser::make_IMPLIED (context.location); }
            "IMPORTS"               { context.location.columns(context.cursor - start); return asn1_parser::make_IMPORTS (context.location); }
            "INCLUDES"              { context.location.columns(context.cursor - start); return asn1_parser::make_INCLUDES (context.location); }
            "INSTANCE"              { context.location.columns(context.cursor - start); return asn1_parser::make_INSTANCE (context.location); }
            "INSTRUCTIONS"          { context.location.columns(context.cursor - start); return asn1_parser::make_INSTRUCTIONS (context.location); }
            "INTEGER"               { context.location.columns(context.cursor - start); return asn1_parser::make_INTEGER (context.location); }
            "INTERSECTION"          { context.location.columns(context.cursor - start); return asn1_parser::make_INTERSECTION (context.location); }
            "ISO646String"          { context.location.columns(context.cursor - start); return asn1_parser::make_ISO646String (context.location); }
            "MAX"                   { context.location.columns(context.cursor - start); return asn1_parser::make_MAX (context.location); }
            "MIN"                   { context.location.columns(context.cursor - start); return asn1_parser::make_MIN (context.location); }
            "MINUS_INFINITY"        { context.location.columns(context.cursor - start); return asn1_parser::make_MINUS_INFINITY (context.location); }
            "NOT_A_NUMBER"          { context.location.columns(context.cursor - start); return asn1_parser::make_NOT_A_NUMBER (context.location); }
            "NumericString"         { context.location.columns(context.cursor - start); return asn1_parser::make_NumericString (context.location); }
            "OBJECT"                { context.location.columns(context.cursor - start); return asn1_parser::make_OBJECT (context.location); }
            "ObjectDescriptor"      { context.location.columns(context.cursor - start); return asn1_parser::make_ObjectDescriptor (context.location); }
            "OCTET"                 { context.location.columns(context.cursor - start); return asn1_parser::make_OCTET (context.location); }
            "OF"                    { context.location.columns(context.cursor - start); return asn1_parser::make_OF (context.location); }
            "OID_IRI"               { context.location.columns(context.cursor - start); return asn1_parser::make_OID_IRI (context.location); }
            "OPTIONAL"              { context.location.columns(context.cursor - start); return asn1_parser::make_OPTIONAL (context.location); }
            "PATTERN"               { context.location.columns(context.cursor - start); return asn1_parser::make_PATTERN (context.location); }
            "PDV"                   { context.location.columns(context.cursor - start); return asn1_parser::make_PDV (context.location); }
            "PLUS_INFINITY"         { context.location.columns(context.cursor - start); return asn1_parser::make_PLUS_INFINITY (context.location); }
            "PRESENT"               { context.location.columns(context.cursor - start); return asn1_parser::make_PRESENT (context.location); }
            "PrintableString"       { context.location.columns(context.cursor - start); return asn1_parser::make_PrintableString (context.location); }
            "PRIVATE"               { context.location.columns(context.cursor - start); return asn1_parser::make_PRIVATE (context.location); }
            "REAL"                  { context.location.columns(context.cursor - start); return asn1_parser::make_REAL (context.location); }
            "RELATIVE_OID"          { context.location.columns(context.cursor - start); return asn1_parser::make_RELATIVE_OID (context.location); }
            "RELATIVE_OID_IRI"      { context.location.columns(context.cursor - start); return asn1_parser::make_RELATIVE_OID_IRI (context.location); }
            "SEQUENCE"              { context.location.columns(context.cursor - start); return asn1_parser::make_SEQUENCE (context.location); }
            "SET"                   { context.location.columns(context.cursor - start); return asn1_parser::make_SET (context.location); }
            "SETTINGS"              { context.location.columns(context.cursor - start); return asn1_parser::make_SETTINGS (context.location); }
            "SIZE"                  { context.location.columns(context.cursor - start); return asn1_parser::make_SIZE (context.location); }
            "STRING"                { context.location.columns(context.cursor - start); return asn1_parser::make_STRING (context.location); }
            "SYNTAX"                { context.location.columns(context.cursor - start); return asn1_parser::make_SYNTAX (context.location); }
            "T61String"             { context.location.columns(context.cursor - start); return asn1_parser::make_T61String (context.location); }
            "TAGS"                  { context.location.columns(context.cursor - start); return asn1_parser::make_TAGS (context.location); }
            "TeletexString"         { context.location.columns(context.cursor - start); return asn1_parser::make_TeletexString (context.location); }
            "TIME"                  { context.location.columns(context.cursor - start); return asn1_parser::make_TIME (context.location); }
            "TIME_OF_DAY"           { context.location.columns(context.cursor - start); return asn1_parser::make_TIME_OF_DAY (context.location); }
            "TRUE"                  { context.location.columns(context.cursor - start); return asn1_parser::make_TRUE (context.location); }
            "TYPE_IDENTIFIER"       { context.location.columns(context.cursor - start); return asn1_parser::make_TYPE_IDENTIFIER (context.location); }
            "UNION"                 { context.location.columns(context.cursor - start); return asn1_parser::make_UNION (context.location); }
            "UNIQUE"                { context.location.columns(context.cursor - start); return asn1_parser::make_UNIQUE (context.location); }
            "UNIVERSAL"             { context.location.columns(context.cursor - start); return asn1_parser::make_UNIVERSAL (context.location); }
            "UniversalString"       { context.location.columns(context.cursor - start); return asn1_parser::make_UniversalString (context.location); }
            "UTCTime"               { context.location.columns(context.cursor - start); return asn1_parser::make_UTCTime (context.location); }
            "UTF8String"            { context.location.columns(context.cursor - start); return asn1_parser::make_UTF8String (context.location); }
            "VideotexString"        { context.location.columns(context.cursor - start); return asn1_parser::make_VideotexString (context.location); }
            "VisibleString"         { context.location.columns(context.cursor - start); return asn1_parser::make_VisibleString (context.location); }
            "WITH"                  { context.location.columns(context.cursor - start); return asn1_parser::make_WITH (context.location); }

            "::="                   { context.location.columns(context.cursor - start); return asn1_parser::make_DEFINED_AS (context.location); }

            // Symbols
            ":"                     { context.location.columns(context.cursor - start); return asn1_parser::make_COLON(context.location); }
            "{"                     { context.location.columns(context.cursor - start); return asn1_parser::make_OPEN_BRACE(context.location); }
            "}"                     { context.location.columns(context.cursor - start); return asn1_parser::make_CLOSE_BRACE(context.location); }
            [{}<>,\.]               { context.location.columns(context.cursor - start); return asn1_parser::symbol_type(asn1_parser::token_type(*start), context.location); }

            // Identifiers
            [A-Za-z_0-9]+               { context.location.columns(context.cursor - start); return asn1_parser::make_GENERIC_IDENTIFIER(std::string(start, context.cursor), context.location); }

            // End of file
            "\000"                  { context.location.columns(context.cursor - start); return asn1_parser::make_END_OF_FILE(context.location); }

            // White space
            "\r\n" | [\r\n]         { context.location.columns(context.cursor - start); context.location.lines();   return yylex(context); }
            [\t\v\b\f ]             { context.location.columns(context.cursor - start); context.location.columns(); return yylex(context); }

            // Comments
            "--" [^\r\n]*           { context.location.columns(context.cursor - start); return yylex(context); }
        %}
    }
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cout << "Please provide ASN file for parsing\n";
        return -1;
    }

    std::string filename = argv[1];
    std::ifstream f(filename);
    if (!f.good())
    {
        std::cout << "Could not open input file\n";
        return -1;
    }

    std::string buffer(std::istreambuf_iterator<char>(f), {});

    Context context;
    context.cursor = buffer.c_str();
    context.location.begin.filename = &filename;
    context.location.end.filename   = &filename;

    yy::asn1_parser parser(context);

    const auto res = parser.parse();

    if (!res)
    {
      std::cout << "Parse success!\n";
    }
}
