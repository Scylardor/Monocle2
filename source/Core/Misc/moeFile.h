// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_STD_SUPPORT

#include <string>
#include <string_view>
#include <optional>

#endif // MOE_STD_SUPPORT

namespace moe
{
	[[nodiscard]] std::optional<std::string>	Monocle_Core_API ReadFile(const std::string_view& fileName, bool binary = false);
	[[nodiscard]] inline std::optional<std::string>	ReadBinaryFile( std::string_view fileName)
	{
		return ReadFile(fileName, true);
	}
	[[nodiscard]] inline std::optional<std::string>	ReadTextFile( std::string_view fileName)
	{
		return ReadFile(fileName, false);
	}
}