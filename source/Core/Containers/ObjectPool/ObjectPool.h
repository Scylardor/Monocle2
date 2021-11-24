// Monocle source files - Alexandre Baron

#pragma once

#include "Core/Preprocessor/moeAssert.h"
#include "Core/Misc/moeCRTP.h"

#include <vector>
#include <array>
#include <variant>

namespace moe
{
	using ObjectPoolID = uint32_t;

	static ObjectPoolID const INVALID_ID = UINT32_MAX;

	struct PoolBlock
	{
		PoolBlock() = default;
		PoolBlock(ObjectPoolID nfb) :
			NextFreeBlock(nfb)
		{}

		ObjectPoolID	NextFreeBlock{ INVALID_ID };

	};


	template <typename TObj>
	using PoolBlockVar = std::variant<PoolBlock, TObj>;


	template<typename T, typename Enable = void>
	struct IsUniquePointer
	{
		enum { value = false };
	};

	template<typename T>
	struct IsUniquePointer<T,
		typename std::enable_if<std::is_same<typename std::remove_cv<T>::type, std::unique_ptr<typename T::element_type>>::value>::type>
	{
		enum { value = true };
	};

	template<typename TObj, typename ValueType, typename Enable = void>
	struct IsPolymorphicCompatible
	{
		enum { value = false };
	};

	template<typename ValueType, typename TObj>
	struct IsPolymorphicCompatible<ValueType, TObj,
		typename std::enable_if<
			IsUniquePointer<ValueType>::value
			&& (std::is_abstract_v<typename ValueType::element_type> || std::is_polymorphic_v<typename ValueType::element_type>)
			&& std::is_base_of_v<typename ValueType::element_type, TObj>>::type>
	{
		enum { value = true };
	};


	template <typename ValueType, class Top>
	class ObjectPool : public CRTP<Top>
	{
	public:

		struct PoolRef
		{
		public:
			PoolRef(ObjectPool& owner, ObjectPoolID id) :
				m_owner(&owner),
				m_id(id)
			{}

			~PoolRef() = default;

			template <typename = std::enable_if_t<IsUniquePointer<ValueType>::value>>
			typename ValueType::element_type* operator->()
			{
				auto& uniquePtr = m_owner->Mut(m_id);
				return uniquePtr.get();
			}

			template <typename = std::enable_if_t<IsUniquePointer<ValueType>::value>>
			typename ValueType::element_type const* operator->() const
			{
				auto const& uniquePtr = m_owner->Get(m_id);
				return uniquePtr.get();
			}

			template <typename = std::enable_if_t<IsUniquePointer<ValueType>::value>>
			typename ValueType::element_type* operator*()
			{
				auto& uniquePtr = m_owner->Mut(m_id);
				return uniquePtr.get();
			}

			template <typename = std::enable_if_t<IsUniquePointer<ValueType>::value>>
			typename ValueType::element_type const* operator*() const
			{
				auto& uniquePtr = m_owner->Get(m_id);
				return uniquePtr.get();
			}

			template <typename T>
			ValueType* operator->()
			{
				return &m_owner->Mut(m_id);
			}

			template <typename T>
			ValueType const* operator->() const
			{
				return &m_owner->Get(m_id);
			}

			template <typename T>
			ValueType& operator*()
			{
				return m_owner->Mut(m_id);
			}

			template <typename T>
			ValueType const& operator*() const
			{
				return m_owner->Get(m_id);
			}

			[[nodiscard]] auto	GetID() const
			{
				return m_id;
			}

		protected:
			ObjectPool* m_owner;
			ObjectPoolID	m_id;

		};

		struct UniqueRef :  PoolRef
		{
			UniqueRef(ObjectPool& owner, ObjectPoolID id) :
				PoolRef(owner, id)
			{}

			~UniqueRef()
			{
				m_owner->Free(m_id);
			}
		};

		// The "pure virtual functions"

		template <typename TObj = ValueType, typename... Ts>
		[[nodiscard]] ObjectPoolID	Emplace(Ts&&... args)
		{
			static_assert(std::is_same_v<TObj, ValueType> || IsPolymorphicCompatible<ValueType, TObj>::value);
			auto id = MOE_CRTP_IMPL_VARIADIC_TEMPLATE(Emplace, TObj, Ts, args);
			return id;
		}

