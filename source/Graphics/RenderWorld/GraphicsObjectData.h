// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Monocle_Graphics_Export.h"

#include "Core/Containers/FreeList/detail/FreeListObject.h"

#include "Graphics/DeviceBuffer/VertexBufferHandle.h"
#include "Graphics/DeviceBuffer/IndexBufferHandle.h"
#include "Graphics/DeviceBuffer/UniformBufferHandle.h"
#include "Graphics/DeviceBuffer/DeviceBufferHandle.h"


#include "Graphics/Mesh/MeshDataDescriptor.h"

namespace moe
{

	struct GraphicObjectData
	{
		GraphicObjectData() = default;

		GraphicObjectData(DeviceBufferHandle vtxHandle, DeviceBufferHandle idxHandle, DeviceBufferHandle ubHandle,
							const MeshDataDescriptor& vertexData, const MeshDataDescriptor& indexData) :
			m_objectDataHandle(vtxHandle), m_vtxDataHandle(vtxHandle), m_idxDataHandle(idxHandle), m_uniformDataHandle(ubHandle),
			m_numVertices((uint32_t) vertexData.m_bufferNumElems), m_numIndices((uint32_t) indexData.m_bufferNumElems),
			m_vertexDataSize((uint32_t) vertexData.m_bufferSizeBytes), m_indexDataSize((uint32_t) indexData.m_bufferSizeBytes)
		{}


		GraphicObjectData(DeviceBufferHandle ubHandle, uint32_t ubDataSize) :
			m_objectDataHandle(ubHandle), m_uniformDataHandle(ubHandle), m_uniformDataSize(ubDataSize)
		{}

		uint32_t	TotalGraphicSize() const { return m_vertexDataSize + m_indexDataSize + m_uniformDataSize; }

		DeviceBufferHandle	m_objectDataHandle;
		DeviceBufferHandle	m_vtxDataHandle{0};
		DeviceBufferHandle	m_idxDataHandle{0};
		DeviceBufferHandle	m_uniformDataHandle{0};

		FreelistID	m_objectID{0};
		uint32_t	m_numVertices{ 0 };
		uint32_t	m_numIndices{ 0 };
		uint32_t	m_vertexDataSize{ 0 };
		uint32_t	m_indexDataSize{ 0 };
		uint32_t	m_uniformDataSize{0};
	};

}