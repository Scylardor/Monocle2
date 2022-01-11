#pragma once
#include <glad/glad.h>


#include "Core/Containers/HashMap/HashMap.h"
#include "Core/HashString/HashString.h"
#include "Core/Resource/Material/MaterialResource.h"
#include "Graphics/Pipeline/OpenGL/OpenGLPipeline.h"
#include "Graphics/RHI/MaterialManager/MaterialManager.h"

namespace moe
{
	class OpenGL4TextureManager;
	class OpenGL4RHI;

	struct OpenGLVertexBindingFormat
	{
		static std::optional<OpenGLVertexBindingFormat>	TranslateFormat(VertexBindingFormat vtxFormat);
		static std::optional<uint32_t>					FindTypeSize(std::uint8_t numCpnts, GLenum type);

		GLenum			Type{ GL_FALSE };
		std::uint8_t	Size{ 0 };
		GLboolean		Normalized{ false };
		uint32_t		NumBindings{ 1 }; // Some types (e.g. mat4) take multiple bindings.
	};

	// Stores the informations of a VAO.
	// Note: in OpenGL, the topology of a mesh is more related to the VAO than to the PSO.
	// That's why specifically for OpenGL, I actually store the topology inside the VAO rather than the PSO.
	struct OpenGL4VertexLayout
	{
		OpenGL4VertexLayout(GLuint vao, GLsizei stride, PrimitiveTopology topo, VertexLayoutDescription layoutDesc) :
			VAO(vao), TotalStride(stride), Topology(OpenGLPipeline::GetOpenGLPrimitiveTopology(topo)), Desc(std::move(layoutDesc))
		{}

		GLuint					VAO{0};
		GLsizei					TotalStride{ 0 };
		GLenum					Topology{ GL_TRIANGLES };
		VertexLayoutDescription	Desc{};
	};


	struct OpenGL4ShaderProgram
	{
		OpenGL4ShaderProgram(GLuint prog, Vector<GLuint> mods) :
			Program(prog), Modules(std::move(mods))
		{}

		GLuint			Program{ 0 };
		Vector<GLuint>	Modules{};
	};


	struct OpenGL4PipelineStateObject
	{
		GLenum	BlendSrcFactor{ GL_SRC_ALPHA };
		GLenum	BlendDestFactor{ GL_ONE_MINUS_SRC_ALPHA };
		GLenum	BlendEquation{ GL_FUNC_ADD };
		bool	BlendEnabled{ GL_FALSE };

		bool	DepthTestEnabled{ GL_FALSE };
		bool	DepthWriteEnabled{ GL_FALSE };
		GLenum	DepthFunction{ GL_LESS };

		bool	StencilTestEnabled{ GL_FALSE };
		GLuint	StencilWriteMask{ 0 };
		GLint	StencilReferenceValue{ 0 };
		GLuint	StencilReadMask{ 0xFF };

		GLenum	FrontFaceStencilFailOp{ GL_KEEP };
		GLenum	FrontFaceDepthFailOp{ GL_KEEP };
		GLenum	FrontFaceBothPassOp{ GL_KEEP };
		GLenum	FrontFaceComparisonFunc{ GL_ALWAYS };

		GLenum	BackFaceStencilFailOp{ GL_KEEP };
		GLenum	BackFaceDepthFailOp{ GL_KEEP };
		GLenum	BackFaceBothPassOp{ GL_KEEP };
		GLenum	BackFaceComparisonFunc{ GL_ALWAYS };

		bool	CullingEnabled{ GL_TRUE };
		GLenum	CulledFace{ GL_BACK };
		GLenum	FrontFace{ GL_CCW };

		GLenum	PolygonFace{ GL_FRONT_AND_BACK };
		GLenum	PolygonMode{ GL_FILL };

		bool	EnableDepthClamp{ GL_FALSE };
		bool	EnableScissorTest{ GL_FALSE };

		bool operator==(const OpenGL4PipelineStateObject& other) const
		{
			if (&other == this)
				return true;

			return (
				BlendDestFactor == other.BlendDestFactor &&
				BlendSrcFactor == other.BlendSrcFactor &&
				BlendEquation == other.BlendEquation &&
				BlendEnabled == other.BlendEnabled &&
				DepthTestEnabled == other.DepthTestEnabled &&
				DepthWriteEnabled == other.DepthWriteEnabled &&
				DepthFunction == other.DepthFunction &&
				StencilTestEnabled == other.StencilTestEnabled &&
				StencilWriteMask == other.StencilWriteMask &&
				StencilReferenceValue == other.StencilReferenceValue &&
				StencilReadMask == other.StencilReadMask &&
				FrontFaceStencilFailOp == other.FrontFaceStencilFailOp &&
				FrontFaceDepthFailOp == other.FrontFaceDepthFailOp &&
				FrontFaceBothPassOp == other.FrontFaceBothPassOp &&
				FrontFaceComparisonFunc == other.FrontFaceBothPassOp &&
				BackFaceStencilFailOp == other.BackFaceStencilFailOp &&
				BackFaceDepthFailOp == other.BackFaceDepthFailOp &&
				BackFaceBothPassOp == other.BackFaceDepthFailOp &&
				BackFaceComparisonFunc == other.BackFaceComparisonFunc &&
				CullingEnabled == other.CullingEnabled &&
				CulledFace == other.CulledFace &&
				FrontFace == other.FrontFace &&
				PolygonFace == other.PolygonFace &&
				PolygonMode == other.PolygonMode &&
				EnableDepthClamp == other.EnableDepthClamp &&
				EnableScissorTest == other.EnableScissorTest
			);
		}
	};


