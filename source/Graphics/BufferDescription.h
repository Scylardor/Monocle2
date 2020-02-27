// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Graphics/DeviceBuffer/BufferUsage.h"

namespace moe
{
	/*
		BufferDescription is a small structure you pass to a graphics device to generate a DeviceBuffer with wanted features.
	*/
	struct BufferDescription
	{
		uint32_t	m_size = 0;
		BufferUsage	m_usage = BufferUsage::None;
	};


}