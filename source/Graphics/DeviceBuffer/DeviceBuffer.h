// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "BufferUsage.h"

#include "Core/Preprocessor/moeDebug.h"

namespace moe
{
	class DeviceBuffer
	{
	public:
		DeviceBuffer() = default;

		virtual ~DeviceBuffer() = default;

		void	AddUsage(BufferUsage usage);

		void	SetUsage(BufferUsage usage);

		void	RemoveUsage(BufferUsage usage);

		void	VerifyUsage() const MOE_NOT_DEBUG({});

	private:


		BufferUsage	m_usage{ BufferUsage::None };
	};


}