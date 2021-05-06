#pragma once

#include <memory> // unique_ptr



#include "GameFramework/Resources/AssetImporter/AssimpAssetImporter.h"
#include "Core/Resource/ResourceFactory.h"
#include "Graphics/Vulkan/Factories/VulkanMeshFactory.h"


namespace moe
{
	template <typename TRsc>
	class ResourceFactory
	{
	public:

		//virtual ~ResourceFactory() = default;

	protected:

		ObjectRegistry<TRsc>	m_registry;


	};




	enum class ResourceType
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
			m_factories.resize((size_t) ResourceType::_MAX_);
		}


		template <typename TFactory>
		void	AddFactory(ResourceType factoryType, TFactory&& factory)
		{
			static_assert(std::is_base_of_v<IResourceFactory, TFactory>, "TFactory must be a subclass of IResourceFactory.");
			if (factoryType == ResourceType::_MAX_)
			{
				MOE_ASSERT(false);
				return;
			}

			m_factories[(size_t)factoryType] = std::make_unique<TFactory>(factory);
		}


		template <typename TFactory, typename... Args>
		void	EmplaceFactory(ResourceType factoryType, Args&&... args)
		{
			static_assert(std::is_base_of_v<IResourceFactory, TFactory>, "TFactory must be a subclass of IResourceFactory.");
			if (factoryType == ResourceType::_MAX_)
			{
				MOE_ASSERT(false);
				return;
			}

			m_factories[(size_t)factoryType] = std::make_unique<TFactory>(*this, std::forward<Args>(args)...);
		}


		template <typename TResource, typename... Args>
		TResource* CreateResource(Args&&... args);


		template <typename TImporter, typename... Ts>
		TImporter&	EmplaceAssetImporter(Ts&&... args)
		{
			static_assert(std::is_base_of_v<AssetImporter, TImporter>, "Only supports derived classes of AssetImporter.");
			m_assetImporter = std::make_unique<TImporter>(*this, std::forward<Ts>(args)...);
			MOE_ASSERT(m_assetImporter);
			return static_cast<TImporter&>(*m_assetImporter.get());
		}


		template <typename TMesh>
		MeshResource	LoadMesh(size_t vertexSize, size_t numVertices, const void* vertexData,
			size_t numIndices, const void* indexData, vk::IndexType indexType)
		{
			if (m_meshFactory == nullptr)
				return {};

			auto ID = m_meshFactory->CreateMesh(vertexSize, numVertices, vertexData, numIndices, indexData, indexType); (void)ID;
			return {};
		}



		void SetMeshFactory(IMeshFactory& factory)
		{
			m_meshFactory = &factory;
		}


	protected:



	private:

		template <typename TFactory>
		TFactory*	EditFactory(ResourceType factoryType)
		{
			static_assert(std::is_base_of_v<IResourceFactory, TFactory>, "TFactory must be a subclass of IResourceFactory.");
			TFactory* factory = static_cast<TFactory*>(m_factories[(size_t)factoryType].get());
			MOE_ASSERT(factory != nullptr);
			return factory;
		}


		std::vector<std::unique_ptr<IResourceFactory>>	m_factories;




		IMeshFactory*	m_meshFactory{nullptr};

		std::unique_ptr<AssetImporter>	m_assetImporter{nullptr};
	};

}
