// Monocle Game Engine source files - Alexandre Baron

#pragma once

namespace moe
{
	/*
		The base class for all other renderer classes.
		Contains the graphics device necessary to output commands to the GPU.
	*/
	class IGraphicsRenderer
	{
	public:
		virtual ~IGraphicsRenderer() = default;

	private:
		std::unique_ptr<GraphicsDevice>	m_device = nullptr;

	};

}