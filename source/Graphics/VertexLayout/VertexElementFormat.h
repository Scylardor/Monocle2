// Monocle Game Engine source files - Alexandre Baron

#pragma once

namespace moe
{

	/**
	 * \brief Describes the way a vertex element can be laid out in memory.
	 * The Norm versions are for fixed-point types only.
	 * When not normalized, integer data is simply typecast into floating-point format before being passed to the vertex shader.
	 * This means that if you place the integer value 4 into a buffer and use the Int token (not normalized), the value 4.0 will be placed into the shader.
	 * When the data is normalized before being passed to the vertex shader, each component will be automatically divided
	 * by the maximum possible representable positive value, normalizing it. The values are clamped between -1 and 1 for signed types, and 0 and 1 for unsigned types.
	 */
	enum class VertexElementFormat : unsigned char
	{
		//
		// \brief
		//     One 32-bit floating point value.
		Float = 0,
		//
		// \brief :
		//     Two 32-bit floating point values.
		Float2,
		//
		// \brief :
		//     Three 32-bit floating point values.
		Float3,
		//
		// \brief :
		//     Four 32-bit floating point values.
		Float4,
		/**
		* \brief  One 8-bit unsigned integer.
		*/
		Byte,
		/**
		* \brief  One 8-bit unsigned normalized integer.
		*/
		Byte_Norm,
		//
		// \brief :
		//     Two 8-bit unsigned normalized integers.
		Byte2_Norm,
		//
		// \brief :
		//     Two 8-bit unisgned integers.
		Byte2,
		/**
		* \brief  Three 8-bit unsigned normalized integers.
		*/
		Byte3_Norm,
		/**
		* \brief  Three 8-bit unsigned integers.
		*/
		Byte3,
		//
		// \brief :
		//     Four 8-bit unsigned normalized integers.
		Byte4_Norm,
		//
		// \brief :
		//     Four 8-bit unsigned integers.
		Byte4,
		/**
		* \brief  One 8-bit signed integer.
		*/
		SByte,
		/**
		* \brief  One 8-bit signed normalized integer.
		*/
		SByte_Norm,
		//
		// \brief :
		//     Two 8-bit signed normalized integers.
		SByte2_Norm,
		//
		// \brief :
		//     Two 8-bit signed integers.
		SByte2,
		/**
		* \brief  Three 8-bit signed normalized integers.
		*/
		SByte3_Norm,
		/**
		* \brief  Three 8-bit signed integers.
		*/
		SByte3,
		//
		// \brief :
		//     Four 8-bit signed normalized integers.
		SByte4_Norm,
		//
		// \brief :
		//     Four 8-bit signed integers.
		SByte4,
		/**
		* \brief  One 16-bit unsigned normalized integer.
		*/
		UShort_Norm,
		/**
		* \brief One 16-bit unsigned integer.
		*/
		UShort,
		//
		// \brief :
		//     Two 16-bit unsigned normalized integers.
		UShort2_Norm,
		//
		// \brief :
		//     Two 16-bit unsigned integers.
		UShort2,
		/**
		* \brief Three 16-bit normalized unsigned integers.
		*/
		UShort3_Norm,
		/**
		* \brief Three 16-bit normalized integers.
		*/
		UShort3,
		//
		// \brief :
		//     Four 16-bit unsigned normalized integers.
		UShort4_Norm,
		//
		// \brief :
		//     Four 16-bit unsigned integers.
		UShort4,
		/**
		* \brief One 16-bit normalized signed integer.
		*/
		Short_Norm,
		/**
		* \brief One 16-bit signed integer.
		*/
		Short,
		//
		// \brief :
		//     Two 16-bit signed normalized integers.
		Short2_Norm,
		//
		// \brief :
		//     Two 16-bit signed integers.
		Short2,
		/**
		 * \brief Three 16-bit normalized signed integers.
		 */
		Short3_Norm,
		/**
		 * \brief Three 16-bit signed integers.
		 */
		Short3,
		//
		// \brief :
		//     Four 16-bit signed normalized integers.
		Short4_Norm,
		//
		// \brief :
		//     Four 16-bit signed integers.
		Short4,
		//
		// \brief :
		//     One 32-bit unsigned integer.
		UInt,
		//
		// \brief :
		//     Two 32-bit unsigned integers.
		UInt2,
		//
		// \brief :
		//     Three 32-bit unsigned integers.
		UInt3,
		//
		// \brief :
		//     Four 32-bit unsigned integers.
		UInt4,
		//
		// \brief :
		//     One 32-bit signed integer.
		Int,
		//
		// \brief :
		//     Two 32-bit signed integers.
		Int2,
		//
		// \brief :
		//     Three 32-bit signed integers.
		Int3,
		//
		// \brief :
		//     Four 32-bit signed integers.
		Int4,
		//
		// \brief :
		//     One 16-bit S1E5M10 (1 sign bit, 5 exponent bits, 10 mantissa bits) floating point value.
		HalfFloat,
		//
		// \brief :
		//     Two 16-bit S1E5M10 (1 sign bit, 5 exponent bits, 10 mantissa bits) floating point values.
		HalfFloat2,
		/**
		 * \brief Three 16-bit S1E5M10 (1 sign bit, 5 exponent bits, 10 mantissa bits) floating point values.
		 */
		HalfFloat3,
		//
		// \brief :
		//     Four 16-bit S1E5M10 (1 sign bit, 5 exponent bits, 10 mantissa bits) floating point values .
		HalfFloat4
	};

}