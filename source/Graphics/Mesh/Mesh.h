// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Misc/Types.h"

#include "Graphics/DeviceBuffer/VertexBufferHandle.h"
#include "Graphics/DeviceBuffer/IndexBufferHandle.h"
#include "Graphics/DeviceBuffer/UniformBufferHandle.h"

#include "Graphics/RenderWorld/GraphicsObject.h"

#include "Core/Containers/FreeList/Freelist.h"

namespace moe
{


	/**
	 * \brief A graphics API-agnostic way of representing a 3D mesh.
	 * It will hold all the vertices information the graphics API needs to draw this mesh.
	 * Optionally, you can reference an index buffer to make this an indexed mesh.
	 * Otherwise, it is considered to contain only pure geometry.
	 */
	class Mesh : public AGraphicObject
	{
	public:

		Mesh(RenderWorld* world, const GraphicObjectData& data) :
			AGraphicObject(world, data)
		{}



		FreelistID	GetID() const
		{
			return m_meshID;
		}

		void	SetID(FreelistID id)
		{
			m_meshID = id;
		}

	private:

		FreelistID			m_meshID{0};

	};

}