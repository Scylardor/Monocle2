// Monocle Game Engine source files - Alexandre Baron

#pragma once


#include "Core/Resource/Material/Shader/Module/ShaderModuleDescriptor.h"

#include <Core/Containers/Vector/Vector.h>


namespace moe
{


	/**
	 * \brief A graphics-API agnostic Description of a shader program.
	 * It basically encapsulates an array of individual shader module Descriptions.
	 */
	struct ShaderProgramDescription
	{
		using ShaderModuleVec = Vector<ShaderModuleDescription>;

		ShaderProgramDescription() = default;
		ShaderProgramDescription(ShaderModuleVec::SizeType reserved)
		{
			m_modules.Reserve(reserved);
		}

		ShaderProgramDescription(std::initializer_list< ShaderModuleDescription > il) :
			m_modules(il)
		{}


		ShaderModuleVec::SizeType	Count() const
		{
			return m_modules.Size();
		}


		// C++11 range-based for loops interface
		[[nodiscard]] Vector<ShaderModuleDescription>::Iterator begin()
		{
			return m_modules.begin();
		}
		[[nodiscard]] Vector<ShaderModuleDescription>::ConstIterator begin() const
		{
			return m_modules.begin();
		}

		[[nodiscard]] Vector<ShaderModuleDescription>::Iterator end()
		{
			return m_modules.end();
		}

		[[nodiscard]] Vector<ShaderModuleDescription>::ConstIterator end() const
		{
			return m_modules.end();
		}



		Vector<ShaderModuleDescription>	m_modules;
	};
}