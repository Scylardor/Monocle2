#pragma once


#include "Math/Matrix.h"


namespace vk
{
	class CommandBuffer;
}

namespace moe
{
	class VulkanPipeline;

	class Drawable
	{
	public:

		using MeshID = uint32_t;
		using MaterialID = uint32_t;


		Drawable() = default;
		Drawable(MeshID meshID, MaterialID materialID, Mat4 transform = Mat4::Identity()) :
			m_mvp(transform), m_meshID(meshID), m_materialID(materialID)
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
			return m_model;
		}

		void	SetModelMatrix(const Mat4& modelMat)
		{
			m_model = modelMat;
			SetNeedsMVPUpdate(true);
		}


		void	SetNeedsMVPUpdate(bool needsMVPUpdate)
		{
			m_updatedSinceLastRender = needsMVPUpdate;
		}


		void	BindPerObjectResources(VulkanPipeline const& pipeline, vk::CommandBuffer command) const;


		[[nodiscard]] MeshID GetMeshID() const
		{
			return m_meshID;
		}

		[[nodiscard]] MaterialID GetMaterialID() const
		{
			return m_materialID;
		}

		[[nodiscard]] bool WasUpdatedSinceLastRender() const
		{
			return m_updatedSinceLastRender;
		}

	protected:

		Mat4			m_model{ Mat4::Identity() };
		Mat4			m_mvp{Mat4::Identity()};

		MeshID			m_meshID{0};
		MaterialID		m_materialID{ 0 };

		bool			m_updatedSinceLastRender{ true };
	};

}
