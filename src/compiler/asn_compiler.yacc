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
    #include "fast_ber/compiler/Logging.hpp"
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
        std::cerr << l << m << '\n';
    }

    namespace yy { asn1_parser::symbol_type yylex(Context& c); }
} // End code

%param { Context& context_ }

%token END_OF_FILE 0

%token comment
%token number
%token negativenumber
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
%token objectreference
%token objectsetreference
%token typefieldreference
%token valuefieldreference
%token valuesetfieldreference
%token objectfieldreference
%token objectsetfieldreference

%token ABSENT
%token ABSTRACT_SYNTAX
%token ALL
%token ANY
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
%type<std::string>       typefieldreference
%type<std::string>       valuefieldreference
%type<std::string>       objectreference
%type<std::string>       objectclassreference
%type<std::string>       word
%type<std::string>       ModuleIdentifier
%type<std::string>       GlobalModuleReference
%type<std::string>       UsefulObjectClassReference
%type<std::string>       bstring
%type<std::string>       xmlbstring
%type<std::string>       hstring
%type<std::string>       xmlhstring
%type<std::string>       cstring
%type<std::string>       xmlcstring
%type<std::string>       simplestring
%type<std::string>       xmltstring
%type<double>            realnumber
%type<int64_t>           number
%type<int64_t>           negativenumber
%type<int64_t>           SignedNumber
%type<DefinedValue>      DefinedValue
%type<BuiltinType>       BuiltinType;
%type<DefinedType>       DefinedType;
%type<DefinedType>       ExternalTypeReference;
%type<std::string>       SimpleDefinedType;
%type<DefinedType>       ParameterizedType;
%type<EnumeratedType>    Enumerations;
%type<EnumeratedType>    Enumeration;
%type<EnumerationValue>  EnumerationItem;
%type<std::vector<Export>> Exports;
%type<std::vector<ClassField>> FieldSpecList;
%type<std::vector<ClassField>> FieldSpec;
%type<std::vector<ClassField>> TypeFieldSpec;
%type<std::vector<ClassField>> FixedTypeValueFieldSpec;
%type<std::string>       FieldName;
%type<std::vector<std::string>> FieldNameList;
%type<ObjectClassAssignment> ObjectClass;
%type<ObjectClassAssignment> ObjectClassDefn;
%type<std::vector<std::string>> OneOrManyTypeFieldReference;
%type<Module>            ModuleBody;
%type<std::vector<NamedType>> AlternativeTypeList;
%type<std::vector<NamedType>> AlternativeTypeLists
%type<std::vector<NamedType>> RootAlternativeTypeList;
%type<std::vector<Assignment>> AssignmentList;
%type<Assignment>        Assignment;
%type<Assignment>        TypeAssignment;
%type<Assignment>        ValueAssignment;
%type<Assignment>        ValueSetTypeAssignment;
%type<Assignment>        ObjectClassAssignment;
%type<Assignment>        ObjectSetAssignment;
%type<Assignment>        ParameterizedAssignment
%type<Assignment>        ParameterizedTypeAssignment;
%type<Assignment>        ParameterizedValueAssignment;
%type<Assignment>        ParameterizedValueSetTypeAssignment;
%type<Assignment>        ParameterizedObjectClassAssignment;
%type<Assignment>        ParameterizedObjectAssignment;
%type<Assignment>        ParameterizedObjectSetAssignment;
%type<Type>              Type;
%type<Type>              ConstrainedType;
%type<Type>              TypeWithConstraint;
%type<BitStringType>     BitStringType;
%type<BooleanType>       BooleanType;
%type<CharacterStringType> CharacterStringType;
%type<CharacterStringType> RestrictedCharacterStringType;
%type<CharacterStringType> UnrestrictedCharacterStringType;
%type<ChoiceType>        ChoiceType;
%type<DateType>          DateType;
%type<DateTimeType>      DateTimeType;
%type<DurationType>      DurationType;
%type<EmbeddedPDVType>   EmbeddedPDVType;
%type<EnumeratedType>    EnumeratedType;
%type<ExternalType>      ExternalType;
%type<InstanceOfType>    InstanceOfType;
%type<IntegerType>       IntegerType;
%type<IRIType>           IRIType;
%type<NullType>          NullType;
%type<ObjectClassFieldType> ObjectClassFieldType;
%type<ObjectIdentifierType> ObjectIdentifierType;
%type<OctetStringType>   OctetStringType;
%type<RealType>          RealType;
%type<RelativeIRIType>   RelativeIRIType;
%type<RelativeOIDType>   RelativeOIDType;
%type<SequenceType>      SequenceType;
%type<SequenceOfType>    SequenceOfType;
%type<SetType>           SetType;
%type<SetOfType>         SetOfType;
%type<PrefixedType>      PrefixedType;
%type<TimeType>          TimeType;
%type<TimeOfDayType>     TimeOfDayType;
%type<std::vector<Type>> ActualParameterList;
%type<Type>              ActualParameter;
%type<Class>             Class;
%type<int>               ClassNumber;
%type<NamedType>         NamedType;
%type<NamedNumber>       NamedNumber;
%type<std::vector<NamedNumber>> NamedNumberList;
%type<ComponentType>     ComponentType;
%type<ComponentTypeList> ComponentTypeList;
%type<ComponentTypeList> ComponentTypeLists;
%type<Value>             Value;
%type<Value>             SingleValue;
%type<Value>             ValueWithoutTypeIdentifier;
%type<BooleanValue>      BooleanValue;
%type<TimeValue>         TimeValue;
%type<std::vector<Value>> SequenceOfValues;
%type<std::string>       Reference;
%type<std::string>       Symbol;
%type<std::vector<Import>> Imports;
%type<std::vector<Import>> SymbolsImported;
%type<std::vector<Import>> SymbolsFromModuleList;
%type<Import>            SymbolsFromModule;
%type<std::vector<std::string>> SymbolList;
%type<TaggingMode>       TagDefault;
%type<Tag>               Tag;
%type<TaggedType>        TaggedType;
%type<ObjectIdComponentValue> ObjIdComponents;
%type<ObjectIdComponentValue> NameForm;
%type<ObjectIdComponentValue> NumberForm;
%type<ObjectIdComponentValue> NameAndNumberForm;
%type<std::vector<ObjectIdComponentValue>> ObjIdComponentsList;
%type<std::vector<ObjectIdComponentValue>> ObjectIdentifierValue;
%type<std::vector<Parameter>> ParameterList;
%type<std::vector<Parameter>> ParameterSeries;
%type<Parameter>         Parameter;
%type<Type>              ParamGovernor;
%type<Type>              Governor;

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
      context_.asn1_tree.modules.push_back($8); }

SyntaxList:
   "{" TokenOrGroupSpecList "}";

DefinedObjectClass:
    ExternalObjectClassReference
