// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_OPENGL

#include "Graphics/Mesh/Mesh.h"
#include "Graphics/DeviceBuffer/OpenGL/OpenGLDeviceBufferRange.h"

namespace moe
{
	class OpenGLMesh : public Mesh
	{
	public:

	private:

		OpenGLDeviceBufferRange	m_meshBufferRange;
	};


}


#endif