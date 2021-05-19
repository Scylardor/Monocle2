#ifdef MOE_VULKAN

#include "VulkanTextureFactory.h"

namespace moe
{
	VulkanTextureFactory::~VulkanTextureFactory()
	{
	}


	RegistryID VulkanTextureFactory::IncrementReference(RegistryID id)
	{
		return m_textures.IncrementReference(id);
	}

	void VulkanTextureFactory::DecrementReference(RegistryID id)
	{
		if (m_textures.GetReferenceCount(id) == 1 && false == m_textures.IsPersistent(id))
		{
			// Last ref : free the texture
			auto& vkTexture = m_textures.MutEntry(id);
			vkTexture.Free(*m_device);
		}

		m_textures.DecrementReference(id);
	}

	RegistryID VulkanTextureFactory::CreateTextureFromFile(std::string_view filename,
		VulkanTextureBuilder& textureBuilder)
	{
		auto ID = m_textures.EmplaceEntry(*m_device, filename, textureBuilder);
		return ID;
	}
}


#endif // MOE_VULKAN
