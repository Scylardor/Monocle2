#pragma once

#include <vector>

#include "Graphics/RHI/RenderHardwareInterface.h"

namespace moe
{
	template <	typename T,
		typename IDType = std::uint32_t>
		class GraphicsSparseArray
	{
		static_assert(sizeof(T) < UINT32_MAX);

	public:
		GraphicsSparseArray() = default;


		void	Initialize(RenderHardwareInterface* RHI, uint32_t nbReserved)
		{
			MOE_ASSERT(nbReserved != 0);
			m_RHI = RHI;
			m_objects = m_RHI->BufferManager().MapCoherentDeviceBuffer((uint32_t)sizeof(T), nbReserved, nullptr, false);
			m_capacity = nbReserved;
			m_size = 0;
			m_denseIndices.reserve(nbReserved);
			m_sparseIndices.reserve(nbReserved);
		}


		~GraphicsSparseArray()
		{
			if (m_RHI)
				m_RHI->BufferManager().Unmap(m_objects);
		}


		[[nodiscard]] IDType	Add(T const& obj)
		{
			auto sparseIdx = PushNewObjectIndices();

			if (m_size == m_capacity)
			{
				m_capacity *= 2;
				m_RHI->BufferManager().ResizeMapping(m_objects, m_capacity);
			}

			m_objects.MutBlock<T>(m_size) = obj;
			m_size++;

			IDType id = static_cast<IDType>(sparseIdx);
			return id;
		}


		[[nodiscard]] IDType	New()
		{
			auto sparseIdx = PushNewObjectIndices();

			if (m_size == m_capacity)
			{
				m_capacity *= 2;
				m_RHI->BufferManager().ResizeMapping(m_objects, m_capacity);
			}

			m_size++;

			IDType id = static_cast<IDType>(sparseIdx);
			return id;
		}


		void	Remove(IDType removedID)
		{
			// An id is actually a sparse index. Lookup the removed item's dense index using the sparse index.
			auto denseIdx = m_denseIndices[removedID];

			// lookup where the last element is referenced in the sparse array.
			auto lastElemSparseIdx = m_sparseIndices.back();
			m_sparseIndices.pop_back();

			if (denseIdx != m_size-1)
			{
				// erase with swap
				m_objects.MutBlock<T>(denseIdx) = m_objects.MutBlock<T>(m_size - 1);

				// The last element just got relocated : we need to update its maps with the right values.
				m_denseIndices[lastElemSparseIdx] = denseIdx;
				m_sparseIndices[denseIdx] = lastElemSparseIdx;
			}

			m_size--;

			// Point the newly freed slot at the current head of the free list.
			// If there is no other free slot in the list, this will effectively be the end of it.
			m_denseIndices[removedID] = m_nextFreeSparseIndex;

			// Make this newly freed slot the new head of the free list.
			m_nextFreeSparseIndex = removedID;
		}


		[[nodiscard]] const T& Get(IDType id) const
		{
			return m_objects.MutBlock<T>(m_denseIndices[id]);
		}

		[[nodiscard]] T& Mut(IDType id)
		{
			return m_objects.MutBlock<T>(m_denseIndices[id]);
		}


		void	Set(IDType id, T const& obj)
		{
			m_objects.MutBlock<T>(m_denseIndices[id]) = obj;
		}


		[[nodiscard]] auto	GetSize() const
		{
			return m_size;
		}

		[[nodiscard]] auto	GetCapacity() const
		{
			return m_capacity;
		}

		[[nodiscard]] auto	GetDataBytesRange() const
		{
			return m_size * m_objects.AlignedBlockSize();
		}


		[[nodiscard]] bool	IsValidID(IDType id) const
		{
			return (id < m_size && id != ms_INVALID_ID);
		}


		[[nodiscard]] auto	DenseIndex(IDType id) const
		{
			return m_denseIndices[id];
		}


		DeviceBufferHandle	GetDeviceHandle() const
		{
			return m_objects.Handle();
		}

		auto const&		GetDeviceMapping() const
		{
			return m_objects;
		}



		template <typename ForEachFunction>
		void	ForEach(ForEachFunction&& func)
		{
			for (T& obj : m_objects)
			{
				func(obj);
			}
		}


	private:

		IDType	PushNewObjectIndices()
		{
			IDType denseIdx = m_size;

			// Try to grab the index of the first free slot if there is one
			auto sparseIdx = m_nextFreeSparseIndex;
			if (m_nextFreeSparseIndex != ms_INVALID_ID)
			{
				// there was a slot in the free list : take it
				// Since we're using the first free slot now,
				// the next free slot needs to become the free list head.
				m_nextFreeSparseIndex = m_denseIndices[sparseIdx];
				m_denseIndices[sparseIdx] = denseIdx;
			}
			else
			{
				// The free list is empty, so we'll just need to grow the array.
				sparseIdx = (IDType)m_denseIndices.size();
				m_denseIndices.push_back(denseIdx);
			}

			m_sparseIndices.push_back(sparseIdx);

			// check for index type overflow (would wrap)
			MOE_ASSERT(m_size <= std::numeric_limits<IDType>::max());

			return sparseIdx;
		}

		// Our dense, contiguous array of data.
		DeviceBufferMapping	m_objects;
		uint32_t			m_size = 0;
		uint32_t			m_capacity = 0;

		// Index into this to obtain dense indices (to index directly objects.)
		std::vector<IDType>	m_denseIndices;

		// Index into this to obtain sparse indices (to index into dense indices array, which might contain holes.)
		std::vector<IDType>	m_sparseIndices;

		IDType m_nextFreeSparseIndex = ms_INVALID_ID;

		RenderHardwareInterface* m_RHI = nullptr;

		static const IDType ms_INVALID_ID = (IDType)-1;
	};



}
