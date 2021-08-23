// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Config/JSONConfig.h"

#include "GameFramework/Service/Service.h"

#include <optional>

#define CONFIG_SPECIALIZATION(Name, type) \
			std::optional<type>	Get##Name(std::string_view configName) const \
			{ \
				return Get<type>(configName); \
			}


namespace moe
{


	/*
	 * A section of config, useful to avoid having to specify the section multiple times
	 * Becomes invalid if the "source" section is deleted.
	 */
	class ConfigSection
	{
	public:
		ConfigSection(rapidjson::Value const* parent) :
			m_section(parent)
		{}

		auto& operator*()
		{
			return m_section;
		}

		auto const& operator*() const
		{
			return m_section;
		}

		template <typename T>
		T	Get(std::string_view configName) const
		{
			auto const paramIt = m_section->FindMember(rapidjson::Value(rapidjson::Value::StringRefType(configName.data(), static_cast<rapidjson::SizeType>(configName.length()))));
			if (paramIt == m_section->MemberEnd())
				return {};

			return { paramIt->value.Get<T>() };
		}

		CONFIG_SPECIALIZATION(Uint, uint32_t)
			CONFIG_SPECIALIZATION(Int, int)
			CONFIG_SPECIALIZATION(Float, float)
			CONFIG_SPECIALIZATION(Bool, bool)
			CONFIG_SPECIALIZATION(String, char const*)


	private:
		rapidjson::Value const* m_section;
	};


	/*
	 * A config service using the YAML config
	 */
	class ConfigService : public Service
	{
	public:

		using ServiceLocatorType = ConfigService;


		ConfigService(Engine& ownerEngine, int argc, char** argv);

		template <typename T>
		[[nodiscard]] std::optional<T>	Get(std::string_view configName) const;

		CONFIG_SPECIALIZATION(Uint, uint32_t)
		CONFIG_SPECIALIZATION(Int, int)
		CONFIG_SPECIALIZATION(Float, float)
		CONFIG_SPECIALIZATION(Bool, bool)
		CONFIG_SPECIALIZATION(String, char const*)


		ConfigSection	EditSection(std::string_view const& sectionName);


		[[nodiscard]] ConfigSection	GetSectionOf(std::string_view& configName) const;

		[[nodiscard]] ConfigSection	GetSection(std::string_view section) const;


	private:


		JSONConfig	m_config;

		static const char	Sm_delimiter = '.';

	};

}
#include "ConfigService.inl"