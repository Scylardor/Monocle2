#ifndef MOE_FILE_WRITE_POLICY_H_
#define MOE_FILE_WRITE_POLICY_H_

// A logging write-policy used to write log entries to a text file.
// Needs to be initialized to function properly.

// TODO: we can't use an fstream in this class because in a DLL, std classes across the DLL boundary are potential ABI problems
// cf. this warning:
//warning C4251: 'moe::FileWritePolicy::m_stream': class 'std::basic_ofstream<char,std::char_traits<char>>' needs to have dll-interface to be used by clients of class 'moe::FileWritePolicy'
// it should be OK if we build linking exe and linked DLL with the same compiler, but ultimately we can't use fstream for this.
#include "Core/Preprocessor/moeDLLVisibility.h"
#ifdef MOE_STD_SUPPORT
#include <string>

namespace moe
{
    class MOE_DLL_API FileWritePolicy
    {
    public:
        FileWritePolicy();
		FileWritePolicy(const char* filename);
		FileWritePolicy(FileWritePolicy&& rPolicy);
        FileWritePolicy(const FileWritePolicy& other) = delete;
		FileWritePolicy& operator=(const FileWritePolicy& rhs) = delete;
        ~FileWritePolicy();

        void    OpenFile(const char* filename);
        void    Close();
        void    Write(const std::string& message);
        bool    HasOpenFile() const;

    private:
        std::FILE*   m_fileHandle;
    };
}
#endif // MOE_STD_SUPPORT

#endif // MOE_FILE_WRITE_POLICY_H_