|   objectclassreference
|   UsefulObjectClassReference;

ExternalObjectClassReference:
    modulereference "." objectclassreference;

UsefulObjectClassReference:
    TYPE_IDENTIFIER
    { $$ = "TYPE-IDENTIFIER"; }
|   ABSTRACT_SYNTAX
    { $$ = "TYPE-IDENTIFIER"; }

ObjectClassAssignment:
    typereference DEFINED_AS ObjectClass
    { $$ = Assignment{$1, $3}; }

ObjectClass:
    DefinedObjectClass
    { feature_not_implemented(context_.location, context_.asn1_tree, "ObjectClass"); }
|   ObjectClassDefn
    { $$ = $1; }
//|   ParameterizedObjectClass;

ObjectClassDefn:
    CLASS "{" FieldSpecList "}" WithSyntaxSpec
    { $$ = {$3}; }

FieldSpecList:
    FieldSpec
    { $$ = $1; }
|   FieldSpecList "," FieldSpec
    { $$ = $1; $$.insert($$.end(), $3.begin(), $3.end()); }

FieldSpec:
    TypeFieldSpec
    { $$ = $1; }
|   FixedTypeValueFieldSpec
    { $$ = $1; }
|   VariableTypeValueFieldSpec
|   FixedTypeValueSetFieldSpec
|   ObjectFieldSpec
|   ObjectSetFieldSpec;

FieldName:
    typefieldreference
    { $$ = $1; }
|   valuefieldreference
    { $$ = $1; }
/*  Same underlying as typefieldreference and valuefieldreference
|   valuesetfieldreference
|   objectfieldreference
|   objectsetfieldreference */

FieldNameList:
    FieldName
    { $$.push_back($1); }
|   FieldNameList "." FieldName
    { $$ = $1; $$.push_back($3); }

TypeFieldSpec:
    OneOrManyTypeFieldReference TypeOptionalitySpec
    { for (const std::string& name : $1) $$.push_back(ClassField{name, TypeField{}}); }
    
OneOrManyTypeFieldReference:
    typefieldreference
    { $$.push_back($1); }
|   OneOrManyTypeFieldReference typefieldreference
    { $$ = $1; $$.push_back($2); }

TypeOptionalitySpec:
    OPTIONAL
|   DEFAULT Type
|   %empty

OptionalUnique:
    OPTIONAL
|   UNIQUE
|   %empty

FixedTypeValueFieldSpec:
    valuefieldreference Type OptionalUnique ValueOptionalitySpec
    { $$.push_back(ClassField{$1, FixedTypeValueField{$2}}); }

ValueOptionalitySpec:
    OPTIONAL
|   DEFAULT SingleValue
|   %empty

VariableTypeValueFieldSpec:
    valuefieldreference FieldName OptionalUnique ValueOptionalitySpec;

FixedTypeValueSetFieldSpec:
    typefieldreference Type OptionalUnique ValueSetDefaultSpec

ValueSetDefaultSpec:
    OPTIONAL
|   DEFAULT ValueSet
|   %empty

ObjectFieldSpec:
    typefieldreference DefinedObjectClass ObjectOptionalitySpec;

ObjectOptionalitySpec:
    OPTIONAL
|   DEFAULT Object
|   %empty

ObjectSetFieldSpec:
    objectsetfieldreference DefinedObjectClass ObjectSetOptionalitySpec;

ObjectSetOptionalitySpec:
    OPTIONAL
|   DEFAULT
|   %empty

//ObjectSeWithSyntaxSpec:
//    WITH SYNTAX SyntaxList;

WithSyntaxSpec:
    WITH SYNTAX SyntaxList
|   %empty

TokenOrGroupSpecList:
    TokenOrGroupSpec
|   TokenOrGroupSpecList TokenOrGroupSpec

TokenOrGroupSpec:
    RequiredToken
|   OptionalGroup;

OptionalGroup:
    "[" TokenOrGroupSpecList "]";

RequiredToken:
    LiteralList FieldName

LiteralList:
    Literal
|   LiteralList Literal
|   %empty

Literal:
    word
|   ","

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
     FieldName Setting;

DefinedSyntax:
    "{" DefinedSyntaxList "}";

DefinedSyntaxList:
    DefinedSyntaxToken
|   DefinedSyntaxList DefinedSyntaxToken

DefinedSyntaxToken:
    Literal
|   Setting;

Setting:
    Type
|   Value
//|   Object

DefinedObjectSet:
    ExternalObjectSetReference
|   typereference;

ExternalObjectSetReference:
    modulereference "." typereference;

ObjectSetAssignment:
    typereference DefinedObjectSet DEFINED_AS ObjectSet
    { $$ = Assignment{$1, ObjectSetAssignment{}}; }

ObjectSet:
    "{" ObjectSetSpec "}";

ObjectSetSpec:
    ElementSetSpec
|   ElementSetSpec "," ELIPSIS
|   ELIPSIS
|   ELIPSIS "," ElementSetSpec
|   ElementSetSpec "," ELIPSIS "," ElementSetSpec;

ObjectSetElements:
    Object
|   DefinedObjectSet
//|   ObjectSetFromObjects
//|   ParameterizedObjectSet;

ObjectClassFieldType:
    UsefulObjectClassReference "." FieldNameList
    { $$ = {DefinedType{{}, }, $3}; }
|   typereference "." FieldNameList
    { $$ = {DefinedType{{}, $1}, $3}; }

ObjectClassFieldValue:
    Type COLON Value;

ParameterizedAssignment:
    ParameterizedTypeAssignment
    { $$ = $1; }
|   ParameterizedValueAssignment
    { $$ = $1; }
|   ParameterizedValueSetTypeAssignment
    { $$ = $1; }
|   ParameterizedObjectClassAssignment
    { $$ = $1; }
//|   ParameterizedObjectAssignment
//    { $$ = $1; }
//|   ParameterizedObjectSetAssignment;

ParameterizedTypeAssignment:
    typereference ParameterList DEFINED_AS Type
    { $$ = Assignment{ $1, TypeAssignment{$4}, {}, $2 }; }
 
ParameterizedValueAssignment:
    valuereference ParameterList Type DEFINED_AS Value
    { $$ = Assignment{ $1, ValueAssignment{} }; }

ParameterizedValueSetTypeAssignment:
    typereference ParameterList Type DEFINED_AS Value
    { $$ = Assignment{ $1, ValueAssignment{} }; }
 
ParameterizedObjectClassAssignment:
    typereference ParameterList DEFINED_AS ObjectClass
    { $$ = Assignment{ $1, ObjectClassAssignment{} }; }

ParameterizedObjectAssignment:
    objectreference ParameterList DefinedObjectClass DEFINED_AS Object
    { $$ = Assignment{ $1, ObjectClassAssignment{} }; }

