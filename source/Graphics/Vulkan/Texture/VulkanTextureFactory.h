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


		~VulkanTextureFactory() override = default;


		RegistryID	IncrementReference(RegistryID id) override;
		bool		DecrementReference(RegistryID id) override;


		RegistryID	CreateTextureFromFile(std::string_view filename, VulkanTextureBuilder& textureBuilder) override;


		[[nodiscard]] VulkanTexture& MutateResource(RegistryID ) override
		{
			return dummy;
		}


		[[nodiscard]] const VulkanTexture& GetResource(RegistryID ) const override
		{
			return dummy;
		}


		[[nodiscard]] std::unique_ptr<VulkanTexture> operator()(std::string_view filename, VulkanTextureBuilder& textureBuilder) const
		{
			return std::make_unique<VulkanTexture>(*m_device, filename, textureBuilder);
		}

	private:

		MyVkDevice* m_device{ nullptr };

		inline static VulkanTexture dummy; // TODO : remove, waiting for TextureFactory to be refactored.
	};
}


#endif // MOE_VULKAN
