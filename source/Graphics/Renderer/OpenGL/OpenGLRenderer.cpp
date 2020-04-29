// Monocle Game Engine source files - Alexandre Baron

#ifdef MOE_OPENGL

#include "OpenGLRenderer.h"

#include <glad/glad.h>

namespace moe
{

	void OpenGLRenderer::Shutdown()
	{
		m_device.Destroy();
	}


	bool OpenGLRenderer::SetupGraphicsContext(GraphicsContextSetup setupFunc)
	{
		return gladLoadGLLoader((GLADloadproc)setupFunc);
	}
}

#endif // MOE_OPENGL