ParameterizedObjectSetAssignment:
    typereference ParameterList DefinedObjectClass DEFINED_AS ObjectSet
    { $$ = Assignment{ $1, ObjectClassAssignment{} }; }

ParameterList:
   "{" ParameterSeries "}"
    { $$ = $2; }

ParameterSeries:
    Parameter
    { $$.push_back($1); }
|   ParameterSeries "," Parameter
    { $$ = $1; $$.push_back($3); }

Parameter:
    ParamGovernor ":" Reference
    { $$ = Parameter{$1, $3}; }
|   Reference
    { $$ = Parameter{{}, $1}; }

ParamGovernor:
    Governor
    { $$ = $1; }
|   Reference
    { $$ = DefinedType{{}, $1}; }

Governor:
    Type
    { $$ = $1; }
|   DefinedObjectClass
    { }

ParameterizedReference:
    Reference
|   Reference "{" "}";

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
    ReferencedObjects "." FieldNameList;

ValueSetFromObjects:
    ReferencedObjects "." FieldNameList;

TypeFromObject:
    ReferencedObjects "." FieldNameList;

ObjectFromObject:
    ReferencedObjects "." FieldNameList;

ObjectSetFromObjects:
    ReferencedObjects "." FieldNameList;

InstanceOfType:
    INSTANCE OF DefinedObjectClass
    { feature_not_implemented(context_.location, context_.asn1_tree, "InstanceOfType"); }

ParameterizedReference:
    Reference
|   Reference "{" "}";

SimpleDefinedType:
    ExternalTypeReference
|   typereference
    { $$ = $1; }

SimpleDefinedValue:
    ExternalValueReference
|   valuereference;

GeneralConstraint:
    UserDefinedConstraint
|   TableConstraint
|   ContentsConstraint;

UserDefinedConstraint:
    CONSTRAINED BY "{" UserDefinedConstraintParameter "}"
    { feature_not_implemented(context_.location, context_.asn1_tree, "UserDefinedConstraint", "Not yet checking contraints. "); }


UserDefinedConstraintParameter:
    Governor ":" Value
|   Governor ":" Object
|   DefinedObjectSet
|   Type
|   DefinedObjectClass
|   ELIPSIS
|   %empty

TableConstraint:
    ObjectSet
    { std::cerr << "Simple constraint\n"; }
|   ObjectSet "{" AtNotationList "}"
    { std::cerr << "Relation constraint\n"; }

AtNotationList:
    AtNotation
|   AtNotationList "," AtNotation

AtNotation:
    "@" ComponentIdList
|   "@" "." Level ComponentIdList;

Level:
    "." Level
|   %empty;

ComponentIdList:
    identifier
|   ComponentIdList "." identifier

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
    { $$ = Import{ $3, {}, {} };
      for (const std::string& ref: $1)
      {
        if (std::isupper(ref[0]))
        {
          $$.imported_types.push_back(ref);
        }
        else
        {
          $$.imported_values.push_back(ref);
        }
      }
    }

GlobalModuleReference:
    modulereference
    { $$ = $1; }
|   modulereference ObjectIdentifierValue
    { $$ = $1; }
// |   modulereference valuereference
//    { $$ = $1; } // Clashes with value import

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
|   typereference "{" "}"
    { $$ = $1; }
|   valuereference
    { $$ = $1; }
    
/*
|   objectclassreference
|   objectreference
|   objectsetreference; */

AssignmentList:
    Assignment
    { $$.push_back($1); }
|   Assignment AssignmentList
    { $$ = $2; $$.push_back($1); }

Assignment:
    TypeAssignment
    { $$ = $1; }
|   ValueAssignment
    { $$ = $1; }
|   ObjectClassAssignment
    { $$ = $1; }
|   ValueSetTypeAssignment
    { $$ = $1; }
/*|   ObjectAssignment */
|   ObjectSetAssignment
    { $$ = $1; }
|   ParameterizedAssignment
    { $$ = $1; }

DefinedType:
    ExternalTypeReference
    { $$ = $1; }
|   typereference
    { $$ = DefinedType{absl::nullopt, $1, {}}; }
|   ParameterizedType
    { $$ = $1; }
//|   ParameterizedValueSetType;

DefinedValue:
//    ExternalValueReference
    valuereference
    { $$ = DefinedValue{$1}; }
|   ParameterizedValue;

ParameterizedType:
    SimpleDefinedType "{" ActualParameterList "}"
    { $$ = DefinedType{ absl::nullopt, $1, $3}; }

ParameterizedValue:
    SimpleDefinedValue "{" ActualParameterList "}"

ActualParameterList:
    ActualParameter 
    { $$.push_back($1); }
|   ActualParameter "," ActualParameterList
    { $$ = $3; $$.push_back($1); }

ActualParameter:
    Type
    { $$ = $1; }
|   ValueWithoutTypeIdentifier
    { std::cerr << "Warning: Not yet dealing with value paramaters\n"; }
//|   ValueSet
|   DefinedObjectClass
//|   Object
//|   ObjectSet 

NonParameterizedTypeName:
    ExternalTypeReference
|   typereference
|   xmlasn1typename;

ExternalTypeReference:
    typereference "." typereference // Param one is actually modulereference, but this causes parsing clash
    { $$ = DefinedType{$1, $3, {}}; }

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
    typereference DEFINED_AS Type
    { $$ = Assignment{ $1, TypeAssignment{$3}, {} }; }

ValueAssignment:
    valuereference Type DEFINED_AS Value
    { $$ = Assignment{ $1, ValueAssignment{$2, $4}, {} }; }

ValueSetTypeAssignment:
    typereference Type DEFINED_AS ValueSet
    { $$ = Assignment{ $1, TypeAssignment{$2}, {} }; }

Type:
    BuiltinType
    { $$ = $1; }
|   ConstrainedType
    { $$ = $1; }
|   DefinedType
    { $$ = $1; }
|   SelectionType
    { feature_not_implemented(context_.location, context_.asn1_tree, "SelectionType"); }
|   TypeFromObject
    { feature_not_implemented(context_.location, context_.asn1_tree, "TypeFromObject"); }
//|   ValueSetFromObjects { std::cerr << std::string("Not handled - ValueSetFromObjects\n"); }

BuiltinType:
    ANY { $$ = AnyType(); }
