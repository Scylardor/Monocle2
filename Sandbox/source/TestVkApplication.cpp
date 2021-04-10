#include "TestVkApplication.h"


#include <chrono>

namespace moe
{



	TestVkApplication::TestVkApplication(const moe::VulkanGlfwAppDescriptor& appDesc) :
		VulkanGlfwApplication(appDesc)
	{
		m_clockStart = m_clock.now();
	}

	void TestVkApplication::Run()
	{
		std::chrono::high_resolution_clock timer;
		using ms = std::chrono::duration<float, std::milli>;
		using sec = std::chrono::duration<float>;




		while (WindowIsOpened())
		{
			PollInputEvents();

			Update();

			m_renderer.RenderFrame();
		}

		m_renderer.Shutdown();

	}


	void TestVkApplication::Update()
	{
		auto stop = m_clock.now();
		//auto dtms = std::chrono::duration_cast<ms>(stop - m_clockStart).count();
		m_deltaTime = std::chrono::duration_cast<sec>(stop - m_clockStart).count();
		m_clockStart = stop;
		//std::cout << "dt milli : " << dtms << std::endl;
		//std::cout << "dt sec : " << m_deltaTime << std::endl;

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
		auto meshID = m_renderer.EmplaceMesh(vertexSize, vertices.size(), vertices.data(), indices.size(), indices.data(), vk::IndexType::eUint16);

		m_scene.Emplace(meshID, 0);
	}

}
