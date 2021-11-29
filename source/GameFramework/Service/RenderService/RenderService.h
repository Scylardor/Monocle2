// Monocle source files - Alexandre Baron

#pragma once

#include "GameFramework/Service/Service.h"
#include "Renderer/Renderer.h"
#include "Graphics/RHI/RenderHardwareInterface.h"
#include "GameFramework/Service/RenderService/RenderScene/RenderScene.h"

namespace moe
{

	class RenderService : public Service
	{
	public:
		using ServiceLocatorType = RenderService;


		RenderService(Engine& ownerEngine);


		template <typename... Args>
		[[nodiscard]] Renderer&	EmplaceRenderer(Args&&... args)
		{
			m_renderers.EmplaceBack(*this, std::forward<Args>(args)...);
			return m_renderers.Back();
		}



		template <typename... Args>
		[[nodiscard]] RenderScene& EmplaceScene(Renderer& renderer, Args&&... args)
		{
			m_scenes.EmplaceBack(renderer, std::forward<Args>(args)...);
			return m_scenes.Back();
		}


		template <typename TRHI, typename... Args>
		TRHI*	EmplaceRenderHardwareInterface(Args&&... args)
		{
			static_assert(std::is_base_of_v<RenderHardwareInterface, TRHI>, "Not a RHI type");

			m_RHI = std::make_unique<TRHI>(std::forward<Args>(args)...);
			return static_cast<TRHI*>(m_RHI.get());
		}


		RenderHardwareInterface*	MutRHI()
		{
			return m_RHI.get();
		}

	private:

		Vector<Renderer>							m_renderers{};
		std::unique_ptr<RenderHardwareInterface>	m_RHI{};

		Vector<RenderScene>							m_scenes{};

	};
}

