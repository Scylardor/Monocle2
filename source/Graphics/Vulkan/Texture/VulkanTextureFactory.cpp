#ifdef MOE_VULKAN

#include "VulkanTextureFactory.h"

namespace moe
{

	RegistryID VulkanTextureFactory::IncrementReference(RegistryID /*id*/)
	{
		return {};
//		return m_textures.IncrementReference(id);
	}

	bool VulkanTextureFactory::DecrementReference(RegistryID /*id*/)
	{
		//const bool toBeFreed = m_textures.GetReferenceCount(id) == 1 && false == m_textures.IsPersistent(id);
		//if (toBeFreed)
		//{
		//	// Last ref : free the texture
		//	auto& vkTexture = m_textures.MutEntry(id);
		//	vkTexture.Free(*m_device);
		//}
		//
		//m_textures.DecrementReference(id);

		return false;
	}

	std::unique_ptr<TextureResource> VulkanTextureFactory::CreateTextureFromFile(std::string_view filename, VulkanTextureBuilder& textureBuilder)
	{
		return std::make_unique<VulkanTexture>(*m_device, filename, textureBuilder);
	}
}


#endif // MOE_VULKAN
