// Monocle Game Engine source files - Alexandre Baron

#include "Core/Misc/moeFile.h"

#ifdef MOE_STD_SUPPORT
#include <fstream>


#endif

namespace moe
{
	std::optional<std::string> ReadFile(const std::string_view fileName, bool binary)
	{
		// std::ios_base::in inputFile always set for ifstream
		const std::ios_base::openmode mode = (binary ? std::ios::binary : std::ios::in);
		std::ifstream inputFile(fileName.data(), mode);

		if (inputFile)
		{
			std::string contents;

			inputFile.seekg(0, std::ios::end);
			contents.resize(inputFile.tellg());
			inputFile.seekg(0, std::ios::beg);

			inputFile.read(&contents[0], contents.size());

			return std::optional<std::string>{std::move(contents)};
		}

		return std::nullopt;
	}
}
