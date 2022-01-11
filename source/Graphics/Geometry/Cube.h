#pragma once
#include "Core/Containers/Array/Array.h"
#include "Graphics/Vertex/VertexFormats.h"


namespace moe
{
	Array<Vertex_PosNormalUV, 36> CreateCubePositionNormalTexture(float halfExtent, bool invertNormals);

	Array<Vertex_PosNormalUV, 24> CreateIndexedCubePositionNormalTexture(
		float halfExtent, bool invertNormals);

	Array<Vertex_PosNormalUV, 36> CreateCubePositionNormalTexture_ReversedNormals(float halfExtent);

	Array<Vertex_Position, 36> CreateCube_Positions(float halfExtent);

	Array<Vertex_PosUV, 36> CreateCube_PositionTexture(float halfExtent);

	// Suitable for GL_TRIANGLES, no index buffer. Counterclockwise vertex ordering
	Array<Vertex_PosColorUV, 36> CreateCube_PositionColorTexture(float halfExtent);
}
