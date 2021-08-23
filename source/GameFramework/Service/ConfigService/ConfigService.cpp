// Monocle Game Engine source files - Alexandre Baron

#include "ConfigService.h"

namespace moe
{

	ConfigService::ConfigService(Engine& ownerEngine, int argc, char** argv) :
		Service(ownerEngine),
		m_config(argc, argv)
	{
	}

	// TODO: return optional
	ConfigSection ConfigService::GetSection(std::string_view& sectionName) const
	{
		rapidjson::Value const* parent = &(*m_config);

		size_t delimiterOffset = sectionName.find(Sm_delimiter);

		while (delimiterOffset != std::string::npos)
		{
			std::string_view parentSection{ sectionName };
			parentSection.remove_suffix(sectionName.size() - delimiterOffset);
			auto parentIt = parent->FindMember(rapidjson::Value(rapidjson::Value::StringRefType(parentSection.data(), static_cast<rapidjson::SizeType>(parentSection.length()))));
			if (parentIt->value.IsObject())
				parent = &parentIt->value;
			else
				return parent;

			sectionName.remove_prefix(delimiterOffset + 1);

			delimiterOffset = sectionName.find(Sm_delimiter);
		}

		return { parent };

	}

}
