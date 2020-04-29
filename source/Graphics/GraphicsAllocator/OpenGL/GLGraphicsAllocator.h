// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_OPENGL

#include "Core/Misc/Types.h"
#include "Monocle_Graphics_Export.h"

#include <glad/glad.h>

#ifdef MOE_STD_SUPPORT
	#include <bitset>
	#include <optional>
	#include <set>
#endif


namespace moe
{
	/*
		An allocator used to manage the memory of an OpenGL data buffer.
		The Allocator makes no assumption about what actually is in the buffer: it's just raw data.
		Implementation inspired by the buddy allocator system.
	*/
	class GLGraphicsAllocator
	{

	public:

		Monocle_Graphics_API				GLGraphicsAllocator(uint32_t bufferHandle);

		Monocle_Graphics_API void		ReservePoolMemory(GLenum target, GLbitfield flags);

		Monocle_Graphics_API uint32_t	Allocate(const void* data, uint32_t size);

		Monocle_Graphics_API void		Free(uint32_t offset);

	private:

		static const uint32_t	ms_UNINITIALIZED = 0;
		static const uint32_t	ms_LEAF_SIZE = 4096; // The size of the smallest possible block allocated.
		static const uint32_t	ms_MAX_LEVELS = 5; // The maximum number of levels the buddy allocator can subdivide into.


		uint32_t	GetDataBlockLevel(uint32_t dataBlockSize) const;

		void		AddFreeBlock(uint32_t uniqueBlockIdx);

		bool		FlipBlockBuddyBit(uint32_t level, uint32_t blockIdxInLevel);

		void		SetBlockSplit(uint32_t uniqueBlockIdx, bool isSplit);

		uint32_t	GetBuddyBit(uint32_t level, uint32_t blockIdxInLevel) const;

		bool		IsBlockSplit(uint32_t uniqueBlockIdx) const;

		uint32_t	GetBlockIndexInLevel(uint32_t level, uint32_t offset) const;

		uint32_t	GetUniqueBlockIndex(uint32_t level, uint32_t offset) const;

		uint32_t	GetLevelBlocksNumber(uint32_t level) const;

		uint32_t	ComputeBlockLevelForOffset(uint32_t offset) const;

		uint32_t	GetUniqueIndexForLevel(uint32_t level, uint32_t levelBlockIdx) const;

		std::optional<uint32_t>	TryRemoveFreeLevelBlock(uint32_t level);

		bool		RemoveFreeBlock(uint32_t uniqueBlockIdx);

		uint32_t	GetLevelIndexFromUniqueIdx(uint32_t level, uint32_t uniqueBlockIdx) const;

		uint32_t	GetDataOffsetForLevelBlock(uint32_t level, uint32_t blockLevelIdx) const;

		uint32_t	GetLevelBlockSize(uint32_t level) const;

		// Total size of the buffer
		uint32_t	m_allocatedSize = 0;

		// The actual buffer handle containing the memory
		uint32_t	m_buffer = ms_UNINITIALIZED;

		std::bitset<1 << (ms_MAX_LEVELS - 1)>	m_buddyIndex;

		// Since the leaf blocks can't be split, we only need 1 << (num_levels - 1) entries in the split index.
		std::bitset<(1 << (ms_MAX_LEVELS - 1)) - 1>	m_splitIndex;

		std::set<uint32_t>	m_freeBlocks;
	};

}

#endif // MOE_OPENGL