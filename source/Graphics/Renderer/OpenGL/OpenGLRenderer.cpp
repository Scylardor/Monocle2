// Monocle Game Engine source files - Alexandre Baron

#ifdef MOE_OPENGL

#include "OpenGLRenderer.h"

#ifdef MOE_STD_SUPPORT
#include <tuple>
#include <optional>
#endif

#include "Core/Misc/Types.h"


namespace moe
{

	struct OpenGLVertexElementFormat
	{
		GLenum			m_type{ GL_FALSE };
		std::uint8_t	m_numCpnts{0};
		GLboolean		m_normalized{false};
	};


	std::optional<OpenGLVertexElementFormat>	TranslateVertexElementFormat(VertexElementFormat vtxFormat)
	{
		switch (vtxFormat)
		{
		case VertexElementFormat::Float:
			return {{GL_FLOAT, 1, false}};
		case VertexElementFormat::Float2:
			return { { GL_FLOAT, 2, false } };
		case VertexElementFormat::Float3:
			return { { GL_FLOAT, 3, false } };
		case VertexElementFormat::Float4:
			return { { GL_FLOAT, 4, false } };
		case VertexElementFormat::Byte:
			return { { GL_UNSIGNED_BYTE, 1, false } };
		case VertexElementFormat::Byte_Norm:
			return { { GL_UNSIGNED_BYTE, 1, true} };
		case VertexElementFormat::Byte2_Norm:
			return { { GL_UNSIGNED_BYTE, 2, true } };
		case VertexElementFormat::Byte2:
			return { { GL_UNSIGNED_BYTE, 2, false } };
		case VertexElementFormat::Byte3_Norm:
			return { { GL_UNSIGNED_BYTE, 3, true } };
		case VertexElementFormat::Byte3:
			return { { GL_UNSIGNED_BYTE, 3, false} };
		case VertexElementFormat::Byte4_Norm:
			return { { GL_UNSIGNED_BYTE, 4, true } };
		case VertexElementFormat::Byte4:
			return { { GL_UNSIGNED_BYTE, 4, false } };
		case VertexElementFormat::SByte:
			return { { GL_BYTE, 4, false } };
		case VertexElementFormat::SByte_Norm:
			return { { GL_BYTE, 4, true } };
		case VertexElementFormat::SByte2_Norm:
			return { { GL_BYTE, 2, true} };
		case VertexElementFormat::SByte2:
			return { { GL_BYTE, 2, false } };
		case VertexElementFormat::SByte3_Norm:
			return { { GL_BYTE, 3, true } };
		case VertexElementFormat::SByte3:
			return { { GL_BYTE, 3, false} };
		case VertexElementFormat::SByte4_Norm:
			return { { GL_BYTE, 4, true} };
		case VertexElementFormat::SByte4:
			return { { GL_BYTE, 4, false} };
		case VertexElementFormat::UShort_Norm:
			return { { GL_UNSIGNED_SHORT, 1, true} };
		case VertexElementFormat::UShort:
			return { { GL_UNSIGNED_SHORT, 1, false } };
		case VertexElementFormat::UShort2_Norm:
			return { { GL_UNSIGNED_SHORT, 2, true } };
		case VertexElementFormat::UShort2:
			return { { GL_UNSIGNED_SHORT, 2, false } };
		case VertexElementFormat::UShort3_Norm:
			return { { GL_UNSIGNED_SHORT, 3, true} };
		case VertexElementFormat::UShort3:
			return { { GL_UNSIGNED_SHORT, 3, false} };
		case VertexElementFormat::UShort4_Norm:
			return { { GL_UNSIGNED_SHORT, 4, true} };
		case VertexElementFormat::UShort4:
			return { { GL_UNSIGNED_SHORT, 4, false } };
		case VertexElementFormat::Short_Norm:
			return { { GL_SHORT, 1, true } };
		case VertexElementFormat::Short:
			return { { GL_SHORT, 1, false} };
		case VertexElementFormat::Short2_Norm:
			return { { GL_SHORT, 2, true } };
		case VertexElementFormat::Short2:
			return { { GL_SHORT, 2, false} };
		case VertexElementFormat::Short3_Norm:
			return { { GL_SHORT, 3, true} };
		case VertexElementFormat::Short3:
			return { { GL_SHORT, 3, false } };
		case VertexElementFormat::Short4_Norm:
			return { { GL_SHORT, 4, true } };
		case VertexElementFormat::Short4:
			return { { GL_SHORT, 4, false } };
		case VertexElementFormat::UInt:
			return { { GL_UNSIGNED_INT, 1, false} };
		case VertexElementFormat::UInt2:
			return { { GL_UNSIGNED_INT, 2, false} };
		case VertexElementFormat::UInt3:
			return { { GL_UNSIGNED_INT, 3, false } };
		case VertexElementFormat::UInt4:
			return { { GL_UNSIGNED_INT, 4, false } };
		case VertexElementFormat::Int:
			return { { GL_INT, 1, false } };
		case VertexElementFormat::Int2:
			return { { GL_INT, 2, false} };
		case VertexElementFormat::Int3:
			return { { GL_INT, 3, false } };
		case VertexElementFormat::Int4:
			return { { GL_INT, 4, false} };
		case VertexElementFormat::HalfFloat:
			return { { GL_HALF_FLOAT, 1, false } };
		case VertexElementFormat::HalfFloat2:
			return { { GL_HALF_FLOAT, 2, false } };
		case VertexElementFormat::HalfFloat3:
			return { { GL_HALF_FLOAT, 3, false } };
		case VertexElementFormat::HalfFloat4:
			return { { GL_HALF_FLOAT, 4, false } };
		default:
			MOE_ERROR(ChanGraphics, "Unrecognized vertex element format value : '%d'", vtxFormat);
			return {};
		}
	}


