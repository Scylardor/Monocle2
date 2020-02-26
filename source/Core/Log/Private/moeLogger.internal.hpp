namespace moe
{
    template <class FilterPolicy, class FormatPolicy, class WritePolicy>
    StdLogger<FilterPolicy, FormatPolicy, WritePolicy>::StdLogger(FilterPolicy&& filter, FormatPolicy&& formatter, WritePolicy&& writer) :
        m_filter(std::move(filter)),
        m_formatter(std::move(formatter)),
        m_writer(std::move(writer))
    {
    }

    template <class FilterPolicy, class FormatPolicy, class WritePolicy>
    void    StdLogger<FilterPolicy, FormatPolicy, WritePolicy>::ConsumeLogMessage(const LogInfo& logInfo)
    {
        if (m_filter.Filter(logInfo))
        {
            std::string tempBuffer;
            const std::string& formattedBuffer = m_formatter.Format(tempBuffer, logInfo);
            m_writer.Write(formattedBuffer);
        }
    }
}