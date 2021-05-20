#pragma once

#include "Core/Preprocessor/moeAssert.h"
#include "Core/Misc/moeCRTP.h"

#include <vector>
#include <array>


namespace moe
{

	static uint32_t const INVALID_BLOCK = UINT32_MAX;

	template <typename TObj>
	union PoolBlock
	{
		PoolBlock()
		{
		}

		// Necessary to declare it or it won't compile.
		// (cannot make it "= default" since compiler doesn't know what to do)
		// Trust the object pool to call the destructor of used objects.
		~PoolBlock()
		{}


		PoolBlock(const PoolBlock& other) noexcept
		{
			NextFreeBlock = other.NextFreeBlock;
			std::memcpy(&Object, &other.Object, sizeof(TObj));
		}

		PoolBlock& operator=(const PoolBlock& other) noexcept
		{
			if (&other != this)
			{
				NextFreeBlock = other.NextFreeBlock;
				std::memcpy(&Object, &other.Object, sizeof(TObj));
			}
			return *this;
		}

		PoolBlock(PoolBlock&& other) noexcept
		{
			NextFreeBlock = other.NextFreeBlock;
			Object = std::move(other.Object);
		}

		PoolBlock& operator=(PoolBlock&& other) noexcept
		{
			if (&other != this)
			{
				NextFreeBlock = other.NextFreeBlock;
				Object = std::move(other.Object);
			}
			return *this;
		}


		uint32_t	NextFreeBlock{ INVALID_BLOCK };
		TObj		Object;
	};


	template <typename ValueType, class Top>
	class ObjectPool : public CRTP<Top>
	{
	public:

		// The "pure virtual functions"

		template <typename... Ts>
		[[nodiscard]] uint32_t	Emplace(Ts&&... args)
		{
			return MOE_CRTP_IMPL_VARIADIC(Emplace, Ts, args);
		}


		void	Free(uint32_t freedObjectID)
		{
			MOE_CRTP_IMPL(Free, freedObjectID);
		}


		[[nodiscard]] const ValueType& Get(uint32_t id) const
		{
			return MOE_CRTP_IMPL(Get, id);
		}

		[[nodiscard]] ValueType& Mut(uint32_t id)
		{
			return MOE_CRTP_IMPL(Mut, id);
		}

		[[nodiscard]] auto	Size() const
		{
			return MOE_CRTP_IMPL(Size);
		}


		[[nodiscard]] bool	Empty() const
		{
			return MOE_CRTP_IMPL(Empty);
		}


	protected:

		template <typename... Ts>
		[[nodiscard]] uint32_t	Append(Ts&&... args)
		{
			return MOE_CRTP_IMPL_VARIADIC(Append, Ts, args);
		}

	};


	template <typename Container, typename ValueType, class Top>
	class AObjectPool : public ObjectPool<ValueType, Top>
	{
		using Base = ObjectPool<ValueType, Top>;
		friend Base;

	public:

		~AObjectPool()
		{
			// Call the destructor for each item still in use in the container.
			for (auto usedIdx : m_usedIndexes)
			{
				auto* object = (ValueType*)&m_objects[usedIdx];
				object->~ValueType();
			}
		}


	protected:

		[[nodiscard]] bool	HasFreeBlock() const
		{
			return m_firstFreeBlock != INVALID_BLOCK;
		}

		template <typename... Ts>
		[[nodiscard]] uint32_t	EmplaceImpl(Ts&&... args)
		{
			uint32_t availableID;
			if (HasFreeBlock())
			{
				availableID = m_firstFreeBlock;

				auto* block = (PoolBlock<ValueType>*) & m_objects[availableID];

				m_firstFreeBlock = block->NextFreeBlock;

				new (&m_objects[availableID]) ValueType(std::forward<Ts>(args)...);
			}
			else
			{
				availableID = this->Underlying().Append(std::forward<Ts>(args)...);
			}

			m_usedIndexes.push_back(availableID);

			return availableID;
		}


		void	FreeImpl(uint32_t freedObjectID)
		{
			// Call the destructor
			auto* object = (ValueType*)&m_objects[freedObjectID];
			object->~ValueType();

			// chain to the next free block
			auto* block = (PoolBlock<ValueType>*)object;
			block->NextFreeBlock = m_firstFreeBlock;
			m_firstFreeBlock = freedObjectID;

			// Remove this block from the used index list
			auto indexIt = std::find(m_usedIndexes.begin(), m_usedIndexes.end(), freedObjectID);
			MOE_ASSERT(indexIt != m_usedIndexes.end());
			std::iter_swap(indexIt, m_usedIndexes.end() - 1);
			m_usedIndexes.pop_back();
		}


