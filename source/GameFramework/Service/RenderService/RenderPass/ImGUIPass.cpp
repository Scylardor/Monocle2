#include "ImGUIPass.h"

#include "../Renderer/Renderer.h"

#include "GameFramework/Service/RenderService/RenderService.h"
#include "GameFramework/Service/RenderService/GraphicsSurface/GraphicsSurface.h"

#include "Graphics/RenderQueue/RenderQueue.h"

#include "GameFramework/Service/WindowService/WindowService.h"

#include <imgui.h>

#include "GameFramework/Service/WindowService/Window/GLFWWindow.h"

namespace moe
{

	ImGUIPass::ImGUIPass(Renderer& owner) :
		m_ownerRenderer(&owner)
	{
		// Setup Platform/Renderer bindings with the simulation window
		// (if it's a window)
		auto* boundWindow = dynamic_cast<IWindow*>(owner.MutSurface());
		if (MOE_ASSERT(boundWindow != nullptr))
			boundWindow->InitializeImmediateGUI();
	}


	RenderQueueKey ImGUIPass::Update(RenderQueue& drawQueue, RenderQueueKey key, int /*subpassIdx*/)
	{
		ImGui::NewFrame();

		// render your GUI
		ImGui::Begin("Demo window");
		ImGui::Button("Hello!");
		ImGui::End();

		// Render into dear imgui's draw data
		ImGui::Render();

		key.ViewLayer = (int) ViewportLayer::GUI;

		// TODO: Commenting it for now, but prepare to uncomment it on Vulkan when we will need to make ImGui a subpass.
		// ImGui is not supposed to clear the framebuffer in any case.
		// key = drawQueue.EmplaceCommand<CmdBeginRenderPass>(key, m_framebuffer, ColorRGBAf::Red());

		// Make sure to reset the viewport to draw on the whole surface
		// Otherwise we could be still using a viewport from a previous pass!
		auto [surfaceWidth, surfaceHeight] = m_ownerRenderer->GetSurface()->GetDimensions();
		Rect2Du fullscreenViewport{ 0, 0, (uint32_t) surfaceWidth, (uint32_t) surfaceHeight };

		drawQueue.EmplaceCommand<CmdSetViewportScissor>(key, fullscreenViewport, fullscreenViewport);

		drawQueue.EmplaceCommand<CmdRenderImGuiDrawData>(key, ImGui::GetDrawData());

		// Commenting it as ImGui cleans after itself by default (hopefully)
		//key = drawQueue.EmplaceCommand<CmdEndRenderPass>(key);

		return key;
	}
}
