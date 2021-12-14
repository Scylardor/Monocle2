
#include "MaterialManager.h"



#include "Graphics/BlendState/OpenGL/OpenGLBlendEquation.h"
#include "Graphics/BlendState/OpenGL/OpenGLBlendFactor.h"
#include "Graphics/Pipeline/OpenGL/OpenGLPipeline.h"
#include "Graphics/Shader/ShaderStage/OpenGL/OpenGLShaderStage.h"
#include "../OGL4RHI.h"


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

		uint32_t vaoIdx = FindOrCreateVAO(firstPass.Pipeline.VertexLayout, firstPass.Pipeline.Topology);

		// Now build and return the material object.
		auto matIt = std::find_if(m_materials.begin(), m_materials.end(),
			[programIdx, psoIdx, vaoIdx, &firstPass](OpenGL4Material const& mat)
		{
				return (mat.ProgramIdx == programIdx && mat.PSOIdx == psoIdx && mat.VAOIdx == vaoIdx
					&& mat.ResourceSets.Bindings == firstPass.ResourceBindings.Bindings);
		});


		uint64_t matID;
		uint32_t matIdx;

		if (matIt != m_materials.End())
		{
			matIdx = (uint32_t)std::distance(m_materials.begin(), matIt);
		}
		else
		{
			matIdx = (uint32_t) m_materials.Size();
			m_materials.EmplaceBack(psoIdx, programIdx, vaoIdx, firstPass.ResourceBindings);
		}

		matID = matIdx;
		matID = (matID << 32) | programIdx;
		return DeviceMaterialHandle{ matID };
	}


	void OpenGL4MaterialManager::BindMaterial(OpenGL4RHI* rhi, uint32_t materialIdx)
	{
		MOE_ASSERT(materialIdx < m_materials.Size());
		OpenGL4Material& material = m_materials[materialIdx];

		MOE_ASSERT(material.PSOIdx < m_pipelineStateObjects.Size());
		OpenGL4PipelineStateObject& pso = m_pipelineStateObjects[material.PSOIdx];
		BindPipelineStateObject(pso);

		MOE_ASSERT(material.ProgramIdx < m_shaderPrograms.Size());
		GLuint program = m_shaderPrograms[material.ProgramIdx].Program;
		glUseProgram(program);

		MOE_ASSERT(material.VAOIdx < m_VAOs.Size());
		GLuint vao = m_VAOs[material.VAOIdx].VAO;
		glBindVertexArray(vao);

		BindResourceSets(rhi, program, material.ResourceSets);
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

	std::optional<OpenGLVertexBindingFormat> OpenGLVertexBindingFormat::TranslateFormat(VertexBindingFormat vtxFormat)
	{
		switch (vtxFormat)
		{
		case VertexBindingFormat::Float:
			return { {GL_FLOAT, 1, false} };
		case VertexBindingFormat::Float2:
			return { { GL_FLOAT, 2, false } };
		case VertexBindingFormat::Float3:
			return { { GL_FLOAT, 3, false } };
		case VertexBindingFormat::Float4:
			return { { GL_FLOAT, 4, false } };
		case VertexBindingFormat::Mat4:
			return { { GL_FLOAT, 4, false, 4 } };
		case VertexBindingFormat::Byte:
			return { { GL_UNSIGNED_BYTE, 1, false } };
		case VertexBindingFormat::Byte_Norm:
			return { { GL_UNSIGNED_BYTE, 1, true} };
		case VertexBindingFormat::Byte2_Norm:
			return { { GL_UNSIGNED_BYTE, 2, true } };
		case VertexBindingFormat::Byte2:
			return { { GL_UNSIGNED_BYTE, 2, false } };
		case VertexBindingFormat::Byte3_Norm:
			return { { GL_UNSIGNED_BYTE, 3, true } };
		case VertexBindingFormat::Byte3:
			return { { GL_UNSIGNED_BYTE, 3, false} };
		case VertexBindingFormat::Byte4_Norm:
			return { { GL_UNSIGNED_BYTE, 4, true } };
		case VertexBindingFormat::Byte4:
			return { { GL_UNSIGNED_BYTE, 4, false } };
		case VertexBindingFormat::SByte:
			return { { GL_BYTE, 4, false } };
		case VertexBindingFormat::SByte_Norm:
			return { { GL_BYTE, 4, true } };
		case VertexBindingFormat::SByte2_Norm:
			return { { GL_BYTE, 2, true} };
		case VertexBindingFormat::SByte2:
			return { { GL_BYTE, 2, false } };
		case VertexBindingFormat::SByte3_Norm:
			return { { GL_BYTE, 3, true } };
		case VertexBindingFormat::SByte3:
			return { { GL_BYTE, 3, false} };
		case VertexBindingFormat::SByte4_Norm:
			return { { GL_BYTE, 4, true} };
		case VertexBindingFormat::SByte4:
			return { { GL_BYTE, 4, false} };
		case VertexBindingFormat::UShort_Norm:
			return { { GL_UNSIGNED_SHORT, 1, true} };
		case VertexBindingFormat::UShort:
			return { { GL_UNSIGNED_SHORT, 1, false } };
		case VertexBindingFormat::UShort2_Norm:
			return { { GL_UNSIGNED_SHORT, 2, true } };
		case VertexBindingFormat::UShort2:
			return { { GL_UNSIGNED_SHORT, 2, false } };
		case VertexBindingFormat::UShort3_Norm:
			return { { GL_UNSIGNED_SHORT, 3, true} };
		case VertexBindingFormat::UShort3:
			return { { GL_UNSIGNED_SHORT, 3, false} };
		case VertexBindingFormat::UShort4_Norm:
			return { { GL_UNSIGNED_SHORT, 4, true} };
		case VertexBindingFormat::UShort4:
			return { { GL_UNSIGNED_SHORT, 4, false } };
		case VertexBindingFormat::Short_Norm:
			return { { GL_SHORT, 1, true } };
		case VertexBindingFormat::Short:
			return { { GL_SHORT, 1, false} };
		case VertexBindingFormat::Short2_Norm:
			return { { GL_SHORT, 2, true } };
		case VertexBindingFormat::Short2:
			return { { GL_SHORT, 2, false} };
		case VertexBindingFormat::Short3_Norm:
			return { { GL_SHORT, 3, true} };
		case VertexBindingFormat::Short3:
			return { { GL_SHORT, 3, false } };
		case VertexBindingFormat::Short4_Norm:
			return { { GL_SHORT, 4, true } };
		case VertexBindingFormat::Short4:
			return { { GL_SHORT, 4, false } };
		case VertexBindingFormat::UInt:
			return { { GL_UNSIGNED_INT, 1, false} };
		case VertexBindingFormat::UInt2:
			return { { GL_UNSIGNED_INT, 2, false} };
		case VertexBindingFormat::UInt3:
			return { { GL_UNSIGNED_INT, 3, false } };
		case VertexBindingFormat::UInt4:
			return { { GL_UNSIGNED_INT, 4, false } };
		case VertexBindingFormat::Int:
			return { { GL_INT, 1, false } };
		case VertexBindingFormat::Int2:
			return { { GL_INT, 2, false} };
		case VertexBindingFormat::Int3:
			return { { GL_INT, 3, false } };
		case VertexBindingFormat::Int4:
			return { { GL_INT, 4, false} };
		case VertexBindingFormat::HalfFloat:
			return { { GL_HALF_FLOAT, 1, false } };
		case VertexBindingFormat::HalfFloat2:
			return { { GL_HALF_FLOAT, 2, false } };
		case VertexBindingFormat::HalfFloat3:
			return { { GL_HALF_FLOAT, 3, false } };
		case VertexBindingFormat::HalfFloat4:
			return { { GL_HALF_FLOAT, 4, false } };
		default:
			MOE_ERROR(ChanGraphics, "Unrecognized vertex element format value : '%d'", vtxFormat);
			return {};
		}
	}

	std::optional<uint32_t> OpenGLVertexBindingFormat::FindTypeSize(std::uint8_t numCpnts, GLenum type)
	{
		MOE_ASSERT(numCpnts != 0);

		switch (type)
		{
		case GL_BYTE:
		case GL_UNSIGNED_BYTE:
			return numCpnts;
		case GL_FLOAT:
		case GL_INT:
		case GL_UNSIGNED_INT:
			return (uint32_t)(sizeof(float) * numCpnts); // Assume sizeof(float) == sizeof(int) == sizeof(unsigned int).
		case GL_SHORT:
		case GL_UNSIGNED_SHORT:
		case GL_HALF_FLOAT:
			return (uint32_t)(sizeof(short) * numCpnts); // Assume sizeof(short) == sizeof(unsigned short) == sizeof(GLhalf).
		default:
			MOE_ERROR(ChanGraphics, "Unmanaged vertex element type value: '%d'.", type);
			return {};
		}
	}

	uint32_t OpenGL4MaterialManager::FindOrCreateVAO(VertexLayoutDescription const& layoutDesc, PrimitiveTopology topo)
	{
		// First and foremost, check that we do not have an existing vertex layout that could fit this description...
		auto vtxLayoutIt = std::find_if(m_VAOs.begin(), m_VAOs.end(), [&layoutDesc](const OpenGL4VertexLayout& glLayout)
			{
				return (glLayout.Desc == layoutDesc);
			});

		if (vtxLayoutIt != m_VAOs.end())
		{
			// we found a matching one : don't bother recreating a new one
			return (uint32_t) std::distance(m_VAOs.begin(), vtxLayoutIt);
		}

		GLuint vaoID = 0;
		GLsizei vaoStride = 0;

		switch (layoutDesc.BindingsLayout)
		{
		case LayoutType::Interleaved:
		{
			std::tie(vaoID, vaoStride) = BuildInterleavedVAO(layoutDesc);
		}
		break;

		case LayoutType::Packed: // TODO: should be sunsetted soon. Packed vertex layout should be avoided.
		{
			vaoID = BuildPackedVAO(layoutDesc);
		}
		break;

		default:
			MOE_ASSERT(false); // not supposed to happen
		}

		MOE_DEBUG_ASSERT(vaoID != 0);
		if (vaoID != 0)
		{
			// The VAO was successfully initialized : we store our vertex layout
			m_VAOs.EmplaceBack(vaoID, vaoStride, topo, layoutDesc);
		}

		return (uint32_t) m_VAOs.Size()-1;
	}


	std::pair<GLuint, GLsizei> OpenGL4MaterialManager::BuildInterleavedVAO(VertexLayoutDescription const& layoutDesc)
	{
		GLuint vaoID;
		glCreateVertexArrays(1, &vaoID);

		bool hasErrors = false; // start optimistic

		GLsizei totalStride{ 0 };
		int iAttrib = 0;

		for (VertexBindingDescription const& desc : layoutDesc.Bindings)
		{
			auto OpenGLInfoOpt = OpenGLVertexBindingFormat::TranslateFormat(desc.Format);
			if (false == OpenGLInfoOpt.has_value())
			{
				MOE_ERROR(ChanGraphics, "Failed to translate following OpenGL format : %u", desc.Format);
				hasErrors = true;
				break;
			}

			const OpenGLVertexBindingFormat& glVertexElemFmt = OpenGLInfoOpt.value();

			// "Repeats" are necessary to handle vertex attributes like mat4 that can exist in shaders, but are not handled by the API code.
			// For a mat4 as an example, we have to describe a matrix as four vec4 attributes following each other; the repeat count will then be 4.
			for (auto iBind = 0u; iBind < glVertexElemFmt.NumBindings; iBind++)
			{
				// Enable attrib index for given VAO
				glEnableVertexArrayAttrib(vaoID, iAttrib);

				// Specify format for this attrib : type; number of components, normalized. Use totalStride as relativeoffset because data is interleaved (XYZ XYZ...)
				glVertexArrayAttribFormat(vaoID, iAttrib, glVertexElemFmt.Size, glVertexElemFmt.Type, glVertexElemFmt.Normalized, totalStride);

				// We can bind all attributes to same binding index because data is interleaved.
				glVertexArrayAttribBinding(vaoID, iAttrib, 0);

				// Set up vertex divisor for instancing
				if (desc.Divisor != 0)
				{
					glVertexArrayBindingDivisor(vaoID, iAttrib, desc.Divisor);
				}

				auto sizeOpt = OpenGLVertexBindingFormat::FindTypeSize(glVertexElemFmt.Size, glVertexElemFmt.Type);
				if (false == sizeOpt.has_value())
				{
					MOE_ERROR(ChanGraphics, "Failed to find the OpenGL type size of following combination : %u %i", glVertexElemFmt.Size, glVertexElemFmt.Type);
					hasErrors = true;
					break; // invalid, could not find component size
				}

				totalStride += sizeOpt.value();
			}

			iAttrib++;
		}

		if (hasErrors)
		{
			// There was a problem somewhere : delete our created VAO
			glDeleteVertexArrays(1, &vaoID);
			vaoID = 0;
		}

		return { vaoID, totalStride };
	}

	GLuint OpenGL4MaterialManager::BuildPackedVAO(VertexLayoutDescription const& layoutDesc)
	{
		GLuint vaoID;
		glCreateVertexArrays(1, &vaoID);

		bool hasErrors = false; // start optimistic

		int iAttrib = 0;

		for (VertexBindingDescription const& desc : layoutDesc.Bindings)
		{
			auto OpenGLInfoOpt = OpenGLVertexBindingFormat::TranslateFormat(desc.Format);
			if (false == OpenGLInfoOpt.has_value())
			{
				MOE_ERROR(ChanGraphics, "Failed to translate following OpenGL format : %u", desc.Format);
				hasErrors = true;
				break;
			}

			const auto& glVertexElemFmt = OpenGLInfoOpt.value();

			// More of a safety measure because we actually don't need the total size in packed mode.
			auto sizeOpt = OpenGLVertexBindingFormat::FindTypeSize(glVertexElemFmt.Size, glVertexElemFmt.Type);
			if (false == sizeOpt.has_value())
			{
				MOE_ERROR(ChanGraphics, "Failed to find the OpenGL type size of following combination : %u %i", glVertexElemFmt.Size, glVertexElemFmt.Type);
				hasErrors = true;
				break;
			}

			// "Repeats" are necessary to handle vertex attributes like mat4 that can exist in shaders, but are not handled by the API code.
			// For a mat4 as an example, we have to describe a matrix as four vec4 attributes following each other; the repeat count will then be 4.
			for (auto iBind = 0u; iBind < glVertexElemFmt.NumBindings; iBind++)
			{
				// Enable attrib index for given VAO
				glEnableVertexArrayAttrib(vaoID, iAttrib);

				// Specify format for this attrib : type; number of components, normalized. Use 0 as relativeoffset because data will be tightly packed (XXX YYY...)
				glVertexArrayAttribFormat(vaoID, iAttrib, glVertexElemFmt.Size, glVertexElemFmt.Type, glVertexElemFmt.Normalized, 0);

				// As data will be tightly packed, we need to bind each attribute to a different binding index.
				glVertexArrayAttribBinding(vaoID, iAttrib, iAttrib);

				if (desc.Divisor != 0)
				{
					glVertexArrayBindingDivisor(vaoID, iAttrib, desc.Divisor);
				}

				iAttrib++;
			}
		}

		if (hasErrors)
		{
			// There was a problem somewhere : delete our created VAO
			glDeleteVertexArrays(1, &vaoID);
			vaoID = 0;
		}

		return vaoID;
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

		return pso;
	}


	void OpenGL4MaterialManager::BindPipelineStateObject(OpenGL4PipelineStateObject const& pso)
	{
		if (pso.BlendEnabled)
		{
			glEnable(GL_BLEND);
			glBlendFunc(pso.BlendSrcFactor, pso.BlendDestFactor);
			glBlendEquation(pso.BlendEquation);
		}
		else
		{
			glDisable(GL_BLEND);
		}

		if (pso.DepthTestEnabled)
		{
			glEnable(GL_DEPTH_TEST);

			// OpenGL allows us to disable writing to the depth buffer by setting its depth mask to GL_FALSE:
			// Basically, you're (temporarily) using a read-only depth buffer.
			glDepthMask(pso.DepthWriteEnabled);

			glDepthFunc(pso.DepthFunction);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}

		if (pso.StencilTestEnabled)
		{
			glEnable(GL_STENCIL_TEST);

			// allows us to set a bitmask that is ANDed with the stencil value about to be written to the buffer.
			// By default this is set to a bitmask of all 1s, leaving the output unaffected.
			// But if we were to set this to 0x00, all the stencil values written to the buffer would end up as 0s.
			glStencilMask(pso.StencilWriteMask);

			glStencilOpSeparate(GL_FRONT, pso.FrontFaceStencilFailOp, pso.FrontFaceDepthFailOp, pso.FrontFaceBothPassOp);
			glStencilFuncSeparate(GL_FRONT, pso.FrontFaceComparisonFunc, pso.StencilReferenceValue, pso.StencilReadMask);

			glStencilOpSeparate(GL_BACK, pso.BackFaceStencilFailOp, pso.BackFaceDepthFailOp, pso.BackFaceBothPassOp);
			glStencilFuncSeparate(GL_BACK, pso.BackFaceComparisonFunc, pso.StencilReferenceValue, pso.StencilReadMask);
		}
		else
		{
			glDisable(GL_STENCIL_TEST);
		}

		if (pso.CullingEnabled)
		{
			glEnable(GL_CULL_FACE);
			glCullFace(pso.CulledFace);
			glFrontFace(pso.FrontFace);
		}
		else
		{
			glDisable(GL_CULL_FACE);
		}

		glPolygonMode(pso.PolygonFace, pso.PolygonMode);

		// The clipping behavior against the Z position of a vertex can be turned off by activating depth clamping.
		// This is done with glEnable(GL_DEPTH_CLAMP).
		// This will cause the clip - space Z to remain unclipped by the front and rear viewing volume.
		// The Z value computations will proceed as normal through the pipeline.
		// After computing the window-space position, the resulting Z value will be clamped to the glDepthRange (provided by viewport).
		if (pso.EnableDepthClamp)
		{
			glEnable(GL_DEPTH_CLAMP);
		}
		else
		{
			glDisable(GL_DEPTH_CLAMP);
		}

		if (pso.EnableScissorTest)
		{
			glEnable(GL_SCISSOR_TEST);
		}
		else
		{
			glDisable(GL_SCISSOR_TEST);
		}
	}

	// Needed to visit variants https://www.bfilipek.com/2018/09/visit-variants.html
	template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
	template<class... Ts> overload(Ts...)->overload<Ts...>;

	void OpenGL4MaterialManager::BindResourceSets(OpenGL4RHI* rhi, GLuint programID, ResourceSetsDescription const& rscDesc)
	{
		uint32_t blockBindingIdx = 0;

		for (auto const& binding : rscDesc.Bindings)
		{
			std::visit(overload{
				[&](BufferBinding const& buffBind)
				{
					GLuint bufferID = rhi->GLBufferManager().GetBuffer(buffBind.BufferHandle);
					glUniformBlockBinding(programID, blockBindingIdx, buffBind.BindingNumber);
					glBindBufferRange(GL_UNIFORM_BUFFER, buffBind.BindingNumber, bufferID, buffBind.Offset, buffBind.Range);
				},
				[&](TextureBinding const& texBind)
				{
					GLuint texID = rhi->GLTextureManager().GetTextureData(texBind.TextureHandle).TextureID;
					glBindTextureUnit(texBind.BindingNumber, texID);
				}
			}, binding);
		}
	}
}
