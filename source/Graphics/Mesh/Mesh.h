// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Misc/Types.h"


#include "Graphics/RenderWorld/GraphicsObject.h"

#include "Core/Containers/FreeList/Freelist.h"

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
	class Mesh : public AGraphicObject
	{
	public:

		Mesh(RenderWorld* world, const GraphicObjectData& data);


		FreelistID	GetID() const
		{
			return m_meshID;
		}

		void	SetID(FreelistID id)
		{
			m_meshID = id;
		}

		[[nodiscard]] ResourceSetHandle	GetPerObjectResourceSet() const { return m_perObjectResourceSetHandle; }

		Monocle_Graphics_API void	UpdateObjectMatrices(const Camera& currentCamera);

	private:

		FreelistID			m_meshID{0};

		DeviceBufferHandle	m_perObjectUniformBuffer; // TODO : temporary place !

		ResourceSetHandle	m_perObjectResourceSetHandle;
	};

}