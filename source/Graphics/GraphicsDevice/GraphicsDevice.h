// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include <memory> // unique_ptr

#include "Graphics/ResourceFactory/ResourceFactory.h"

#include "Core/Misc/Types.h"

namespace moe
{
	/* An abstract graphics device whose sole purpose is to create and manage GPU resources. */
	class GraphicsDevice
	{
	public:

		template<typename TFactory, typename... Args>
		void	InitializeFactory(Args&&... args)
		{
			m_resourceFactory = std::make_unique<TFactory>(std::forward<Args>(args)...);
		}

		virtual void	UpdateBuffer(class DeviceBuffer& buffer, const byte_t* data, uint32_t sizeInBytes, uint32_t bufferOffsetInBytes = 0) = 0;

	private:

		std::unique_ptr<IGraphicsResourceFactory>	m_resourceFactory = nullptr;
	};


}