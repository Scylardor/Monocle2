#pragma once

#include "Core/Delegates/Event.h"
#include "Graphics/Handle/DeviceHandles.h"
#include "Graphics/RenderQueue/RenderQueue.h"
#include "Math/Matrix.h"


namespace moe
{
	class RenderScene;

	class RenderObject
	{
	public:

		using MeshID = uint32_t;
		using MaterialID = uint32_t;
		using ID = std::uint32_t;

		static const uint32_t INVALID_ID = MaxValue<uint32_t>();


		RenderObject() = default;
		RenderObject(DeviceMeshHandle meshHandle, DeviceMaterialHandle matHandle) :
			 m_meshHandle(meshHandle), m_matHandle(matHandle)
		{}



		[[nodiscard]] uint32_t GetTransformID() const
		{
			return m_transformID;
		}


		[[nodiscard]] DeviceMeshHandle const& GetMeshHandle() const
		{
			return m_meshHandle;
		}

		[[nodiscard]] DeviceMaterialHandle GetMaterialHandle() const
		{
			return m_matHandle;
		}

		void	SetMaterialHandle(DeviceMaterialHandle handle)
		{
			m_matHandle = handle;
		}


		[[nodiscard]] DeviceDynamicResourceSetHandle	GetDynamicSetsHandle() const
		{
			return m_dynamicSets;
		}

		void	SetDynamicSetsHandle(DeviceDynamicResourceSetHandle ddrsh)
		{
			m_dynamicSets = ddrsh;
		}


		void	SetTransformID(uint32_t transfID)
		{
			m_transformID = transfID;
		}


		using OnObjectRemovedEvent = Event< void(RenderScene*, RenderObject&)>;
		OnObjectRemovedEvent&	RemovedEvent()
		{
			return m_removedEvent;
		}


	protected:

		uint32_t				m_transformID{ (uint32_t) -1 };
		DeviceMeshHandle		m_meshHandle{ 0 };
		DeviceMaterialHandle	m_matHandle{ 0 };
		DeviceDynamicResourceSetHandle	m_dynamicSets{ INVALID_ID };

		OnObjectRemovedEvent	m_removedEvent{};

	};


	/**
	 * \brief A handle that a Renderer can use to uniquely identify a graphics object.
	 * Because graphics APIs have vastly different ways to handle objects, using an opaque handle in our public interfaces
	 * is the easiest option we have (instead of pointers to virtual interfaces, for example).
	 * Each renderer is then free to reinterpret the handle to retrieve the actual data
	 */
	struct RenderObjectHandle
	{
		RenderObjectHandle() = default;
		RenderObjectHandle(RenderScene* parentScene, RenderObject::ID id) :
			m_scene(parentScene), m_handle(id)
		{}

		RenderScene*		m_scene{ nullptr };
		RenderObject::ID	m_handle{ 0 };

		RenderObject const* GetObject() const;
		RenderObject *		MutObject();

		[[nodiscard]] RenderObject::ID	GetID() const { return m_handle; }

		[[nodiscard]] bool	IsNull() const { return m_handle == 0; }
		[[nodiscard]] bool	IsNotNull() const { return !IsNull(); }

		bool operator==(const RenderObjectHandle& rhs) const
		{
			return m_handle == rhs.m_handle;
		}

		bool operator!=(const RenderObjectHandle& rhs) const
		{
			return !(*this == rhs);
		}

		static const RenderObjectHandle	Null;
	};


}