|   BitStringType { $$ = $1; }
|   BooleanType { $$ = $1; }
|   CharacterStringType { $$ = $1; }
|   ChoiceType { $$ = $1; }
|   DateType { $$ = $1; feature_not_implemented(context_.location, context_.asn1_tree, "DateType"); }
|   DateTimeType { $$ = $1; feature_not_implemented(context_.location, context_.asn1_tree, "DateTimeType"); }
|   DurationType { $$ = $1; feature_not_implemented(context_.location, context_.asn1_tree, "DurationType"); }
|   EmbeddedPDVType { $$ = $1; feature_not_implemented(context_.location, context_.asn1_tree, "EmbeddedPDVType"); }
|   EnumeratedType { $$ = $1; }
|   ExternalType { $$ = $1; feature_not_implemented(context_.location, context_.asn1_tree, "ExternalType"); }
|   GeneralizedTime { $$ = GeneralizedTimeType(); }
|   InstanceOfType { $$ = $1; feature_not_implemented(context_.location, context_.asn1_tree, "InstanceOfType"); }
|   IntegerType { $$ = $1; }
|   IRIType { $$ = $1; feature_not_implemented(context_.location, context_.asn1_tree, "IRIType"); }
|   NullType { $$ = $1; }
|   ObjectClassFieldType { $$ = $1; feature_not_implemented(context_.location, context_.asn1_tree, "ObjectClassFieldType"); }
|   ObjectDescriptor { $$ = ObjectDescriptorType(); }
|   ObjectIdentifierType { $$ = $1; }
|   OctetStringType { $$ = $1; }
|   RealType { $$ = $1; feature_not_implemented(context_.location, context_.asn1_tree, "RealType"); }
|   RelativeIRIType { $$ = $1; feature_not_implemented(context_.location, context_.asn1_tree, "RelativeIRIType"); }
|   RelativeOIDType { $$ = $1; feature_not_implemented(context_.location, context_.asn1_tree, "RelativeOIDType"); }
|   SequenceType { $$ = $1; }
|   SequenceOfType { $$ = $1; }
|   SetType { $$ = $1; }
|   SetOfType { $$ = $1; }
|   PrefixedType { $$ = $1; }
|   TimeType { $$ = $1; feature_not_implemented(context_.location, context_.asn1_tree, "TimeType"); }
|   TimeOfDayType { $$ = $1; feature_not_implemented(context_.location, context_.asn1_tree, "TimeOfDayType"); }
|   UTCTime { $$ = UTCTimeType(); feature_not_implemented(context_.location, context_.asn1_tree, "UTCTime"); }

NamedType:
    identifier Type
    { $$ = NamedType{ $1, $2 }; }

ValueWithoutTypeIdentifier:
    BooleanValue
    { std::cerr << std::string("Warning: Unhandled field: BooleanValue\n"); }
|   IRIValue
    { std::cerr << std::string("Warning: Unhandled field: IRIValue\n"); }
|   ASN_NULL
    { std::cerr << std::string("Warning: Unhandled field: ASN_NULL\n"); }
|   TimeValue
    { std::cerr << std::string("Warning: Unhandled field: TimeValue\n"); }
|   bstring
    { $$.value_selection = BitStringValue{$1}; }
|   hstring
    { $$.value_selection = HexStringValue{$1}; }
|   cstring
    { $$.value_selection = CharStringValue{$1}; }
|   CONTAINING Value
    { std::cerr << std::string("Warning: Unhandled field: CONTAINING\n"); }
|   DefinedValue
    { $$.value_selection = $1; }
|   GENERIC_IDENTIFIER_LOWERCASE "(" number ")"
    { $$.value_selection = NamedNumber{$1, $3}; }
|   ":"
|   SignedNumber
    { $$.value_selection = $1; }
|   realnumber
    { $$.value_selection = $1; }
|   ObjectClassFieldType
    { std::cerr << std::string("Warning: Unhandled field: ValueCommaListChoice\n"); }
|   Value COLON Value
    { std::cerr << std::string("Warning: Unhandled field: ValueCommaListChoice\n"); }
//|   ObjectClassFieldValue
//    { std::cerr << std::string("Warning: Unhandled field: ObjectClassFieldValue\n"); }
|    valuereference "." typefieldreference
|    valuereference "." valuefieldreference
|    typefieldreference
|    valuefieldreference
|   "{" SequenceOfValues "}"
    { $$.value_selection = $2; }
|   ValueChoice
    { std::cerr << std::string("Warning: Unhandled field: ValueChoice\n"); }
|   OPTIONAL
    { std::cerr << std::string("Warning: Unhandled field: OPTIONAL\n"); }
|   ValueCommaListChoice
    { std::cerr << std::string("Warning: Unhandled field: ValueCommaListChoice\n"); }
|   BY
    { std::cerr << std::string("Warning: Unhandled field: BY\n"); }
|   WITH
    { std::cerr << std::string("Warning: Unhandled field: WITH\n"); }


Value:
    ValueWithoutTypeIdentifier
    { $$ = $1; }
|   GENERIC_IDENTIFIER_UPPERCASE
    { $$.value_selection = $1; }
|   INTEGER

ValueCommaListChoice:
    Value "," Value
|   ValueCommaListChoice "," Value

ValueChoice:
    Value VERTICAL_LINE Value
|   ValueChoice VERTICAL_LINE Value

ValueSet:
    "{" ElementSetSpecs "}"

SequenceOfValues:
    Value
    { $$.push_back($1); }
|   SequenceOfValues Value
    { $$ = $1; $$.push_back($2); }

NamedValue:
    identifier
    Value;

BooleanType:
    BOOLEAN;

BooleanValue:
    TRUE
    { $$ = {true}; }
|   FALSE
    { $$ = {false}; }

IntegerType:
    INTEGER
    { $$ = IntegerType{{}}; }
|   INTEGER "{" NamedNumberList "}"
    { $$ = IntegerType{$3}; }

NamedNumberList:
    NamedNumber
    { $$ = {$1}; }
|   NamedNumberList "," NamedNumber
    { $$ = $1; $$.push_back($3); }

NamedNumber:
    identifier "(" SignedNumber ")"
    { $$ = NamedNumber{ $1, $3 }; }
|   identifier "(" DefinedValue ")"
    { $$ = NamedNumber{ $1, 0 }; }

SignedNumber:
    number
    { $$ = $1; }
|   negativenumber
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
    { $$ = BitStringType{}; }
|   BIT STRING "{" NamedBitList "}"
    { $$ = BitStringType{}; }

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

ComponentTypeLists:
    ComponentTypeList
    { $$ = $1; }
|   ComponentTypeList "," ELIPSIS ExceptionSpec
    { $$ = $1; }
|   ComponentTypeList "," ELIPSIS ExceptionSpec "," ComponentTypeList
    { $$ = $1; $$.insert($$.end(), $6.begin(), $6.end()); }
|   ComponentTypeList "," ELIPSIS ExceptionSpec "," ComponentTypeList "," ELIPSIS
    { $$ = $1; $$.insert($$.end(), $6.begin(), $6.end()); }
|   ComponentTypeList "," ELIPSIS ExceptionSpec "," ComponentTypeList "," ELIPSIS "," ComponentTypeList
    { $$ = $1; $$.insert($$.end(), $6.begin(), $6.end()); $$.insert($$.end(), $10.begin(), $10.end()); }
