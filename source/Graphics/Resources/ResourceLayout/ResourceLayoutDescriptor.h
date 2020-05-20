// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Containers/Vector/Vector.h"
#include "Core/Containers/Array/Array.h"

#include "ResourceLayoutBindingDescriptor.h"

namespace moe
{

	/**
	 * \brief Description of a resource layout (this is mostly to comply with the Vulkan API).
	 * A resource layout describes which resources are used, at which shader stages.
	 * In a Material<->Material Instance workflow, a resource layout is the Material :
	 * it describes what the system will await as input, but does not provide any data itself.
	 * Other way to see it : in OpenGL, a Resource Layout would be, for a ResourceSet, the equivalent of a VAO for a VBO.
	 */
	class ResourceLayoutDescriptor
	{
	public:
		using ResourceBindingVector = Vector<ResourceLayoutBindingDescriptor>;
		template <std::size_t N> using ResourceBindingArray = Array<ResourceLayoutBindingDescriptor, N>;


		ResourceLayoutDescriptor(ResourceBindingVector bindingsVec) :
			m_bindings(std::move(bindingsVec))
		{}


		ResourceLayoutDescriptor(ResourceBindingVector&& bindingsVec) :
			m_bindings(std::move(bindingsVec))
		{}


		template <std::size_t N>
		ResourceLayoutDescriptor(ResourceBindingArray<N>&& bindingsArray) :
			m_bindings(bindingsArray.Begin(), bindingsArray.End())
		{}


		ResourceLayoutDescriptor(std::initializer_list<ResourceLayoutBindingDescriptor> il) :
			m_bindings(il) {}


		ResourceLayoutDescriptor(size_t reserved)
		{
			m_bindings.Reserve(reserved);
		}

		void	Push(ResourceLayoutBindingDescriptor desc)
		{
			m_bindings.PushBack(std::move(desc));
		}

		template <typename... Args>
		void	Emplace(Args&&... args)
		{
			m_bindings.EmplaceBack(std::forward<Args>(args)...);
		}


		//C++11 range for interface implementation
		ResourceBindingVector::Iterator	begin()
		{
			return m_bindings.Begin();
		}


		ResourceBindingVector::ConstIterator	begin() const
		{
			return m_bindings.Begin();
		}


		ResourceBindingVector::Iterator	end()
		{
			return m_bindings.End();
		}

		ResourceBindingVector::ConstIterator	end() const
		{
			return m_bindings.End();
		}

		ResourceBindingVector::SizeType	NumBindings() const { return m_bindings.Size(); }


	private:
		ResourceBindingVector	m_bindings;
	};

}