	struct OpenGL4Material
	{
		OpenGL4Material(uint32_t pso, uint32_t pipeline, uint32_t vao, ResourceSetsDescription sets) :
			PSOIdx(pso), ProgramIdx(pipeline), VAOIdx(vao), ResourceSets(std::move(sets))
		{}

		uint32_t				PSOIdx{ 0 };
		uint32_t				ProgramIdx{ 0 };
		uint32_t				VAOIdx{ 0 };
		ResourceSetsDescription	ResourceSets;
	};



	struct MaterialResourceSets
	{
		ResourceSetsDescription	ResourceSets;
	};



	/*
	 * An abstract class for pipeline state object (PSO) management and resource set (aka descriptor set in e.g. Vulkan) allocation.
	 */
	class OpenGL4MaterialManager : public IMaterialManager
	{
	public:

		OpenGL4MaterialManager(OpenGL4TextureManager& texMgr) :
			m_textureManager(texMgr)
		{}


		bool	CreatePipelineStateObjectLayout(PipelineDescription const& pipeDesc) override;

		bool	CreateShaderProgram(ShaderProgramDescription const& programDescription) override;

		[[nodiscard]] DeviceMaterialHandle		CreateMaterial(MaterialDescription const& matDesc) override;

		[[nodiscard]] DeviceDynamicResourceSetHandle	AddDynamicBufferBinding(DeviceDynamicResourceSetHandle dynamicSetHandle,
			BufferBinding const& dynamicBufferBinding) override;

		void									FreeDynamicSets(DeviceDynamicResourceSetHandle freedHandle) override;

		[[nodiscard]] DeviceResourceSetHandle	AllocateResourceSet(ResourceSetsDescription const& setDescription) override;
		void									FreeResourceSet(DeviceResourceSetHandle setHandle) override;

		GLuint	BindMaterial(OpenGL4RHI* rhi, uint32_t materialIdx);

		void	BindDynamicResourceSets(OpenGL4RHI* rhi, DeviceDynamicResourceSetHandle dynRscHandle, GLuint usedProgram);

		void	BindResourceSet(DeviceResourceSetHandle setHandle);

		[[nodiscard]] OpenGL4VertexLayout const*	GetMaterialVertexLayout(uint32_t materialIdx) const
		{
			MOE_ASSERT(materialIdx < m_materials.Size());
			OpenGL4Material const& material = m_materials[materialIdx];

			MOE_ASSERT(material.VAOIdx < m_VAOs.Size() || material.VAOIdx == NO_VAO);
			if (material.VAOIdx == NO_VAO)
				return nullptr;

			return &m_VAOs[material.VAOIdx];
		}


	private:

		inline static const auto NO_VAO = UINT32_MAX;

		uint32_t	FindOrBuildShaderProgram(ShaderProgramDescription const& progDesc);

		GLuint		FindOrCompilleShaderModule(ShaderModuleDescription const& moduleDesc);

		uint32_t	FindShaderProgramIndex(Vector<GLuint>&& programModules);

		uint32_t	FindOrBuildPipelineStateObject(PipelineDescription const& pipelineDesc);

		uint32_t	FindOrCreateVAO(VertexLayoutDescription const& layoutDesc, PrimitiveTopology topo);

		uint32_t	FindOrCreateSampler(SamplerDescription const& samplerDesc);


		static std::pair<GLuint, GLsizei>	BuildInterleavedVAO(VertexLayoutDescription const& layoutDesc);
		static GLuint						BuildPackedVAO(VertexLayoutDescription const& layoutDesc);

		static OpenGL4PipelineStateObject	TranslateConfigToPipelineStateObject(PipelineDescription const& pipelineDesc);

		static void	BindPipelineStateObject(OpenGL4PipelineStateObject const& pso);

		void		BindResourceSets(OpenGL4RHI* rhi, GLuint programID, ResourceSetsDescription const& rscDesc);

		HashMap<HashString, GLuint>			m_shaderModules{};

		Vector<OpenGL4ShaderProgram>		m_shaderPrograms;

		Vector<OpenGL4PipelineStateObject>	m_pipelineStateObjects;

		Vector<OpenGL4VertexLayout>			m_VAOs;

		Vector<OpenGL4Material>				m_materials;

		DynamicObjectPool<ResourceSetsDescription>	m_dynamicResourceSets;

		OpenGL4TextureManager&				m_textureManager;
	};


}
