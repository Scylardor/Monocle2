#ifndef MOE_STRINGFORMAT_H_
#define MOE_STRINGFORMAT_H_

// An implementation of String Formatting using variadic templates
// cf. articles like https://msdn.microsoft.com/en-us/magazine/dn913181.aspx

namespace moe
{
    // A wrapper function for format args.
    // Specialize it for a custom type to pass any member of an object
    // to act as a format parameter. Cf. std::basic_string specialization.
    // (include T's definition before StringFormat.h or it won't compile)
    #ifdef MOE_STD_SUPPORT
    // If std is supported, provide convenient specializations for StringFormat to work with std::string "out of the box".
    // This specialization has to be put BEFORE the more generic template because GCC takes the first one that fits
    template <typename CharT>
    const CharT * StringFormatArg(const std::basic_string<CharT> & stringArg)
    {
      return stringArg.c_str();
    }
    #endif // MOE_STD_SUPPORT

    template <typename T, typename U = T>
    const U& StringFormatArg(const T& value)
    {
        return value;
    }

    // The decorator for the string buffer Format works on. It allows to decouple the type and the algorithm.
    // To use a custom string class as a buffer with Format, you'll need to specialize this template.
    template <class String>
    class StringFormatBuffer
    {
    public:
        StringFormatBuffer(String& buf) : m_buf(buf)
        {
            // This is always false on purpose.
            static_assert(sizeof(String) == -1, "Specialize this template with your String class to be able to use it as a buffer for Format.");
        }

        std::size_t Size();
        void        Resize(std::size_t newSize);
        char&       operator[](std::size_t pos); // char = default return type

    private:
        String& m_buf;
    };

    template <class String, typename FormatChar, typename... Args>
    void StringFormat(String & buffer, const FormatChar * const format, const Args& ... args);
}

// Swprintf has a platform-specific behavior
#ifdef MOE_WINDOWS
    #include "Core/StringFormat/Private/Windows/Win_moeSwprintf.internal.hpp"
#elif defined(MOE_LINUX)
    #include "Core/StringFormat/Private/Linux/Linux_moeSwprintf.internal.hpp"
#endif

#include "Core/StringFormat/Private/moeStringFormat.internal.hpp"


#endif // MOE_STRINGFORMAT_H_
