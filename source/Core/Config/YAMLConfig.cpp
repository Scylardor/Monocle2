// Monocle Game Engine source files - Alexandre Baron


#include "YAMLConfig.h"

namespace moe
{
	bool StringEndsWith(std::string_view const& str, std::string_view const& suffix)
	{
		return (str.size() >= suffix.size() &&
			str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0);
	}


	YAMLConfig::YAMLConfig(int argc, char** argv)
	{
		// Load the first file ending in ".yaml" or ".yml"
		static const std::string_view suffix[2] = { ".yaml", ".yml" };

		for (int iArg = 0; iArg < argc; iArg++)
		{
			std::string_view arg{ argv[iArg] };
			if (StringEndsWith(arg, suffix[0]) || StringEndsWith(arg, suffix[1]))
			{
				LoadConfigFile(arg);
				break;
			}
		}
	}


	void YAMLConfig::LoadConfigFile(std::string_view const& configFile)
	{
		m_config = YAML::LoadFile(std::string(configFile));
	}
}
