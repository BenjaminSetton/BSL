#pragma once

#include <cstdint> // (u)int8_t, (u)int16_t, (u)int32_t, (u)int64_t 
#include <limits>  // std::numeric_limits

// BSL's integral type aliases (u8, i32, etc) are defined at global scope by default
// because they are written extremely often
//
// Define BSL_NO_INTEGRAL_TYPES before including this header to suppress these typedefs
// Use this if you have your own integral types that you want to use instead
#ifndef BSL_NO_INTEGRAL_TYPES

// unsigned int
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

inline constexpr u8  U8_MIN  = 0;
inline constexpr u16 U16_MIN = 0;
inline constexpr u32 U32_MIN = 0;
inline constexpr u64 U64_MIN = 0;

inline constexpr u8  U8_MAX  = std::numeric_limits<u8 >::max();
inline constexpr u16 U16_MAX = std::numeric_limits<u16>::max();
inline constexpr u32 U32_MAX = std::numeric_limits<u32>::max();
inline constexpr u64 U64_MAX = std::numeric_limits<u64>::max();

// signed int
typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

inline constexpr i8  I8_MIN  = std::numeric_limits<i8 >::min();
inline constexpr i16 I16_MIN = std::numeric_limits<i16>::min();
inline constexpr i32 I32_MIN = std::numeric_limits<i32>::min();
inline constexpr i64 I64_MIN = std::numeric_limits<i64>::min();

inline constexpr i8  I8_MAX  = std::numeric_limits<i8 >::max();
inline constexpr i16 I16_MAX = std::numeric_limits<i16>::max();
inline constexpr i32 I32_MAX = std::numeric_limits<i32>::max();
inline constexpr i64 I64_MAX = std::numeric_limits<i64>::max();

#else // BSL_NO_INTEGRAL_TYPES

// The rest of BSL references these types directly. If you define
// BSL_NO_INTEGRAL_TYPES, you MUST provide your own equivalent typedefs with these exact
// names at global scope before including any BSL header. These static_asserts catch the
// missing types early with a clear message instead of letting the compiler produce a
// cascade of cryptic errors deep inside BSL internals.
static_assert(sizeof(u8)  == 1, "BSL_NO_INTEGRAL_TYPES: u8  is not defined or is not 8-bit. You must provide your own typedef");
static_assert(sizeof(u16) == 2, "BSL_NO_INTEGRAL_TYPES: u16 is not defined or is not 16-bit. You must provide your own typedef");
static_assert(sizeof(u32) == 4, "BSL_NO_INTEGRAL_TYPES: u32 is not defined or is not 32-bit. You must provide your own typedef");
static_assert(sizeof(u64) == 8, "BSL_NO_INTEGRAL_TYPES: u64 is not defined or is not 64-bit. You must provide your own typedef");
static_assert(sizeof(i8)  == 1, "BSL_NO_INTEGRAL_TYPES: i8  is not defined or is not 8-bit. You must provide your own typedef");
static_assert(sizeof(i16) == 2, "BSL_NO_INTEGRAL_TYPES: i16 is not defined or is not 16-bit. You must provide your own typedef");
static_assert(sizeof(i32) == 4, "BSL_NO_INTEGRAL_TYPES: i32 is not defined or is not 32-bit. You must provide your own typedef");
static_assert(sizeof(i64) == 8, "BSL_NO_INTEGRAL_TYPES: i64 is not defined or is not 64-bit. You must provide your own typedef");

#endif
