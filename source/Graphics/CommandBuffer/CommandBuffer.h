#pragma once

#include <variant>

#include "Core/Containers/Vector/Vector.h"
#include "GameFramework/Service/RenderService/RenderScene/RenderObject.h"
#include "Graphics/Color/Color.h"
#include "Graphics/RHI/FramebufferManager/FramebufferManager.h"
#include "Graphics/RHI/MaterialManager/MaterialManager.h"
#include "Graphics/RHI/SwapchainManager/SwapchainManager.h"

namespace moe
{

	struct CmdBeginRenderPass
	{
		CmdBeginRenderPass() = default;
		CmdBeginRenderPass(DeviceFramebufferHandle fbHandle, ColorRGBAf const& cc = ColorRGBAf::White(), float dc = 1, uint32_t sc = 0) :
			PassFramebuffer(fbHandle), ClearColor(cc), DepthClear(dc), StencilClear(sc)
		{}

		DeviceFramebufferHandle	PassFramebuffer{};
		ColorRGBAf				ClearColor{1.f};
		float					DepthClear{ 1.f };
		uint32_t				StencilClear{ 0 };
	};

	struct CmdEndRenderPass
	{
	};

	struct CmdPresentSwapchain
	{
		CmdPresentSwapchain(DeviceSwapchainHandle scHandle) :
			Handle(scHandle)
		{}

		DeviceSwapchainHandle	Handle{};
	};

	struct CmdBindMaterial
	{
		CmdBindMaterial(DeviceMaterialHandle matHandle) :
			Handle(matHandle)
		{}

		DeviceMaterialHandle	Handle {};
	};


	struct CmdDrawMesh
	{
		CmdDrawMesh(RenderMeshHandle meshHandle) :
			Handle(meshHandle)
		{}

		RenderMeshHandle	Handle;
	};

	using CommandBufferVariant = std::variant<
		CmdBeginRenderPass,
		CmdBindMaterial,
		CmdDrawMesh,
		CmdEndRenderPass,
		CmdPresentSwapchain
	>;


	template <class T, class U> struct is_one_of;

	template <class T, class... Ts>
	struct is_one_of<T, std::variant<Ts...>>
		: std::bool_constant<(std::is_same_v<T, Ts> || ...)>
	{ };

	class CommandBuffer
	{
	public:

		// Stolen from https://stackoverflow.com/questions/45892170/how-do-i-check-if-an-stdvariant-can-hold-a-certain-type
		template <class T> using Allowed = is_one_of<T, CommandBufferVariant>;

		CommandBuffer(uint32_t reserved = 0)
		{
			m_commands.Reserve(reserved);
		}


		template <typename TCmd, typename... Args>
		std::enable_if_t < Allowed<TCmd>::value >	Emplace(Args&&... args)
		{
			m_commands.EmplaceBack(std::in_place_type<TCmd>, std::forward<Args>(args)...);
		}

		auto Size() const
		{
			return m_commands.Size();
		}


		using CommandList = Vector<CommandBufferVariant>;

		// C++11 range-based for loops interface
		CommandList::Iterator		begin()
		{
			return m_commands.Begin();
		}
		CommandList::ConstIterator	begin() const
		{
			return m_commands.Begin();
		}

		CommandList::Iterator		end()
		{
			return m_commands.End();
		}
		CommandList::ConstIterator	end() const
		{
			return m_commands.End();
		}
		//--------------------------------



	private:
		CommandList	m_commands;

	};


}
