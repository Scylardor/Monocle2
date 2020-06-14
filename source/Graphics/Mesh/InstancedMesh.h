// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Mesh.h"

#include "Monocle_Graphics_Export.h"

namespace moe
{
	class Camera;


	/**
	 * \brief A graphics API-agnostic way of representing a 3D mesh.
	 * It will hold all the vertices information the graphics API needs to draw this mesh.
	 * Optionally, you can reference an index buffer to make this an indexed mesh.
	 * Otherwise, it is considered to contain only pure geometry.
	 */
	class InstancedMesh : public Mesh
	{
	public:

		InstancedMesh(RenderWorld* world, const GraphicObjectData& data);

		void	SetInstancingBufferBinding(DeviceBufferHandle instancingBuffer, uint32_t instancesAmount)
		{
			m_instancingDataBuffer = instancingBuffer;
			m_instancesAmount = instancesAmount;
		}

		DeviceBufferHandle	GetInstancingBuffer() const { return m_instancingDataBuffer; }
		uint32_t			GetInstancesAmount() const { return m_instancesAmount; }

	private:
		DeviceBufferHandle	m_instancingDataBuffer; // Contains handle to instancing data buffer + amount of instances.
		uint32_t			m_instancesAmount{0};
	};

}