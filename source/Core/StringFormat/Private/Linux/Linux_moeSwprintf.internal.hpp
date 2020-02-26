
// LINUX version
namespace moe
{
    // Linux implementation of std::swprintf always returns -1 if the buffer doesn't fit. As a result the only solution
    // we have when it doesn't fit is trying to increment its size until it fits...
    template <typename... Args>
    int SnPrintf(wchar_t * const buffer, std::size_t bufferSize, const wchar_t * const format, const Args& ... args)
    {
        static const int bufferSizeIncrement = 64;
        int result = std::swprintf(buffer, bufferSize, format, StringFormatArg(args)...);
        if (result == -1)
        {
            result = bufferSize + bufferSizeIncrement;
        }
        return result;
    }
}