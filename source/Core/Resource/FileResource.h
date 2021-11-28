#pragma once
#include <string>
#include <filesystem>

#include "BaseResource.h"
#include "Core/Misc/moeFile.h"


namespace moe
{

	class FileResource : public IBaseResource
	{
		using FilePath = std::filesystem::path;

	public:


		FileResource(FilePath const& filePath, FileMode mode = FileMode::Binary);

		[[nodiscard]] std::string const&	Contents() const
		{
			return m_fileContents;
		}


		[[nodiscard]] bool	ReloadFile();


		[[nodiscard]] bool	IsEmpty() const
		{
			return m_fileContents.empty();
		}


	private:

		FilePath		m_filePath{};
		FileMode		m_mode{ FileMode::Binary };
		std::string		m_fileContents{};

	};




}
