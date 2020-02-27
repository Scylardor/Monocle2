// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include <memory>

#include "Graphics/ResourceFactory/ResourceFactory.h"

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


	private:

		std::unique_ptr<IGraphicsResourceFactory>	m_resourceFactory = nullptr;
	};


}