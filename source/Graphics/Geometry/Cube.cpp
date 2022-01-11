#include "Cube.h"
#include "Graphics/Color/Color.h"

namespace moe
{
	Array<Vertex_PosNormalUV, 36> CreateCubePositionNormalTexture(float halfExtent, bool invertNormals)
	{
		const float invNormCoef = (invertNormals ? -1.f : 1.f);
		return {
			{Vec3{-halfExtent, -halfExtent, -halfExtent}, {0.0f, 0.0f, -1.0f * invNormCoef}, {0.0f, 0.0f}},
			{Vec3{halfExtent, halfExtent, -halfExtent}, {0.0f, 0.0f, -1.0f * invNormCoef}, {1.0f, 1.0f}},
			{Vec3{halfExtent, -halfExtent, -halfExtent}, {0.0f, 0.0f, -1.0f * invNormCoef}, {1.0f, 0.0f}},
			{Vec3{halfExtent, halfExtent, -halfExtent}, {0.0f, 0.0f, -1.0f * invNormCoef}, {1.0f, 1.0f}},
			{Vec3{-halfExtent, -halfExtent, -halfExtent}, {0.0f, 0.0f, -1.0f * invNormCoef}, {0.0f, 0.0f}},
			{Vec3{-halfExtent, halfExtent, -halfExtent}, {0.0f, 0.0f, -1.0f * invNormCoef}, {0.0f, 1.0f}},

			{Vec3{-halfExtent, -halfExtent, halfExtent}, {0.0f, 0.0f, 1.0f * invNormCoef}, {0.0f, 0.0f}},
			{Vec3{halfExtent, -halfExtent, halfExtent}, {0.0f, 0.0f, 1.0f * invNormCoef}, {1.0f, 0.0f}},
			{Vec3{halfExtent, halfExtent, halfExtent}, {0.0f, 0.0f, 1.0f * invNormCoef}, {1.0f, 1.0f}},
			{Vec3{halfExtent, halfExtent, halfExtent}, {0.0f, 0.0f, 1.0f * invNormCoef}, {1.0f, 1.0f}},
			{Vec3{-halfExtent, halfExtent, halfExtent}, {0.0f, 0.0f, 1.0f * invNormCoef}, {0.0f, 1.0f}},
			{Vec3{-halfExtent, -halfExtent, halfExtent}, {0.0f, 0.0f, 1.0f * invNormCoef}, {0.0f, 0.0f}},

			{Vec3{-halfExtent, halfExtent, halfExtent}, {-1.0f * invNormCoef, 0.0f, 0.0}, {1.0f, 1.0f}},
			{Vec3{-halfExtent, halfExtent, -halfExtent}, {-1.0f * invNormCoef, 0.0f, 0.0}, {0.0f, 1.0f}},
			{Vec3{-halfExtent, -halfExtent, -halfExtent}, {-1.0f * invNormCoef, 0.0f, 0.0}, {0.0f, 0.0f}},
			{Vec3{-halfExtent, -halfExtent, -halfExtent}, {-1.0f * invNormCoef, 0.0f, 0.0}, {0.0f, 0.0f}},
			{Vec3{-halfExtent, -halfExtent, halfExtent}, {-1.0f * invNormCoef, 0.0f, 0.0}, {1.0f, 0.0f}},
			{Vec3{-halfExtent, halfExtent, halfExtent}, {-1.0f * invNormCoef, 0.0f, 0.0}, {1.0f, 1.0f}},

			{Vec3{halfExtent, halfExtent, halfExtent}, {1.0f * invNormCoef, 0.0f, 0.0f}, {0.0f, 1.0f}},
			{Vec3{halfExtent, -halfExtent, -halfExtent}, {1.0f * invNormCoef, 0.0f, 0.0f}, {1.0f, 0.0f}},
			{Vec3{halfExtent, halfExtent, -halfExtent}, {1.0f * invNormCoef, 0.0f, 0.0f}, {1.0f, 1.0f}},
			{Vec3{halfExtent, -halfExtent, -halfExtent}, {1.0f * invNormCoef, 0.0f, 0.0f}, {1.0f, 0.0f}},
			{Vec3{halfExtent, halfExtent, halfExtent}, {1.0f * invNormCoef, 0.0f, 0.0f}, {0.0f, 1.0f}},
			{Vec3{halfExtent, -halfExtent, halfExtent}, {1.0f * invNormCoef, 0.0f, 0.0f}, {0.0f, 0.0f}},

			{Vec3{-halfExtent, -halfExtent, -halfExtent}, {0.0f, -1.0f * invNormCoef, 0.0f}, {0.0f, 1.0f}},
			{Vec3{halfExtent, -halfExtent, -halfExtent}, {0.0f, -1.0f * invNormCoef, 0.0f}, {1.0f, 1.0f}},
			{Vec3{halfExtent, -halfExtent, halfExtent}, {0.0f, -1.0f * invNormCoef, 0.0f}, {1.0f, 0.0f}},
			{Vec3{halfExtent, -halfExtent, halfExtent}, {0.0f, -1.0f * invNormCoef, 0.0f}, {1.0f, 0.0f}},
			{Vec3{-halfExtent, -halfExtent, halfExtent}, {0.0f, -1.0f * invNormCoef, 0.0f}, {0.0f, 0.0f}},
			{Vec3{-halfExtent, -halfExtent, -halfExtent}, {0.0f, -1.0f * invNormCoef, 0.0f}, {0.0f, 1.0f}},

			{Vec3{-halfExtent, halfExtent, -halfExtent}, {0.0f, 1.0f * invNormCoef, 0.0f}, {0.0f, 0.0f}},
			{Vec3{halfExtent, halfExtent, halfExtent}, {0.0f, 1.0f * invNormCoef, 0.0f}, {1.0f, 1.0f}},
			{Vec3{halfExtent, halfExtent, -halfExtent}, {0.0f, 1.0f * invNormCoef, 0.0f}, {0.0f, 1.0f}},
			{Vec3{halfExtent, halfExtent, halfExtent}, {0.0f, 1.0f * invNormCoef, 0.0f}, {1.0f, 1.0f}},
			{Vec3{-halfExtent, halfExtent, -halfExtent}, {0.0f, 1.0f * invNormCoef, 0.0f}, {0.0f, 0.0f}},
			{Vec3{-halfExtent, halfExtent, halfExtent}, {0.0f, 1.0f * invNormCoef, 0.0f}, {1.0f, 0.0f}}
		};
	}

