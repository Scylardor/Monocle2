
#include "Core/Log/Policies/FileWritePolicy.h"
#include "Core/Preprocessor/moeAssert.h"


namespace moe
{
    FileWritePolicy::FileWritePolicy() :
		m_fileHandle(nullptr)
    {
    }

	FileWritePolicy::FileWritePolicy(const char* filename)
	{
		OpenFile(filename);
	}

	FileWritePolicy::FileWritePolicy(FileWritePolicy&& rPolicy)
	{
		// steal the file handle
		m_fileHandle = rPolicy.m_fileHandle;
		rPolicy.m_fileHandle = nullptr;
	}

    FileWritePolicy::~FileWritePolicy()
    {
        Close();
    }

    void    FileWritePolicy::OpenFile(const char* filename)
    {
        if (MOE_ASSERT(filename != nullptr))
        {
            fopen_s(&m_fileHandle, filename, "w");
        }
        MOE_DEBUG_ASSERT(m_fileHandle != nullptr);
    }

    void    FileWritePolicy::Close()
    {
        if (HasOpenFile())
        {
            fclose(m_fileHandle);
			m_fileHandle = nullptr;
        }
    }

    void    FileWritePolicy::Write(const std::string& message)
    {
        if (HasOpenFile())
        {
            fwrite(message.c_str(), sizeof(char), message.length(), m_fileHandle);
        }
    }

    bool    FileWritePolicy::HasOpenFile() const
    {
        return (m_fileHandle != nullptr);
    }
}
