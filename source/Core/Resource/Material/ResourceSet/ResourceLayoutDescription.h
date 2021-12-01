// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "ResourceLayoutBindingDescription.h"
#include "Core/Containers/Vector/Vector.h"
#include "Core/Containers/Array/Array.h"


namespace moe
{

	/**
	 * \brief Description of a resource layout (this is mostly to comply with the Vulkan API).
	 * A resource layout describes which resources are used, at which shader stages.
	 * In a Material<->Material Instance workflow, a resource layout is the Material :
	 * it describes what the system will await as input, but does not provide any data itself.
	 * Other way to see it : in OpenGL, a Resource Layout would be, for a ResourceSet, the equivalent of a VAO for a VBO.
	 */
	class ResourceLayoutDescription
	{
	public:
		using SetNbr_t = uint16_t;

		using ResourceBindingVector = Vector<ResourceLayoutBindingDescription>;
		template <std::size_t N> using ResourceBindingArray = Array<ResourceLayoutBindingDescription, N>;


		ResourceLayoutDescription(ResourceBindingVector bindingsVec, SetNbr_t setNumber = 0) :
			m_setNumber(setNumber), m_bindings(std::move(bindingsVec))
		{}


		ResourceLayoutDescription(ResourceBindingVector&& bindingsVec, SetNbr_t setNumber = 0) :
			m_setNumber(setNumber), m_bindings(std::move(bindingsVec))
		{}


		template <std::size_t N>
		ResourceLayoutDescription(ResourceBindingArray<N>&& bindingsArray, SetNbr_t setNumber = 0) :
			m_setNumber(setNumber), m_bindings(bindingsArray.Begin(), bindingsArray.End())
		{}


		ResourceLayoutDescription(std::initializer_list<ResourceLayoutBindingDescription> il, SetNbr_t setNumber = 0) :
			m_setNumber(setNumber), m_bindings(il) {}


		ResourceLayoutDescription(size_t reserved)
		{
			m_bindings.Reserve(reserved);
		}

		void	Push(ResourceLayoutBindingDescription desc)
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

		auto	GetResourceSetNumber() const
		{
			return m_setNumber;
		}


	private:
		SetNbr_t				m_setNumber{ 0 };	// Descriptor set number used for APIs like Vulkan.
		ResourceBindingVector	m_bindings;			// The bindings (e.g. textures, UBO, SSBO...) layout used by this descriptor set.
	};

}