		[[nodiscard]] const ValueType& GetImpl(uint32_t id) const
		{
			return m_objects[id].Object;
		}

		[[nodiscard]] ValueType& MutImpl(uint32_t id)
		{
			return m_objects[id].Object;
		}

		[[nodiscard]] auto	SizeImpl() const
		{
			return m_objects.size();
		}


		[[nodiscard]] bool	EmptyImpl() const
		{
			return m_objects.empty();
		}

		template <typename... Ts>
		[[nodiscard]] uint32_t	AppendImpl(Ts&&...)
		{
			MOE_ASSERT(false); // unimplemented!
			return 0;
		}

		Container				m_objects;
		uint32_t				m_firstFreeBlock{ INVALID_BLOCK };

		std::vector<uint32_t>	m_usedIndexes{}; // mostly used to clear
	};


	template<typename TObj>
	class DynamicObjectPool : public AObjectPool<std::vector<PoolBlock<TObj>>, TObj, DynamicObjectPool<TObj >>
	{
		using Base = ObjectPool<TObj, DynamicObjectPool<TObj>>;
		friend Base;

	public:

		DynamicObjectPool(uint32_t reserved = 0)
		{
			m_objects.reserve(reserved);
			m_usedIndexes.reserve(reserved);
		}


		//DynamicObjectPool(DynamicObjectPool&& other)
		//{
		//	m_objects = std::move(other);
		//	m_used
		//}


		void	SetMaximumAllowedGrowth(uint32_t maxGrowth)
		{
			m_maxAllowedGrowth = maxGrowth;
		}

	protected:
		template <typename... Ts>
		[[nodiscard]] uint32_t	AppendImpl(Ts&&... args)
		{
			if (m_objects.size() == m_maxAllowedGrowth)
				return INVALID_BLOCK;

			auto id = (uint32_t)m_objects.size();
			auto& block = m_objects.emplace_back();
			auto* object = (TObj*)&block;
			new (object) TObj(std::forward<Ts>(args)...);

			return id;
		}


	private:

		std::uint32_t	m_maxAllowedGrowth{ UINT32_MAX };
	};



	template<typename TObj, size_t TSize>
	class FixedObjectPool : public AObjectPool<std::array<PoolBlock<TObj>, TSize>, TObj, FixedObjectPool<TObj, TSize>>
	{
		using Base = ObjectPool<TObj, FixedObjectPool<TObj, TSize>>;
		friend Base;

	public:
		FixedObjectPool()
		{
			auto* block = (PoolBlock<TObj>*) & m_objects[0];
			block->NextFreeBlock = INVALID_BLOCK;
			m_firstFreeBlock = 0;

			m_usedIndexes.reserve(TSize);
		}

	protected:

		template <typename... Ts>
		[[nodiscard]] uint32_t	EmplaceImpl(Ts&&... args)
		{
			uint32_t availableID{ 0 };

			if (false == HasFreeBlock())
			{
				MOE_FATAL(moe::ChanDefault, "FixedObjectPool capacity overflown ! You need to increase the capacity of the pool.");
				MOE_ASSERT(false); // Object pool overflow
				return UINT32_MAX;
			}

			availableID = m_firstFreeBlock;

			// chain to the next free block
			auto* block = (PoolBlock<TObj>*) & m_objects[availableID];

			if (block->NextFreeBlock == INVALID_BLOCK) // we were the last : bump the free block number
			{
				if (m_highestFreeBlock + 1 < TSize) // Do we need to push up the high water mark ?
				{
					m_highestFreeBlock++;
					m_firstFreeBlock = m_highestFreeBlock;
					block = (PoolBlock<TObj>*) & m_objects[m_firstFreeBlock];
					block->NextFreeBlock = INVALID_BLOCK;
				}
				else
				{
					m_firstFreeBlock = INVALID_BLOCK;
				}
			}
			else
			{
				m_firstFreeBlock = block->NextFreeBlock;
			}

			new (&m_objects[availableID]) TObj(std::forward<Ts>(args)...);

			// Don't forget to add this to used indexes
			m_usedIndexes.push_back(availableID);

			return availableID;
		}

		template <typename... Ts>
		[[nodiscard]] uint32_t	AppendImpl(Ts&&...)
		{
			MOE_ASSERT(false); // unimplemented!
			return 0;
		}


		uint32_t	m_highestFreeBlock = 0;

	};



}
