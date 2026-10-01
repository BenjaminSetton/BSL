#pragma once

#include <type_traits> // std::is_integral_v, std::is_arithmetic_v, std::enable_if

#include "integral_types.h"

#define ENABLE_IF_INTEGRAL_TYPE(T) class = typename std::enable_if<std::is_integral_v<T>>::type
#define ENABLE_IF_ARITHMETIC_TYPE(T) class = typename std::enable_if<std::is_arithmetic_v<T>>::type

namespace BSL
{
    inline constexpr double PI         = 3.1415926535897932;
    inline constexpr double HALF_PI    = PI * 0.5;
    inline constexpr double QUARTER_PI = PI * 0.25;

    template<typename T, ENABLE_IF_ARITHMETIC_TYPE(T)>
    inline T Min(const T left, const T right) noexcept { return (left < right ? left : right); }

    template<typename T, ENABLE_IF_ARITHMETIC_TYPE(T)>
    inline T Max(const T left, const T right) noexcept { return (left > right ? left : right); }

    template<typename T, ENABLE_IF_ARITHMETIC_TYPE(T)>
    inline T Clamp(const T val, const T min, const T max) noexcept { return Min(Max(val, min), max); }

    // Aligns a value up to the next multiple of alignment. Alignment must be a power of two
    template<typename T, ENABLE_IF_INTEGRAL_TYPE(T)>
    inline T AlignUp(T value, T alignment) noexcept { return (value + alignment - 1) & ~(alignment - 1); }
}
