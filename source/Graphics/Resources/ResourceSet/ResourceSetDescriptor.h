// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Containers/Vector/Vector.h"
#include "Core/Containers/Array/Array.h"

#include "ResourceHandle.h"
#include "Graphics/Resources/ResourceLayout/ResourceLayoutHandle.h"

namespace moe
{

	/**
	 * \brief
	 */
	class ResourceSetDescriptor
	{

	public:
		using ResourceHandleVector = Vector<ResourceHandle>;
		template <std::size_t N> using ResourceHandleArray = Array<ResourceHandle, N>;


		ResourceSetDescriptor(ResourceLayoutHandle layoutHandle, ResourceHandleVector rscHandlesVec) :
			m_boundLayout(layoutHandle), m_rscHandles(std::move(rscHandlesVec))
		{}


		ResourceSetDescriptor(ResourceLayoutHandle layoutHandle, ResourceHandleVector&& rscHandlesVec) :
			m_boundLayout(layoutHandle), m_rscHandles(std::move(rscHandlesVec))
		{}


		template <std::size_t N>
		ResourceSetDescriptor(ResourceLayoutHandle layoutHandle, ResourceHandleArray<N>&& rscHandlesArray) :
			m_boundLayout(layoutHandle), m_rscHandles(rscHandlesArray.Begin(), rscHandlesArray.End())
		{}


		ResourceSetDescriptor(ResourceLayoutHandle layoutHandle, std::initializer_list<ResourceHandle> il) :
			m_boundLayout(layoutHandle), m_rscHandles(il) {}



		//C++11 range for interface implementation
		[[nodiscard]] ResourceHandleVector::Iterator	begin()
		{
			return m_rscHandles.Begin();
		}


		[[nodiscard]] ResourceHandleVector::ConstIterator	begin() const
		{
			return m_rscHandles.Begin();
		}


		[[nodiscard]] ResourceHandleVector::Iterator	end()
		{
			return m_rscHandles.End();
		}

		[[nodiscard]] ResourceHandleVector::ConstIterator	end() const
		{
			return m_rscHandles.End();
		}

		ResourceLayoutHandle	GetResourceLayoutHandle() const { return m_boundLayout; }


		template <typename T>
		auto	Get(int rscIndex) const
		{
			MOE_ASSERT(rscIndex < m_rscHandles.Size());
			return m_rscHandles[rscIndex].Get<T>();
		}

	private:
		ResourceLayoutHandle	m_boundLayout{0};
		ResourceHandleVector	m_rscHandles;

	};
}