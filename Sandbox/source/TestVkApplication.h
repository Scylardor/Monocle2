#pragma once
#ifdef MOE_VULKAN
#include "Application/GlfwApplication/Vulkan/VulkanGlfwApplication.h"

#include "Application/GlfwApplication/Vulkan/VulkanGlfwAppDescriptor.h"

#include "Graphics/Vulkan/RenderScene/RenderScene.h"

#include <chrono>

namespace moe
{


	class TestVkApplication : public moe::VulkanGlfwApplication
	{
	public:

		TestVkApplication(const moe::VulkanGlfwAppDescriptor& appDesc);
		~TestVkApplication() = default;

		void	Run();

		virtual void	Update();

	protected:

		using Clock = std::chrono::high_resolution_clock;
		using Timepoint = std::chrono::time_point<Clock>;
		using ms = std::chrono::duration<float, std::milli>;
		using sec = std::chrono::duration<float>;

		Clock		m_clock{};
		Timepoint	m_clockStart{};
		float		m_deltaTime{ 0.f };


		RenderScene	m_scene;

	};


	class BasicVkApp : public TestVkApplication
	{
	public:
		BasicVkApp(const moe::VulkanGlfwAppDescriptor& appDesc);


	};
}
#endif