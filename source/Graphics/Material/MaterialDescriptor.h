// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Containers/Vector/Vector.h"

#include "Graphics/Shader/ShaderStage/ShaderStage.h"

#include <string>

#include <utility> // std::pair

namespace moe
{
	class MaterialDescriptor
	{
	public:
		using BindingsVec = Vector<std::pair<std::string, ShaderStage>>;


		MaterialDescriptor() = default;

		MaterialDescriptor(std::initializer_list<std::pair<std::string, ShaderStage >> il) :
			m_bindingList(il)
		{}


		[[nodiscard]] BindingsVec::ConstIterator	begin() const
		{
			return m_bindingList.begin();
		}


		[[nodiscard]] BindingsVec::ConstIterator	end() const
		{
			return m_bindingList.end();
		}


		[[nodiscard]] size_t	BindingsNum() const { return m_bindingList.Size(); }


		void	AddBinding(const std::string& bindingName, ShaderStage wantedStage)
		{
			m_bindingList.EmplaceBack(bindingName, wantedStage);
		}

	private:
		Vector<std::pair<std::string, ShaderStage>>	m_bindingList;
	};



	}