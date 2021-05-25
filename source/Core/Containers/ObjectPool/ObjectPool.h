#pragma once

#include "Core/Preprocessor/moeAssert.h"
#include "Core/Misc/moeCRTP.h"

#include <vector>
#include <array>
#include <variant>

namespace moe
{

	static uint32_t const INVALID_BLOCK = UINT32_MAX;

	struct PoolBlock
	{
		PoolBlock() = default;
		PoolBlock(uint32_t nfb) :
			NextFreeBlock(nfb)
		{}

		uint32_t	NextFreeBlock{ INVALID_BLOCK };

	};


	template <typename TObj>
	using PoolBlockVar = std::variant<PoolBlock, TObj>;



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

		~AObjectPool() = default;


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

				// chain to the next free block
				PoolBlock& block = std::get<PoolBlock>(m_objects[availableID]);

				m_firstFreeBlock = block.NextFreeBlock;

				m_objects[availableID].template emplace<ValueType>(std::forward<Ts>(args)...);
			}
			else
			{
				availableID = this->Underlying().Append(std::forward<Ts>(args)...);
			}


			return availableID;
		}


		void	FreeImpl(uint32_t freedObjectID)
		{
			// chain to the next free block
			m_objects[freedObjectID].template emplace<PoolBlock>(m_firstFreeBlock);
			m_firstFreeBlock = freedObjectID;

		}


		[[nodiscard]] const ValueType& GetImpl(uint32_t id) const
		{
			return std::get<ValueType>(m_objects[id]);
		}

		[[nodiscard]] ValueType& MutImpl(uint32_t id)
		{
			return std::get<ValueType>(m_objects[id]);
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

	};


	template<typename TObj>
	class DynamicObjectPool : public AObjectPool<std::vector<PoolBlockVar<TObj>>, TObj, DynamicObjectPool<TObj>>
	{
		using Base = ObjectPool<TObj, DynamicObjectPool<TObj>>;
		friend Base;

	public:

		DynamicObjectPool(uint32_t reserved = 0)
		{
			m_objects.reserve(reserved);
		}


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
			m_objects.emplace_back(std::in_place_type<TObj>, std::forward<Ts>(args)...);

			return id;
		}


	private:

		std::uint32_t	m_maxAllowedGrowth{ UINT32_MAX };
	};



	template<typename TObj, size_t TSize>
	class FixedObjectPool : public AObjectPool<std::array<PoolBlockVar<TObj>, TSize>, TObj, FixedObjectPool<TObj, TSize>>
	{
		using Base = ObjectPool<TObj, FixedObjectPool<TObj, TSize>>;
		friend Base;

	public:
		FixedObjectPool()
		{
			std::get<PoolBlock>(m_objects[0]) = INVALID_BLOCK;
			m_firstFreeBlock = 0;
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
			PoolBlock& block = std::get<PoolBlock>(m_objects[availableID]);

			if (block.NextFreeBlock == INVALID_BLOCK) // we were the last : bump the free block number
			{
				if (m_highestFreeBlock + 1 < TSize) // Do we need to push up the high water mark ?
				{
					m_highestFreeBlock++;
					m_firstFreeBlock = m_highestFreeBlock;
					// Don't forget to set the new high water mark's next to INVALID
					PoolBlock& highestBlock = std::get<PoolBlock>(m_objects[m_firstFreeBlock]);
					highestBlock.NextFreeBlock = INVALID_BLOCK;
				}
				else // this is the last available block : any subsequent Emplace without a Free will crash!
				{
					m_firstFreeBlock = INVALID_BLOCK;
				}
			}
			else // this block is going to be used: mark the next free block as the new first one
			{
				m_firstFreeBlock = block.NextFreeBlock;
			}

			m_objects[availableID].emplace<TObj>(std::forward<Ts>(args)...);

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
