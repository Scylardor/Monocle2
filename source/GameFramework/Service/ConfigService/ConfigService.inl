#pragma once


namespace moe
{
	template <typename T>
	std::optional<T> ConfigService::Get(std::string_view configName) const
	{
		if (false == m_config.IsValid())
			return {};

		auto const section = GetSection(configName);

		auto const param = (*section)->FindMember(configName.data());
		if (param == (*section)->MemberEnd())
			return {};

		return { param->value.Get<T>() };
	}



}
