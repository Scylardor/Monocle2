#pragma once

#include <memory>


#include "Core/Resource/BaseResource.h"
#include "Core/Resource/ResourceRef.h"
#include "Core/Resource/TextureResource.h"
#include "BlendState/BlendStateDescriptor.h"
#include "DepthStencilState/DepthStencilStateDescriptor.h"
#include "RasterizerState/RasterizerStateDescriptor.h"
#include "Shader/Program/ShaderProgramDescription.h"
#include "Topology/PrimitiveTopology.h"


namespace moe
{
	class MaterialModulesResource;

	/**
	* \brief This describes how a graphics pipeline should be built.
	* It is meant to describe a pipeline object in an agnostic way (regardless of OpenGL, D3D, Vulkan or Metal).
	*/
	struct PipelineDescription
	{
		BlendStateDescriptor		BlendStateDesc;
		DepthStencilStateDescriptor	DepthStencilStateDesc;
		RasterizerStateDescriptor	RasterizerStateDesc;
		PrimitiveTopology			Topology{ PrimitiveTopology::TriangleList };
	};

	struct MaterialPassDescription
	{
		MaterialPassDescription&	AssignShaderProgramDescription(ShaderProgramDescription programDesc)
		{
			ShaderProgram = std::move(programDesc);
			return *this;
		}

		PipelineDescription			Pipeline;
		ShaderProgramDescription	ShaderProgram;
	};

	struct MaterialDescription
	{
		MaterialPassDescription& NewPassDescription()
		{
			PassDescriptors.EmplaceBack();
			return PassDescriptors.Back();
		}

		Vector<MaterialPassDescription>	PassDescriptors;
	};



	class MaterialResource : public IBaseResource
	{
	public:

		MaterialResource(MaterialDescription desc) :
			Description(std::move(desc))
		{}

		MaterialDescription const&	GetDescription() const
		{
			return Description;
		}

	private:

		MaterialDescription	Description;

	};

	class AMaterialModule
	{
	public:

		AMaterialModule(uint8_t setNumber) :
			m_setNumber(setNumber)
		{}


		virtual ~AMaterialModule() = default;


		virtual void	UpdateResources(MaterialModulesResource& updatedMaterial) = 0;


	protected:

		uint8_t	m_setNumber{ 0xFF };

	};

	class MaterialModulesResource : public IBaseResource
	{
	public:

		MaterialModulesResource() = default;

		~MaterialModulesResource() override = default;

		virtual std::unique_ptr<MaterialModulesResource> Clone()
		{
			return nullptr;
		}

		template <typename TModule, typename... Args>
		TModule& EmplaceModule(uint8_t setNumber, Args&&... args)
		{
			static_assert(std::is_base_of_v<AMaterialModule, TModule>);
			auto modulePtr = std::make_unique<TModule>(setNumber, std::forward<Args>(args)...);
			// get the ptr before it gets moved
			TModule* ptr = modulePtr.get();

			AddNewModule(std::move(modulePtr));

			return *ptr;
		}


		virtual void				UpdateResources(uint8_t /*resourceSet*/)
		{

		}

		virtual MaterialModulesResource& BindTextureResource(uint32_t /*set*/, uint32_t /*binding*/, const Ref<TextureResource>& /*tex*/)
		{
			return *this;
		}

	protected:

		virtual void	AddNewModule(std::unique_ptr<AMaterialModule> newModule)
		{}

	};



}
