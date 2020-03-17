

set(Monocle_Math_SOURCES
	./GLM/Matrix.h
./GLM/Vec2.h
./GLM/Vec3.h
./GLM/Vector.h
./Math.h
./MathUsing.h
./Vec2.h
./Vec3.h
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
