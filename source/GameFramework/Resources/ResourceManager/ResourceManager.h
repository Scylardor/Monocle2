#pragma once

#include <memory> // unique_ptr
#include <unordered_map>
#include <vector>

#include "GameFramework/Resources/ResourceFactory/ResourceFactory.h"
#include "GameFramework/Resources/ResourceFactory/TextureFactory.h"


class ResourceManager
{
public:

	ResourceManager() = default;

	ResourceManager(uint32_t nbrFactories)
	{
		m_factories.reserve(nbrFactories);
	}


	void	AddFactory(std::unique_ptr<IResourceFactory> factoryPtr)
	{
		m_factories.push_back(std::move(factoryPtr));
	}


	template <typename FactoryType, typename... Args>
	void	EmplaceFactory(Args&&... args)
	{
		static_assert(std::is_base_of_v<IResourceFactory, FactoryType>, "FactoryType must be a subclass of IResourceFactory.");

		m_factories.emplace_back(std::make_unique<FactoryType>(std::forward<Args>(args)...));
	}





protected:



private:

//	std::unique_ptr<TextureFactory>	m_textureFactory = nullptr;

	std::vector<std::unique_ptr<IResourceFactory>>	m_factories;


};
