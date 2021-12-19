#pragma once

template<typename T>
concept Integer = std::is_integral_v<T>;

template<typename> struct next_integer_type;
template<typename T> using next_integer_type_t = typename next_integer_type<T>::type;
template<typename T> struct tag { using type = T; };

template<> struct next_integer_type<uint32_t>  : tag<uint64_t> { };
template<> struct next_integer_type<uint64_t> : tag<__uint128_t> { };
