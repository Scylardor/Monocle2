
#include "MaterialManager.h"



#include "Graphics/BlendState/OpenGL/OpenGLBlendEquation.h"
#include "Graphics/BlendState/OpenGL/OpenGLBlendFactor.h"
#include "Graphics/Pipeline/OpenGL/OpenGLPipeline.h"
#include "Graphics/Shader/ShaderStage/OpenGL/OpenGLShaderStage.h"



static const unsigned ms_INFOLOG_BUF_SIZE = 512;


/**
 * \brief A generic shader status checking macro, printing an error message in case of failing
 * \param shaderObj The GL handle of the shader to check
 * \param status The status checked ( GL_COMPILE_STATUS, etc...)
 */
#define GL_CHECK_SHADER_ERROR(shaderObj, status, format, ...) \
{ \
	int success; \
	glGetShaderiv(shaderObj, status, &success); \
	if (false == success) \
	{ \
		char infoLog[ms_INFOLOG_BUF_SIZE]; \
		glGetShaderInfoLog(shaderObj, ms_INFOLOG_BUF_SIZE, nullptr, infoLog); \
		MOE_ERROR(ChanGraphics, format, infoLog, ##__VA_ARGS__); \
		MOE_ASSERT(false); \
	} \
}


 /**
  * \brief A generic program status checking macro, printing an error message in case of failing
  * \param programObj The GL handle of the program to check
  * \param status The status checked ( GL_LINK_STATUS, etc...)
  */
#define GL_CHECK_PROGRAM_ERROR(programObj, status, format, ...) \
{ \
	int success; \
	glGetProgramiv(programObj, status, &success); \
	if (false == success) \
	{ \
		char infoLog[ms_INFOLOG_BUF_SIZE]; \
		glGetProgramInfoLog(programObj, ms_INFOLOG_BUF_SIZE, nullptr, infoLog); \
		MOE_ERROR(ChanGraphics, infoLog, ##__VA_ARGS__); \
		MOE_ASSERT(false); \
	} \
}


namespace moe
{
	bool OpenGL4MaterialManager::CreatePipelineStateObjectLayout(PipelineDescription const& pipeDesc)
	{
		pipeDesc;
		return true;
	}

	bool OpenGL4MaterialManager::CreateShaderProgram(ShaderProgramDescription const& programDescription)
	{
		programDescription;
		return true;
	}

	DeviceMaterialHandle OpenGL4MaterialManager::CreateMaterial(MaterialDescription const& matDesc)
	{
		if (matDesc.PassDescriptors.Empty())
			return DeviceMaterialHandle::Null();

		// TODO: only manage single-pass materials for now.
		MaterialPassDescription const& firstPass = matDesc.PassDescriptors[0];
		uint32_t programIdx = FindOrBuildShaderProgram(firstPass.ShaderProgram);

		// Now translate the pipeline configuration to an OpenGL one, and find if one already exists (or add it)
		uint32_t psoIdx = FindOrBuildPipelineStateObject(firstPass.Pipeline);

		// Now build and return the material object.
		OpenGL4PMaterial material{ psoIdx, programIdx, firstPass.ResourceBindings };
		auto matIt = std::find_if(m_materials.begin(), m_materials.end(),
			[programIdx, psoIdx, &firstPass](OpenGL4PMaterial const& mat)
		{
				return (mat.ProgramIdx == programIdx && mat.PSOIdx == psoIdx && mat.ResourceSets.Bindings == firstPass.ResourceBindings.Bindings);
		});

		return {};
	}


	uint32_t OpenGL4MaterialManager::FindOrBuildShaderProgram(ShaderProgramDescription const& progDesc)
	{
		// First, retrieve or compile all our shaders...
		// To do so, we're going to hash the code of the shader.
		Vector<GLuint> programModules;
		programModules.Reserve(progDesc.Modules.Size());

		for (auto const& moduleDesc : progDesc.Modules)
		{
			GLuint shaderModule = FindOrCompilleShaderModule(moduleDesc);
			programModules.PushBack(shaderModule);
		}

		// Now Find a program that is composed of the same shaders, or build one if not found
		uint32_t programIdx = FindShaderProgramIndex(std::move(programModules));

		return programIdx;

	}


	GLuint OpenGL4MaterialManager::FindOrCompilleShaderModule(ShaderModuleDescription const& moduleDesc)
	{
		std::string const* shaderCode;
		bool isBytecode;

		// Either we were passed raw shader code, or a reference to a shader file we can read the contents of...
		if (moduleDesc.ShaderFile)
		{
			MOE_ASSERT(false == moduleDesc.ShaderFile->IsEmpty());
			shaderCode = &moduleDesc.ShaderFile->Contents();
			isBytecode = moduleDesc.ShaderFile->IsBinaryFile();
		}
		else
		{
			// if we were passed a hardcoded shader, since it's unlikely it's embedded bytecode, we assume we need to compile it.
			MOE_ASSERT(false == moduleDesc.ShaderCode.empty());
			shaderCode = &moduleDesc.ShaderCode;
			isBytecode = false;
		}

		HashString hashedCode = HashString(*shaderCode);
		auto moduleIt = m_shaderModules.Find(hashedCode);
		if (moduleIt != m_shaderModules.End())
		{
			// found an existing shader module
			return (moduleIt->second);
		}

		const GLenum shaderStageEnum = GetShaderStageEnum(moduleDesc.Stage);
		if (shaderStageEnum == GL_FALSE)
		{
			MOE_ERROR(ChanGraphics, "Could not translate shader stage value '%u'. Aborting shader program creation.", moduleDesc.Stage);
			return 0;
		}

		const GLuint newShader = glCreateShader(shaderStageEnum);

		if (isBytecode)
		{
			// build a shader module from bytecode
			// TODO...
		}
		else
		{
			// compile the shader code from GLSL source
			const GLchar* code = shaderCode->c_str();
			glShaderSource(newShader, 1, &code, nullptr);

			// Check for shader compile errors
			glCompileShader(newShader);

			GL_CHECK_SHADER_ERROR(newShader, GL_COMPILE_STATUS,
				"Shader compilation failed : '%s' (at shader stage '%u'). Aborting shader program creation.", moduleDesc.Stage)
		}

		m_shaderModules.Emplace(hashedCode, newShader);

		return newShader;
	}


	uint32_t OpenGL4MaterialManager::FindShaderProgramIndex(Vector<GLuint>&& programModules)
	{
		uint32_t programIdx;

		auto existingProgramIt = std::find_if(m_shaderPrograms.begin(), m_shaderPrograms.end(),
			[&programModules](OpenGL4ShaderProgram const& program)
			{
				return (program.Modules == programModules);
			});

		if (existingProgramIt != m_shaderPrograms.end())
		{
			// existing program found
			programIdx = (uint32_t) std::distance(m_shaderPrograms.begin(), existingProgramIt);
		}
		else
		{
			// build a new one
			GLuint shaderProgram = glCreateProgram();
			for (GLuint module : programModules)
				glAttachShader(shaderProgram, module);

			glLinkProgram(shaderProgram);
			// Check for linking errors
			GL_CHECK_PROGRAM_ERROR(shaderProgram, GL_LINK_STATUS,
				"Linking failed for shader program : '%s' (program ID : '%u'). Aborting shader program creation.", shaderProgram)

			m_shaderPrograms.EmplaceBack(shaderProgram, std::move(programModules));

			programIdx = (uint32_t)m_shaderPrograms.Size() - 1;
		}

		return programIdx;
	}


	uint32_t OpenGL4MaterialManager::FindOrBuildPipelineStateObject(PipelineDescription const& pipelineDesc)
	{
		OpenGL4PipelineStateObject pso = TranslateConfigToPipelineStateObject(pipelineDesc);

		auto psoIt = std::find(m_pipelineStateObjects.begin(), m_pipelineStateObjects.end(), pso);
		if (psoIt != m_pipelineStateObjects.end())
			return (uint32_t)std::distance(m_pipelineStateObjects.begin(), psoIt);
		else
		{
			m_pipelineStateObjects.EmplaceBack(pso);
			return (uint32_t)m_pipelineStateObjects.Size() - 1;
		}
	}


	OpenGL4PipelineStateObject OpenGL4MaterialManager::TranslateConfigToPipelineStateObject(PipelineDescription const& pipelineDesc)
	{
		OpenGL4PipelineStateObject pso;

		pso.BlendSrcFactor = GetOpenGLBlendFactor(pipelineDesc.BlendStateDesc.m_srcFactor);
		pso.BlendDestFactor = GetOpenGLBlendFactor(pipelineDesc.BlendStateDesc.m_destFactor);
		pso.BlendEquation = GetOpenGLBlendEquation(pipelineDesc.BlendStateDesc.m_equation);
		pso.BlendEnabled = (pipelineDesc.BlendStateDesc.m_enabled == BlendStateDescriptor::Enabled);
		pso.DepthTestEnabled = (pipelineDesc.DepthStencilStateDesc.m_depthTest == DepthTest::Enabled);
		pso.DepthWriteEnabled = (pipelineDesc.DepthStencilStateDesc.m_depthWriting == DepthWriting::Enabled);
		pso.DepthFunction = OpenGLPipeline::GetOpenGLDSComparisonFunc(pipelineDesc.DepthStencilStateDesc.m_depthFunc);
		pso.StencilTestEnabled = (pipelineDesc.DepthStencilStateDesc.m_stencilTest == StencilTest::Enabled);
		pso.StencilWriteMask = pipelineDesc.DepthStencilStateDesc.m_stencilWriteMask;
		pso.StencilReferenceValue = pipelineDesc.DepthStencilStateDesc.m_stencilRefVal;
		pso.StencilReadMask = pipelineDesc.DepthStencilStateDesc.m_stencilReadMask;
		pso.FrontFaceStencilFailOp = OpenGLPipeline::GetOpenGLStencilOpEnum(pipelineDesc.DepthStencilStateDesc.m_frontFaceOps.m_stencilFail);
		pso.FrontFaceDepthFailOp = OpenGLPipeline::GetOpenGLStencilOpEnum(pipelineDesc.DepthStencilStateDesc.m_frontFaceOps.m_depthFail);
		pso.FrontFaceBothPassOp = OpenGLPipeline::GetOpenGLStencilOpEnum(pipelineDesc.DepthStencilStateDesc.m_frontFaceOps.m_allPass);
		pso.FrontFaceComparisonFunc = OpenGLPipeline::GetOpenGLDSComparisonFunc(pipelineDesc.DepthStencilStateDesc.m_frontFaceOps.m_comparisonFunc);
		pso.BackFaceStencilFailOp = OpenGLPipeline::GetOpenGLStencilOpEnum(pipelineDesc.DepthStencilStateDesc.m_backFaceOps.m_stencilFail);
		pso.BackFaceDepthFailOp = OpenGLPipeline::GetOpenGLStencilOpEnum(pipelineDesc.DepthStencilStateDesc.m_backFaceOps.m_depthFail);
		pso.BackFaceBothPassOp = OpenGLPipeline::GetOpenGLStencilOpEnum(pipelineDesc.DepthStencilStateDesc.m_backFaceOps.m_allPass);
		pso.BackFaceComparisonFunc = OpenGLPipeline::GetOpenGLDSComparisonFunc(pipelineDesc.DepthStencilStateDesc.m_backFaceOps.m_comparisonFunc);
		pso.CullingEnabled = (pipelineDesc.RasterizerStateDesc.m_cullMode != CullFace::None);
		pso.CulledFace = (pipelineDesc.RasterizerStateDesc.m_cullMode == CullFace::Back ? GL_BACK : GL_FRONT);
		pso.FrontFace = (pipelineDesc.RasterizerStateDesc.m_frontFace == FrontFace::Clockwise ? GL_CW : GL_CCW);
		pso.PolygonFace = GL_FRONT_AND_BACK;

		if (pipelineDesc.RasterizerStateDesc.m_polyMode == PolygonMode::Points)
			pso.PolygonMode = GL_POINTS;
		else if (pipelineDesc.RasterizerStateDesc.m_polyMode == PolygonMode::Wireframe)
			pso.PolygonMode = GL_LINE;
		else
			pso.PolygonMode = GL_FILL;

		pso.EnableDepthClamp = (pipelineDesc.RasterizerStateDesc.m_depthClip == RasterizerStateDescriptor::Enabled);
		pso.EnableScissorTest = (pipelineDesc.RasterizerStateDesc.m_scissorTest == RasterizerStateDescriptor::Enabled);
		pso.Topology = OpenGLPipeline::GetOpenGLPrimitiveTopology(pipelineDesc.Topology);

		return pso;
	}
}