	Array<Vertex_PosNormalUV, 24> CreateIndexedCubePositionNormalTexture(float halfExtent, bool invertNormals)
	{
		const float invNormCoef = (invertNormals ? -1.f : 1.f);
		return {
			// front
			{Vec3{-halfExtent, -halfExtent, halfExtent}, {0.0f, 0.0f, 1.0f * invNormCoef}, {0.0f, 0.0f}},
			{Vec3{halfExtent, -halfExtent, halfExtent}, {0.0f, 0.0f, 1.0f * invNormCoef}, {1.0f, 0.0f}},
			{Vec3{halfExtent, halfExtent, halfExtent}, {0.0f, 0.f, 1.0f * invNormCoef}, {1.0f, 1.0f}},
			{Vec3{-halfExtent, halfExtent, halfExtent}, {0.0f, 0.f, 1.0f * invNormCoef}, {0.0f, 1.0f}},

			// top
			{Vec3{-halfExtent, halfExtent, halfExtent}, {0.0f, 1.0f * invNormCoef, 0.0f}, {0.0f, 0.0f}},
			{Vec3{halfExtent, halfExtent, halfExtent}, {0.0f, 1.0f * invNormCoef, 0.0f}, {1.0f, 0.0f}},
			{Vec3{halfExtent, halfExtent, -halfExtent}, {0.0f, 1.0f * invNormCoef, 0.0f}, {1.0f, 1.0f}},
			{Vec3{-halfExtent, halfExtent, -halfExtent}, {0.0f, 1.0f * invNormCoef, 0.0f}, {0.0f, 1.0f}},

			// back
			{Vec3{halfExtent, -halfExtent, -halfExtent}, {0.0f, 0.f, -1.0f * invNormCoef}, {0.0f, 0.0f}},
			{Vec3{-halfExtent, -halfExtent, -halfExtent}, {0.0f, 0.f, -1.0f * invNormCoef}, {1.0f, 0.0f}},
			{Vec3{-halfExtent, halfExtent, -halfExtent}, {0.0f, 0.f, -1.0f * invNormCoef}, {1.0f, 1.0f}},
			{Vec3{halfExtent, halfExtent, -halfExtent}, {0.0f, 0.f, -1.0f * invNormCoef}, {0.0f, 1.0f}},

			// bottom
			{Vec3{-halfExtent, -halfExtent, -halfExtent}, {0.0f, -1.0f * invNormCoef, 0.0f}, {0.0f, 0.0f}},
			{Vec3{halfExtent, -halfExtent, -halfExtent}, {0.0f, -1.0f * invNormCoef, 0.0f}, {1.0f, 0.0f}},
			{Vec3{halfExtent, -halfExtent, halfExtent}, {0.0f, -1.0f * invNormCoef, 0.0f}, {1.0f, 1.0f}},
			{Vec3{-halfExtent, -halfExtent, halfExtent}, {0.0f, -1.0f * invNormCoef, 0.0f}, {0.0f, 1.0f}},

			// left
			{Vec3{-halfExtent, -halfExtent, -halfExtent}, {-1.0f * invNormCoef, 0.f, 0.0f}, {0.0f, 0.0f}},
			{Vec3{-halfExtent, -halfExtent, halfExtent}, {-1.0f * invNormCoef, 0.0f, 0.f}, {1.0f, 0.0f}},
			{Vec3{-halfExtent, halfExtent, halfExtent}, {-1.0f * invNormCoef, 0.f, 0.0f}, {1.0f, 1.0f}},
			{Vec3{-halfExtent, halfExtent, -halfExtent}, {-1.0f * invNormCoef, 0.f, 0.0f}, {0.0f, 1.0f}},

			// right
			{Vec3{halfExtent, -halfExtent, halfExtent}, {1.0f * invNormCoef, 0.0f, 0.0f}, {0.0f, 0.0f}},
			{Vec3{halfExtent, -halfExtent, -halfExtent}, {1.0f * invNormCoef, 0.0f, 0.0f}, {1.0f, 0.0f}},
			{Vec3{halfExtent, halfExtent, -halfExtent}, {1.0f * invNormCoef, 0.0f, 0.0f}, {1.0f, 1.0f}},
			{Vec3{halfExtent, halfExtent, halfExtent}, {1.0f * invNormCoef, 0.0f, 0.0f}, {0.0f, 1.0f}}
		};
	}

