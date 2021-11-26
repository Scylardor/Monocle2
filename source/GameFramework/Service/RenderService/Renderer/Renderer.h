// Monocle source files - Alexandre Baron

#pragma once

#include "Core/Containers/ObjectPool/ObjectPool.h"
#include "Core/Containers/Vector/Vector.h"
#include "GameFramework/Service/RenderService/MaterialSystem/MaterialSystem.h"
#include "GameFramework/Service/RenderService/RenderPass/RenderPass.h"
#include "GameFramework/Service/RenderService/RenderPass/RenderPassType.h"

namespace moe
{
	class RenderScene;
	class RenderService;
	class GraphicsSurface;

	class Renderer
	{
	public:

		Renderer(RenderService& ownerService) :
			m_myService(&ownerService)
		{}

		Renderer(Renderer const& other) = delete;
		Renderer(Renderer&& other) = default;

		template <typename TRp, typename... Ts>
		TRp&	EmplaceRenderPass(Ts&&... args)
		{
			m_renderPasses.PushBack(std::make_unique<TRp>(*this, std::forward<Ts>(args)...));
			TRp& rp = static_cast<TRp&>(*m_renderPasses.Back().get());
			return rp;
		}


		void	ClearRenderPasses()
		{
			m_renderPasses.Clear();
		}


		void	MakeGeometryPass();


		void	AttachScene(RenderScene& scene)
		{
			m_attachedScene = &scene;
		}


		void	AttachSurface(GraphicsSurface& surface)
		{
			m_attachedSurface = &surface;
		}


	private:

		RenderScene*		m_attachedScene = nullptr;
		GraphicsSurface*	m_attachedSurface = nullptr;

		RenderService*		m_myService = nullptr;

		std::unordered_map<RenderPassType, PolymorphicObjectPool<IMaterialSystem>>	m_materialSystems{};

		Vector<std::unique_ptr<IRenderPass>>	m_renderPasses{};
	};
}
