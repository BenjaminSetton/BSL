#pragma once

#include <array>          // std::array
#include <cstddef>        // std::size_t
#include <cstring>        // std::strlen
#include <type_traits>    // std::enable_if_t, std::is_pointer_v, std::is_same_v, std::remove_cv_t, std::remove_pointer_t
#include <utility>        // std::index_sequence, std::make_index_sequence

#include "integral_types.h"

namespace BSL
{
	typedef u32 CRC32;

	inline constexpr u32 CRC_TABLE_SIZE = 256;
	inline constexpr u32 CRC32_POLYNOMIAL = 0xEDB88320u;

	// Helper to expand an index_sequence into a brace-enclosed initializer list,
	// which is the C++17 idiom for populating a constexpr std::array with a
	// per-element computation.
	template<u32... Indices>
	constexpr std::array<u32, CRC_TABLE_SIZE> MakeCRCTableImpl(std::index_sequence<Indices...>)
	{
		constexpr auto computeEntry = [](u32 i) constexpr {
			u32 c = i;
			for (u32 j = 0; j < 8; j++)
			{
				c = (c & 1u) ? (CRC32_POLYNOMIAL ^ (c >> 1)) : (c >> 1);
			}
			return c;
		};
		return { computeEntry(Indices)... };
	}
	inline constexpr std::array<u32, CRC_TABLE_SIZE> s_crcTable = MakeCRCTableImpl(std::make_index_sequence<CRC_TABLE_SIZE>{});

	// Internal function to compute CRC32 hash. While this can be called directly, prefer using overrides below which
	// don't require an explicit length parameter 
	constexpr CRC32 HashCRC32_Internal(const char* str, u32 length)
	{
		u32 c = 0xFFFFFFFFu;
		for (u32 i = 0; i < length; i++)
		{
			c = s_crcTable[(c ^ static_cast<u8>(str[i])) & 0xFFu] ^ (c >> 8);
		}
		return c ^ 0xFFFFFFFFu;
	}

	// Hash a string literal at compile time
	template<u32 N>
	constexpr CRC32 HashCRC32(const char (&str)[N])
	{
		return HashCRC32_Internal(str, N - 1);
	}

	// Hash a string literal at runtime (std::strlen is not constexpr in C++17)
	template<typename T, typename = std::enable_if_t<std::is_pointer_v<T> && std::is_same_v<std::remove_cv_t<std::remove_pointer_t<T>>, char>>>
	inline CRC32 HashCRC32(T str)
	{
		return HashCRC32_Internal(str, static_cast<u32>(std::strlen(str)));
	}
}
