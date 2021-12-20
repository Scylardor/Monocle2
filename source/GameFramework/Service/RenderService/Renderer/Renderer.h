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

		using RenderPassList = Vector < std::unique_ptr<IRenderPass>>;

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

		RenderScene*	MutAttachedScene()
		{
			return m_attachedScene;
		}

		RenderScene const*	GetAttachedScene() const
		{
			return m_attachedScene;
		}


		void AttachSurface(IGraphicsSurface& surface);

		IGraphicsSurface*	MutSurface()
		{
			return m_attachedSurface;
		}


		RenderHardwareInterface*	MutRHI()
		{
			return m_RHI;
		}

		RenderService*			MutRenderService()
		{
			return m_myService;
		}

		RenderService const*	GetRenderService() const
		{
			return m_myService;
		}

		bool	Ready() const
		{
			return m_readyToRender;
		}


		// C++11 range-based for loops interface
		RenderPassList::Iterator		begin()
		{ return m_renderPasses.Begin(); }
		RenderPassList::ConstIterator	begin() const
		{ return m_renderPasses.Begin(); }

		RenderPassList::Iterator		end()
		{ return m_renderPasses.End(); }
		RenderPassList::ConstIterator	end() const
		{ return m_renderPasses.End(); }
		//--------------------------------




	private:

		void	OnAttachedSurfaceLost()
		{
			m_readyToRender = false;
		}

		RenderScene*		m_attachedScene = nullptr;
		IGraphicsSurface*	m_attachedSurface = nullptr;

		RenderService*		m_myService = nullptr;

		RenderHardwareInterface* m_RHI{ nullptr };

		//std::unordered_map<RenderPassType, PolymorphicObjectPool<IMaterialSystem>>	m_materialSystems{};

		Vector<std::unique_ptr<IRenderPass>>	m_renderPasses{};

		bool	m_readyToRender = true;
	};
}

