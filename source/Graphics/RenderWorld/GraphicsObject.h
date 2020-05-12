// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_STD_SUPPORT
#include <string>
#endif

#include "Monocle_Graphics_Export.h"

#include "Core/Containers/HashMap/HashMap.h"
#include "Core/Containers/FreeList/detail/FreeListObject.h"

#include "Graphics/Transform/Transform.h"

#include "Graphics/DeviceBuffer/VertexBufferHandle.h"
#include "Graphics/DeviceBuffer/IndexBufferHandle.h"

#include "Graphics/Material/Material.h"

#include "GraphicsObjectData.h"

namespace moe
{


	class AGraphicObject
	{
	public:

		AGraphicObject(RenderWorld* myWorld, const GraphicObjectData& data, Transform transf = Transform::Identity()) :
			m_world(myWorld), m_transform(std::move(transf)), m_graphicData(data)
		{}

		virtual ~AGraphicObject() = default;


		virtual	void SetTransform(const Transform& transf)
		{
			m_transform = transf;
			m_transformIsUpToDate = true;
		}

		virtual	const Transform& AddTransform(const Transform& transf)
		{
			m_transform *= transf;
			m_transformIsUpToDate = true;
			return m_transform;
		}


		const Transform&	GetTransform() const
		{
			return m_transform;
		}


		bool	IsTransformUpToDate() const { return m_transformIsUpToDate; }
		void	SetTransformUpToDate() { m_transformIsUpToDate = true; }



		[[nodiscard]] DeviceBufferHandle		GetVertexBufferHandle() const { return m_graphicData.m_vtxDataHandle; }
		[[nodiscard]] DeviceBufferHandle		GetIndexBufferHandle() const { return m_graphicData.m_idxDataHandle; }



		size_t	NumVertices() const
		{
			return m_graphicData.m_numVertices;
		}


		size_t	NumIndices() const
		{
			return m_graphicData.m_numIndices;
		}


		bool	IsIndexed() const
		{
			return m_graphicData.m_idxDataHandle.IsNotNull();
		}

		void	SetObjectID(FreelistID objectID)
		{
			m_graphicData.m_objectID = objectID;
		}


		RenderWorld*	GetRenderWorld() const { return m_world; }

	protected:

		RenderWorld*	m_world = nullptr;

		Transform		m_transform;
		bool			m_transformIsUpToDate{true};

		HashMap<std::string, uint32_t>	m_uniformBlockDataIndex;

		GraphicObjectData	m_graphicData;
	};

}