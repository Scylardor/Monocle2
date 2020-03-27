// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_STD_SUPPORT

#include <string>
#include <string_view>
#include <optional>

#endif // MOE_STD_SUPPORT

namespace moe
{
	[[nodiscard]]  std::optional<std::string>	ReadFile(const std::string_view fileName, bool binary = false);
}