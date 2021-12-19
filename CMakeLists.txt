cmake_minimum_required(VERSION 3.20)
project(BigNumberLibrary)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)

set(CMAKE_CXX_FLAGS
"-Ofast -Wall -Wextra -Werror -Wpedantic -Wdeprecated \
-Wconversion -Wshadow -Wnon-virtual-dtor -Wcast-align \
-Wpointer-arith -Wunused -Woverloaded-virtual -flto=auto \
-Wno-stringop-overflow -Wuninitialized"
)

include_directories(components)

add_executable(app
        components/BigNumber.hpp
        components/BigInteger.hpp
        components/NumberFormatException.hpp
        components/ArithmeticException.hpp
        components/Traits.hpp
        main.cpp
)

add_executable(test
        components/BigNumber.hpp
        components/BigInteger.hpp
        components/NumberFormatException.hpp
        components/ArithmeticException.hpp
        components/Traits.hpp
        unit-tests/BigInteger.cpp
        unit-tests/main.cpp
)

target_link_libraries(test gtest)

