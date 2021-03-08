#ifdef MOE_VULKAN

#include "VulkanRHI.h"

// Activating the dynamic dispatcher in order to easily use Vulkan EXTension functions.
// TODO: It may not be enough in DLL mode. See : https://github.com/KhronosGroup/Vulkan-Hpp#extensions--per-device-function-pointers
// "In some cases the storage for the DispatchLoaderDynamic should be embedded in a DLL.
// For those cases you need to define VULKAN_HPP_STORAGE_SHARED to tell Vulkan - Hpp that the storage resides in a DLL.
// When compiling the DLL with the storage it is also required to define VULKAN_HPP_STORAGE_SHARED_EXPORT to export the required symbols.
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

moe::VulkanRHI::~VulkanRHI()
{
}


bool moe::VulkanRHI::Initialize(VulkanInstance::CreationParams&& instanceParams)
{
	bool ok = m_instance.Initialize(std::move(instanceParams));
	MOE_ASSERT(ok);

	return ok;
}


bool moe::VulkanRHI::InitializeGraphicsDevice(vk::SurfaceKHR targetSurface)
{
	bool ok = m_devices.Initialize(m_instance.Instance());
	MOE_ASSERT(ok);

	auto theGraphicsDevice = m_devices.PickGraphicsDevice(targetSurface);
	MOE_ASSERT(theGraphicsDevice != nullptr);


	return true;
}


#endif // MOE_VULKAN
