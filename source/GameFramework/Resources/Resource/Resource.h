#pragma once

#include "Core/Resource/ResourceFactory.h"
#include "Graphics/Vulkan/Texture/VulkanTextureFactory.h"
#include "Graphics/Vulkan/Factories/VulkanMeshFactory.h"
#include "Graphics/Vulkan/Material/VulkanMaterialFactory.h"

namespace moe
{
	class IMeshFactory;

	template <typename TFactory, typename TRsc>
	class IResource
	{
	public:

		IResource() = default;

		IResource(TFactory& factory, RegistryID myID) :
			m_myFactory(&factory), m_ID(myID)
		{
			if (m_myFactory && m_ID != INVALID_ENTRY)
				m_myFactory->IncrementReference(m_ID);
		}


		~IResource()
		{
			if (m_myFactory && m_ID != INVALID_ENTRY)
				m_myFactory->DecrementReference(m_ID);
		}


		IResource(IResource&& other) noexcept
		{
			*this = std::move(other);
		}

		IResource& operator=(IResource&& other) noexcept
		{
			if (&other == this)
				return *this;

			m_myFactory = other.m_myFactory;
			other.m_myFactory = nullptr;

			m_ID = other.m_ID;
			other.m_ID = INVALID_ENTRY;

			return *this;
		}

		IResource& operator=(const IResource& other)
		{
			if (&other == this)
				return *this;

			m_myFactory = other.m_myFactory;
			m_ID = other.m_ID;

			if (ID() != INVALID_ENTRY)
				m_myFactory->IncrementReference(ID());

			return *this;
		}


		[[nodiscard]] RegistryID	ID() const
		{
			return m_ID;
		}

	protected:

		TFactory* m_myFactory{ nullptr };
		RegistryID	m_ID{ 0 };
	};




	class MeshResource : public IResource<IMeshFactory, VulkanMesh>
	{
	public:

		MeshResource() = default;

		MeshResource(IMeshFactory& factory, RegistryID id) :
			IResource(factory, id)
		{}


		[[nodiscard]] VulkanMesh* operator->()
		{
			IMeshFactory* factory = static_cast<IMeshFactory*>(m_myFactory);
			auto& mesh = factory->MutateResource(m_ID);
			return &mesh;
		}


		[[nodiscard]] const VulkanMesh* operator->() const
		{
			IMeshFactory* factory = static_cast<IMeshFactory*>(m_myFactory);
			auto& mesh = factory->GetResource(m_ID);
			return &mesh;
		}
	};


	class TextureResource : public IResource<ITextureFactory, VulkanTexture>
	{
	public:

		TextureResource() = default;

		TextureResource(ITextureFactory& factory, RegistryID id) :
			IResource(factory, id)
		{}


		[[nodiscard]] VulkanTexture* operator->()
		{
			auto* factory = static_cast<ITextureFactory*>(m_myFactory);
			auto& tex = factory->MutateResource(m_ID);
			return &tex;
		}


		[[nodiscard]] const VulkanTexture* operator->() const
		{
			auto* factory = static_cast<ITextureFactory*>(m_myFactory);
			auto& tex = factory->GetResource(m_ID);
			return &tex;
		}
	};

	class MaterialResource : public IResource<IMaterialFactory, VulkanMaterial>
	{
	public:

		MaterialResource(IMaterialFactory& factory, RegistryID id) :
			IResource(factory, id)
		{}
	};

}
