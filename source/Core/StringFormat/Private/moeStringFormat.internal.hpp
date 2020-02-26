#ifdef MOE_STD_SUPPORT
#include <string>
#endif // MOE_STD_SUPPORT

#include "Core/Preprocessor/moeAssert.h"

namespace moe
{
    // Under the hood, StringFormat calls sprintf functions.
    template <typename... Args>
    int SnPrintf(char * const buffer, std::size_t bufferSize, const char * const format, const Args& ... args)
    {
        int const result = std::snprintf(buffer, bufferSize, format, StringFormatArg(args)...);
        MOE_DEBUG_ASSERT(result != -1);
        return result;
    }

    // This function passes +1 to buffer sizes because SnPrintf assumes the reported buffer size includes space for the null terminator.
    template <class String, typename FormatChar, typename... Args>
    void StringFormat(String & buffer, const FormatChar * const format, const Args& ... args)
    {
        StringFormatBuffer<String> fmtBuffer(buffer);
        std::size_t currentBufferSize = fmtBuffer.Size();
        // Assume the target buffer is large enough and resize only if necessary
        std::size_t formattedSize = SnPrintf(&fmtBuffer[0], currentBufferSize + 1, format, args...);

        if (formattedSize < currentBufferSize)
        {
            fmtBuffer.Resize(formattedSize);
        }
        else
        {
            while (formattedSize > currentBufferSize)
            {
                fmtBuffer.Resize(formattedSize);
                currentBufferSize = formattedSize;
                formattedSize = SnPrintf(&fmtBuffer[0], formattedSize + 1, format, args...);
                MOE_DEBUG_ASSERT(formattedSize != (std::size_t)-1);
                if (fmtBuffer.Size() > formattedSize)
                {
                    fmtBuffer.Resize(formattedSize);
                }
            }
        }
    }

    // Specializations to protect against single format strings
    template <class String>
    void StringFormat(String & buffer, const char * const format)
    {
        StringFormat(buffer, "%s", format);
    }

    template <class String>
    void StringFormat(String & buffer, const wchar_t * const format)
    {
        StringFormat(buffer, L"%ls", format);
    }



#ifdef MOE_STD_SUPPORT

    template <typename CharT>
    class StringFormatBuffer<std::basic_string<CharT>>
    {
    public:
        StringFormatBuffer(std::basic_string<CharT>& buf) : m_buf(buf) {}

        std::size_t Size() const
        {
            return m_buf.size();
        }

        void    Resize(std::size_t newSize)
        {
            m_buf.resize(newSize);
        }

        CharT&  operator[](std::size_t pos)
        {
            return m_buf[pos];
        }

    private:
        std::basic_string<CharT>& m_buf;
    };
#endif // MOE_STD_SUPPORT

}
