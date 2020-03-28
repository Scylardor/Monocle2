// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Misc/Types.h"

namespace moe
{
	/**
	 * \brief A handle that a Renderer can use to uniquely identify a shader.
	 * Because graphics APIs have vastly different ways to handle shaders, using an opaque handle in our public interfaces
	 * is the easiest option we have (instead of pointers to virtual interfaces, for example).
	 * Each renderer is then free to reinterpret the handle data to retrieve the actual shader program.
	 */
	struct MOE_DLL_API ShaderProgramHandle
	{
		using Handle = std::uint64_t;

		Handle	m_handle{ 0 };

		static const ShaderProgramHandle&	Null() { return ms_NullHandle; }

	private:
		static const ShaderProgramHandle	ms_NullHandle;
	};

}