|   ELIPSIS ExceptionSpec "," ComponentTypeList
    { $$ = $4; }
|   ELIPSIS ExceptionSpec "," ComponentTypeList "," ELIPSIS
    { $$ = $4; }
|   ELIPSIS ExceptionSpec
    { $$ = {}; }
|   ELIPSIS ExceptionSpec "," ELIPSIS
    { $$ = {}; }

ComponentTypeList:
    ComponentType
    { $$ = ComponentTypeList{$1}; }
|   ComponentTypeList "," ComponentType
    { $1.push_back($3); $$ = $1; }

ComponentType:
    Type
    { std::cerr << context_.location << " WARNING: unnamed type\n";
      $$ = ComponentType{{gen_anon_member_name(), $1}, false, absl::nullopt, absl::nullopt, StorageMode::static_ }; }
|   Type OPTIONAL
    { std::cerr << context_.location << " WARNING: unnamed type\n";
      $$ = ComponentType{{gen_anon_member_name(), $1}, true, absl::nullopt, absl::nullopt, StorageMode::static_ }; }
|   Type DEFAULT SingleValue
    { $$ = ComponentType{{gen_anon_member_name(), $1}, false, $3, absl::nullopt, StorageMode::static_ };
      std::cerr << context_.location << " WARNING: unnamed type\n"; }
|   NamedType
    { $$ = ComponentType{$1, false, absl::nullopt, absl::nullopt, StorageMode::static_ }; }
|   NamedType OPTIONAL
    { $$ = ComponentType{$1, true, absl::nullopt, absl::nullopt, StorageMode::static_ }; }
|   NamedType DEFAULT SingleValue
    { $$ = ComponentType{$1, false, $3, absl::nullopt, StorageMode::static_ }; }
|   COMPONENTS OF Type
    { $$ = ComponentType{{}, false, absl::nullopt, $3, StorageMode::static_}; }

SequenceValue:
    "{" ComponentValueList "}"
|   "{" "}";

ComponentValueList:
    NamedValue
|   ComponentValueList "," NamedValue;

SequenceOfType:
    SEQUENCE OF Type
    { $$ = SequenceOfType{ false, nullptr, std::unique_ptr<Type>(new Type($3)) }; }
|   SEQUENCE OF NamedType
    { $$ = SequenceOfType{ true, std::unique_ptr<NamedType>(new NamedType($3)), nullptr }; }

SetType:
    SET "{" "}"
    { $$ = SetType{}; }
|   SET "{" ComponentTypeLists "}"
    { $$ = SetType{$3}; }

SetOfType:
    SET OF Type
    { $$ = SetOfType{ false, nullptr, std::unique_ptr<Type>(new Type($3)) }; }
|   SET OF NamedType
    { $$ = SetOfType{ true, std::unique_ptr<NamedType>(new NamedType($3)), nullptr }; }

ChoiceType:
    CHOICE "{" AlternativeTypeLists "}"
    { $$ = ChoiceType{ $3, StorageMode::static_ }; }

AlternativeTypeLists:
    RootAlternativeTypeList
    { $$ = $1; }

RootAlternativeTypeList:
    AlternativeTypeList
    { $$ = $1; }
|   AlternativeTypeList "," ELIPSIS
    { $$ = $1; }
|   AlternativeTypeList "," ELIPSIS "," AlternativeTypeList
    { $$ = $1; $$.insert($$.begin(), $5.begin(), $5.end()); }

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
    { $$ = $1; }
|   UnrestrictedCharacterStringType
    { $$ = $1; }

RestrictedCharacterStringType:
    BMPString
    { $$ = CharacterStringType::bmp_string; }
|   GeneralString
    { $$ = CharacterStringType::general_string; }
|   GraphicString
    { $$ = CharacterStringType::graphic_string; }
|   IA5String
    { $$ = CharacterStringType::ia5_string; }
|   ISO646String
    { $$ = CharacterStringType::iso646_string; }
|   NumericString
    { $$ = CharacterStringType::numeric_string; }
|   PrintableString
    { $$ = CharacterStringType::printable_string; }
|   TeletexString
    { $$ = CharacterStringType::teletex_string; }
|   T61String
    { $$ = CharacterStringType::t61_string; }
|   UniversalString
    { $$ = CharacterStringType::universal_string; }
|   UTF8String
    { $$ = CharacterStringType::utf8_string; }
|   VideotexString
    { $$ = CharacterStringType::videotex_string; }
|   VisibleString
    { $$ = CharacterStringType::visible_string; }

UnrestrictedCharacterStringType:
    CHARACTER STRING
    { $$ = CharacterStringType::character_string; }

ConstrainedType:
    Type Constraint
    { $$ = $1; }
|   TypeWithConstraint
    { $$ = $1; feature_not_implemented(context_.location, context_.asn1_tree, "TypeWithConstraint", "Not yet checking contraints. ");  }

TypeWithConstraint:
    SET Constraint OF Type
    { $$ = SetOfType{ false, nullptr, std::unique_ptr<Type>(new Type($4)) }; }
|   SET SizeConstraint OF Type
    { $$ = SetOfType{ false, nullptr, std::unique_ptr<Type>(new Type($4)) }; }
|   SEQUENCE Constraint OF Type
    { $$ = SequenceOfType{ false, nullptr, std::unique_ptr<Type>(new Type($4)) }; }
|   SEQUENCE SizeConstraint OF Type
    { $$ = SequenceOfType{ false, nullptr, std::unique_ptr<Type>(new Type($4)) }; }
|   SET Constraint OF NamedType
    { $$ = SetOfType{ true, std::unique_ptr<NamedType>(new NamedType($4)), nullptr }; }
|   SET SizeConstraint OF NamedType
    { $$ = SetOfType{ true, std::unique_ptr<NamedType>(new NamedType($4)), nullptr }; }
|   SEQUENCE Constraint OF NamedType
    { $$ = SequenceOfType{ true, std::unique_ptr<NamedType>(new NamedType($4)), nullptr }; }
|   SEQUENCE SizeConstraint OF NamedType
    { $$ = SequenceOfType{ true, std::unique_ptr<NamedType>(new NamedType($4)), nullptr }; }

Constraint:
    "(" ConstraintSpec ExceptionSpec ")"
    { feature_not_implemented(context_.location, context_.asn1_tree, "ConstraintSpec", "Not yet checking contraints. "); }

ConstraintSpec:
    SubtypeConstraint
|   GeneralConstraint;

SubtypeConstraint:
    ElementSetSpecs;

ElementSetSpecs:
    ElementSetSpec
|   ElementSetSpec "," ELIPSIS
|   ElementSetSpec "," ELIPSIS "," ElementSetSpec;

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
//|   ObjectSetElements
|   "(" ElementSetSpecs ")";

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
    BooleanValue
    { $$.value_selection = $1; }
