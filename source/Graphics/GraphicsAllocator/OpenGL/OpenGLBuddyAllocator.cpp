// Monocle Game Engine source files - Alexandre Baron

#ifdef MOE_OPENGL

#include "OpenGLBuddyAllocator.h"

#include "Core/Preprocessor/moeAssert.h"

#include "Core/Log/moeLog.h"

#include <cmath>


namespace moe
{
	OpenGLBuddyAllocator::OpenGLBuddyAllocator(GLbitfield flags)
	{
		ReservePoolMemory(flags);
	}

	OpenGLBuddyAllocator::~OpenGLBuddyAllocator()
	{
		DeleteMemory();
	}



	void OpenGLBuddyAllocator::ReservePoolMemory(GLbitfield flags)
	{
		glCreateBuffers(1, &m_buffer);

		MOE_DEBUG_ASSERT(m_buffer != ms_UNINITIALIZED);

		m_allocatedSize = ms_LEAF_SIZE << (ms_MAX_LEVELS - 1);

		glNamedBufferStorage(m_buffer, m_allocatedSize, nullptr, flags);

		// At first, only the largest block is in the free list.
		m_freeBlocks.insert(0);
	}


	uint32_t OpenGLBuddyAllocator::Allocate(const void* data, uint32_t size)
	{
		// Find in which level we should search a free block
		const int wantedLevel = GetDataBlockLevel(size);

		uint32_t offset = 0;

		// Is there a free block in the wanted level ?
		std::optional<uint32_t> freeBlockUniqueIdx = TryRemoveFreeLevelBlock(wantedLevel);

		if (freeBlockUniqueIdx.has_value())
		{
			const uint32_t levelBlockIdx = GetLevelIndexFromUniqueIdx(wantedLevel, freeBlockUniqueIdx.value());
			FlipBlockBuddyBit(wantedLevel, levelBlockIdx); // mark this block occupied
			offset = GetDataOffsetForLevelBlock(wantedLevel, levelBlockIdx);
		}
		else
		{
			// Go all the way up until we can split blocks of wanted level
			int higherLevel = wantedLevel - 1;
			while (higherLevel >= 0)
			{
				freeBlockUniqueIdx = TryRemoveFreeLevelBlock(higherLevel);
				if (freeBlockUniqueIdx.has_value())
					break;

				higherLevel--;
			}

			if (higherLevel < 0)
			{
				// No memory available !
				MOE_ERROR(moe::ChanGraphics, "Buddy Allocator failed to allocate new memory (memory exhausted)");
				return ms_INVALID_OFFSET;
			}

			// If we're here we're guaranteed the optional has a value
			const uint32_t highLevelFreeBlockUniqueIdx = freeBlockUniqueIdx.value();

			// Cut this block in half on all levels until we reach destination level.
			uint32_t splitBlockUniqueIdx = highLevelFreeBlockUniqueIdx;
			for (int iLvl = higherLevel; iLvl <= wantedLevel; iLvl++)
			{
				SetBlockSplit(splitBlockUniqueIdx, true);

				const uint32_t levelBlockIdx = GetLevelIndexFromUniqueIdx(iLvl, splitBlockUniqueIdx);
				FlipBlockBuddyBit(iLvl, levelBlockIdx);

				if (splitBlockUniqueIdx != 0) // "root" block is the only one with no buddy
				{
					AddFreeBlock(splitBlockUniqueIdx + 1); // mark the buddy of the block we just split as a free block
				}

				// go down one level
				splitBlockUniqueIdx += (1 << iLvl) + levelBlockIdx;
			}

			// now compute the data offset we need
			const uint32_t highLevelFreeBlockLevelIdx = GetLevelIndexFromUniqueIdx(higherLevel, highLevelFreeBlockUniqueIdx);
			offset = GetDataOffsetForLevelBlock(higherLevel, highLevelFreeBlockLevelIdx);
		}

		// A suitable offset has been found : push the data inside
		glNamedBufferSubData(m_buffer, offset, size, data);

		return offset;
	}


