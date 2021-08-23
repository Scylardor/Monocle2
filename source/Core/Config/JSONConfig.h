// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "rapidjson/document.h"
#include <optional>

namespace moe
{
	class JSONConfig
	{
	public:

		JSONConfig(int argc, char** argv);

		std::optional<std::string>	LoadConfigFile(std::string_view const& configFile);

		auto& operator*()
		{
			return m_doc;
		}

		auto const& operator*() const
		{
			return m_doc;
		}


		[[nodiscard]] bool	IsValid() const
		{
			return m_doc.IsObject();
		}


	private:
		rapidjson::Document	m_doc;
	};
}
