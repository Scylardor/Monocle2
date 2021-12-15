// Monocle Game Engine source files

#pragma once

#include "Core/Misc/Types.h"

#include "Graphics/Color/Color.h"

namespace moe
{

	/**
	 * \brief The filter used by a given sampler for texture minification / magnification.
	 * Magnification happens when the texture mapping makes the texture appear bigger in screen space than its actual resolution.
	 * Minification is the opposite: when the texture is being shrunken relative to its natural resolution.
	 * NOTE: for now, only Nearest and Linear are supported for magnification filter.
	 */
	enum class SamplerFilter : uint8_t
	{
		Nearest = 0,			// Takes nearest-neighbor texel value.
		Linear,					// Takes the nearest 4 texels and interpolates. Using Linear for Mag and Min filters is commonly called bilinear filtering.
		NearestMipmapNearest,	// Detects which mipmap is nearest to our fragment area, then takes nearest-neighbor texel value within that one mipmap.
		LinearMipmapNearest,	// Detects which mipmap is nearest to our fragment area, then use linear filtering of the four nearest samples within that one mipmap.
		LinearMipmapLinear		// Pick the two nearest mipmap levels, then use linear filtering to blend between the values fetched from the two textures. This is is commonly called trilinear filtering.
	};


	/**
	 * \brief The wrapping algorithm used to fetch texels when texture coordinates are outside the [0-1] standard range.
	 */
	enum class SamplerWrapping : uint8_t
	{
		Repeat = 0,		// The default behavior for textures. Repeats the texture image.
		MirroredRepeat,	// Same as Repeat but mirrors the image with each repeat.
		ClampToEdge,	// Clamps the coordinates between 0 and 1. The result is that higher coordinates become clamped to the edge, resulting in a stretched edge pattern.
		ClampToBorder	// Coordinates outside the range are now given a user-specified border color.
	};


	/**
	 * \brief A graphics APi-agnostic descriptor of a sampler.
	 * A sampler is a graphics object that you can bind to a texture and that will control how this texture's texels are processed.
	 * They are usually holding a bunch of following parameters:
	 * - edge value sampling, i.e. the texel fetching behavior when wrapping in S/T/R directions
	 * - texture filtering parameters (for minification / magnification)
	 * - border color
	 * - anisotropic filtering
	 * - LOD range and bias
	 * - etc...
	 */
	struct SamplerDescriptor
	{
		enum class AnisotropyLevel
		{
			Disabled = -1,
			Minimum = -2,
			Maximum = -3
		};

		SamplerDescriptor() = default;

		SamplerFilter	m_magFilter{SamplerFilter::Linear};				// magnification filter
		SamplerFilter	m_minFilter{SamplerFilter::LinearMipmapLinear};	// minification filter, use trilinear filtering by default
		float			m_anisotropy{1.f};	// Number of samples used for anisotropic filtering, will be clamped to the implementation's max anisotropy value.

		SamplerWrapping	m_wrap_S{SamplerWrapping::Repeat};	// Wrapping method used on texture horizontal axis (S).
		SamplerWrapping	m_wrap_T{SamplerWrapping::Repeat};	// Wrapping method used on texture vertical axis (T).
		SamplerWrapping	m_wrap_R{SamplerWrapping::Repeat};	// Wrapping method used on texture depth axis for 3D textures (R).
		ColorRGBAf		m_borderColor{ColorRGBAf::Black()};

		float			m_lodRangeMin{ -1000 };	// Sets the minimum level-of-detail parameter. This floating-point value limits the selection of highest resolution mipmap (lowest mipmap level).
		float			m_lodRangeMax{ 1000 };	// Sets the maximum level-of-detail parameter.This floating-point value limits the selection of the lowest resolution mipmap (highest mipmap level).
		float			m_lodBias{ 0.0f };		// Sets a fixed bias value that is to be added to the level-of-detail parameter of the texture. Clamped to the implementation's max lod bias.
	};


	/**
	 * \brief A graphics APi-agnostic descriptor of a sampler.
	 * A sampler is a graphics object that you can bind to a texture and that will control how this texture's texels are processed.
	 * They are usually holding a bunch of following parameters:
	 * - edge value sampling, i.e. the texel fetching behavior when wrapping in S/T/R directions
	 * - texture filtering parameters (for minification / magnification)
	 * - border color
	 * - anisotropic filtering
	 * - LOD range and bias
	 * - etc...
	 */
	struct SamplerDescription
	{
		enum class AnisotropyLevel
		{
			Disabled = -1,
			Minimum = -2,
			Maximum = -3
		};

		SamplerDescription() = default;

		void	SetAnisotropicFiltering(float level)
		{
			m_anisotropy = (AnisotropyLevel)level;
		}

	private:

		[[nodiscard]] auto Properties() const
		{
			return std::tie(m_magFilter, m_minFilter, m_anisotropy, m_wrap_S, m_wrap_T, m_wrap_R, m_borderColor, m_lodRangeMin, m_lodRangeMax, m_lodBias);
		}

	public:

		bool operator==(SamplerDescription const& other) const
		{
			return (Properties() == other.Properties());
		}


		SamplerFilter	m_magFilter{ SamplerFilter::Linear };				// magnification filter
		SamplerFilter	m_minFilter{ SamplerFilter::LinearMipmapLinear };	// minification filter, use trilinear filtering by default
		AnisotropyLevel	m_anisotropy{ AnisotropyLevel::Maximum };			// Number of samples used for anisotropic filtering, will be clamped to the implementation's max anisotropy value.

		SamplerWrapping	m_wrap_S{ SamplerWrapping::Repeat };	// Wrapping method used on texture horizontal axis (S).
		SamplerWrapping	m_wrap_T{ SamplerWrapping::Repeat };	// Wrapping method used on texture vertical axis (T).
		SamplerWrapping	m_wrap_R{ SamplerWrapping::Repeat };	// Wrapping method used on texture depth axis for 3D textures (R).
		ColorRGBAf		m_borderColor{ ColorRGBAf::Black() };

		float			m_lodRangeMin{ -1000 };	// Sets the minimum level-of-detail parameter. This floating-point value limits the selection of highest resolution mipmap (lowest mipmap level).
		float			m_lodRangeMax{ 1000 };	// Sets the maximum level-of-detail parameter.This floating-point value limits the selection of the lowest resolution mipmap (highest mipmap level).
		float			m_lodBias{ 0.0f };		// Sets a fixed bias value that is to be added to the level-of-detail parameter of the texture. Clamped to the implementation's max lod bias.
	};

}