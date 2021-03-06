// Monocle Game Engine source files - Alexandre Baron


#include "TestApplication.h"

#include "Core/Misc/Literals.h"

//#include "btBulletCollisionCommon.h"


int main()
{

	moe::OpenGLGlfwAppDescriptor appDesc(1024_width, 728_height, "Monocle Sandbox");

	moe::TestApplication app(appDesc);

	if (app.IsInitialized())
	{
		app.Run();
	}

	return 0;
}
