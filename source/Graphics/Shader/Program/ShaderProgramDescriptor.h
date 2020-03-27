// Monocle Game Engine source files - Alexandre Baron

#pragma once


#include "Graphics/Shader/Module/ShaderModuleDescriptor.h"

#include <Core/Containers/Vector/Vector.h>


namespace moe
{


	struct ShaderProgramDescriptor
	{
		using ShaderModuleVec = Vector<ShaderModuleDescriptor>;

		ShaderModuleVec::SizeType	Count() const
		{
			return m_modules.Size();
		}


		// C++11 range-based for loops interface
		[[nodiscard]] Vector<ShaderModuleDescriptor>::Iterator begin()
		{
			return m_modules.begin();
		}
		[[nodiscard]] Vector<ShaderModuleDescriptor>::ConstIterator begin() const
		{
			return m_modules.begin();
		}

		[[nodiscard]] Vector<ShaderModuleDescriptor>::Iterator end()
		{
			return m_modules.end();
		}

		[[nodiscard]] Vector<ShaderModuleDescriptor>::ConstIterator end() const
		{
			return m_modules.end();
		}



		Vector<ShaderModuleDescriptor>	m_modules;
	};
}