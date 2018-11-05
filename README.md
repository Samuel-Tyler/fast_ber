# fast_ber
A performant ASN.1 BER encoding and decoding library written in C++11.

## Introduction
fast_ber is a small, lightweight library for BER encoding and decoding. Fast ber forgoes some tight ASN.1 specification conformance to provide fast encoding and decoding performance in common use cases.

#### Design decisions
- Simple, modern C++ interface using POD structs
- No exceptions, no RTTI, limited memory allocations (everything is small buffer optimised)
- Header only
- View classes for zero copy decoding
- Interfaces mimic STL types such as std::string, std::vector and std::optional

#### Limitations
- No circular data structures
- Field size and value constraints implemented

## Usage:
1. Build the compiler:
```
mkdir build_cmake
cd build_cmake
cmake ..
make
ctest
```

2. Define a ASN.1 file - Example: pokemon.asn
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
./src/fast_ber_compiler pokemon.asn > pokemon.hpp
```

4. Include the header file into your application. Fields in the generated structure can be assigned to standard types. Encode and decode functions are use to serialize and deserialize the data.
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
    const auto encode_result = fast_ber::Pokemon::encode_with_new_id(absl::MakeSpan(buffer.data(), buffer.size()), team);
    if (!encode_result.success)
    {
        return -1;
    }
    
    return 0;
```
