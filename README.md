
## Under development, no garantees provided until the first release

[![GCC Build Status](https://travis-ci.org/Dwarfobserver/AggregateVector.svg?branch=master)](https://travis-ci.org/Dwarfobserver/AggregateVector) [![MSVC Build Status](https://ci.appveyor.com/api/projects/status/github/Dwarfobserver/AggregateVector?svg=true)](https://ci.appveyor.com/project/Dwarfobserver/aggregatevector) [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

# Aggregate vector

This project is an attempt to resolve the usability issues coming when working with structure of arrays rather than array of structures.

This single-header library in C++17 implements a std::vector-like data structure which separates it's aggregate components into different arrays. It improves performance when there is different access patterns for the aggregate components, and when we want to perform vectorized operations on it's components.

It works for MSVC-19.14, Clang-6 and GCC-7.2.

Simple usage exemple :

```cpp

namespace user {
    struct person {
        std::string name;
        int age;
    };
}

AV_DEFINE_TYPE(user::person, name, age);
/*
The line above is equivalent to :
namespace av {
    template <> struct members<::user::person> {
        vector_span<0, ::user::person, std::string> name;
        vector_span<1, ::user::person, int>         age;
    };
}
*/

void update(av::vector<user::person>& persons) {

    // Components are accessible with span-like structures through av::vector.
    for (auto& age : persons.age) ++age;
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

TODO List :

 - Make a tutorial test (getting started)
 - Document exception garantees and assert suppositions