	void OpenGLBuddyAllocator::Free(uint32_t offset)
	{
		// First, we have to find the level this block belongs to.
		// Start from the bottom and compute the level of the given offset
		int level = (int)ComputeBlockLevelForOffset(offset);

		do
		{
			const uint32_t uniqueBlockIdx = GetUniqueBlockIndex(level, offset);

			// See if we can merge this block (if the buddy of this block is free too)
			const uint32_t blockLevelIdx = GetBlockIndexInLevel(level, offset);
			const bool bothBuddiesFree = FlipBlockBuddyBit(level, blockLevelIdx);

			if (bothBuddiesFree && level != 0) // we can merge ! But level 0 won't have any buddy.
			{
				// The buddy should be in the free list so start by removing it from the free list
				const uint32_t buddyUniqueIdx = uniqueBlockIdx + ((blockLevelIdx & 1) ? -1 : 1);

				const bool removed = RemoveFreeBlock(buddyUniqueIdx);
				MOE_DEBUG_ASSERT(removed);

				SetBlockSplit(uniqueBlockIdx, false); // do not forget to flag this block "not split"
			}
			else // we cannot merge : just add this block to the free list
			{
				// First flag this block free
				AddFreeBlock(uniqueBlockIdx);

				SetBlockSplit(uniqueBlockIdx, false); // do not forget to flag this block "not split"
				break;
			}

			level--;
		} while (level >= 0);

	}


	void OpenGLBuddyAllocator::DeleteMemory()
	{
		if (m_buffer != ms_UNINITIALIZED)
			glDeleteBuffers(1, &m_buffer);

		m_freeBlocks.clear();

		m_allocatedSize = 0;

		m_buffer = ms_UNINITIALIZED;

		m_buddyIndex.reset();

		m_splitIndex.reset();
	}


	uint32_t OpenGLBuddyAllocator::GetDataBlockLevel(uint32_t dataBlockSize)
	{
		if (dataBlockSize < ms_LEAF_SIZE)
		{
			return ms_MAX_LEVELS - 1;
		}

		// I use the property "log(A/B) = log(A) - log(B)" to save one log2f call,
		// but need to cast leaf_size to float to avoid unsigned division's precision loss.
		float logDiff = std::log2f(dataBlockSize / (float)ms_LEAF_SIZE);

		const uint32_t level = (uint32_t)std::ceilf(logDiff);

		// return ms_MAX_LEVELS-1 - computed level because the index is "reversed" - first level is "root" block, the biggest !
		return (ms_MAX_LEVELS-1 - level);
	}


	void OpenGLBuddyAllocator::AddFreeBlock(uint32_t uniqueBlockIdx)
	{
		m_freeBlocks.insert(uniqueBlockIdx);
	}


	/* Returns whether both buddies are free or not */
	bool OpenGLBuddyAllocator::FlipBlockBuddyBit(uint32_t level, uint32_t blockIdxInLevel)
	{
		// We use one bit for each buddy pair, so if blockIdx is even,
		// this block shares its bit with the previous one (except the first one).
		blockIdxInLevel &= ~1; // &= ~1 means we remove one if the value is odd.

		uint32_t buddyBitIdx = GetBuddyBit(level, blockIdxInLevel);

		m_buddyIndex.flip(buddyBitIdx);
		return !(m_buddyIndex[buddyBitIdx]);
	}


	void OpenGLBuddyAllocator::SetBlockSplit(uint32_t uniqueBlockIdx, bool isSplit)
	{
		// Leaf blocks cannot be split anyway, so we ignore a whole range of unique block indexes (the last power of two level)
		if (uniqueBlockIdx >= m_splitIndex.size())
			return;

		m_splitIndex.set(uniqueBlockIdx, isSplit);
	}


	bool OpenGLBuddyAllocator::IsBlockSplit(uint32_t uniqueBlockIdx) const
	{
		return m_splitIndex[uniqueBlockIdx];
	}


	uint32_t OpenGLBuddyAllocator::GetBuddyBit(uint32_t level, uint32_t blockIdxInLevel) const
	{
		if (level == 0) // "root" block (at level 0) is the only one without a buddy
			return 0;

		uint32_t buddyBitIdx = (1 << (level - 1)) + blockIdxInLevel;
		return buddyBitIdx;
	}


