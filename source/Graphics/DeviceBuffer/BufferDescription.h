// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Graphics/DeviceBuffer/BufferUsage.h"

namespace moe
{
	/*
		BufferStorageDescription is a small structure you pass to a graphics device to generate a DeviceBuffer with wanted features.
	*/
	struct BufferStorageDescription
	{
		const byte_t*	m_data = nullptr;
		uint32_t		m_size = 0;
		BufferUsage		m_bufferUsage{ BufferUsage::None };
	};


	struct VertexBufferStorageDescription : public BufferStorageDescription
	{
		VertexBufferStorageDescription(const byte_t* data, uint32_t size, BufferUsage additionalFlags) :
			BufferStorageDescription{data, size, additionalFlags | (BufferUsage)BufferUsage::VertexBuffer }
		{}

	};
}