	Array<Vertex_PosNormalUV, 36> CreateCubePositionNormalTexture_ReversedNormals(float halfExtent)
	{
		return {
			{Vec3{-halfExtent, -halfExtent, -halfExtent}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
			{Vec3{halfExtent, halfExtent, -halfExtent}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
			{Vec3{halfExtent, -halfExtent, -halfExtent}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
			{Vec3{halfExtent, halfExtent, -halfExtent}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
			{Vec3{-halfExtent, -halfExtent, -halfExtent}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
			{Vec3{-halfExtent, halfExtent, -halfExtent}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},

			{Vec3{-halfExtent, -halfExtent, halfExtent}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},
			{Vec3{halfExtent, -halfExtent, halfExtent}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},
			{Vec3{halfExtent, halfExtent, halfExtent}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
			{Vec3{halfExtent, halfExtent, halfExtent}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
			{Vec3{-halfExtent, halfExtent, halfExtent}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
			{Vec3{-halfExtent, -halfExtent, halfExtent}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},

			{Vec3{-halfExtent, halfExtent, halfExtent}, {1.0f, 0.0f, 0.0}, {1.0f, 1.0f}},
			{Vec3{-halfExtent, halfExtent, -halfExtent}, {1.0f, 0.0f, 0.0}, {0.0f, 1.0f}},
			{Vec3{-halfExtent, -halfExtent, -halfExtent}, {1.0f, 0.0f, 0.0}, {0.0f, 0.0f}},
			{Vec3{-halfExtent, -halfExtent, -halfExtent}, {1.0f, 0.0f, 0.0}, {0.0f, 0.0f}},
			{Vec3{-halfExtent, -halfExtent, halfExtent}, {1.0f, 0.0f, 0.0}, {1.0f, 0.0f}},
			{Vec3{-halfExtent, halfExtent, halfExtent}, {1.0f, 0.0f, 0.0}, {1.0f, 1.0f}},

			{Vec3{halfExtent, halfExtent, halfExtent}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
			{Vec3{halfExtent, -halfExtent, -halfExtent}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
			{Vec3{halfExtent, halfExtent, -halfExtent}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
			{Vec3{halfExtent, -halfExtent, -halfExtent}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
			{Vec3{halfExtent, halfExtent, halfExtent}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
			{Vec3{halfExtent, -halfExtent, halfExtent}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},

			{Vec3{-halfExtent, -halfExtent, -halfExtent}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
			{Vec3{halfExtent, -halfExtent, -halfExtent}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
			{Vec3{halfExtent, -halfExtent, halfExtent}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
			{Vec3{halfExtent, -halfExtent, halfExtent}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
			{Vec3{-halfExtent, -halfExtent, halfExtent}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
			{Vec3{-halfExtent, -halfExtent, -halfExtent}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},

			{Vec3{-halfExtent, halfExtent, -halfExtent}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
			{Vec3{halfExtent, halfExtent, halfExtent}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
			{Vec3{halfExtent, halfExtent, -halfExtent}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
			{Vec3{halfExtent, halfExtent, halfExtent}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
			{Vec3{-halfExtent, halfExtent, -halfExtent}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
			{Vec3{-halfExtent, halfExtent, halfExtent}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}}
		};
	}

	Array<Vertex_Position, 36> CreateCube_Positions(float halfExtent)
	{
		return {
			{Vec3{-halfExtent, -halfExtent, -halfExtent}},
			{Vec3{halfExtent, halfExtent, -halfExtent}},
			{Vec3{halfExtent, -halfExtent, -halfExtent}},
			{Vec3{halfExtent, halfExtent, -halfExtent}},
			{Vec3{-halfExtent, -halfExtent, -halfExtent}},
			{Vec3{-halfExtent, halfExtent, -halfExtent}},

			{Vec3{-halfExtent, -halfExtent, halfExtent}},
			{Vec3{halfExtent, -halfExtent, halfExtent}},
			{Vec3{halfExtent, halfExtent, halfExtent}},
			{Vec3{halfExtent, halfExtent, halfExtent}},
			{Vec3{-halfExtent, halfExtent, halfExtent}},
			{Vec3{-halfExtent, -halfExtent, halfExtent}},

			{Vec3{-halfExtent, halfExtent, halfExtent}},
			{Vec3{-halfExtent, halfExtent, -halfExtent}},
			{Vec3{-halfExtent, -halfExtent, -halfExtent}},
			{Vec3{-halfExtent, -halfExtent, -halfExtent}},
			{Vec3{-halfExtent, -halfExtent, halfExtent}},
			{Vec3{-halfExtent, halfExtent, halfExtent}},

			{Vec3{halfExtent, halfExtent, halfExtent}},
			{Vec3{halfExtent, -halfExtent, -halfExtent}},
			{Vec3{halfExtent, halfExtent, -halfExtent}},
			{Vec3{halfExtent, -halfExtent, -halfExtent}},
			{Vec3{halfExtent, halfExtent, halfExtent}},
			{Vec3{halfExtent, -halfExtent, halfExtent}},

			{Vec3{-halfExtent, -halfExtent, -halfExtent}},
			{Vec3{halfExtent, -halfExtent, -halfExtent}},
			{Vec3{halfExtent, -halfExtent, halfExtent}},
			{Vec3{halfExtent, -halfExtent, halfExtent}},
			{Vec3{-halfExtent, -halfExtent, halfExtent}},
			{Vec3{-halfExtent, -halfExtent, -halfExtent}},

			{Vec3{-halfExtent, halfExtent, -halfExtent}},
			{Vec3{halfExtent, halfExtent, halfExtent}},
			{Vec3{halfExtent, halfExtent, -halfExtent}},
			{Vec3{halfExtent, halfExtent, halfExtent}},
			{Vec3{-halfExtent, halfExtent, -halfExtent}},
			{Vec3{-halfExtent, halfExtent, halfExtent}}
		};
	}


	Array<Vertex_PosUV, 36> CreateCube_PositionTexture(float halfExtent)
	{
		return {
			// back
			{Vec3{-halfExtent, -halfExtent, -halfExtent}, Vec2{1.0f, 0.0f}},
			{Vec3{-halfExtent, halfExtent, -halfExtent}, Vec2{1.0f, 1.0f}},
			{Vec3{halfExtent, halfExtent, -halfExtent}, Vec2{0.0f, 1.0f}},
			{Vec3{halfExtent, -halfExtent, -halfExtent}, Vec2{0.0f, 0.0f}},
			{Vec3{-halfExtent, -halfExtent, -halfExtent}, Vec2{1.0f, 0.0f}},
			{Vec3{halfExtent, halfExtent, -halfExtent}, Vec2{0.0f, 1.0f}},

			// left
			{Vec3{-halfExtent, -halfExtent, halfExtent}, Vec2{1.0f, 0.0f}},
			{Vec3{-halfExtent, halfExtent, halfExtent}, Vec2{1.0f, 1.0f}},
			{Vec3{-halfExtent, halfExtent, -halfExtent}, Vec2{0.0f, 1.0f}},
			{Vec3{-halfExtent, -halfExtent, -halfExtent}, Vec2{0.0f, 0.0f}},
			{Vec3{-halfExtent, -halfExtent, halfExtent}, Vec2{1.0f, 0.0f}},
			{Vec3{-halfExtent, halfExtent, -halfExtent}, Vec2{0.0f, 1.0f}},

			// front
			{Vec3{halfExtent, -halfExtent, halfExtent}, Vec2{1.0f, 0.0f}},
			{Vec3{halfExtent, halfExtent, halfExtent}, Vec2{1.0f, 1.0f}},
			{Vec3{-halfExtent, halfExtent, halfExtent}, Vec2{0.0f, 1.0f}},
			{Vec3{-halfExtent, -halfExtent, halfExtent}, Vec2{0.0f, 0.0f}},
			{Vec3{halfExtent, -halfExtent, halfExtent}, Vec2{1.0f, 0.0f}},
			{Vec3{-halfExtent, halfExtent, halfExtent}, Vec2{0.0f, 1.0f}},

			// right
			{Vec3{halfExtent, -halfExtent, -halfExtent}, Vec2{1.0f, 0.0f}},
			{Vec3{halfExtent, halfExtent, -halfExtent}, Vec2{1.0f, 1.0f}},
			{Vec3{halfExtent, halfExtent, halfExtent}, Vec2{0.0f, 1.0f}},
			{Vec3{halfExtent, -halfExtent, halfExtent}, Vec2{0.0f, 0.0f}},
			{Vec3{halfExtent, -halfExtent, -halfExtent}, Vec2{1.0f, 0.0f}},
			{Vec3{halfExtent, halfExtent, halfExtent}, Vec2{0.0f, 1.0f}},

			// top
			{Vec3{halfExtent, halfExtent, halfExtent}, Vec2{1.0f, 0.0f}},
			{Vec3{halfExtent, halfExtent, -halfExtent}, Vec2{1.0f, 1.0f}},
			{Vec3{-halfExtent, halfExtent, -halfExtent}, Vec2{0.0f, 1.0f}},
			{Vec3{-halfExtent, halfExtent, halfExtent}, Vec2{0.0f, 0.0f}},
			{Vec3{halfExtent, halfExtent, halfExtent}, Vec2{1.0f, 0.0f}},
			{Vec3{-halfExtent, halfExtent, -halfExtent}, Vec2{0.0f, 1.0f}},

			// bottom
			{Vec3{halfExtent, -halfExtent, -halfExtent}, Vec2{1.0f, 0.0f}},
			{Vec3{halfExtent, -halfExtent, halfExtent}, Vec2{1.0f, 1.0f}},
			{Vec3{-halfExtent, -halfExtent, halfExtent}, Vec2{0.0f, 1.0f}},
			{Vec3{-halfExtent, -halfExtent, -halfExtent}, Vec2{0.0f, 0.0f}},
			{Vec3{halfExtent, -halfExtent, -halfExtent}, Vec2{1.0f, 0.0f}},
			{Vec3{-halfExtent, -halfExtent, halfExtent}, Vec2{0.0f, 1.0f}}
		};
	}

	Array<Vertex_PosColorUV, 36> CreateCube_PositionColorTexture(float halfExtent)
	{
		static const ColorRGBAf white = ColorRGBAf::White();
		static const Vec3 whiteAsVec3 = Vec3(white.R(), white.G(), white.B());

		return {
			// back
			{Vec3{-halfExtent, -halfExtent, -halfExtent}, whiteAsVec3, Vec2{1.0f, 0.0f}},
			{Vec3{-halfExtent, halfExtent, -halfExtent}, whiteAsVec3, Vec2{1.0f, 1.0f}},
			{Vec3{halfExtent, halfExtent, -halfExtent}, whiteAsVec3, Vec2{0.0f, 1.0f}},
			{Vec3{halfExtent, -halfExtent, -halfExtent}, whiteAsVec3, Vec2{0.0f, 0.0f}},
			{Vec3{-halfExtent, -halfExtent, -halfExtent}, whiteAsVec3, Vec2{1.0f, 0.0f}},
			{Vec3{halfExtent, halfExtent, -halfExtent}, whiteAsVec3, Vec2{0.0f, 1.0f}},

			// left
			{Vec3{-halfExtent, -halfExtent, halfExtent}, whiteAsVec3, Vec2{1.0f, 0.0f}},
			{Vec3{-halfExtent, halfExtent, halfExtent}, whiteAsVec3, Vec2{1.0f, 1.0f}},
			{Vec3{-halfExtent, halfExtent, -halfExtent}, whiteAsVec3, Vec2{0.0f, 1.0f}},
			{Vec3{-halfExtent, -halfExtent, -halfExtent}, whiteAsVec3, Vec2{0.0f, 0.0f}},
			{Vec3{-halfExtent, -halfExtent, halfExtent}, whiteAsVec3, Vec2{1.0f, 0.0f}},
			{Vec3{-halfExtent, halfExtent, -halfExtent}, whiteAsVec3, Vec2{0.0f, 1.0f}},

			// front
			{Vec3{halfExtent, -halfExtent, halfExtent}, whiteAsVec3, Vec2{1.0f, 0.0f}},
			{Vec3{halfExtent, halfExtent, halfExtent}, whiteAsVec3, Vec2{1.0f, 1.0f}},
			{Vec3{-halfExtent, halfExtent, halfExtent}, whiteAsVec3, Vec2{0.0f, 1.0f}},
			{Vec3{-halfExtent, -halfExtent, halfExtent}, whiteAsVec3, Vec2{0.0f, 0.0f}},
			{Vec3{halfExtent, -halfExtent, halfExtent}, whiteAsVec3, Vec2{1.0f, 0.0f}},
			{Vec3{-halfExtent, halfExtent, halfExtent}, whiteAsVec3, Vec2{0.0f, 1.0f}},

			// right
			{Vec3{halfExtent, -halfExtent, -halfExtent}, whiteAsVec3, Vec2{1.0f, 0.0f}},
			{Vec3{halfExtent, halfExtent, -halfExtent}, whiteAsVec3, Vec2{1.0f, 1.0f}},
			{Vec3{halfExtent, halfExtent, halfExtent}, whiteAsVec3, Vec2{0.0f, 1.0f}},
			{Vec3{halfExtent, -halfExtent, halfExtent}, whiteAsVec3, Vec2{0.0f, 0.0f}},
			{Vec3{halfExtent, -halfExtent, -halfExtent}, whiteAsVec3, Vec2{1.0f, 0.0f}},
			{Vec3{halfExtent, halfExtent, halfExtent}, whiteAsVec3, Vec2{0.0f, 1.0f}},

			// top
			{Vec3{halfExtent, halfExtent, halfExtent}, whiteAsVec3, Vec2{1.0f, 0.0f}},
			{Vec3{halfExtent, halfExtent, -halfExtent}, whiteAsVec3, Vec2{1.0f, 1.0f}},
			{Vec3{-halfExtent, halfExtent, -halfExtent}, whiteAsVec3, Vec2{0.0f, 1.0f}},
			{Vec3{-halfExtent, halfExtent, halfExtent}, whiteAsVec3, Vec2{0.0f, 0.0f}},
			{Vec3{halfExtent, halfExtent, halfExtent}, whiteAsVec3, Vec2{1.0f, 0.0f}},
			{Vec3{-halfExtent, halfExtent, -halfExtent}, whiteAsVec3, Vec2{0.0f, 1.0f}},

			// bottom
			{Vec3{halfExtent, -halfExtent, -halfExtent}, whiteAsVec3, Vec2{1.0f, 0.0f}},
			{Vec3{halfExtent, -halfExtent, halfExtent}, whiteAsVec3, Vec2{1.0f, 1.0f}},
			{Vec3{-halfExtent, -halfExtent, halfExtent}, whiteAsVec3, Vec2{0.0f, 1.0f}},
			{Vec3{-halfExtent, -halfExtent, -halfExtent}, whiteAsVec3, Vec2{0.0f, 0.0f}},
			{Vec3{halfExtent, -halfExtent, -halfExtent}, whiteAsVec3, Vec2{1.0f, 0.0f}},
			{Vec3{-halfExtent, -halfExtent, halfExtent}, whiteAsVec3, Vec2{0.0f, 1.0f}}
		};
	}
}