		template <typename TObj = ValueType, typename... Ts>
		[[nodiscard]] UniqueRef	EmplaceRef(Ts&&... args)
		{
			static_assert(std::is_same_v<TObj, ValueType> || IsPolymorphicCompatible<ValueType, TObj>::value);
			auto id = MOE_CRTP_IMPL_VARIADIC_TEMPLATE(Emplace, TObj, Ts, args);
			return { *this, id };
		}


		void	Free(ObjectPoolID freedObjectID)
		{
			MOE_CRTP_IMPL(Free, freedObjectID);
		}

		void	FreeRef(PoolRef const& poolRef)
		{
			MOE_CRTP_IMPL(Free, poolRef.GetID());
		}


		[[nodiscard]] PoolRef MutRef(ObjectPoolID id)
		{
			return { *this, id };
		}

		[[nodiscard]] const ValueType& Get(ObjectPoolID id) const
		{
			return MOE_CRTP_IMPL(Get, id);
		}

		[[nodiscard]] ValueType& Mut(ObjectPoolID id)
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

		template <typename TObj = ValueType, typename... Ts>
		[[nodiscard]] ObjectPoolID	Append(Ts&&... args)
		{
			return MOE_CRTP_IMPL_VARIADIC_TEMPLATE(Append, TObj, Ts, args);
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
			return m_firstFreeBlock != INVALID_ID;
		}

		template <typename TObj, typename... Ts>
		[[nodiscard]] ObjectPoolID	EmplaceImpl(Ts&&... args)
		{
			ObjectPoolID availableID;
			if (HasFreeBlock())
			{
				availableID = m_firstFreeBlock;

				// chain to the next free block
				PoolBlock& block = std::get<PoolBlock>(m_objects[availableID]);

				m_firstFreeBlock = block.NextFreeBlock;

				if constexpr (IsUniquePointer<ValueType>::value)
				{
					if constexpr (std::is_same_v<ValueType, TObj>)
					{
						m_objects[availableID].template emplace<ValueType>(std::make_unique<ValueType::element_type>(std::forward<Ts>(args)...));
					}
					else
					{
						m_objects[availableID].template emplace<ValueType>(std::make_unique<TObj>(std::forward<Ts>(args)...));
					}
				}
				else
				{
					m_objects[availableID].template emplace<ValueType>(std::forward<Ts>(args)...);
				}
			}
			else
			{
				availableID = this->Underlying().template Append<TObj>(std::forward<Ts>(args)...);
			}

			return availableID;
		}


		void	FreeImpl(ObjectPoolID freedObjectID)
		{
			// chain to the next free block
			m_objects[freedObjectID].template emplace<PoolBlock>(m_firstFreeBlock);
			m_firstFreeBlock = freedObjectID;

		}


		[[nodiscard]] const ValueType& GetImpl(ObjectPoolID id) const
		{
			return std::get<ValueType>(m_objects[id]);
		}

		[[nodiscard]] ValueType& MutImpl(ObjectPoolID id)
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

		template <typename TObj, typename... Ts>
		[[nodiscard]] ObjectPoolID	AppendImpl(Ts&&...)
		{
			MOE_ASSERT(false); // unimplemented!
			return 0;
		}

		Container				m_objects;
		ObjectPoolID				m_firstFreeBlock{ INVALID_ID };

	};



	template<typename TObj, class Top>
	class VectorObjectPool : public AObjectPool<std::vector<PoolBlockVar<TObj>>, TObj, Top>
	{
		using Base = ObjectPool<TObj, Top>;
		friend Base;

	public:

		VectorObjectPool(ObjectPoolID reserved = 0)
		{
			m_objects.reserve(reserved);
		}


		void	SetMaximumAllowedGrowth(ObjectPoolID maxGrowth)
		{
			m_maxAllowedGrowth = maxGrowth;
		}

