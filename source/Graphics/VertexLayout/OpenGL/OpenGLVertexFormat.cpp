// Monocle Game Engine source files - Alexandre Baron

#ifdef MOE_OPENGL

#include "OpenGLVertexFormat.h"

#include "Core/Log/moeLog.h"

#include "Core/Preprocessor/moeAssert.h"


namespace moe
{
	std::optional<OpenGLVertexElementFormat> OpenGLVertexElementFormat::TranslateFormat(
		VertexElementFormat vtxFormat)
	{
		switch (vtxFormat)
		{
		case VertexElementFormat::Float:
			return { {GL_FLOAT, 1, false} };
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


	std::optional<uint32_t> OpenGLVertexElementFormat::FindTypeSize(std::uint8_t numCpnts, GLenum type)
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
}
#endif