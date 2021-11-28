#include "FileResource.h"


moe::FileResource::FileResource(FilePath const& filePath, FileMode mode) :
	m_filePath(filePath), m_mode(mode)
{
	bool ok = ReloadFile();
	if (!ok)
	{
		MOE_ERROR(ChanDefault, "Unable to create FileResource.");
	}
}


bool moe::FileResource::ReloadFile()
{
	auto optContents = moe::ReadFile(m_filePath.c_str(), m_mode);

	if (optContents)
	{
		m_fileContents = optContents.value();
		return true;
	}
	else
	{
		MOE_ERROR(ChanDefault, "Could not read file %s!", m_filePath.c_str());
		return false;
	}
}
