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



}

#endif