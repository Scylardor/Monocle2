// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_STD_SUPPORT

#include <string>
#include <optional>
#include <filesystem>

#endif // MOE_STD_SUPPORT

namespace moe
{

	enum class FileMode
	{
		Text,
		Binary
	};


	[[nodiscard]] std::optional<std::string>	Monocle_Core_API ReadFile(const std::string_view& fileName, FileMode mode = FileMode::Text);
	[[nodiscard]] std::optional<std::string>	Monocle_Core_API ReadFile(const std::filesystem::path::value_type* fileName, FileMode mode = FileMode::Text);

	[[nodiscard]] inline std::optional<std::string>	ReadBinaryFile( std::string_view fileName)
	{
		return ReadFile(fileName, FileMode::Binary);
	}

	[[nodiscard]] inline std::optional<std::string>	ReadTextFile( std::string_view fileName)
	{
		return ReadFile(fileName, FileMode::Text);
	}
}