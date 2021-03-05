#pragma once

#include <memory> // unique_ptr


#include "GameFramework/Resources/Factories/ResourceFactory.h"


namespace moe
{
	enum class ResourceManagerFactories
	{
		Texture = 0,
		Shader,
		Mesh,
		Audio,
		File,
		Custom1,
		Custom2,
		Custom3,
		Custom4,
		Custom5,
		Custom6,
		Custom7,
		_MAX_
	};


	class ResourceManager
	{
	public:
		ResourceManager()
		{
			m_factories.resize((size_t) ResourceManagerFactories::_MAX_);
		}


		template <typename TFactory>
		void	AddFactory(ResourceManagerFactories factoryType, TFactory&& factory)
		{
			static_assert(std::is_base_of_v<IResourceFactory, TFactory>, "TFactory must be a subclass of IResourceFactory.");
			if (factoryType == ResourceManagerFactories::_MAX_)
			{
				MOE_ASSERT(false);
				return;
			}

			m_factories[(size_t)factoryType] = std::make_unique<TFactory>(factory);
		}


		template <typename TFactory, typename... Args>
		void	EmplaceFactory(ResourceManagerFactories factoryType, Args&&... args)
		{
			static_assert(std::is_base_of_v<IResourceFactory, TFactory>, "TFactory must be a subclass of IResourceFactory.");
			if (factoryType == ResourceManagerFactories::_MAX_)
			{
				MOE_ASSERT(false);
				return;
			}

			m_factories[(size_t)factoryType] = std::make_unique<TFactory>(*this, std::forward<Args>(args)...);
		}


		template <typename TResource, typename... Args>
		TResource* CreateResource(Args&&... args);



	protected:



	private:

		template <typename TFactory>
		TFactory*	EditFactory(ResourceManagerFactories factoryType)
		{
			static_assert(std::is_base_of_v<IResourceFactory, TFactory>, "TFactory must be a subclass of IResourceFactory.");
			TFactory* factory = static_cast<TFactory*>(m_factories[(size_t)factoryType].get());
			MOE_ASSERT(factory != nullptr);
			return factory;
		}


		std::vector<std::unique_ptr<IResourceFactory>>	m_factories;

		std::vector<IResource*>	m_resources;
	};

}
