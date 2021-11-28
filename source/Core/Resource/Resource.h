#pragma once

#include "Core/Resource/BaseResource.h"


namespace moe
{
	class MaterialModulesResource;
	class IMeshFactory;



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
