// Monocle Game Engine source files - Alexandre Baron

#pragma once
#if defined(MOE_GLFW) && defined(MOE_VULKAN)

#include "Monocle_Application_Export.h"

#include "VulkanGlfwAppDescriptor.h"

#include "Application/GlfwApplication/BaseGlfwApplication.h"

#include "Graphics/Renderer/Vulkan/VulkanRenderer.h"
#include "Graphics/Vulkan/Surface/VulkanSurfaceProvider.h"


namespace moe
{
	/**
	 * \brief Application class that uses an Vulkan context.
	 * Can be parameterized about major/minor Vulkan versions and needed Vulkan profile type (core/compat).
	 */
	class VulkanGlfwApplication  : public BaseGlfwApplication, public IVulkanSurfaceProvider
	{
	public:
		Monocle_Application_API VulkanGlfwApplication(const VulkanGlfwAppDescriptor& appDesc);
		Monocle_Application_API virtual ~VulkanGlfwApplication() override;

		VulkanGlfwApplication(const VulkanGlfwApplication&) = delete;


		const IGraphicsRenderer&	GetRenderer() const override
		{
			return m_renderer;
		}
		IGraphicsRenderer&			MutRenderer() override
		{
			return m_renderer;
		}

		[[nodiscard]] Width_t	GetWindowWidth() const final override
		{
			return m_description.m_windowWidth;
		}

		[[nodiscard]] Height_t	GetWindowHeight() const final override
		{
			return m_description.m_windowHeight;
		}


	protected:

		void	OnWindowResized(int newWidth, int newHeight) override;

		/* Vulkan Surface Provider interface implementations */
		vk::SurfaceKHR		CreateSurface(VkInstance instance) override;
		SurfaceDimensions	GetSurfaceDimensions() override;
		void				RegisterSurfaceResizeCallback(SurfaceResizeCallback&& cb) override;

		// TODO : change that to use Vulkan instead.
		// This is super ugly and basically should never have happened in the first place :'(
		VulkanRenderer			m_renderer;

		VulkanGlfwAppDescriptor	m_description;

		uint32_t				m_requiredExtensionCount = 0;
		const char**			m_requiredExtensions = nullptr;

		std::vector<SurfaceResizeCallback>	m_framebufferResizedCallbacks;
	};
}

#endif // defined(MOE_GLFW) && defined(MOE_VULKAN)