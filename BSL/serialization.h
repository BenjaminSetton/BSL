#pragma once

#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

#include "integral_types.h"
#include "logger.h"

namespace BSL
{
	// Generate a u32 magic number from a 4-char string literal by packing the 4 chars into a u32 in little-endian order. 
	// Requires 4 chars + null terminator
	constexpr u32 MakeMagicNumber(const char (&str)[5])
	{
		return static_cast<u32>(static_cast<u8>(str[0]))
		     | (static_cast<u32>(static_cast<u8>(str[1])) << 8)
		     | (static_cast<u32>(static_cast<u8>(str[2])) << 16)
		     | (static_cast<u32>(static_cast<u8>(str[3])) << 24);
	}

	template<typename T, typename = std::enable_if_t<std::is_trivial_v<T>>>
	inline void WriteTrivial(std::ostream& os, const T& value)
	{
		os.write(reinterpret_cast<const char*>(&value), sizeof(T));
	}

	template<typename T, typename = std::enable_if_t<std::is_trivial_v<T>>>
	inline T ReadTrivial(std::istream& is)
	{
		T value{};
		is.read(reinterpret_cast<char*>(&value), sizeof(T));
		return value;
	}

	inline void WriteString(std::ostream& os, const std::string& str)
	{
		const u32 length = static_cast<u32>(str.size());
		WriteTrivial(os, length);
		os.write(str.data(), length);
	}

	inline std::string ReadString(std::istream& is)
	{
		const u32 length = ReadTrivial<u32>(is);
		std::string str(length, '\0');
		is.read(&str[0], length);
		return str;
	}

	inline void WriteBytes(std::ostream& os, const u8* data, u64 size)
	{
		WriteTrivial(os, size);
		os.write(reinterpret_cast<const char*>(data), size);
	}

	inline std::vector<u8> ReadBytes(std::istream& is)
	{
		u64 size = ReadTrivial<u64>(is);
		std::vector<u8> data(size);
		is.read(reinterpret_cast<char*>(data.data()), size);
		return data;
	}

	struct FileHeader
	{
		char     magic[4];
		u32      version;
		u32      typeHash;
	};

	inline bool WriteHeader(std::ostream& os, const char* magic, u32 version, u32 typeHash)
	{
		FileHeader header{};
		std::memcpy(header.magic, magic, 4);
		header.version  = version;
		header.typeHash = typeHash;

		os.write(reinterpret_cast<const char*>(&header), sizeof(FileHeader));
		return os.good();
	}

	inline bool ReadAndValidateHeader(std::istream& is, const char* expectedMagic, u32 expectedVersion, u32 expectedTypeHash)
	{
		FileHeader header{};
		is.read(reinterpret_cast<char*>(&header), sizeof(FileHeader));
		if (!is.good())
		{
			LogError("Failed to read file header. Could not read from stream!");
			return false;
		}

		if (std::memcmp(header.magic, expectedMagic, 4) != 0)
		{
			LogError("Failed to read file header. Magic mismatch, expected \"%s\", got \"%s\"", expectedMagic, header.magic);
			return false;
		}

		if (header.version != expectedVersion)
		{
			LogError("Failed to read file header. Version mismatch, expected \"%u\", got\"%u\"", expectedVersion, header.version);
			return false;
		}

		if (header.typeHash != expectedTypeHash)
		{
			LogError("Failed to read file header. Type hash mismatch, expected \"%u\", got \"%u\"", expectedTypeHash, header.typeHash);
			return false;
		}

		return true;
	}
}
