// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "BufferUsage.h"
#include "BufferDescription.h"

#include "Core/Preprocessor/moeDebug.h"

namespace moe
{
	class DeviceBuffer
	{
	public:
		DeviceBuffer(BufferUsage usage);

		virtual ~DeviceBuffer() = default;

		void	AddUsage(BufferUsage usage);

		void	SetUsage(BufferUsage usage);

		void	RemoveUsage(BufferUsage usage);

		void	VerifyUsage() const MOE_NOT_DEBUG({});

		virtual void	UpdateData(const BufferStorageDescription& desc) = 0;

	private:


		BufferUsage	m_usage{ BufferUsage::None };
	};


}