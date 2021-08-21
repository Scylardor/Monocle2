// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Config/YAMLConfig.h"

#include "GameFramework/Service/Service.h"

#include <optional>


namespace moe
{
	class ConfigService : public Service
	{
	public:

		ConfigService(int argc, char** argv);

		template <typename T>
		std::optional<T>	Get(std::string_view configName) const;

	private:

		YAMLConfig	m_config;

		static const char	Sm_delimiter = ':'; // In true YAML fashion

	};


}
#include "ConfigService.inl"