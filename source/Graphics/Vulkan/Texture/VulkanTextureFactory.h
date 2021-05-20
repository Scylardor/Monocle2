#pragma once
#ifdef MOE_VULKAN

#include "Graphics/Vulkan/VulkanMacros.h"
#include "Core/Resource/ResourceFactory.h"
#include "VulkanTexture.h"

namespace moe
{
	class ITextureFactory : public IResourceFactory
	{
	public:

		virtual RegistryID	CreateTextureFromFile(std::string_view filename, VulkanTextureBuilder& textureBuilder) = 0;


		[[nodiscard]] virtual VulkanTexture&		MutateResource(RegistryID id) = 0;
		[[nodiscard]] virtual const VulkanTexture&	GetResource(RegistryID id) const = 0;
	};


	class VulkanTextureFactory : public ITextureFactory
	{
	public:
		VulkanTextureFactory() = default;

		VulkanTextureFactory(MyVkDevice& device) :
			m_device(&device)
		{}

		~VulkanTextureFactory();


		RegistryID	IncrementReference(RegistryID id) override;
		bool		DecrementReference(RegistryID id) override;


		RegistryID	CreateTextureFromFile(std::string_view filename, VulkanTextureBuilder& textureBuilder) override;


		[[nodiscard]] VulkanTexture& MutateResource(RegistryID id) override
		{
			return m_textures.MutEntry(id);
		}


		[[nodiscard]] const VulkanTexture& GetResource(RegistryID id) const override
		{
			return m_textures.GetEntry(id);
		}


	private:

		MyVkDevice* m_device{ nullptr };

		ObjectRegistry<VulkanTexture>	m_textures;
	};
}


#endif // MOE_VULKAN
