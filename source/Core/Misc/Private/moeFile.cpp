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
		std::ifstream inputFile(fileName.data(), (binary ? std::ifstream::binary : std::ios_base::in));

		if (false == inputFile.is_open())
		{
			MOE_ERROR(ChanDefault, "ReadFile failed to read file %s.", fileName.data());
			return std::nullopt;
		}

		// get length of file:
		inputFile.seekg(0, inputFile.end); // go to the file end
		const int fileLength = inputFile.tellg(); // Note the length we walked
		inputFile.seekg(0, inputFile.beg); // rewind to the beginning

		std::string buf;
		buf.resize(fileLength);
		// read data as a block:
		inputFile.read(buf.data(), fileLength);

		if (!inputFile)
		{
			MOE_ERROR(ChanDefault, "Reading file %s failed: need to read %d characters, could only read %d.", fileName.data(), fileLength, inputFile.gcount());
			return std::nullopt;
		}

		return std::optional<std::string>{buf};
	}
}