	std::optional<size_t>	GetTypeSize(std::uint8_t numCpnts, GLenum type)
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
				return sizeof(float) * numCpnts; // Assume sizeof(float) == sizeof(int) == sizeof(unsigned int).
			case GL_SHORT:
			case GL_UNSIGNED_SHORT:
			case GL_HALF_FLOAT:
				return sizeof(short) * numCpnts; // Assume sizeof(short) == sizeof(unsigned short) == sizeof(GLhalf).
			default:
				MOE_ERROR(ChanGraphics, "Unmanaged vertex element type value: '%d'.", type);
				return {};
		}
	}


	void OpenGLRenderer::Shutdown()
	{
		m_shaderManager.Clear();
	}


	VertexLayoutHandle OpenGLRenderer::CreateVertexLayout(const VertexLayoutDescriptor& vertexLayoutDesc)
	{
		GLuint vao;
		// TODO : switch to glGenVertexArrays if retrocompat needed
		glCreateVertexArrays(1, &vao);

		if (vertexLayoutDesc.Type() == VertexLayoutDescriptor::Interleaved)
		{
			std::size_t totalStride{0};

			int iAttrib = 0;

			for (const VertexElementDescriptor& desc : vertexLayoutDesc)
			{
				auto OpenGLInfoOpt = TranslateVertexElementFormat(desc.m_format);
				if (false == OpenGLInfoOpt.has_value())
				{
					return VertexLayoutHandle::Null();
				}

				const auto& glVertexElemFmt = OpenGLInfoOpt.value();

				glEnableVertexArrayAttrib(vao, iAttrib); // vao, index
				glVertexArrayAttribFormat(vao, iAttrib, glVertexElemFmt.m_numCpnts, glVertexElemFmt.m_type, glVertexElemFmt.m_normalized, 0);
				glVertexArrayAttribBinding(vao, iAttrib, 0);

				auto sizeOpt = GetTypeSize(glVertexElemFmt.m_numCpnts, glVertexElemFmt.m_type);
				if (false == sizeOpt.has_value())
				{
					return VertexLayoutHandle::Null();
				}

				totalStride += sizeOpt.value();

				iAttrib++;
			}


			// position attribute
			//const GLuint positionAttribLoc = 0;
			//glEnableVertexArrayAttrib(vao, positionAttribLoc); // vao, index

			//glVertexArrayAttribFormat(vao, positionAttribLoc, 3, GL_FLOAT, GL_FALSE, 0);
			//glVertexArrayAttribBinding(vao, positionAttribLoc, 0);

			//// color attribute
			//const GLuint colorAttribLoc = 1;
			//glEnableVertexArrayAttrib(vao, colorAttribLoc); // vao, index

			//glVertexArrayAttribFormat(VAO, colorAttribLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat));
			//glVertexArrayAttribBinding(VAO, colorAttribLoc, 0);

			//// texture coord attribute
			//const GLuint texAttribLoc = 2;
			//glEnableVertexArrayAttrib(VAO, texAttribLoc); // vao, index

			//glVertexArrayAttribFormat(VAO, texAttribLoc, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat));
			//glVertexArrayAttribBinding(VAO, texAttribLoc, 0);
		}

		return VertexLayoutHandle::Null();
	}


	bool OpenGLRenderer::SetupGraphicsContext(GraphicsContextSetup setupFunc)
	{
		return gladLoadGLLoader((GLADloadproc)setupFunc);
	}
}

#endif // MOE_OPENGL