// Monocle source files - Alexandre Baron

#pragma once

#include "Core/Containers/ObjectPool/ObjectPool.h"
#include "Core/Containers/Vector/Vector.h"
#include "GameFramework/Service/RenderService/MaterialSystem/MaterialSystem.h"
#include "GameFramework/Service/RenderService/RenderPass/RenderPass.h"
#include "GameFramework/Service/RenderService/RenderPass/RenderPassType.h"

namespace moe
{
	class RenderHardwareInterface;
	class RenderScene;
	class RenderService;
	class IGraphicsSurface;

	class Renderer
	{
	public:

		Renderer(RenderService& ownerService, RenderHardwareInterface* RHI) :
			m_myService(&ownerService), m_RHI(RHI)
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


		void	AttachSurface(IGraphicsSurface& surface)
		{
			m_attachedSurface = &surface;
		}

		IGraphicsSurface*	MutSurface()
		{
			return m_attachedSurface;
		}


		RenderHardwareInterface*	MutRHI()
		{
			return m_RHI;
		}

	private:

		RenderScene*		m_attachedScene = nullptr;
		IGraphicsSurface*	m_attachedSurface = nullptr;

		RenderService*		m_myService = nullptr;

		RenderHardwareInterface* m_RHI{ nullptr };

		std::unordered_map<RenderPassType, PolymorphicObjectPool<IMaterialSystem>>	m_materialSystems{};

		Vector<std::unique_ptr<IRenderPass>>	m_renderPasses{};
	};
}