	protected:
		template <typename ValueType, typename... Ts>
		[[nodiscard]] ObjectPoolID	AppendImpl(Ts&&... args)
		{
			if (m_objects.size() == GetMaxAllowedGrowth())
				return INVALID_ID;

			auto id = (ObjectPoolID)m_objects.size();

			if constexpr (IsUniquePointer<TObj>::value)
			{
				if constexpr (std::is_same_v<TObj, ValueType>)
				{
					m_objects.emplace_back(std::in_place_type<TObj>, std::make_unique<TObj::element_type>(std::forward<Ts>(args)...));
				}
				else
				{
					m_objects.emplace_back(std::in_place_type<TObj>, std::make_unique<ValueType>(std::forward<Ts>(args)...));
				}
			}
			else
			{
				m_objects.emplace_back(std::in_place_type<TObj>, std::forward<Ts>(args)...);
			}

			return id;
		}

		auto	GetMaxAllowedGrowth() const
		{
			return m_maxAllowedGrowth;
		}

	private:

		uint32_t	m_maxAllowedGrowth{ UINT32_MAX };
	};


	template<typename TObj>
	class DynamicObjectPool : public VectorObjectPool<TObj, DynamicObjectPool<TObj>>
	{
		using Base = ObjectPool<TObj, DynamicObjectPool<TObj>>;
		friend Base;

	public:

		DynamicObjectPool(ObjectPoolID reserved = 0) :
			VectorObjectPool(reserved)
		{}
	};


	template<typename TObj>
	class PolymorphicObjectPool : public VectorObjectPool<std::unique_ptr<TObj>, PolymorphicObjectPool<TObj>>
	{
		using ObjPtr = std::unique_ptr<TObj>;

		using Base = ObjectPool<ObjPtr, PolymorphicObjectPool<TObj>>;
		friend Base;


	public:

		PolymorphicObjectPool(ObjectPoolID reserved = 0) :
			VectorObjectPool(reserved)
		{}

	protected:



	};



	template<typename TObj, size_t TSize>
	class FixedObjectPool : public AObjectPool<std::array<PoolBlockVar<TObj>, TSize>, TObj, FixedObjectPool<TObj, TSize>>
	{
		using Base = ObjectPool<TObj, FixedObjectPool<TObj, TSize>>;
		friend Base;

	public:
		FixedObjectPool()
		{
			std::get<PoolBlock>(m_objects[0]) = INVALID_ID;
			m_firstFreeBlock = 0;
		}

	protected:

		template <typename... Ts>
		[[nodiscard]] ObjectPoolID	EmplaceImpl(Ts&&... args)
		{
			ObjectPoolID availableID{ 0 };

			if (false == HasFreeBlock())
			{
				MOE_FATAL(moe::ChanDefault, "FixedObjectPool capacity overflown ! You need to increase the capacity of the pool.");
				MOE_ASSERT(false); // Object pool overflow
				return UINT32_MAX;
			}

			availableID = m_firstFreeBlock;

			// chain to the next free block
			PoolBlock& block = std::get<PoolBlock>(m_objects[availableID]);

			if (block.NextFreeBlock == INVALID_ID) // we were the last : bump the free block number
			{
				if (m_highestFreeBlock + 1 < TSize) // Do we need to push up the high water mark ?
				{
					m_highestFreeBlock++;
					m_firstFreeBlock = m_highestFreeBlock;
					// Don't forget to set the new high water mark's next to INVALID
					PoolBlock& highestBlock = std::get<PoolBlock>(m_objects[m_firstFreeBlock]);
					highestBlock.NextFreeBlock = INVALID_ID;
				}
				else // this is the last available block : any subsequent Emplace without a Free will crash!
				{
					m_firstFreeBlock = INVALID_ID;
				}
			}
			else // this block is going to be used: mark the next free block as the new first one
			{
				m_firstFreeBlock = block.NextFreeBlock;
			}

			m_objects[availableID].emplace<TObj>(std::forward<Ts>(args)...);

			return availableID;
		}

		template <typename ValueType, typename... Ts>
		[[nodiscard]] ObjectPoolID	AppendImpl(Ts&&...)
		{
			MOE_ASSERT(false); // unimplemented!
			return 0;
		}


		ObjectPoolID	m_highestFreeBlock = 0;

	};



}
