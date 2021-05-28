#pragma once

#include "Core/HashString/HashString.h"
#include "Core/Resource/Resource.h"
#include "ResourceManagerInterface.h"

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
		ResourceManager() = default;




		template <typename TImporter, typename... Ts>
		TImporter& EmplaceAssetImporter(Ts&&... args)
		{
			static_assert(std::is_base_of_v<IAssetImporter, TImporter>, "Only supports derived classes of IAssetImporter.");
			m_assetImporter = std::make_unique<TImporter>(*this, std::forward<Ts>(args)...);
			MOE_ASSERT(m_assetImporter);
			return static_cast<TImporter&>(*m_assetImporter.get());
		}


		template <typename TRsc>
		Ref<TRsc>	Insert(const HashString& rscHandle, std::unique_ptr<TRsc>&& rsc)
		{
			static_assert(std::is_base_of_v<IBaseResource, TRsc>);

			MOE_ASSERT(rsc != nullptr);
			TRsc* rscPtr = rsc.get(); // get the ptr before it gets moved
			auto entryID = m_resourcesData.EmplaceEntry(std::move(rsc));

			// Make sure the bookkeeping is uptodate
			auto [_, inserted] = m_rscHandleToID.emplace(rscHandle, entryID);
			auto [__, inserted2] = m_rscIDToHandle.emplace(entryID, rscHandle);
			MOE_ASSERT(inserted && inserted2); // make sure there is no duplicate (or something is very wrong)

			return std::move(Ref(*this, *rscPtr, entryID));
		}


		template <typename TResource, typename TFactory, typename... Args>
		Ref<TResource> Load(const HashString& rscHandle, TFactory& factory, Args&&... args)
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

			return Insert(rscHandle, std::move(newResource));
		}


		enum class Persistent
		{
			No,
			Yes
		};

		template <typename TResource, typename FactoryFunc>
		Ref<TResource> Load(HashString rscHandle, FactoryFunc&& factoryFn, Persistent isPersistent = Persistent::No)
		{
			// if it already exists :
			std::optional<Ref<TResource>> existingRef = FindResource<TResource>(rscHandle);
			if (existingRef.has_value())
			{
				Ref<TResource>&& rsc = std::move(existingRef.value());
				return rsc;
			}

			// if it needs to be created:
			std::unique_ptr<TResource> newResource = factoryFn();
			MOE_ASSERT(newResource != nullptr);
			TResource* rsc = newResource.get(); // get the ptr before it gets moved
			auto entryID = isPersistent == Persistent::No ?
				m_resourcesData.EmplaceEntry(std::move(newResource)) :
				m_resourcesData.EmplacePersistentEntry(std::move(newResource));
			// Make sure the bookkeeping is uptodate
			m_rscHandleToID.emplace(rscHandle, entryID);
			m_rscIDToHandle.emplace(entryID, rscHandle);

			return Ref(*this, *rsc, entryID);
		}


		template <typename TRsc>
		[[nodiscard]] const TRsc&	GetResourceAs(RegistryID rscID) const
		{
			const auto& rscPtr = m_resourcesData.GetEntry(rscID);
			MOE_ASSERT(rscPtr);
			return static_cast<const TRsc&>(*rscPtr.get());
		}

		template <typename TRsc>
		[[nodiscard]] TRsc& MutResourceAs(RegistryID rscID)
		{
			auto& rscPtr = m_resourcesData.MutEntry(rscID);
			MOE_ASSERT(rscPtr);
			return static_cast<TRsc&>(*rscPtr.get());
		}


		uint32_t	IncrementReference(RegistryID rscID) override
		{
			return m_resourcesData.IncrementReference(rscID);
		}


		void	DecrementReference(RegistryID rscID) override
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


		template <typename TRsc>
		Ref<TRsc>	FindExisting(const HashString& rscHandle)
		{
			// Resource MUST be existing, otherwise the program crashes.
			auto rscOpt = FindResource<TRsc>(rscHandle);
			MOE_ASSERT(rscOpt.has_value());

			return std::move(*rscOpt);
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



		using CreateResourceFunc = std::function<RegistryID ()>;
		template <typename TRsc, typename TRscFactory>
		TRsc FindOrCreateResource(std::string_view rscIdentifier, TRscFactory* factory, CreateResourceFunc createFunc);



		IMeshFactory*		m_meshFactory{nullptr};
		ITextureFactory*	m_textureFactory{ nullptr };
		std::unique_ptr<IAssetImporter>	m_assetImporter{nullptr};

		std::unordered_map<HashString, RegistryID>	m_resourceIDs{};

		std::unordered_map<HashString, RegistryID>		m_rscHandleToID{};
		std::unordered_map<RegistryID, HashString>		m_rscIDToHandle{};
		ObjectRegistry<std::unique_ptr<IBaseResource>>	m_resourcesData{};

		bool	m_shutdown{ false }; // The manager has shut down and won't service any more resource requests.
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
