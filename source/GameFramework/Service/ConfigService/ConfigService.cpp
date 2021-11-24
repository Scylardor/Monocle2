// Monocle Game Engine source files - Alexandre Baron

#include "ConfigService.h"

namespace moe
{

	ConfigService::ConfigService(Engine& ownerEngine, int argc, char** argv) :
		Service(ownerEngine),
		m_config(argc, argv)
	{
	}


	ConfigSection ConfigService::GetSectionOf(std::string_view& configName) const
	{
		rapidjson::Value const* parent = &(*m_config);

		size_t delimiterOffset = configName.find(Sm_delimiter);

		while (delimiterOffset != std::string::npos)
		{
			std::string_view parentSection{ configName };
			parentSection.remove_suffix(configName.size() - delimiterOffset);
			auto parentIt = parent->FindMember(rapidjson::Value(rapidjson::Value::StringRefType(parentSection.data(), static_cast<rapidjson::SizeType>(parentSection.length()))));
			if (parentIt->value.IsObject())
				parent = &parentIt->value;
			else
				return parent;

			configName.remove_prefix(delimiterOffset + 1);

			delimiterOffset = configName.find(Sm_delimiter);
		}

		return { parent };
	}


	ConfigSection ConfigService::GetSection(std::string_view section) const
	{
		if (m_config.IsValid() == false)
			return {nullptr};

		rapidjson::Value const* parent = &(*m_config);

		size_t delimiterOffset = section.size();

		do
		{
			std::string_view parentSection{ section };
			parentSection.remove_prefix(section.size() - delimiterOffset);
			auto parentIt = parent->FindMember(rapidjson::Value(rapidjson::Value::StringRefType(parentSection.data(), static_cast<rapidjson::SizeType>(parentSection.length()))));
			if (parentIt->value.IsObject())
				parent = &parentIt->value;
			else
				return parent;

			delimiterOffset = section.find(Sm_delimiter);
		} while (delimiterOffset != std::string_view::npos);


		return { parent };
	}
}