//|   IRIValue
|   ASN_NULL
    { $$.value_selection = NullValue{}; }
|   TimeValue
    { $$.value_selection = $1; }
|   bstring
    { $$.value_selection = $1; }
|   hstring
    { $$.value_selection = $1; }
|   cstring
    { $$.value_selection = $1; }
|   GENERIC_IDENTIFIER_UPPERCASE
    { $$.value_selection = $1; }
|   DefinedValue
    { $$.value_selection = $1; }
|   SignedNumber
    { $$.value_selection = $1; }
|   realnumber
    { $$.value_selection = $1; }

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
    SingleValue
|   MIN;

UpperEndValue:
    SingleValue
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
|   "{" ELIPSIS "}"
|   "{" ELIPSIS "," TypeConstraints "}"
|   "{" TypeConstraints "}"

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
|   Type ":" Value;

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

objectclassreference:
    GENERIC_IDENTIFIER_UPPERCASE
    { $$ = $1; }

word:
    GENERIC_IDENTIFIER_UPPERCASE
|   ABSENT
|   ABSTRACT_SYNTAX
|   ALL
|   APPLICATION
|   ASN_NULL
|   AUTOMATIC
|   BEGIN
|   BIT
|   BMPString
|   BOOLEAN
|   BY
|   CHARACTER
|   CHOICE
|   CLASS
|   COMPONENT
|   COMPONENTS
|   CONSTRAINED
|   CONTAINING
|   DATE
|   DATE_TIME
|   DEFAULT
|   DEFINITIONS
|   DURATION
|   EMBEDDED
|   ENCODED
|   ENCODING_CONTROL
|   END
|   ENUMERATED
|   EXCEPT
|   EXPLICIT
|   EXPORTS
|   EXTENSIBILITY
|   EXTERNAL
|   FALSE
|   FROM
|   IDENTIFIER
|   IMPLICIT
|   IMPLIED
|   IMPORTS
|   INCLUDES
|   INSTANCE
|   INSTRUCTIONS
|   INTEGER
|   INTERSECTION
|   MAX
|   MIN
|   MINUS_INFINITY
|   NOT_A_NUMBER
|   OBJECT
|   OCTET
|   OF
|   OID_IRI
|   OPTIONAL
|   PATTERN
|   PDV
|   PLUS_INFINITY
|   PRESENT
|   PRIVATE
|   REAL
|   RELATIVE_OID
|   RELATIVE_OID_IRI
|   SEQUENCE
|   SET
|   SETTINGS
|   SIZE
|   STRING
|   SYNTAX
|   TAGS
|   TIME
|   TIME_OF_DAY
|   TRUE
|   TYPE_IDENTIFIER
|   UNION
|   UNIQUE
|   UNIVERSAL
|   WITH

%%


namespace yy {

