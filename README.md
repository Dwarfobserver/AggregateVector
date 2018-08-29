
[![GCC Build Status](https://travis-ci.org/Dwarfobserver/AggregateVector.svg?branch=master)](https://travis-ci.org/Dwarfobserver/AggregateVector) [![MSVC Build Status](https://ci.appveyor.com/api/projects/status/github/Dwarfobserver/AggregateVector?svg=true)](https://ci.appveyor.com/project/Dwarfobserver/aggregatevector) [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

# Aggregate vector

This project is an attempt to resolve the usability issues coming when working with structure of arrays rather than array of structures :

This single-header library in C++17 implements a std::vector-like data structure which separates it's aggregate components into different arrays. It improves performance when there is different access patterns for the aggregate components, or when we want to perform vectorized operations on it's components.

It works on MSVC-19.14, Clang-5.0 and GCC-7.2.

This project is in early development and prone to change. I'd be happy to receive any feedback on it's design or performance !

Simple exemple usage :

```cpp

#include <aggregate_vector.hpp>
#include <iostream>

// We define an aggregate type : a type with only public members, no virtual functions and no constructor.
namespace user {
    struct person {
        std::string name;
        int age;
    };
}

// We expose the aggregate so it can be used by av::vector.
AV_DEFINE_TYPE(user::person, name, age);
// The line above is equivalent to :
// namespace av {
//     template <> struct members<::user::person> {
//         vector_span<0, ::user::person, std::string> name;
//         vector_span<1, ::user::person, int>         age;
//     };
// }

// We can now manipulate our av::vector.
av::vector<user::person> make_persons() {

    // The semantics are similar to std::vector.
    // Only one allocation is performed, so the av::vector content looks like this in memory :
    // [name1, name2, ..., age1, age2, ...]
    auto persons = av::vector<user::person>{};
    persons.reserve(2);
    persons.push_back({ "Jack", 35 });
    // emplace_back takes components as arguments, or default-construct them.
    persons.emplace_back("New Born");

    // Components are accessed with their name, through a range structure :
    // av::vector stores internally a pointer for each component.
    for (auto& age : persons.age)
        age += 1;
    
    for (auto& name : persons.name)
        std::cout << "new person : " << name << '\n';
    
    return persons;
}

```

Tests can be built and launched with CMake.

```bash

mkdir build
cd build
cmake ..
cmake --build .
ctest -V

```

Project limitations :

 - The aggregate max size is limited (10 by default, it can be increased with more copy-pasta of the 'av::detail::as_tuple' function).
 - It does not support aggregates with native arrays (eg. T[N], use std::array<T, N> instead).
 - It does not support aggregates with base classes (they are detected as aggregates but can't be destructured).
 - It does not support over-aligned types from the aggregates.
