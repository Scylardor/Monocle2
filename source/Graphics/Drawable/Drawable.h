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


		Mat4 const& MVP() const
		{ return m_mvp; }

		Mat4& MutateMVP()
		{
			return m_mvp;
		}


		void	BindTransform(VulkanPipeline const& pipeline, vk::CommandBuffer command) const;


		MeshID GetMeshID() const
		{
			return m_meshID;
		}

		MaterialID GetMaterialID() const
		{
			return m_materialID;
		}


	protected:

		Mat4			m_mvp{};

		MeshID			m_meshID{0};
		MaterialID		m_materialID{ 0 };

	};

}
