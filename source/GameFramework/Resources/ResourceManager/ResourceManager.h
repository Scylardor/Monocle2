#pragma once

#include "Core/HashString/HashString.h"
#include "GameFramework/Resources/AssetImporter/AssimpAssetImporter.h"
#include "Core/Resource/ResourceFactory.h"

#include "ResourceRef.h"

#include <memory> // unique_ptr

#include <optional>


namespace moe
{
	class ITextureFactory;
	class IMeshFactory;

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


	class ResourceManager : public IResourceManager
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
			static_assert(std::is_base_of_v<IAssetImporter, TImporter>, "Only supports derived classes of IAssetImporter.");
			m_assetImporter = std::make_unique<TImporter>(*this, std::forward<Ts>(args)...);
			MOE_ASSERT(m_assetImporter);
			return static_cast<TImporter&>(*m_assetImporter.get());
		}


		MeshResource LoadMesh(std::string_view meshID, size_t vertexSize, size_t numVertices, const void* vertexData,
		                      size_t numIndices, const void* indexData, vk::IndexType indexType);



		void SetMeshFactory(IMeshFactory& factory)
		{
			m_meshFactory = &factory;
		}


		void SetTextureFactory(ITextureFactory& factory)
		{
			m_textureFactory = &factory;
		}


		void	RemoveResource(const HashString& hash) override
		{
			m_resourceIDs.erase(hash);
		}



		template <typename TResource, typename TFactory, typename... Args>
		Ref<TResource> Load(HashString rscHandle, TFactory& factory, Args&&... args)
		{
			// if it already exists :
			std::optional<Ref<TResource>> existingRef = FindResource<TResource>(rscHandle);
			if (existingRef.has_value())
			{
				Ref<TResource>&& rsc = std::move(existingRef.value());
				return rsc;
			}

			// if it needs to be created:
			std::unique_ptr<TResource> newResource = factory(std::forward<Args>(args)...);
			MOE_ASSERT(newResource != nullptr);
			TResource* rsc = newResource.get(); // get the ptr before it gets moved
			auto entryID = m_resourcesData.EmplaceEntry(std::move(newResource));

			// Make sure the bookkeeping is uptodate
			m_rscHandleToID.emplace(rscHandle, entryID);
			m_rscIDToHandle.emplace(entryID, rscHandle);

			return Ref(*this, *rsc, entryID);
		}


		uint32_t	IncrementReference(RegistryID rscID)
		{
			return m_resourcesData.IncrementReference(rscID);
		}


		void	DecrementReference(RegistryID rscID)
		{
			bool deleted = m_resourcesData.DecrementReference(rscID);
			if (deleted)
			{
				// remove the hash string references too.
				auto handleIt = m_rscIDToHandle.find(rscID);
				MOE_ASSERT(handleIt != m_rscIDToHandle.end());
				m_rscHandleToID.erase(handleIt->second);
				m_rscIDToHandle.erase(handleIt);
			}
		}


	protected:



	private:

		template <typename TRsc>
		std::optional<Ref<TRsc>>	FindResource(const HashString& rscHandle)
		{
			auto idIt = m_rscHandleToID.find(rscHandle);
			if (idIt != m_rscHandleToID.end())
			{
				const auto& rscPtr = m_resourcesData.GetEntry(idIt->second);
				TRsc* rsc = static_cast<TRsc*>(rscPtr.get());
				return Ref<TRsc>(*this, *rsc, idIt->second);
			}

			return {};
		}




		template <typename TFactory>
		TFactory*	EditFactory(ResourceType factoryType)
		{
			static_assert(std::is_base_of_v<IResourceFactory, TFactory>, "TFactory must be a subclass of IResourceFactory.");
			TFactory* factory = static_cast<TFactory*>(m_factories[(size_t)factoryType].get());
			MOE_ASSERT(factory != nullptr);
			return factory;
		}


		using CreateResourceFunc = std::function<RegistryID ()>;
		template <typename TRsc, typename TRscFactory>
		TRsc FindOrCreateResource(std::string_view rscIdentifier, TRscFactory* factory, CreateResourceFunc createFunc);


		std::vector<std::unique_ptr<IResourceFactory>>	m_factories;

		IMeshFactory*		m_meshFactory{nullptr};
		ITextureFactory*	m_textureFactory{ nullptr };
		std::unique_ptr<IAssetImporter>	m_assetImporter{nullptr};

		std::unordered_map<HashString, RegistryID>	m_resourceIDs{};

		std::unordered_map<HashString, RegistryID>		m_rscHandleToID{};
		std::unordered_map<RegistryID, HashString>		m_rscIDToHandle{};
		ObjectRegistry<std::unique_ptr<IBaseResource>>	m_resourcesData{};
	};



	template <typename TRsc, typename TRscFactory>
	TRsc ResourceManager::FindOrCreateResource(std::string_view rscIdentifier, TRscFactory* factory,
	                                           CreateResourceFunc createFunc)
	{
		if (factory == nullptr)
			return {};

		auto HashedStr = HashString(rscIdentifier);
		const auto rscIt = m_resourceIDs.find(HashedStr);

		RegistryID rscID{INVALID_ENTRY};

		if (rscIt != m_resourceIDs.end())
		{
			rscID = rscIt->second;
		}
		else
		{
			rscID = createFunc();
			m_resourceIDs.emplace(HashedStr, rscID);
		}

		return TRsc{ *this, std::move(HashedStr), *factory, rscID};
	}




}
