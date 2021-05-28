#pragma once

#include "ResourceRef.h"
#include "Core/Resource/BaseResource.h"

namespace moe
{
	class MaterialResource;
	class IMeshFactory;


	class TextureResource : public IBaseResource
	{
	public:

		TextureResource() = default;

		~TextureResource() override = default;

	};


	class ShaderResource : public IBaseResource
	{
	public:

		ShaderResource() = default;

		~ShaderResource() override = default;

	};


	class ShaderProgramResource : public IBaseResource
	{
	public:

		ShaderProgramResource() = default;

		~ShaderProgramResource() override = default;

	};


	class MeshResource : public IBaseResource
	{
	public:

		MeshResource() = default;

		~MeshResource() override = default;

	};


	class AMaterialModule
	{
	public:

		AMaterialModule(uint8_t setNumber) :
			m_setNumber(setNumber)
		{}


		virtual ~AMaterialModule() = default;


		virtual void	UpdateResources(MaterialResource& updatedMaterial) = 0;


	protected:

		uint8_t	m_setNumber{ 0xFF };

	};

	class MaterialResource : public IBaseResource
	{
	public:

		MaterialResource() = default;

		~MaterialResource() override = default;

		virtual std::unique_ptr<MaterialResource> Clone() = 0;

		template <typename TModule, typename... Args>
		TModule&	EmplaceModule(uint8_t setNumber, Args&&... args)
		{
			static_assert(std::is_base_of_v<AMaterialModule, TModule>);
			auto modulePtr = std::make_unique<TModule>(setNumber, std::forward<Args>(args)...);
			// get the ptr before it gets moved
			TModule* ptr = modulePtr.get();

			AddNewModule(std::move(modulePtr));

			return *ptr;
		}


		virtual void				UpdateResources(uint8_t resourceSet) = 0;

		virtual MaterialResource&	BindTextureResource(uint32_t set, uint32_t binding, const Ref<TextureResource>& tex) = 0;

	protected:

		virtual void	AddNewModule(std::unique_ptr<AMaterialModule> newModule) = 0;

	};


	class ShaderPipelineResource : public IBaseResource
	{
	public:

		ShaderPipelineResource() = default;

		~ShaderPipelineResource() override = default;

	};


	class IAssetImporter
	{

	public:

		enum class NormalsGeneration
		{
			Enabled,
			SmoothEnabled,
			Disabled
		};

		bool				Triangulate{ true };
		bool				FlipUVs{ false };
		NormalsGeneration	GenerateNormals{ NormalsGeneration::Disabled };
	};

}
