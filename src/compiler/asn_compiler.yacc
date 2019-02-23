%skeleton "lalr1.cc"
%define parser_class_name {asn1_parser}
%define api.token.constructor
%define api.value.type variant
%define parse.assert
%define parse.error verbose
%locations

%code requires
{
    #include "fast_ber/compiler/CompilerTypes.hpp"
}

%code
{
    struct Context
    {
        const char* cursor;
        yy::location location;

        Asn1Tree asn1_tree;
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
%token INCLUDES "INCLUDES"
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

%token DEFINED_AS "::="
%token ELIPSIS "..."
%token RANGE ".."
%token OPEN_BRACE "{"
%token CLOSE_BRACE "}"
%token OPEN_PARENTHESIS "("
%token CLOSE_PARENTHESIS ")"
%token OPEN_SQUARE_BRACKET "["
%token CLOSE_SQUARE_BRACKET "]"
%token LESS_THAN "<"
%token GREATER_THAN ">"
%token EXCLAMATION_MARK "!"
%token QUOTATION_MARK "\""
%token AMPERSAND "&"
%token APOSTROPHE "\'"
%token ASTERISK "*"
%token COMMA ","
%token FULL_STOP "."
%token HYPHEN_MINUS "-"
%token SOLIDUS "/"
%token COLON ":"
%token SEMICOLON ";"
%token EQUALS_SIGN "="
%token AT "@"
%token VERTICAL_LINE "|"
%token ACCENT "^"

%token PLUS
%token STAR
%token QUESTION_MARK
%token GENERIC_IDENTIFIER_UPPERCASE
%token GENERIC_IDENTIFIER_LOWERCASE
%token GENERIC_INTEGER

%token xmlasn1typename

%type<std::string>       GENERIC_IDENTIFIER_UPPERCASE
%type<std::string>       GENERIC_IDENTIFIER_LOWERCASE
%type<std::string>       typereference
%type<std::string>       identifier
%type<std::string>       modulereference
%type<std::string>       valuereference
%type<std::string>       ModuleIdentifier
%type<std::string>       GlobalModuleReference
%type<std::string>       bstring
%type<std::string>       xmlbstring
%type<std::string>       hstring
%type<std::string>       xmlhstring
%type<std::string>       cstring
%type<std::string>       xmlcstring
%type<std::string>       simplestring
%type<std::string>       xmltstring
%type<double>            realnumber
%type<long long>         number
%type<long long>         SignedNumber
%type<DefinedValue>      DefinedValue
%type<Value>             ReferencedValue
%type<long long>         IntegerValue
%type<BuiltinType>       BuiltinType;
%type<DefinedType>       DefinedType;
%type<EnumeratedType>    EnumeratedType;
%type<EnumeratedType>    Enumerations;
%type<EnumeratedType>    Enumeration;
%type<EnumerationValue>  EnumerationItem;
%type<std::vector<Export>> Exports;
%type<Module>            ModuleBody;
%type<std::vector<NamedType>> AlternativeTypeList;
%type<std::vector<NamedType>> AlternativeTypeLists
%type<std::vector<NamedType>> RootAlternativeTypeList;
%type<std::vector<Assignment>> AssignmentList;
%type<Assignment>        Assignment;
%type<Assignment>        TypeAssignment;
%type<Assignment>        ValueAssignment;
%type<Type>              Type;
%type<Class>             Class;
%type<int>               ClassNumber;

%type<SetType>           SetType;
%type<SequenceType>      SequenceType;
%type<NamedType>         NamedType;
%type<NamedNumber>       NamedNumber;
%type<ComponentType>     ComponentType;
%type<ComponentTypeList> ComponentTypeList;
%type<ComponentTypeList> ComponentTypeLists;
%type<ComponentTypeList> RootComponentTypeList;
%type<Value>             Value
%type<Value>             BuiltinValue;
%type<PrefixedType>      PrefixedType;
%type<std::string>       Reference;
%type<SetOfType>         SetOfType;
%type<SequenceOfType>    SequenceOfType;
%type<std::string>       Symbol;
%type<std::vector<Import>> Imports;
%type<std::vector<Import>> SymbolsImported;
%type<std::vector<Import>> SymbolsFromModuleList;
%type<Import>            SymbolsFromModule;
%type<std::vector<std::string>> SymbolList;
%type<ChoiceType>        ChoiceType;
%type<TaggingMode>       TagDefault;
%type<Tag>               Tag;
%type<TaggedType>        TaggedType;
%type<ObjectIdComponentValue> ObjIdComponents;
%type<ObjectIdComponentValue> NameForm;
%type<ObjectIdComponentValue> NumberForm;
%type<ObjectIdComponentValue> NameAndNumberForm;
%type<std::vector<ObjectIdComponentValue>> ObjIdComponentsList;
%type<std::vector<ObjectIdComponentValue>> ObjectIdentifierValue;

%%

ModuleDefinitionList:
    ModuleDefinition
|   ModuleDefinition ModuleDefinitionList

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
    END
    { $8.module_reference = $1;
      $8.tagging_default = $4;
      context.asn1_tree.modules.push_back($8); }

//SyntaxList:
//   "{" TokenOrGroupSpec "}";

//EncodingInstruction:
//    %empty;

//EncodingInstructionAssignmentList:
//    %empty;

//ExensionEndMarker:
//    %empty;

DefinedObjectClass:
    ExternalObjectClassReference
|   objectclassreference
|   UsefulObjectClassReference;

ExternalObjectClassReference:
    modulereference "." objectclassreference;

UsefulObjectClassReference:
    TYPE_IDENTIFIER
|   ABSTRACT_SYNTAX;

//ObjectClassAssignment:
//    objectclassreference DEFINED_AS ObjectClass;

//ObjectClass:
//    DefinedObjectClass
//|   ObjectClassDefn
//|   ParameterizedObjectClass;

//ObjectClassDefn:
//    CLASS "{" FieldSpec "," PLUS "}" WithSyntaxSpec QUESTION_MARK;

//FieldSpec:
//    TypeFieldSpec
//|   FixedTypeValueFieldSpec
//|   VariableTypeValueFieldSpec
//|   ObjectFieldSpec
//|   ObjectSetFieldSpec;

PrimitiveFieldName:
    typefieldreference
|   valuefieldreference
|   valuesetfieldreference
|   objectfieldreference
|   objectsetfieldreference;

FieldName:
    PrimitiveFieldName "." PLUS;

//TypeFieldSpec:
//    typefieldreference TypeOptionalitySpec QUESTION_MARK;

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

//ObjectSeWithSyntaxSpec:
//    WITH SYNTAX SyntaxList;

//WithSyntaxSpec:
//    WITH SYNTAX SyntaxList;

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
//|   ObjectFromObject
//|   ParameterizedObject;

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
|   Object

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
|   RootElementSetSpec "," ELIPSIS
|   ELIPSIS
|   ELIPSIS "," AdditionalElementSetSpec
|   RootElementSetSpec "," ELIPSIS "," AdditionalElementSetSpec;

ObjectSetElements:
    Object
|   DefinedObjectSet
//|   ObjectSetFromObjects
//|   ParameterizedObjectSet;

ObjectClassFieldType:
    DefinedObjectClass "." FieldName;

ObjectClassFieldValue:
    OpenTypeFieldVal

OpenTypeFieldVal:
    Type COLON Value;

/*
InformationFromObjects:
    ValueFromObject
|   ValueSetFromObjects
|   TypeFromObject
|   ObjectFromObject
|   ObjectSetFromObjects;
*/
ReferencedObjects:
    DefinedObject
//|   ParameterizedObject
//|   DefinedObjectSet
//|   ParameterizedObjectSet;

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

ParameterizedReference:
    Reference
|   Reference "{" "}";

SimpleDefinedType:
    ExternalTypeReference
|   typereference;

SimpleDefinedValue:
    ExternalValueReference
|   valuereference;

GeneralConstraint:
//    UserDefinedConstraint
//|   TableConstraint
    ContentsConstraint;

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

//EncodingPrefixedType:
//    EncodingPrefix Type;

//EncodingPrefix:
//    "[" EncodingReference EncodingInstruction "]";

EncodingControlSections:
//    EncodingControlSection EncodingControlSections
//|
    %empty;

//EncodingControlSection:
//    ENCODING_CONTROL
//    encodingreference
//    EncodingInstructionAssignmentList;

ModuleIdentifier:
    modulereference
    DefinitiveIdentification
{
    $$ = $1;
};

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
    { $$ = TaggingMode::explicit_; }
|   IMPLICIT TAGS
    { $$ = TaggingMode::implicit; }
|   AUTOMATIC TAGS
    { $$ = TaggingMode::automatic; }
|   %empty
    { $$ = TaggingMode::explicit_; }

ExtensionDefault:
    EXTENSIBILITY IMPLIED
|   %empty;

ModuleBody:
    Exports Imports AssignmentList
    { $$ = Module{ {}, TaggingMode(), $1, $2, $3}; }
|   %empty
    { }

Exports:
    EXPORTS SymbolsExported ";"
|   EXPORTS ALL ";"
|   %empty;

SymbolsExported:
    SymbolList
|   %empty;

Imports:
    IMPORTS SymbolsImported ";"
    { $$ = $2; }
|   %empty;

SymbolsImported:
    SymbolsFromModuleList
    { $$ = $1; }
|   %empty;

SymbolsFromModuleList:
    SymbolsFromModule
    { $$.push_back($1); }
|   SymbolsFromModuleList SymbolsFromModule
    { $$ = $1; $$.push_back($2); }

SymbolsFromModule:
    SymbolList FROM GlobalModuleReference
    { $$ = Import{ $3, $1 }; }

GlobalModuleReference:
    modulereference AssignedIdentifier
    { $$ = $1; }

AssignedIdentifier:
    ObjectIdentifierValue
|   DefinedValue
|   %empty;

SymbolList:
    Symbol
    { $$.push_back($1); }
|   SymbolList "," Symbol
    { $$ = $1; $$.push_back($3); }

Symbol:
    Reference
    { $$ = $1; }

Reference:
    typereference
    { $$ = $1; }
|   valuereference
    { $$ = $1; }
|   objectclassreference
|   objectreference
|   objectsetreference;

AssignmentList:
    Assignment
    { $$.push_back($1);}
|   AssignmentList Assignment
    { $$ = $1; $$.push_back($2); }

Assignment:
    TypeAssignment
    { $$ = $1; }
|   ValueAssignment
    { $$ = $1; }
/*|   ValueSetTypeAssignment
|   ObjectClassAssignment
|   ObjectAssignment
|   ObjectSetAssignment
|   ParameterizedAssignment*/

DefinedType:
    ExternalTypeReference
|   typereference
    { $$ = DefinedType{$1}; }
//|   ParameterizedType
//|   ParameterizedValueSetType;

DefinedValue:
//    ExternalValueReference
    valuereference
    { $$ = DefinedValue{$1}; }
//|   ParameterizedValue;

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

/*
AbsoluteReference:
    "@"
    ModuleIdentifier
    "."
    ItemSpec;

ItemSpec:
    typereference
|   ItemId FULL_STOP ComponentId;

ItemId:
    ItemSpec;

ComponentId:
    identifier
|   number
|   "*";
*/

TypeAssignment:
    typereference
    DEFINED_AS
    Type
    { $$ = Assignment{ $1, $3, absl::nullopt, {} }; }

ValueAssignment:
    valuereference
    Type
    DEFINED_AS
    Value
    { $$ = Assignment{ $1, $2, $4, {} }; }

Type:
    BuiltinType
    { $$ = $1; }
|   ConstrainedType
    { $$ = Type(); }
|   DefinedType
    { $$ = $1; }
|   SelectionType
    { throw std::runtime_error("Not handled - SelectionType"); }
|   TypeFromObject
    { throw std::runtime_error("Not handled - TypeFromObject"); }
//|   ValueSetFromObjects { throw std::runtime_error("Not handled - ValueSetFromObjects"); }

BuiltinType:
    BitStringType { $$ = BitStringType(); }
|   BooleanType { $$ = BooleanType(); }
|   CharacterStringType { $$ = CharacterStringType(); }
|   ChoiceType { $$ = $1; }
|   DateType { $$ = DateType(); }
|   DateTimeType { $$ = DateTimeType(); }
|   DurationType { $$ = DurationType(); }
|   EmbeddedPDVType { $$ = EmbeddedPDVType(); }
|   EnumeratedType { $$ = $1; }
|   ExternalType { $$ = ExternalType(); }
|   GeneralizedTime { $$ = GeneralizedTimeType(); }
|   InstanceOfType { $$ = InstanceOfType(); }
|   IntegerType { $$ = IntegerType(); }
|   IRIType { $$ = IRIType(); }
|   NullType { $$ = NullType(); }
|   ObjectClassFieldType { $$ = ObjectClassFieldType(); }
|   ObjectDescriptor { $$ = ObjectDescriptorType(); }
|   ObjectIdentifierType { $$ = ObjectIdentifierType(); }
|   OctetStringType { $$ = OctetStringType(); }
|   RealType { $$ = RealType(); }
|   RelativeIRIType { $$ = RelativeIRIType(); }
|   RelativeOIDType { $$ = RelativeOIDType(); }
|   SequenceType { $$ = $1; }
|   SequenceOfType { $$ = $1; }
|   SetType { $$ = $1; }
|   SetOfType { $$ = $1; }
|   PrefixedType { $$ = $1; }
|   TimeType { $$ = TimeType(); }
|   TimeOfDayType { $$ = TimeOfDayType(); }
|   UTCTime { $$ = UTCTimeType(); }

NamedType:
    identifier Type
    { $$ = NamedType{ $1, $2 }; }

Value:
    BuiltinValue
    { $$ = $1; }
|   ReferencedValue
    { $$ = $1; }
|   ObjectClassFieldValue
    { throw std::runtime_error("Unhandled field: ObjectClassFieldValue"); }

BuiltinValue:
//    BitStringValue
   BooleanValue
//|   ChoiceValue
//|   EmbeddedPDVValue
//|   EnumeratedValue
//|   ExternalValue
|   IntegerValue
    { $$.value_selection = $1; }
|   IRIValue
//|   NullValue
|   ObjectIdentifierValue
    { $$.value_selection = $1; }
//|   RealValue
|   TimeValue
|   bstring
|   hstring
|   cstring
    { $$.value_selection = $1; }
|   CONTAINING Value;

ReferencedValue:
    DefinedValue
    { $$.defined_value = $1; }
//|   ValueFromObject;

NamedValue:
    identifier
    Value;

BooleanType:
    BOOLEAN;

BooleanValue:
    TRUE
|   FALSE;

IntegerType:
    INTEGER
|   INTEGER "{" NamedNumberList "}";

NamedNumberList:
    NamedNumber
|   NamedNumberList "," NamedNumber;

NamedNumber:
    identifier "(" SignedNumber ")"
    { $$ = NamedNumber{ $1, $3 }; }
|   identifier "(" DefinedValue ")"
    { $$ = NamedNumber{ $1, 0 }; }

SignedNumber:
    number
    { $$ = $1; }
|   "-" number
    { $$ = -$2; }

IntegerValue:
    SignedNumber
    { $$ = $1; }

EnumeratedType:
    ENUMERATED "{" Enumerations "}"
    { $$ = $3; }

Enumerations:
    Enumeration
    { $$ = $1;
      $$.accept_anything = false; }
|   Enumeration "," ELIPSIS ExceptionSpec
    { $$ = $1;
      $$.accept_anything = true; }
|   Enumeration "," ELIPSIS ExceptionSpec "," Enumeration
    { $$ = $1;
      $$.accept_anything = true;
      $$.enum_values.insert($$.enum_values.end(), $6.enum_values.begin(), $6.enum_values.end()); }
|   ELIPSIS ExceptionSpec

Enumeration:
    EnumerationItem
    { $$.enum_values.push_back($1); }
|   Enumeration "," EnumerationItem
    { $$ = $1; $$.enum_values.push_back($3); }

EnumerationItem:
    identifier
    { $$.name  = $1; }
|   NamedNumber
    { $$.name  = $1.name;
      $$.value = $1.number; }

EnumeratedValue:
    identifier;

RealType:
    REAL;

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

OctetStringType:
    OCTET STRING;

NullType:
    ASN_NULL;

SequenceType:
    SEQUENCE "{" "}"
    { $$ = SequenceType(); }
|   SEQUENCE "{" ComponentTypeLists "}"
    { $$ = SequenceType{$3}; }

ExtensionAndException:
    ELIPSIS
|   ELIPSIS ExceptionSpec;

ComponentTypeLists:
    RootComponentTypeList
    { $$ = $1; }

RootComponentTypeList:
    ComponentTypeList
    { $$ = $1; }
|   ComponentTypeList "," ELIPSIS
    { $$ = $1; }
|   ComponentTypeList "," ELIPSIS "," ComponentTypeList
    { $$ = $1; $$.insert($$.end(), $5.begin(), $5.end()); }
|   ComponentTypeList "," ELIPSIS "," ComponentTypeList "," ELIPSIS
    { $$ = $1; $$.insert($$.end(), $5.begin(), $5.end()); }
|   ELIPSIS "," ComponentTypeList
    { $$ = $3; }
|   ELIPSIS "," ComponentTypeList "," ELIPSIS
    { $$ = $3; }
|   ELIPSIS
    { $$ = {}; }
|   ELIPSIS "," ELIPSIS
    { $$ = {}; }

ComponentTypeList:
    ComponentType
    { $$ = ComponentTypeList{$1}; }
|   ComponentTypeList "," ComponentType
    { $1.push_back($3); $$ = $1; }

ComponentType:
    NamedType
    { $$ = ComponentType{$1, false, absl::nullopt}; }
|   NamedType OPTIONAL
    { $$ = ComponentType{$1, true, absl::nullopt}; }
|   NamedType DEFAULT Value
    { $$ = ComponentType{$1, false, $3}; }
|   COMPONENTS OF Type

SequenceValue:
    "{" ComponentValueList "}"
|   "{" "}";

ComponentValueList:
    NamedValue
|   ComponentValueList "," NamedValue;

SequenceOfType:
    SEQUENCE OF Type
    { $$ = SequenceOfType{ false, nullptr, std::make_shared<Type>($3) }; }
|   SEQUENCE OF NamedType
    { $$ = SequenceOfType{ true, std::make_shared<NamedType>($3), nullptr }; }

SetType:
    SET "{" "}"
    { $$ = SetType{}; }
|   SET "{" ComponentTypeLists "}"
    { $$ = SetType{$3}; }

SetOfType:
    SET OF Type
    { $$ = SetOfType{ false, nullptr, std::make_shared<Type>($3) }; }
|   SET OF NamedType
    { $$ = SetOfType{ true, std::make_shared<NamedType>($3), nullptr }; }

ChoiceType:
    CHOICE "{" AlternativeTypeLists "}"
    { $$ = ChoiceType{ $3 }; }

AlternativeTypeLists:
    RootAlternativeTypeList
    { $$ = $1; }

RootAlternativeTypeList:
    AlternativeTypeList
    { $$ = $1; }
|   AlternativeTypeList "," ELIPSIS
    { $$ = $1; }

AlternativeTypeList:
    NamedType
    { $$ = std::vector<NamedType> { $1 }; }
|   AlternativeTypeList "," NamedType
    { $1.push_back( $3 ); $$ = $1; }

ChoiceValue:
    identifier DEFINED_AS Value;

SelectionType:
    identifier "<" Type;

PrefixedType:
    TaggedType
    { $$ = PrefixedType($1); }

TaggedType:
    Tag Type
    { $$ = TaggedType{ $1, TaggingMode::automatic, $2 }; }
|   Tag IMPLICIT Type
    { $$ = TaggedType{ $1, TaggingMode::implicit, $3 }; }
|   Tag EXPLICIT Type
    { $$ = TaggedType{ $1, TaggingMode::explicit_, $3 }; }

Tag:
    "[" EncodingReference Class ClassNumber "]"
    { $$ = Tag{ $3, $4 }; }

EncodingReference:
    encodingreference ":"
|   %empty
    
ClassNumber:
    number
    { $$ = $1; }
|   DefinedValue;

Class:
    UNIVERSAL
    { $$ = Class::universal; }
|   APPLICATION
    { $$ = Class::application; }
|   PRIVATE
    { $$ = Class::private_; }
|   %empty
    { $$ = Class::context_specific; }

//EncodingPrefixedType:
//    EncodingPrefix Type;

//EncodingPrefix:
//  "[" EncodingReference EncodingInstruction "]";

ObjectIdentifierType:
    OBJECT IDENTIFIER;

ObjectIdentifierValue:
    "{" ObjIdComponentsList "}"
    { $$ = $2; std::reverse($$.begin(), $$.end()); }
|   "{" DefinedValue ObjIdComponentsList "}"
    { $$ = $3; std::reverse($$.begin(), $$.end()); }

ObjIdComponentsList:
    ObjIdComponents
    { $$.push_back($1); }
|   ObjIdComponents ObjIdComponentsList
    { $$ = $2; $$.push_back($1); }

ObjIdComponents:
    NameForm
    { $$ = $1; }
|   NumberForm
    { $$ = $1; }
|   NameAndNumberForm
    { $$ = $1; }

NameForm:
    identifier
    { $$.name = $1; }

NumberForm:
    number
    { $$.value = $1; }
|   DefinedValue

NameAndNumberForm:
    identifier "(" NumberForm ")"
    { $$ = $3; $$.name = $1; }

RelativeOIDType:
    RELATIVE_OID;

//RelativeOIDComponentsList:
//    RelativeOIDComponents
//|   RelativeOIDComponents RelativeOIDComponentsList;

//RelativeOIDComponents:
//    NumberForm
//|   NameAndNumberForm
//|   DefinedValue;

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

RelativeIRIType:
    RELATIVE_OID_IRI;

FirstRelativeArcIdentifier:
    ArcIdentifier;

EmbeddedPDVType:
    EMBEDDED
    PDV;

EmbeddedPDVValue:
    SequenceValue

ExternalType:
    EXTERNAL;

ExternalValue:
    SequenceValue;

TimeType:
    TIME;

TimeValue:
    tstring;

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


UnrestrictedCharacterStringType:
    CHARACTER STRING;

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
|   RootElementSetSpec "," ELIPSIS
|   RootElementSetSpec "," ELIPSIS "," AdditionalElementSetSpec;

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
    "^"
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
|   PropertySettings;
/*|   DurationRange
|   TimePointRange
|   RecurrenceRange;*/

SingleValue:
    Value;

ContainedSubtype:
    Includes Type;

Includes:
    INCLUDES

ValueRange:
    LowerEndpoint RANGE UpperEndpoint;

LowerEndpoint:
    LowerEndValue
|   LowerEndValue "<"

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
    "{" ELIPSIS "," TypeConstraints "}";

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

ExceptionSpec:
    "!" ExceptionIdentification
|   %empty;

ExceptionIdentification:
    SignedNumber
|   DefinedValue
|   Type DEFINED_AS Value;

typereference:
    GENERIC_IDENTIFIER_UPPERCASE
{ $$ = $1; }

identifier:
    GENERIC_IDENTIFIER_LOWERCASE
{ $$ = $1; }

valuereference:
    GENERIC_IDENTIFIER_LOWERCASE
{ $$ = $1; }

modulereference:
    GENERIC_IDENTIFIER_UPPERCASE
{ $$ = $1; }

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
"NULL"                  { context.location.columns(context.cursor - start); return asn1_parser::make_ASN_NULL (context.location); }
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

// Comments
"--" ([\-]?[^\r\n\-])* "--" 
                        { context.location.columns(context.cursor - start); return yylex(context); }
"--" ([\-]?[^\r\n\-])*  { context.location.columns(context.cursor - start); return yylex(context); }

// Identifiers
//[0-9]+\.[0-9]+        { context.location.columns(context.cursor - start); return asn1_parser::make_realnumber(std::stod(std::string(start, context.cursor)), context.location); }
[0-9]+                  { context.location.columns(context.cursor - start); return asn1_parser::make_number(std::stoll(std::string(start, context.cursor)), context.location); }
["\""]("\\".|[^"\""])*["\""] 
                        { context.location.columns(context.cursor - start); return asn1_parser::make_cstring(std::string(start, context.cursor), context.location); }
[A-Z][A-Za-z_0-9\-]+    { context.location.columns(context.cursor - start); return asn1_parser::make_GENERIC_IDENTIFIER_UPPERCASE(santize_name(std::string(start, context.cursor)), context.location); }
[a-z][A-Za-z_0-9\-]+    { context.location.columns(context.cursor - start); return asn1_parser::make_GENERIC_IDENTIFIER_LOWERCASE(santize_name(std::string(start, context.cursor)), context.location); }

// End of file
"\000"                  { context.location.columns(context.cursor - start); return asn1_parser::make_END_OF_FILE(context.location); }

// White space
"\r\n" | "\n"           { context.location.columns(context.cursor - start); context.location.lines();   return yylex(context); }
[\t\v\b\f ]             { context.location.columns(context.cursor - start); context.location.columns(); return yylex(context); }

// Symbols
"::="                   { context.location.columns(context.cursor - start); return asn1_parser::make_DEFINED_AS (context.location); }
"\.\.\."                { context.location.columns(context.cursor - start); return asn1_parser::make_ELIPSIS (context.location); }
"\.\."                  { context.location.columns(context.cursor - start); return asn1_parser::make_RANGE (context.location); }
"{"                     { context.location.columns(context.cursor - start); return asn1_parser::make_OPEN_BRACE (context.location); }
"}"                     { context.location.columns(context.cursor - start); return asn1_parser::make_CLOSE_BRACE (context.location); }
"("                     { context.location.columns(context.cursor - start); return asn1_parser::make_OPEN_PARENTHESIS (context.location); }
")"                     { context.location.columns(context.cursor - start); return asn1_parser::make_CLOSE_PARENTHESIS (context.location); }
"["                     { context.location.columns(context.cursor - start); return asn1_parser::make_OPEN_SQUARE_BRACKET (context.location); }
"]"                     { context.location.columns(context.cursor - start); return asn1_parser::make_CLOSE_SQUARE_BRACKET (context.location); }
":"                     { context.location.columns(context.cursor - start); return asn1_parser::make_COLON (context.location); }
";"                     { context.location.columns(context.cursor - start); return asn1_parser::make_SEMICOLON (context.location); }
","                     { context.location.columns(context.cursor - start); return asn1_parser::make_COMMA (context.location); }
"-"                     { context.location.columns(context.cursor - start); return asn1_parser::make_HYPHEN_MINUS (context.location); }
"\."                    { context.location.columns(context.cursor - start); return asn1_parser::make_FULL_STOP (context.location); }
.                       { throw(std::runtime_error(std::string("Unknown symbol!") + *start)); context.location.columns(context.cursor - start); return asn1_parser::symbol_type(asn1_parser::token_type(*start), context.location); }
%}
    }
}
