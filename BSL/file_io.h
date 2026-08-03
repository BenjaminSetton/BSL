#pragma once

#include <filesystem>
#include <fstream>

#include "integral_types.h"
#include "sanity.h"

namespace BSL
{
    // Synchronous file utilities
    // TODO - Clean up, simple wrapper for now
    class FileIO
    {
    public:
        explicit FileIO(const char* filePath)
        {
            // Create directory if necessary
            std::filesystem::path filePathObj = std::filesystem::path(filePath);
            std::filesystem::path parentPathObj = filePathObj.parent_path();

            if (!parentPathObj.empty() && !std::filesystem::exists(parentPathObj))
            {
                std::filesystem::create_directories(parentPathObj);
            }

            m_stream.open(filePath, std::ios::out);
        }

        // TODO - Properly implement missing big 5 members
        ~FileIO() { Close(); }

        bool IsOpen() const { return m_stream.is_open(); }

        bool Write(const char* data, u32 length)
        {
            m_stream.write(data, static_cast<std::streamsize>(length));
            return m_stream.good();
        }

        u32 Read(char* data, u32 length)
        {
            // Make sure file is open for read. Was having trouble with this flag before
            TODO();
            m_stream.read(data, static_cast<std::streamsize>(length));
            return static_cast<u32>(m_stream.gcount());
        }

        u32 GetSize() const
        {
            std::streampos currentPos = m_stream.tellg();
            m_stream.seekg(0, std::ios::end);
            std::streampos size = m_stream.tellg();
            m_stream.seekg(currentPos);
            return static_cast<u32>(size);
        }

        bool Seek(u32 position)
        {
            std::streamoff offset = static_cast<std::streamoff>(position);
            m_stream.seekg(offset, std::ios::beg);
            m_stream.seekp(offset, std::ios::beg);
            return m_stream.good();
        }

        void Close()
        {
            if (m_stream.is_open())
            {
                m_stream.close();
            }
        }

    private:
        mutable std::fstream m_stream;
    };
}
