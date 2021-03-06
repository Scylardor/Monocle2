// Monocle Game Engine source files - Alexandre Baron

#include "catch.hpp"

#include <Graphics/GraphicsAllocator/OpenGL/OpenGLBuddyAllocator.h>

// TODO : This test is disabled for now because it needs a working backing rendering context
// (basically it needs a running application)
TEST_CASE("OpenGL Buffer Buddy Allocator", "[Graphics][.]")
{
	SECTION("Allocate leaf block + immediate free")
	{
		moe::OpenGLBuddyAllocator test(GL_DYNAMIC_STORAGE_BIT);

		uint32_t offset1 = test.Allocate(nullptr, 12);
		test.Free(offset1);
	}


	SECTION("allocate leaf block + immediate free + realloc leaf block")
	{
		moe::OpenGLBuddyAllocator test(GL_DYNAMIC_STORAGE_BIT);

		uint32_t offset1 = test.Allocate(nullptr, 12);
		test.Free(offset1);
		uint32_t offset2 = test.Allocate(nullptr, 12);
		test.Free(offset2);
	}


	SECTION("allocate two leaf blocks next to another then free 2 and 1")
	{
		moe::OpenGLBuddyAllocator test(GL_DYNAMIC_STORAGE_BIT);

		uint32_t offset1 = test.Allocate(nullptr, 12);
		uint32_t offset2 = test.Allocate(nullptr, 12);
		test.Free(offset2);
		test.Free(offset1);
	}


	SECTION("allocate two leaf blocks next to another then free 1 and 2")
	{
		moe::OpenGLBuddyAllocator test(GL_DYNAMIC_STORAGE_BIT);

		uint32_t offset1 = test.Allocate(nullptr, 12);
		uint32_t offset2 = test.Allocate(nullptr, 12);
		test.Free(offset1);
		test.Free(offset2);
	}


	SECTION("allocate two leaf blocks next to another, free 1, reallocate one in its place, then free both")
	{
		moe::OpenGLBuddyAllocator test(GL_DYNAMIC_STORAGE_BIT);

		uint32_t offset1 = test.Allocate(nullptr, 12);
		uint32_t offset2 = test.Allocate(nullptr, 12);
		test.Free(offset1);
		offset1 = test.Allocate(nullptr, 12);
		test.Free(offset2);
		test.Free(offset1);
	}


	SECTION("allocate two leaf blocks, then another on bigger level and free 2,1,3")
	{
		moe::OpenGLBuddyAllocator test(GL_DYNAMIC_STORAGE_BIT);

		uint32_t offset1 = test.Allocate(nullptr, 12);
		uint32_t offset2 = test.Allocate(nullptr, 12);
		uint32_t offset3 = test.Allocate(nullptr, 8000);
		test.Free(offset2);
		test.Free(offset1);
		test.Free(offset3);
	}


	SECTION("allocate two leaf blocks, then two bigger ones and free 4,2,1,3")
	{
		moe::OpenGLBuddyAllocator test(GL_DYNAMIC_STORAGE_BIT);

		uint32_t offset1 = test.Allocate(nullptr, 12);
		uint32_t offset2 = test.Allocate(nullptr, 12);
		uint32_t offset3 = test.Allocate(nullptr, 8000);
		uint32_t offset4 = test.Allocate(nullptr, 10000);
		test.Free(offset4);
		test.Free(offset2);
		test.Free(offset1);
		test.Free(offset3);
	}


	SECTION("allocate two leaf blocks, then three bigger ones and free 4,2,1,5,3")
	{
		moe::OpenGLBuddyAllocator test(GL_DYNAMIC_STORAGE_BIT);

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


	SECTION("allocate all memory block in one go")
	{
		moe::OpenGLBuddyAllocator test(GL_DYNAMIC_STORAGE_BIT);

		uint32_t offset1 = test.Allocate(nullptr, 65535);
		test.Free(offset1);
		offset1 = test.Allocate(nullptr, 65535);
		test.Free(offset1);
	}


	SECTION("try to allocate with exhausted memory")
	{
		moe::OpenGLBuddyAllocator test(GL_DYNAMIC_STORAGE_BIT);

		uint32_t offset1 = test.Allocate(nullptr, 65535);
		test.Free(offset1);
		offset1 = test.Allocate(nullptr, 65535);
		test.Free(offset1);
	}


	SECTION("Lots of small allocations")
	{
		moe::OpenGLBuddyAllocator test(GL_DYNAMIC_STORAGE_BIT);

		uint32_t offset1 = test.Allocate(nullptr, 240);
		uint32_t offset2 = test.Allocate(nullptr, 68);
		uint32_t offset3 = test.Allocate(nullptr, 192);
		uint32_t offset4 = test.Allocate(nullptr, 16);
		uint32_t offset5 = test.Allocate(nullptr, 16);
		uint32_t offset6 = test.Allocate(nullptr, 240);
		uint32_t offset7 = test.Allocate(nullptr, 68);
		uint32_t offset8 = test.Allocate(nullptr, 16);
		uint32_t offset9 = test.Allocate(nullptr, 192);
		test.Free(offset4);
		test.Free(offset2);
		test.Free(offset1);
		test.Free(offset3);
		test.Free(offset5);
		test.Free(offset6);
		test.Free(offset7);
		test.Free(offset8);
		test.Free(offset9);
	}

	SECTION("One big alloc among tiny ones")
	{
		moe::OpenGLBuddyAllocator test(GL_DYNAMIC_STORAGE_BIT);

		uint32_t offset1 = test.Allocate(nullptr, 240);
		uint32_t offset2 = test.Allocate(nullptr, 64);
		uint32_t offset3 = test.Allocate(nullptr, 192);
		uint32_t offset4 = test.Allocate(nullptr, 20);
		uint32_t offset5 = test.Allocate(nullptr, 16384);
		uint32_t offset6 = test.Allocate(nullptr, 16);
		uint32_t offset7 = test.Allocate(nullptr, 240);
		test.Free(offset1);
		test.Free(offset2);
		test.Free(offset3);
		test.Free(offset4);
		test.Free(offset5);
		test.Free(offset6);
		test.Free(offset7);
	}

}