#pragma once


namespace moe
{


	template <typename T>
	std::optional<T> ConfigService::Get(std::string_view configName) const
	{
		auto delimiterOffset = configName.find(Sm_delimiter);

		if (delimiterOffset != configName.npos)
		{
			std::string section(configName.data(), delimiterOffset);
			auto sectionNode = (*m_config)[section];
			auto type = sectionNode.Type(); type;
			if (!sectionNode)
				return {};

			configName.remove_prefix(delimiterOffset+1);
			auto cfg = sectionNode[std::string(configName)];
			if (!cfg)
				return {};

			return { cfg.as<T>() };
		}


		auto cfg = (*m_config)[std::string(configName)];
		if (!cfg)
			return {};

		return { cfg.as<T>() };
	}
}
