

set(Monocle_Math_SOURCES
	./Angles/Angles.cpp
./Angles/Angles.h
./GLM/Angles_glm.h
./GLM/Matrix.h
./GLM/Vector.h
./Math.h
./Matrix4.h
./Vec2.h
./Vec3.h
./Vec4.h
	)
	
if(WIN32)
	set(Monocle_Math_SOURCES
	
	${Monocle_Math_SOURCES} 
	
	
	)
	
elseif(APPLE)
	set(Monocle_Math_SOURCES
	
	${Monocle_Math_SOURCES} 
	
	
	)
	
elseif(UNIX)
	set(Monocle_Math_SOURCES
	
	${Monocle_Math_SOURCES} 
	
	
	)
	
	
endif()
