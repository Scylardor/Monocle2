// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Misc/Types.h"

namespace moe
{
	struct FreelistObject
	{
		uint32_t	m_next{ 0 };
	};


	struct FreelistID
	{
	public:

		FreelistID() = default;

		FreelistID(uint32_t index) :
			m_index(index)
		{}

		[[nodiscard]] uint32_t	Index() const { return m_index; }


		/**
		 * \brief This function always returns index + 1 to avoid returning 0 on first ID, resulting in a null handle.
		 * \return
		 */
		template <typename HandleType>
		[[nodiscard]] HandleType	ToHandle() const { return HandleType{ HandleType::Underlying(m_index + 1) }; }

	private:

		uint32_t	m_index{ 0 };
	};


}