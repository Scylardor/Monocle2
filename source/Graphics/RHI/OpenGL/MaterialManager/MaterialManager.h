#pragma once
#include <glad/glad.h>


#include "Core/Containers/HashMap/HashMap.h"
#include "Core/HashString/HashString.h"
#include "Core/Resource/Material/MaterialResource.h"
#include "Graphics/RHI/MaterialManager/MaterialManager.h"

namespace moe
{

	struct OpenGLVertexBindingFormat
	{
		static std::optional<OpenGLVertexBindingFormat>	TranslateFormat(VertexBindingFormat vtxFormat);
		static std::optional<uint32_t>					FindTypeSize(std::uint8_t numCpnts, GLenum type);

		GLenum			Type{ GL_FALSE };
		std::uint8_t	Size{ 0 };
		GLboolean		Normalized{ false };
		uint32_t		NumBindings{ 1 }; // Some types (e.g. mat4) take multiple bindings.
	};

	struct OpenGL4VertexLayout
	{
		OpenGL4VertexLayout(GLuint vao, VertexLayoutDescription layoutDesc) :
			VAO(vao), Desc(std::move(layoutDesc))
		{}

		GLuint					VAO{0};
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

		GLenum	Topology{ GL_TRIANGLES };

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
				EnableScissorTest == other.EnableScissorTest &&
				Topology == other.Topology
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


	/*
	 * An abstract class for pipeline state object (PSO) management and resource set (aka descriptor set in e.g. Vulkan) allocation.
	 */
	class OpenGL4MaterialManager : public IMaterialManager
	{
	public:


		bool	CreatePipelineStateObjectLayout(PipelineDescription const& pipeDesc) override;

		bool	CreateShaderProgram(ShaderProgramDescription const& programDescription) override;

		DeviceMaterialHandle	CreateMaterial(MaterialDescription const& matDesc) override;


	private:

		uint32_t	FindOrBuildShaderProgram(ShaderProgramDescription const& progDesc);

		GLuint		FindOrCompilleShaderModule(ShaderModuleDescription const& moduleDesc);

		uint32_t	FindShaderProgramIndex(Vector<GLuint>&& programModules);

		uint32_t	FindOrBuildPipelineStateObject(PipelineDescription const& pipelineDesc);

		GLuint			FindOrCreateVAO(VertexLayoutDescription const& layoutDesc);
		static GLuint	BuildInterleavedVAO(VertexLayoutDescription const& layoutDesc);
		static GLuint	BuildPackedVAO(VertexLayoutDescription const& layoutDesc);

		static OpenGL4PipelineStateObject	TranslateConfigToPipelineStateObject(PipelineDescription const& pipelineDesc);

		HashMap<HashString, GLuint>			m_shaderModules{};

		Vector<OpenGL4ShaderProgram>		m_shaderPrograms;

		Vector<OpenGL4PipelineStateObject>	m_pipelineStateObjects;

		Vector<OpenGL4VertexLayout>			m_VAOs;

		Vector<OpenGL4Material>				m_materials;


	};


}
