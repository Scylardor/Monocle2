// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Graphics/DeviceBuffer/DeviceBufferHandle.h"
#include "Graphics/Texture/TextureHandle.h"


namespace moe
{

	/**
	 * \brief This is a generic resource handle meant to contain any handle to a resource used by a resource set.
	 */
	union ResourceHandle
	{
		ResourceHandle(DeviceBufferHandle bufferHandle) :
			m_ubHandle(bufferHandle)
		{}

		ResourceHandle(TextureHandle texHandle) :
			m_texHandle(texHandle)
		{}

		template <typename T>
		auto	Get() const;


		DeviceBufferHandle	m_ubHandle;
		TextureHandle		m_texHandle;
	};


	template <>
	inline auto ResourceHandle::Get<DeviceBufferHandle>() const
	{
		return m_ubHandle;
	}

	template <>
	inline auto ResourceHandle::Get<TextureHandle>() const
	{
		return m_texHandle;
	}
}
