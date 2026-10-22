#pragma once

#include <cstdio>
#include <filesystem>
#include <system_error>

#include "integral_types.h"

#if defined(_WIN32)
#include <share.h> // _SH_DENYNO, _SH_DENYRW
#endif

namespace BSL
{
	enum class FileMode
	{
		Read,
		Truncate,
		Append
	};

    struct FileOpenData
    {
        FileMode mode = FileMode::Truncate;
        bool binary = false;
        bool shareable = false;
    };

	// Synchronous file utilities
	class FileIO
	{
	public:
		FileIO() = default;

		FileIO(const FileIO&) = delete;
		FileIO& operator=(const FileIO&) = delete;

		FileIO(FileIO&& other) noexcept : m_file(other.m_file), m_mode(other.m_mode)
		{
			other.m_file = nullptr;
		}

		FileIO& operator=(FileIO&& other) noexcept
		{
			if (this != &other)
			{
				Close();
				m_file = other.m_file;
				m_mode = other.m_mode;
				other.m_file = nullptr;
			}
			return *this;
		}

		~FileIO()
		{
			Close();
		}

		bool Open(const char* filePath, const FileOpenData& openData = {})
		{
            // Close another file before opening a new one
			Close();

			if (openData.mode != FileMode::Read)
			{
				CreateParentDirectories(filePath);
			}

            // Add the open mode flags
			char openMode[3] = {};
			openMode[0] = openData.mode == FileMode::Read ? 'r' : (openData.mode == FileMode::Append ? 'a' : 'w');
			if (openData.binary)
			{
				openMode[1] = 'b';
			}

#if defined(_WIN32)
			m_file = _fsopen(filePath, openMode, openData.shareable ? _SH_DENYNO : _SH_DENYRW);
#else
			m_file = std::fopen(filePath, openMode);
#endif
			m_mode = openData.mode;
			return IsOpen();
		}

		void Close()
		{
			if (m_file != nullptr)
			{
				std::fclose(m_file);
				m_file = nullptr;
			}
		}

		bool IsOpen() const
		{
			return m_file != nullptr;
		}

		bool Write(const char* data, u32 length)
		{
			if(m_file == nullptr || m_mode == FileMode::Read)
            {
                return false;
            }

            return std::fwrite(data, 1, length, m_file) == length;
		}

		u32 Read(char* data, u32 length)
		{
			if(m_file == nullptr || m_mode != FileMode::Read)
            {
                return 0;
            }

            return static_cast<u32>(std::fread(data, 1, length, m_file));
		}

		u32 GetSize() const
		{
			if (m_file == nullptr)
			{
				return 0;
			}

			const long currentPos = std::ftell(m_file);
			std::fseek(m_file, 0, SEEK_END);
			const long size = std::ftell(m_file);
			std::fseek(m_file, currentPos, SEEK_SET);
			return static_cast<u32>(size);
		}

		bool Seek(u32 position)
		{
			if(m_file == nullptr)
            {
                return false;
            }
            
            return (std::fseek(m_file, static_cast<long>(position), SEEK_SET) == 0);
		}

		void Flush()
		{
			if (m_file != nullptr)
			{
				std::fflush(m_file);
			}
		}

		// Disable stdio buffering. Call immediately after Open, before any I/O
		bool MakeUnbuffered()
		{
            if(m_file == nullptr)
            {
                return false;
            }

			return (std::setvbuf(m_file, nullptr, _IONBF, 0) == 0);
		}

	private:

		void CreateParentDirectories(const char* filePath)
		{
			const std::filesystem::path parentPath = std::filesystem::path(filePath).parent_path();
			if (!parentPath.empty())
			{
				std::error_code ec;
				std::filesystem::create_directories(parentPath, ec);
			}
		}

	private:
		FILE* m_file = nullptr;
		FileMode m_mode = FileMode::Read;
	};
}
