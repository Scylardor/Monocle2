#include "TestVkApplication.h"


#include <chrono>

namespace moe
{



	TestVkApplication::TestVkApplication(const moe::VulkanGlfwAppDescriptor& appDesc) :
		VulkanGlfwApplication(appDesc)
	{

	}

	void TestVkApplication::Run()
	{
		std::chrono::high_resolution_clock timer;
		using ms = std::chrono::duration<float, std::milli>;
		using sec = std::chrono::duration<float>;


		auto start = timer.now();


		while (WindowIsOpened())
		{
			//auto stop = timer.now();
			//auto deltaTime = std::chrono::duration_cast<ms>(stop - start).count();
			//auto deltaTimeSecs = std::chrono::duration_cast<sec>(stop - start).count();
			//std::cout << "dt milli : " << deltaTime << std::endl;
			//std::cout << "dt sec : " << deltaTimeSecs << std::endl;
			//start = stop;

			PollInputEvents();

			Update();

			m_renderer.RenderFrame();
		}

		m_renderer.Shutdown();

	}


	BasicVkApp::BasicVkApp(const moe::VulkanGlfwAppDescriptor& appDesc) :
		TestVkApplication(appDesc)
	{

		const std::vector<moe::VertexData> vertices =
		{
			{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
			{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
		};

		const std::vector<uint16_t> indices =
		{
			0, 1, 2, 2, 3, 0
		};

		VkDeviceSize vertexSize = sizeof(vertices[0]) * vertices.size();
		m_renderer.EmplaceMesh(vertexSize, vertices.size(), vertices.data(), indices.size(), indices.data(), vk::IndexType::eUint16);

	}

}
