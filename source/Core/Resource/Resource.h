#pragma once

#include "Core/Resource/ResourceFactory.h"
#include "Core/Resource/BaseResource.h"
#include "Core/HashString/HashString.h"

namespace moe
{
	class IMeshFactory;


	class IResourceManager
	{
	public:
		virtual ~IResourceManager() = default;

		virtual uint32_t	IncrementReference(RegistryID rscID) = 0;

		virtual void		DecrementReference(RegistryID rscID) = 0;
	};




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


	class MaterialResource : public IBaseResource
	{
	public:

		MaterialResource() = default;

		~MaterialResource() override = default;

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
