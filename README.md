# fast_ber ![version](https://img.shields.io/github/tag/samuel-tyler/fast_ber.svg) [![Travis status](https://ci.appveyor.com/api/projects/status/github/Samuel-Tyler/fast_ber?branch=master&svg=true)](https://ci.appveyor.com/project/Samuel-Tyler/fast-ber) [![Appveyor status](https://travis-ci.org/Samuel-Tyler/fast_ber.svg?branch=master)](https://travis-ci.org/Samuel-Tyler/fast_ber)
A performant ASN.1 BER encoding and decoding library written in C++11

## Introduction
fast_ber is a small, lightweight library for BER encoding and decoding. Fast ber forgoes some tight ASN.1 specification conformance to provide fast encoding and decoding performance in common use cases

#### Design Decisions
- Simple, modern C++ interface
- ASN.1 sequences are represented as POD structs - no private members or complex getters and setters
- No exceptions, no RTTI and limited memory allocations (everything is small buffer optimised)
- Header only
- View classes are provided for zero copy decoding
- Interfaces mimic STL types such as std::string, std::vector and std::optional

[Read more about the decisions made writing this library](https://medium.com/@samtyler6/translating-asn-1-ber-types-to-modern-c-226fb06d026f)

#### Limitations
- No circular data structures
- Size and value constraints are not implemented

## Usage
1. Build the compiler:
```
git submodule update --init
mkdir build_cmake
cd build_cmake
cmake ..
make
ctest
```

2. Define an ASN.1 file - Example: pokemon.asn
```
Pokemon DEFINITIONS AUTOMATIC TAGS ::= BEGIN

Team ::= SEQUENCE {
    members SEQUENCE OF Pokemon
}

Pokemon ::= SEQUENCE {
    name OCTET STRING,
    category OCTET STRING,
    type OCTET STRING,
    ability OCTET STRING,
    weakness OCTET STRING,
    weight INTEGER
}

END
```

3. Compile an asn file into a header file defining the ber structure
```
cd build_cmake
./src/fast_ber_compiler pokemon.asn pokemon
```
output:
```
#include "fast_ber/ber_types/All.hpp"
#include "fast_ber/util/Encode.hpp"
#include "fast_ber/util/Decode.hpp"

namespace fast_ber {
namespace Pokemon {

struct Pokemon {
    OctetString name;
    OctetString category;
    OctetString type;
    OctetString ability;
    OctetString weakness;
    Integer weight;
};

struct Team {
    SequenceOf<Pokemon> members;
};

... helper functions ...

} // End namespace Pokemon
} // End namespace fast_ber
```

4. Include the header file into your application. Fields in the generated structure can be assigned to standard types. Encode and decode functions are used to serialize and deserialize the data
```
#include "pokemon.hpp"

int main()
{
    fast_ber::Pokemon::Team    team;
    fast_ber::Pokemon::Pokemon muchlax = {"Munchlax", "Big Eater", "Normal", "Thick Fat, Pickup", "Fighting", 105};
    fast_ber::Pokemon::Pokemon piplup  = {"Piplup", "Penguin", "Water", "Torrent", "Electric, Grass", 12};
    team.members.push_back(muchlax);
    team.members.push_back(piplup);

    std::array<uint8_t, 2000> buffer;
    const auto encode_result = fast_ber::encode(absl::MakeSpan(buffer.data(), buffer.size()), team);
    if (!encode_result.success)
    {
        return -1;
    }

    return 0;
}
```

## Features
fast_ber is in development. The following table documents what has been implemented and what features are coming soon

| Type | Implemented |
| --- | --- |
| Integer | Yes |
| Boolean | Yes |
| Strings | Yes |
| Sequence | Yes |
| Sequence Of | Yes |
| Choice | Yes |
| Set | No |
| Set Of | Yes |
| Enumerated | Yes |
| Object Identifier | Yes |
| Dates / Times | No |
| Null | Yes |
| Any | No |

| Feature | Implemented |
| --- | --- |
| Explicit tags | Yes |
| Implicit tags | Yes |
| Indefinitive length | No |
| Arbitrary top level structure | Yes |
| Arbritary definition order | Yes |
| Embedded Types | Yes |
| Imports | Yes |

## Possible Future Extensions
The following features may be added in the future

```
Partial decoding
Partial encode at compile time (templated / constexpr)
High / low conformance modes
DER mode
```

## Benchmarks

fast_ber includes benchmarks against asn1c, an ASN library written in C. Here is an example of the output:
```
-------------------------------------------------------------------------------
Benchmark: Decode Performance
-------------------------------------------------------------------------------
...............................................................................

benchmark name                                  iters   elapsed ns      average
-------------------------------------------------------------------------------
fast_ber        - decode 2107 byte packet           1    465768155   465.768 ms
asn1c           - decode 2107 byte packet           1   5963799719     5.9638 s
fast_ber        - decode 64 byte packet             1    116247675   116.248 ms
asn1c           - decode 64 byte packet             1   3068504188     3.0685 s

-------------------------------------------------------------------------------
Benchmark: Encode Performance
-------------------------------------------------------------------------------
...............................................................................

benchmark name                                  iters   elapsed ns      average
-------------------------------------------------------------------------------
fast_ber        - encode                            1    309072302   309.072 ms
asn1c           - encode                            1   8936055854    8.93606 s

-------------------------------------------------------------------------------
Benchmark: Object Construction Performance
-------------------------------------------------------------------------------
...............................................................................

benchmark name                                  iters   elapsed ns      average
-------------------------------------------------------------------------------
fast_ber        - construct                         1    664078025   664.078 ms
asn1c           - construct                         1    671927051   671.927 ms

===============================================================================
All tests passed (28 assertions in 7 test cases)
```
