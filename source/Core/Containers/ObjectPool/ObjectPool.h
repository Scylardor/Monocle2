#pragma once

#include "Core/Preprocessor/moeAssert.h"

#include <vector>
#include <array>

template <typename Derived>
struct CRTP
{
protected:

	Derived&		Underlying()		{ return static_cast<Derived&>(*this); }
	Derived const&	Underlying() const	{ return static_cast<Derived const&>(*this); }

	// To protect from misuse
	CRTP() {}
	friend Derived;
};


#define MOE_CRTP_IMPL(Method, ...) \
	this->Underlying().Method##Impl(##__VA_ARGS__)

#define MOE_CRTP_IMPL_VARIADIC(Method, Ts, args) \
	this->Underlying().Method##Impl(std::forward<Ts>(args)...)

namespace moe
{

	static uint32_t const INVALID_BLOCK = UINT32_MAX;

	template <typename TObj>
	union PoolBlock
	{
		PoolBlock() = default;

	/*	template <typename... Ts>
		PoolBlock(Ts&&... args) :
			Object(std::forward<Ts>(args)...)
		{}*/


		~PoolBlock()
		{}
		//{
		//	Object.~TObj();
		//}

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
			return MOE_CRTP_IMPL(Size,);
		}


		[[nodiscard]] bool	Empty() const
		{
			return MOE_CRTP_IMPL(Empty,);
		}


	protected:

		template <typename... Ts>
		[[nodiscard]] uint32_t	Append(Ts&&... args)
		{
			return MOE_CRTP_IMPL_VARIADIC(Append, Ts, args);
		}

	};


	template <typename Container, typename ValueType, class Top>
	class AObjectPool : public SparseArray<ValueType, Top>
	{
		using Base = SparseArray<ValueType, Top>;
		friend Base;


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

				//m_freeIndices.pop();
				auto* block = (PoolBlock<ValueType>*) & m_objects[availableID];

				m_firstFreeBlock = block->NextFreeBlock;

				new (&m_objects[availableID]) ValueType(std::forward<Ts>(args)...);

				return availableID;
			}
			else
			{
				availableID = this->Underlying().Append(std::forward<Ts>(args)...);
			}

			return availableID;
		}

		void	FreeImpl(uint32_t freedObjectID)
		{
			auto* object = (ValueType*)&m_objects[freedObjectID];
			object->~ValueType();
			//		m_freeIndices.push(freedObjectID);
					// chain to the next free block
			auto* block = (PoolBlock<ValueType>*)object;
			block->NextFreeBlock = m_firstFreeBlock;

			m_firstFreeBlock = freedObjectID;
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
	};


	template<typename TObj>
	class DynamicObjectPool : public AObjectPool<std::vector<PoolBlock<TObj>>, TObj, DynamicObjectPool<TObj >>
	{
		using Base = SparseArray<TObj, DynamicObjectPool<TObj>>;
		friend Base;

	public:

		DynamicObjectPool(uint32_t reserved = 0)
		{
			m_objects.reserve(reserved);

		}

	protected:
		template <typename... Ts>
		[[nodiscard]] uint32_t	AppendImpl(Ts&&... args)
		{
			auto id = (uint32_t)m_objects.size();
			auto& block = m_objects.emplace_back();
			auto* object = (TObj*)&block;
			new (object) TObj(std::forward<Ts>(args)...);

			return id;
		}

	};

	template<typename TObj, size_t TSize>
	class FixedObjectPool : public AObjectPool<std::array<PoolBlock<TObj>, TSize>, TObj, FixedObjectPool<TObj, TSize>>
	{
		using Base = SparseArray<TObj, FixedObjectPool<TObj, TSize>>;
		friend Base;

	public:
		FixedObjectPool()
		{
			auto* block = (PoolBlock<TObj>*) & m_objects[0];
			block->NextFreeBlock = INVALID_BLOCK;
			m_firstFreeBlock = 0;
		}

	protected:

		template <typename... Ts>
		[[nodiscard]] uint32_t	EmplaceImpl(Ts&&... args)
		{
			uint32_t availableID{ 0 };
			if (HasFreeBlock())
			{
				availableID = m_firstFreeBlock;

				// chain to the next free block
				auto* block = (PoolBlock<TObj>*) & m_objects[availableID];

				if (block->NextFreeBlock == INVALID_BLOCK) // we were the last : bump the free block number
				{
					if (m_highestFreeBlock + 1 < TSize)
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
			}
			else
			{
				MOE_ASSERT(false); // object pool overflow !
			}

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
