// Monocle Game Engine source files - Alexandre Baron

#pragma once

namespace moe
{
	class Material;
	class RenderWorld;

	class IMaterialFrameBlock
	{
	public:
		virtual ~IMaterialFrameBlock() = default;

		virtual void	OnFrameDraw(RenderWorld& currentWorld, Material& currentMaterial) = 0;

	};

}