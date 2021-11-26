// Monocle source files - Alexandre Baron

#pragma once

#include "GameFramework/Service/Service.h"
#include "Renderer/Renderer.h"

namespace moe
{

	class RenderService : public Service
	{
	public:
		using ServiceLocatorType = RenderService;


		RenderService(Engine& ownerEngine);


		template <typename... Args>
		Renderer&	EmplaceRenderer(Args&&... args)
		{
			m_renderers.EmplaceBack(*this, std::forward<Args>(args)...);
			return m_renderers.Back();
		}


	private:

		Vector<Renderer>	m_renderers{};

	};
}

