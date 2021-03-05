
#ifdef MOE_VULKAN
#pragma once

#include "Graphics/Vulkan/Instance/VulkanInstance.h"



namespace moe
{



	class VulkanRHI
	{
	public:

		VulkanRHI() = default;
		~VulkanRHI();

		bool	Initialize(VulkanInstance::CreationParams&& instanceParams);

	private:

		VulkanInstance	m_instance;



	};



}

#endif