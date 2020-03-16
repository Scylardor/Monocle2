// Monocle Game Engine source files - Alexandre Baron

#include "TestApplication.h"

#include <Graphics/DeviceBuffer/DeviceBuffer.h>

#include <Graphics/GraphicsAllocator/OpenGL/GLGraphicsAllocator.h>

namespace moe
{

TestApplication::TestApplication(const moe::GraphicsContextDescriptor& contextDesc, const moe::WindowDescriptor& windowDesc) :
	moe::GlfwApplication(contextDesc, windowDesc)
{
}


void TestApplication::Run()
{
	// test 1 : allocate leaf block + immediate free
	{
		moe::GLGraphicsAllocator test(1);
		test.ReservePoolMemory(0, 0);
		uint32_t offset1 = test.Allocate(nullptr, 12);
		test.Free(offset1);
	}

	// test 2 : allocate leaf block + immediate free + realloc leaf block
	{
		moe::GLGraphicsAllocator test(1);
		test.ReservePoolMemory(0, 0);
		uint32_t offset1 = test.Allocate(nullptr, 12);
		test.Free(offset1);
		uint32_t offset2 = test.Allocate(nullptr, 12);
		test.Free(offset2);
	}
	// test 3 : allocate two leaf blocks next to another then free 2 and 1
	{
		moe::GLGraphicsAllocator test(1);
		test.ReservePoolMemory(0, 0);
		uint32_t offset1 = test.Allocate(nullptr, 12);
		uint32_t offset2 = test.Allocate(nullptr, 12);
		test.Free(offset2);
		test.Free(offset1);
	}
	// test 3 : allocate two leaf blocks next to another then free 1 and 2
	{
		moe::GLGraphicsAllocator test(1);
		test.ReservePoolMemory(0, 0);
		uint32_t offset1 = test.Allocate(nullptr, 12);
		uint32_t offset2 = test.Allocate(nullptr, 12);
		test.Free(offset1);
		test.Free(offset2);
	}
	// test 3 : allocate two leaf blocks next to another then free 1 and 2
	{
		moe::GLGraphicsAllocator test(1);
		test.ReservePoolMemory(0, 0);
		uint32_t offset1 = test.Allocate(nullptr, 12);
		uint32_t offset2 = test.Allocate(nullptr, 12);
		test.Free(offset1);
		offset1 = test.Allocate(nullptr, 12);
		test.Free(offset2);
		test.Free(offset1);
	}
	// test 4 : allocate two leaf blocks, then another on bigger level and free 2,1,3
	{
		moe::GLGraphicsAllocator test(1);
		test.ReservePoolMemory(0, 0);
		uint32_t offset1 = test.Allocate(nullptr, 12);
		uint32_t offset2 = test.Allocate(nullptr, 12);
		uint32_t offset3 = test.Allocate(nullptr, 8000);
		test.Free(offset2);
		test.Free(offset1);
		test.Free(offset3);
	}
	// test 4 : allocate two leaf blocks, then another on bigger level and free 2,1,3
	{
		moe::GLGraphicsAllocator test(1);
		test.ReservePoolMemory(0, 0);
		uint32_t offset1 = test.Allocate(nullptr, 12);
		uint32_t offset2 = test.Allocate(nullptr, 12);
		uint32_t offset3 = test.Allocate(nullptr, 8000);
		test.Free(offset2);
		test.Free(offset1);
		test.Free(offset3);
	}
	// test 5 : allocate two leaf blocks, then another on bigger level and free 4,2,1,3
	{
		moe::GLGraphicsAllocator test(1);
		test.ReservePoolMemory(0, 0);
		uint32_t offset1 = test.Allocate(nullptr, 12);
		uint32_t offset2 = test.Allocate(nullptr, 12);
		uint32_t offset3 = test.Allocate(nullptr, 8000);
		uint32_t offset4 = test.Allocate(nullptr, 10000);
		test.Free(offset4);
		test.Free(offset2);
		test.Free(offset1);
		test.Free(offset3);
	}
	// test 6 : allocate two leaf blocks, then another on bigger level and free 2,1,5,3
	{
		moe::GLGraphicsAllocator test(1);
		test.ReservePoolMemory(0, 0);
		uint32_t offset1 = test.Allocate(nullptr, 12);
		uint32_t offset2 = test.Allocate(nullptr, 12);
		uint32_t offset3 = test.Allocate(nullptr, 8000);
		uint32_t offset4 = test.Allocate(nullptr, 10000);
		uint32_t offset5 = test.Allocate(nullptr, 31256);
		test.Free(offset4);
		test.Free(offset2);
		test.Free(offset1);
		test.Free(offset5);
		test.Free(offset3);
	}
	// test 7 : allocate all memory block in one go
	{
		moe::GLGraphicsAllocator test(1);
		test.ReservePoolMemory(0, 0);
		uint32_t offset1 = test.Allocate(nullptr, 65535);
		test.Free(offset1);
		offset1 = test.Allocate(nullptr, 65535);
		test.Free(offset1);
	}

	// test: try to allocate with exhausted memory
	{
		moe::GLGraphicsAllocator test(1);
		test.ReservePoolMemory(0, 0);
		uint32_t offset1 = test.Allocate(nullptr, 65535);
		test.Free(offset1);
		offset1 = test.Allocate(nullptr, 65535);
		test.Free(offset1);
	}

	// test 2 :
	//uint32_t offset2 = test.Allocate(nullptr, 515);

	//uint32_t offset3 = test.Allocate(nullptr, 2839);
	//uint32_t offset4 = test.Allocate(nullptr, 251);
	//uint32_t offset5 = test.Allocate(nullptr, 251);


	while (!m_window->ShouldWindowClose())
	{
		m_inputHandler->PumpEvents();
	}

}

}