	uint32_t OpenGLBuddyAllocator::GetUniqueIndexForLevel(uint32_t level, uint32_t levelBlockIdx) const
	{
		uint32_t uniqueIdx = (1 << level) + levelBlockIdx - 1;
		return uniqueIdx;
	}


	std::optional<uint32_t> OpenGLBuddyAllocator::TryRemoveFreeLevelBlock(uint32_t level)
	{
		// Find the first block that is lower or equal to the first block of the level, and check if this block is in the level
		uint32_t totalBlocksForLevel = (1 << level);
		uint32_t firstBlockIdx = GetUniqueIndexForLevel(level, 0);
		uint32_t lastBlockIdx = GetUniqueIndexForLevel(level, totalBlocksForLevel - 1);

		auto foundBlock = m_freeBlocks.lower_bound(firstBlockIdx);
		if (foundBlock != m_freeBlocks.end() && *foundBlock <= lastBlockIdx)
		{
			// we found a free block in this level
			std::optional<uint32_t> freeBlockIdx(*foundBlock);

			m_freeBlocks.erase(foundBlock);

			return freeBlockIdx;
		}

		return std::optional<uint32_t>();
	}


	/* Returns whether we effectively removed a block or not. Should always return true ! */
	bool OpenGLBuddyAllocator::RemoveFreeBlock(uint32_t uniqueBlockIdx)
	{
		size_t nbrErased = m_freeBlocks.erase(uniqueBlockIdx);
		return (nbrErased > 0);
	}


	uint32_t OpenGLBuddyAllocator::GetLevelIndexFromUniqueIdx(uint32_t level, uint32_t uniqueBlockIdx) const
	{
		uint32_t levelBlockIdx = uniqueBlockIdx - ((1 << level) - 1);
		return levelBlockIdx;
	}


	uint32_t OpenGLBuddyAllocator::GetDataOffsetForLevelBlock(uint32_t level, uint32_t blockLevelIdx) const
	{
		uint32_t dataOffset = blockLevelIdx * (1 << (ms_MAX_LEVELS - 1 - level)) * ms_LEAF_SIZE;
		return dataOffset;
	}


	uint32_t OpenGLBuddyAllocator::GetLevelBlockSize(uint32_t level) const
	{
		uint32_t levelBlockSize = m_allocatedSize / (1 << level);
		return levelBlockSize;
	}


	uint32_t OpenGLBuddyAllocator::GetBlockIndexInLevel(uint32_t level, uint32_t offset) const
	{
		uint32_t indexInLevel = offset / GetLevelBlockSize(level);
		return indexInLevel;
	}


	uint32_t OpenGLBuddyAllocator::GetUniqueBlockIndex(uint32_t level, uint32_t offset) const
	{
		uint32_t blockLevelIdx = GetBlockIndexInLevel(level, offset); // the index of the block for this level
		uint32_t uniqueBlockIdx = GetUniqueIndexForLevel(level, blockLevelIdx); // the unique index of this block in the whole memory pool
		return uniqueBlockIdx;
	}


	uint32_t OpenGLBuddyAllocator::GetLevelBlocksNumber(uint32_t level) const
	{
		uint32_t numberOfBlocksForThisLevel = (1 << level);

		return numberOfBlocksForThisLevel;
	}


	uint32_t OpenGLBuddyAllocator::ComputeBlockLevelForOffset(uint32_t offset) const
	{
		// Given just a memory offset, we cannot deduce what's the level of the block this offset points to.
		// we have to start from the "smallest" level and climb our way up until we find an allocated block.
		for (uint32_t iLvl = ms_MAX_LEVELS - 1; iLvl > 0; iLvl--)
		{
			uint32_t higherLevelUniqueBlockIdx = GetUniqueBlockIndex(iLvl - 1, offset);
			const bool isUpperLevelBlockSplit = IsBlockSplit(higherLevelUniqueBlockIdx);
			if (isUpperLevelBlockSplit)
			{
				return iLvl;
			}
		}

		return 0;
	}

}

#endif // MOE_OPENGL