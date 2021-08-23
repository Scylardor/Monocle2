// Monocle Game Engine source files - Alexandre Baron

#include "JSONConfig.h"
#include "rapidjson/filereadstream.h"
#include <cstdio>
#include <cerrno>
#include <errno.h>
#include <string.h>

#include "Core/Exception/RuntimeException.h"

namespace moe
{
	bool StringEndsWith(std::string_view const& str, std::string_view const& suffix)
	{
		return (str.size() >= suffix.size() &&
			str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0);
	}


	JSONConfig::JSONConfig(int argc, char** argv)
	{
		// Load the files ending in json and collapse them into a big document
		static const std::string_view suffix{ ".json" };

		//bool firstOne = true;
		// Merge documents, but left unimplemented for now
		// See https://stackoverflow.com/questions/40013355/how-to-merge-two-json-file-using-rapidjson

		for (int iArg = 0; iArg < argc; iArg++)
		{
			std::string_view arg{ argv[iArg] };
			if (StringEndsWith(arg, suffix))
			{
				auto error = LoadConfigFile(arg);
				if (error.has_value())
				{
					throw RuntimeException(*error);
				}
				break;
			}
		}
	}


	std::optional<std::string> JSONConfig::LoadConfigFile(std::string_view const& configFile)
	{
		FILE* fp = nullptr;
		errno_t err = fopen_s(&fp, configFile.data(), "r"); // non-Windows use "r"
		if (fp == nullptr)
		{
			std::string errBuf(256, '\0');
			strerror_s(errBuf.data(), errBuf.size(), err);
			return { errBuf };
		}
		fseek(fp, 0L, SEEK_END);
		size_t fileSize = ftell(fp);
		rewind(fp);

		std::string readBuffer(fileSize, '\0');
		rapidjson::FileReadStream is(fp, readBuffer.data(), readBuffer.size());

		m_doc.ParseStream(is);

		MOE_ASSERT(m_doc.IsObject());

		fclose(fp);

		return {};
	}
}
