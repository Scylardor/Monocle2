#pragma once

#include "Graphics/RHI/BufferManager/BufferManager.h"
#include "Graphics/RHI/MaterialManager/MaterialManager.h"
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

		static const auto INVALID_ID = -1;


		RenderObject() = default;
		RenderObject(DeviceMeshHandle meshHandle, DeviceMaterialHandle matHandle, Mat4 transform = Mat4::Identity()) :
			m_transform(transform), m_meshHandle(std::move(meshHandle)), m_matHandle(matHandle)
		{}


		[[nodiscard]] Mat4 const& GetMVP() const
		{
			return m_mvp;
		}

		void	SetMVP(const Mat4& mvp)
		{
			m_mvp = mvp;
			m_updatedSinceLastRender = false;
		}

		[[nodiscard]] Mat4 const& GetModelMatrix() const
		{
			return m_transform;
		}

		void	SetModelMatrix(const Mat4& modelMat)
		{
			m_transform = modelMat;
			SetNeedsMVPUpdate(true);
		}


		void	SetNeedsMVPUpdate(bool needsMVPUpdate)
		{
			m_updatedSinceLastRender = needsMVPUpdate;
		}


		[[nodiscard]] DeviceMeshHandle const& GetMeshHandle() const
		{
			return m_meshHandle;
		}

		[[nodiscard]] DeviceMaterialHandle GetMaterialHandle() const
		{
			return m_matHandle;
		}

		[[nodiscard]] bool WasUpdatedSinceLastRender() const
		{
			return m_updatedSinceLastRender;
		}

	protected:

		Mat4				m_transform{ Mat4::Identity() };
		Mat4				m_mvp{ Mat4::Identity() };

		DeviceMeshHandle		m_meshHandle{ 0 };
		DeviceMaterialHandle	m_matHandle{ 0 };

		bool			m_updatedSinceLastRender{ true };
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
