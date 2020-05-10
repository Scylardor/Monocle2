// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Graphics/DeviceBuffer/UniformBufferHandle.h"
#include "Graphics/Texture/Texture2DHandle.h"


namespace moe
{

	/**
	 * \brief This is a generic resource handle meant to contain any handle to a resource used by a resource set.
	 */
	union ResourceHandle
	{
		ResourceHandle(UniformBufferHandle bufferHandle) :
			m_ubHandle(bufferHandle)
		{}

		ResourceHandle(Texture2DHandle tex2DHandle) :
			m_tex2DHandle(tex2DHandle)
		{}

		template <typename T>
		auto	Get() const;


		UniformBufferHandle	m_ubHandle;
		Texture2DHandle		m_tex2DHandle;
	};


	template <>
	inline auto ResourceHandle::Get<UniformBufferHandle>() const
	{
		return m_ubHandle;
	}

	template <>
	inline auto ResourceHandle::Get<Texture2DHandle>() const
	{
		return m_tex2DHandle;
	}
}
