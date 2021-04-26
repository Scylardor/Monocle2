#pragma once
#include <vector>


namespace moe
{
	template <	typename ObjectType,
				typename IDType = std::uint32_t>
	class SparseArray
	{
	public:

		typedef std::vector<ObjectType>	ObjectArray;


		SparseArray() = default;

		explicit SparseArray(std::size_t nbReserved)
		{
			Reserve(nbReserved);
		}


		void	Reserve(size_t nbReserved)
		{
			m_objects.reserve(nbReserved);
			m_denseIndices.reserve(nbReserved);
			m_sparseIndices.reserve(nbReserved);
		}


		template <typename T = ObjectType>
		[[nodiscard]] IDType	Add(T&& obj)
		{
			auto sparseIdx = PushNewObjectIndices();

			m_objects.push_back(std::forward<T>(obj));

			IDType id = static_cast<IDType>(sparseIdx);
			return id;
		}


		template <typename... Args>
		[[nodiscard]] IDType	Emplace(Args&&... args)
		{
			auto sparseIdx = PushNewObjectIndices();

			m_objects.emplace_back(std::forward<Args>(args)...);

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

			// erase with swap
			auto erasedIt = m_objects.begin() + denseIdx;
			auto lastIt = m_objects.end() - 1;
			if (erasedIt != lastIt)
			{
				std::iter_swap(erasedIt, m_objects.end() - 1);
				m_objects.pop_back();

				// The last element just got relocated : we need to update its maps with the right values.
				m_denseIndices[lastElemSparseIdx] = denseIdx;
				m_sparseIndices[denseIdx] = lastElemSparseIdx;
			}
			else
			{
				// if we erased the last element, no need to swap and update dense indices
				// and we absolutely must not try to update sparse indices (because it's the last one and we already popped back).
				m_objects.pop_back();
			}


			// Point the newly freed slot at the current head of the free list.
			// If there is no other free slot in the list, this will effectively be the end of it.
			m_denseIndices[removedID] = m_nextFreeSparseIndex;

			// Make this newly freed slot the new head of the free list.
			m_nextFreeSparseIndex = removedID;
		}


		[[nodiscard]] const ObjectType&	Get(IDType id) const
		{
			return m_objects[m_denseIndices[id]];
		}

		[[nodiscard]] ObjectType& Mut(IDType id)
		{
			return m_objects[m_denseIndices[id]];
		}

		template <typename T = ObjectType>
		void	Set(IDType id, T&& obj)
		{
			m_objects[m_denseIndices[id]] = std::forward<T>(obj);
		}

		template <typename... Args>
		void	SetEmplace(IDType id, Args&&... args)
		{
			m_objects[m_denseIndices[id]] = ObjectType(std::forward<Args>(args)...);
		}


		[[nodiscard]] auto	GetSize() const
		{
			return m_objects.size();
		}


		[[nodiscard]] bool	IsValidID(IDType id) const
		{
			return (id < m_objects.size() && id != ms_INVALID_ID);
		}


		template <typename ForEachFunction>
		void	ForEach(ForEachFunction&& func)
		{
			for (ObjectType& obj : m_objects)
			{
				func(obj);
			}
		}


		// For C++11 range for syntax
		typename ObjectArray::iterator	begin()
		{
			return m_objects.begin();
		}

		typename ObjectArray::const_iterator begin() const
		{
			return m_objects.begin();
		}

		typename ObjectArray::iterator end()
		{
			return m_objects.end();
		}

		typename ObjectArray::const_iterator end() const
		{
			return m_objects.end();
		}



	private:

		std::size_t	PushNewObjectIndices()
		{
			auto denseIdx = m_objects.size();

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
				sparseIdx = m_denseIndices.size();
				m_denseIndices.push_back(denseIdx);
			}

			m_sparseIndices.push_back(sparseIdx);

			return sparseIdx;
		}

		// Our dense, contiguous array of data.
		ObjectArray		m_objects;

		// Index into this to obtain dense indices (to index directly objects.)
		std::vector<std::size_t>	m_denseIndices;

		// Index into this to obtain sparse indices (to index into dense indices array, which might contain holes.)
		std::vector<std::size_t>	m_sparseIndices;

		std::size_t m_nextFreeSparseIndex = ms_INVALID_ID;

		static const std::size_t ms_INVALID_ID = (std::size_t) -1;
	};



}
