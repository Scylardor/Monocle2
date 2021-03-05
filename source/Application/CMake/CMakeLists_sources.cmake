

set(Monocle_Application_SOURCES
	./AppDescriptor/AppDescriptor.cpp
./AppDescriptor/AppDescriptor.h
./Application.h
./GlfwApplication/BaseGlfwApplication.cpp
./GlfwApplication/BaseGlfwApplication.h
./GlfwApplication/OpenGL/OpenGLGlfwAppDescriptor.cpp
./GlfwApplication/OpenGL/OpenGLGlfwAppDescriptor.h
./GlfwApplication/OpenGL/OpenGLGlfwApplication.cpp
./GlfwApplication/OpenGL/OpenGLGlfwApplication.h
./GlfwApplication/Vulkan/VulkanGlfwAppDescriptor.cpp
./GlfwApplication/Vulkan/VulkanGlfwAppDescriptor.h
./GlfwApplication/Vulkan/VulkanGlfwApplication.cpp
./GlfwApplication/Vulkan/VulkanGlfwApplication.h
	)
	
if(WIN32)
	set(Monocle_Application_SOURCES
	
	${Monocle_Application_SOURCES} 
	
	
	)
	
elseif(APPLE)
	set(Monocle_Application_SOURCES
	
	${Monocle_Application_SOURCES} 
	
	
	)
	
elseif(UNIX)
	set(Monocle_Application_SOURCES
	
	${Monocle_Application_SOURCES} 
	
	
	)
	
	
endif()
