// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "yaml-cpp/yaml.h"

namespace moe
{
	class YAMLConfig
	{
	public:

		YAMLConfig(int argc, char** argv);

		void	LoadConfigFile(std::string_view const& configFile);

		auto operator*()
		{
			return m_config;
		}

		auto operator*() const
		{
			return m_config;
		}

	private:
		YAML::Node	m_config;
	};
}
