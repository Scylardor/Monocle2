// Monocle Game Engine source files - Alexandre Baron

#pragma once


namespace moe
{
	class IGraphicsRenderer;

	class IMaterialBlock
	{
	public:
		virtual ~IMaterialBlock() = default;


		virtual void OnMaterialUse(IGraphicsRenderer& renderer) = 0;
	};

}