    asn1_parser::symbol_type yylex(Context& context_)
    {
        const char* start = context_.cursor;
        const char* YYMARKER = nullptr;
        context_.location.step();
        // Lexer
%{
re2c:yyfill:enable   = 0;
re2c:define:YYCTYPE  = "char";
re2c:define:YYCURSOR = "context_.cursor";

// Keywords
"ABSENT"                { context_.location.columns(context_.cursor - start); return asn1_parser::make_ABSENT (context_.location); }
"ABSTRACT-SYNTAX"       { context_.location.columns(context_.cursor - start); return asn1_parser::make_ABSTRACT_SYNTAX (context_.location); }
"ALL"                   { context_.location.columns(context_.cursor - start); return asn1_parser::make_ALL (context_.location); }
"ANY"                   { context_.location.columns(context_.cursor - start); return asn1_parser::make_ANY (context_.location); }
"APPLICATION"           { context_.location.columns(context_.cursor - start); return asn1_parser::make_APPLICATION (context_.location); }
"AUTOMATIC"             { context_.location.columns(context_.cursor - start); return asn1_parser::make_AUTOMATIC (context_.location); }
"BEGIN"                 { context_.location.columns(context_.cursor - start); return asn1_parser::make_BEGIN (context_.location); }
"BIT"                   { context_.location.columns(context_.cursor - start); return asn1_parser::make_BIT (context_.location); }
"BMPString"             { context_.location.columns(context_.cursor - start); return asn1_parser::make_BMPString (context_.location); }
"BOOLEAN"               { context_.location.columns(context_.cursor - start); return asn1_parser::make_BOOLEAN (context_.location); }
"BY"                    { context_.location.columns(context_.cursor - start); return asn1_parser::make_BY (context_.location); }
"CHARACTER"             { context_.location.columns(context_.cursor - start); return asn1_parser::make_CHARACTER (context_.location); }
"CHOICE"                { context_.location.columns(context_.cursor - start); return asn1_parser::make_CHOICE (context_.location); }
"CLASS"                 { context_.location.columns(context_.cursor - start); return asn1_parser::make_CLASS (context_.location); }
"COMPONENT"             { context_.location.columns(context_.cursor - start); return asn1_parser::make_COMPONENT (context_.location); }
"COMPONENTS"            { context_.location.columns(context_.cursor - start); return asn1_parser::make_COMPONENTS (context_.location); }
"CONSTRAINED"           { context_.location.columns(context_.cursor - start); return asn1_parser::make_CONSTRAINED (context_.location); }
"CONTAINING"            { context_.location.columns(context_.cursor - start); return asn1_parser::make_CONTAINING (context_.location); }
"DATE"                  { context_.location.columns(context_.cursor - start); return asn1_parser::make_DATE (context_.location); }
"DATE-TIME"             { context_.location.columns(context_.cursor - start); return asn1_parser::make_DATE_TIME (context_.location); }
"DEFAULT"               { context_.location.columns(context_.cursor - start); return asn1_parser::make_DEFAULT (context_.location); }
"DEFINITIONS"           { context_.location.columns(context_.cursor - start); return asn1_parser::make_DEFINITIONS (context_.location); }
"DURATION"              { context_.location.columns(context_.cursor - start); return asn1_parser::make_DURATION (context_.location); }
"EMBEDDED"              { context_.location.columns(context_.cursor - start); return asn1_parser::make_EMBEDDED (context_.location); }
"ENCODED"               { context_.location.columns(context_.cursor - start); return asn1_parser::make_ENCODED (context_.location); }
"ENCODING_CONTROL"      { context_.location.columns(context_.cursor - start); return asn1_parser::make_ENCODING_CONTROL (context_.location); }
"END"                   { context_.location.columns(context_.cursor - start); return asn1_parser::make_END (context_.location); }
"ENUMERATED"            { context_.location.columns(context_.cursor - start); return asn1_parser::make_ENUMERATED (context_.location); }
"EXCEPT"                { context_.location.columns(context_.cursor - start); return asn1_parser::make_EXCEPT (context_.location); }
"EXPLICIT"              { context_.location.columns(context_.cursor - start); return asn1_parser::make_EXPLICIT (context_.location); }
"EXPORTS"               { context_.location.columns(context_.cursor - start); return asn1_parser::make_EXPORTS (context_.location); }
"EXTENSIBILITY"         { context_.location.columns(context_.cursor - start); return asn1_parser::make_EXTENSIBILITY (context_.location); }
"EXTERNAL"              { context_.location.columns(context_.cursor - start); return asn1_parser::make_EXTERNAL (context_.location); }
"FALSE"                 { context_.location.columns(context_.cursor - start); return asn1_parser::make_FALSE (context_.location); }
"FROM"                  { context_.location.columns(context_.cursor - start); return asn1_parser::make_FROM (context_.location); }
"GeneralizedTime"       { context_.location.columns(context_.cursor - start); return asn1_parser::make_GeneralizedTime (context_.location); }
"GeneralString"         { context_.location.columns(context_.cursor - start); return asn1_parser::make_GeneralString (context_.location); }
"GraphicString"         { context_.location.columns(context_.cursor - start); return asn1_parser::make_GraphicString (context_.location); }
"IA5String"             { context_.location.columns(context_.cursor - start); return asn1_parser::make_IA5String (context_.location); }
"IDENTIFIER"            { context_.location.columns(context_.cursor - start); return asn1_parser::make_IDENTIFIER (context_.location); }
"IMPLICIT"              { context_.location.columns(context_.cursor - start); return asn1_parser::make_IMPLICIT (context_.location); }
"IMPLIED"               { context_.location.columns(context_.cursor - start); return asn1_parser::make_IMPLIED (context_.location); }
"IMPORTS"               { context_.location.columns(context_.cursor - start); return asn1_parser::make_IMPORTS (context_.location); }
"INCLUDES"              { context_.location.columns(context_.cursor - start); return asn1_parser::make_INCLUDES (context_.location); }
"INSTANCE"              { context_.location.columns(context_.cursor - start); return asn1_parser::make_INSTANCE (context_.location); }
"INSTRUCTIONS"          { context_.location.columns(context_.cursor - start); return asn1_parser::make_INSTRUCTIONS (context_.location); }
"INTEGER"               { context_.location.columns(context_.cursor - start); return asn1_parser::make_INTEGER (context_.location); }
"INTERSECTION"          { context_.location.columns(context_.cursor - start); return asn1_parser::make_INTERSECTION (context_.location); }
"ISO646String"          { context_.location.columns(context_.cursor - start); return asn1_parser::make_ISO646String (context_.location); }
"MAX"                   { context_.location.columns(context_.cursor - start); return asn1_parser::make_MAX (context_.location); }
"MIN"                   { context_.location.columns(context_.cursor - start); return asn1_parser::make_MIN (context_.location); }
"MINUS-INFINITY"        { context_.location.columns(context_.cursor - start); return asn1_parser::make_MINUS_INFINITY (context_.location); }
"NOT-A-NUMBER"          { context_.location.columns(context_.cursor - start); return asn1_parser::make_NOT_A_NUMBER (context_.location); }
"NULL"                  { context_.location.columns(context_.cursor - start); return asn1_parser::make_ASN_NULL (context_.location); }
"NumericString"         { context_.location.columns(context_.cursor - start); return asn1_parser::make_NumericString (context_.location); }
"OBJECT"                { context_.location.columns(context_.cursor - start); return asn1_parser::make_OBJECT (context_.location); }
"ObjectDescriptor"      { context_.location.columns(context_.cursor - start); return asn1_parser::make_ObjectDescriptor (context_.location); }
"OCTET"                 { context_.location.columns(context_.cursor - start); return asn1_parser::make_OCTET (context_.location); }
"OF"                    { context_.location.columns(context_.cursor - start); return asn1_parser::make_OF (context_.location); }
"OID_IRI"               { context_.location.columns(context_.cursor - start); return asn1_parser::make_OID_IRI (context_.location); }
"OPTIONAL"              { context_.location.columns(context_.cursor - start); return asn1_parser::make_OPTIONAL (context_.location); }
"PATTERN"               { context_.location.columns(context_.cursor - start); return asn1_parser::make_PATTERN (context_.location); }
"PDV"                   { context_.location.columns(context_.cursor - start); return asn1_parser::make_PDV (context_.location); }
"PLUS_INFINITY"         { context_.location.columns(context_.cursor - start); return asn1_parser::make_PLUS_INFINITY (context_.location); }
"PRESENT"               { context_.location.columns(context_.cursor - start); return asn1_parser::make_PRESENT (context_.location); }
"PrintableString"       { context_.location.columns(context_.cursor - start); return asn1_parser::make_PrintableString (context_.location); }
"PRIVATE"               { context_.location.columns(context_.cursor - start); return asn1_parser::make_PRIVATE (context_.location); }
"REAL"                  { context_.location.columns(context_.cursor - start); return asn1_parser::make_REAL (context_.location); }
"RELATIVE-OID"          { context_.location.columns(context_.cursor - start); return asn1_parser::make_RELATIVE_OID (context_.location); }
"RELATIVE-OID-IRI"      { context_.location.columns(context_.cursor - start); return asn1_parser::make_RELATIVE_OID_IRI (context_.location); }
"SEQUENCE"              { context_.location.columns(context_.cursor - start); return asn1_parser::make_SEQUENCE (context_.location); }
"SET"                   { context_.location.columns(context_.cursor - start); return asn1_parser::make_SET (context_.location); }
"SETTINGS"              { context_.location.columns(context_.cursor - start); return asn1_parser::make_SETTINGS (context_.location); }
"SIZE"                  { context_.location.columns(context_.cursor - start); return asn1_parser::make_SIZE (context_.location); }
"STRING"                { context_.location.columns(context_.cursor - start); return asn1_parser::make_STRING (context_.location); }
"SYNTAX"                { context_.location.columns(context_.cursor - start); return asn1_parser::make_SYNTAX (context_.location); }
"T61String"             { context_.location.columns(context_.cursor - start); return asn1_parser::make_T61String (context_.location); }
"TAGS"                  { context_.location.columns(context_.cursor - start); return asn1_parser::make_TAGS (context_.location); }
"TeletexString"         { context_.location.columns(context_.cursor - start); return asn1_parser::make_TeletexString (context_.location); }
"TIME"                  { context_.location.columns(context_.cursor - start); return asn1_parser::make_TIME (context_.location); }
"TIME-OF-DAY"           { context_.location.columns(context_.cursor - start); return asn1_parser::make_TIME_OF_DAY (context_.location); }
"TRUE"                  { context_.location.columns(context_.cursor - start); return asn1_parser::make_TRUE (context_.location); }
"TYPE-IDENTIFIER"       { context_.location.columns(context_.cursor - start); return asn1_parser::make_TYPE_IDENTIFIER (context_.location); }
"UNION"                 { context_.location.columns(context_.cursor - start); return asn1_parser::make_UNION (context_.location); }
"UNIQUE"                { context_.location.columns(context_.cursor - start); return asn1_parser::make_UNIQUE (context_.location); }
"UNIVERSAL"             { context_.location.columns(context_.cursor - start); return asn1_parser::make_UNIVERSAL (context_.location); }
"UniversalString"       { context_.location.columns(context_.cursor - start); return asn1_parser::make_UniversalString (context_.location); }
"UTCTime"               { context_.location.columns(context_.cursor - start); return asn1_parser::make_UTCTime (context_.location); }
"UTF8String"            { context_.location.columns(context_.cursor - start); return asn1_parser::make_UTF8String (context_.location); }
"VideotexString"        { context_.location.columns(context_.cursor - start); return asn1_parser::make_VideotexString (context_.location); }
"VisibleString"         { context_.location.columns(context_.cursor - start); return asn1_parser::make_VisibleString (context_.location); }
"WITH"                  { context_.location.columns(context_.cursor - start); return asn1_parser::make_WITH (context_.location); }

// Comments
"--" ([\-]?[^\r\n\-])* "--"
                        { context_.location.columns(context_.cursor - start); return yylex(context_); }
"--" ([\-]?[^\r\n\-])*[\-]?
                        { context_.location.columns(context_.cursor - start); return yylex(context_); }
"/*" ([^\*]|([\*]+[^\*\/]))*[\*]+ "/"
                        { for (char c: std::string(start, context_.cursor)) { context_.location.columns(); if (c == '\n') context_.location.lines(); } return yylex(context_); }

// Identifiers
[0-9]+'\.'[0-9]+        { context_.location.columns(context_.cursor - start); return asn1_parser::make_realnumber(std::stod(std::string(start, context_.cursor)), context_.location); }
[0-9]+                  { context_.location.columns(context_.cursor - start); return asn1_parser::make_number(std::stoll(std::string(start, context_.cursor)), context_.location); }
"-"[0-9]+               { context_.location.columns(context_.cursor - start); return asn1_parser::make_negativenumber(std::stoll(std::string(start, context_.cursor)), context_.location); }
['\"']('\\'.|"\"\""|[^'\"'])*['\"']
                        { context_.location.columns(context_.cursor - start); return asn1_parser::make_cstring(std::string(start, context_.cursor), context_.location); }
['\'']('0'|'1')*['\'']"B"
                        { context_.location.columns(context_.cursor - start); return asn1_parser::make_bstring(std::string(start + 1, context_.cursor - 2), context_.location); }
['\''][0-9A-Fa-f]*['\'']"H"
                        { context_.location.columns(context_.cursor - start); return asn1_parser::make_hstring(std::string(start + 1, context_.cursor - 2), context_.location); }
[A-Z]([\-]?[A-Za-z_0-9])*  { context_.location.columns(context_.cursor - start); return asn1_parser::make_GENERIC_IDENTIFIER_UPPERCASE(santize_name(std::string(start, context_.cursor)), context_.location); }
[a-z]([\-]?[A-Za-z_0-9])*  { context_.location.columns(context_.cursor - start); return asn1_parser::make_GENERIC_IDENTIFIER_LOWERCASE(santize_name(std::string(start, context_.cursor)), context_.location); }
[&][A-Z]([\-]?[A-Za-z_0-9])* { context_.location.columns(context_.cursor - start); return asn1_parser::make_typefieldreference(santize_name(std::string(start, context_.cursor)), context_.location); }
[&][a-z]([\-]?[A-Za-z_0-9])* { context_.location.columns(context_.cursor - start); return asn1_parser::make_valuefieldreference(santize_name(std::string(start, context_.cursor)), context_.location); }

// End of file
"\000"                  { context_.location.columns(context_.cursor - start); return asn1_parser::make_END_OF_FILE(context_.location); }

// White space
"\r\n" | "\n"           { context_.location.columns(context_.cursor - start); context_.location.lines();   return yylex(context_); }
[\t\v\b\f ]             { context_.location.columns(context_.cursor - start); context_.location.columns(); return yylex(context_); }

// Symbols
"::="                   { context_.location.columns(context_.cursor - start); return asn1_parser::make_DEFINED_AS (context_.location); }
"\.\.\."                { context_.location.columns(context_.cursor - start); return asn1_parser::make_ELIPSIS (context_.location); }
"\.\."                  { context_.location.columns(context_.cursor - start); return asn1_parser::make_RANGE (context_.location); }
"{"                     { context_.location.columns(context_.cursor - start); return asn1_parser::make_OPEN_BRACE (context_.location); }
"}"                     { context_.location.columns(context_.cursor - start); return asn1_parser::make_CLOSE_BRACE (context_.location); }
"("                     { context_.location.columns(context_.cursor - start); return asn1_parser::make_OPEN_PARENTHESIS (context_.location); }
")"                     { context_.location.columns(context_.cursor - start); return asn1_parser::make_CLOSE_PARENTHESIS (context_.location); }
"["                     { context_.location.columns(context_.cursor - start); return asn1_parser::make_OPEN_SQUARE_BRACKET (context_.location); }
"]"                     { context_.location.columns(context_.cursor - start); return asn1_parser::make_CLOSE_SQUARE_BRACKET (context_.location); }
":"                     { context_.location.columns(context_.cursor - start); return asn1_parser::make_COLON (context_.location); }
";"                     { context_.location.columns(context_.cursor - start); return asn1_parser::make_SEMICOLON (context_.location); }
","                     { context_.location.columns(context_.cursor - start); return asn1_parser::make_COMMA (context_.location); }
"-"                     { context_.location.columns(context_.cursor - start); return asn1_parser::make_HYPHEN_MINUS (context_.location); }
"\."                    { context_.location.columns(context_.cursor - start); return asn1_parser::make_FULL_STOP (context_.location); }
"|"                     { context_.location.columns(context_.cursor - start); return asn1_parser::make_VERTICAL_LINE (context_.location); }
"!"                     { context_.location.columns(context_.cursor - start); return asn1_parser::make_EXCLAMATION_MARK (context_.location); }
"<"                     { context_.location.columns(context_.cursor - start); return asn1_parser::make_LESS_THAN (context_.location); }
"^"                     { context_.location.columns(context_.cursor - start); return asn1_parser::make_ACCENT (context_.location); }
"@"                     { context_.location.columns(context_.cursor - start); return asn1_parser::make_AT (context_.location); }
.                       { std::cerr << "Ignoring unknown symbol: " <<  static_cast<int>(*start) << std::endl; return yylex(context_); }
%}
    }
}
