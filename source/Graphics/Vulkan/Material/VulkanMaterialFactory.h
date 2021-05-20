#pragma once

#ifdef MOE_VULKAN

#include "Graphics/Vulkan/Material/VulkanMaterial.h"
#include "Core/Resource/ResourceFactory.h"

namespace moe
{
	class IMaterialFactory : public IResourceFactory
	{
	public:

		[[nodiscard]] virtual VulkanMaterial&		MutateResource(RegistryID id) = 0;
		[[nodiscard]] virtual const VulkanMaterial&	GetResource(RegistryID id) const = 0;
	};


	class VulkanMaterialFactory : public IMaterialFactory
	{
	public:
		VulkanMaterialFactory() = default;

		VulkanMaterialFactory(MyVkDevice& device) :
			m_device(&device)
		{}

		RegistryID	IncrementReference(RegistryID id) override
		{
			return m_materials.IncrementReference(id);
		}

		bool		DecrementReference(RegistryID id) override
		{
			m_materials.DecrementReference(id);
			return false;
		}


		[[nodiscard]] VulkanMaterial& MutateResource(RegistryID id) override
		{
			return m_materials.MutEntry(id);
		}


		[[nodiscard]] const VulkanMaterial& GetResource(RegistryID id) const override
		{
			return m_materials.GetEntry(id);
		}


	private:

		MyVkDevice* m_device{nullptr};

		ObjectRegistry<VulkanMaterial>	m_materials;
	};



}

#endif