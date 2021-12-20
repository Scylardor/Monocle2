// Monocle Game Engine source files - Alexandre Baron

#pragma once


/* TODO : Make a generic Vector header... */
#include "Math/Vec3.h"


#include "Monocle_Graphics_Export.h"


namespace moe
{
	template <typename T>
	struct Limits
	{

		static T min();
		static T max();
	};

	template <>
	struct Limits<uint8_t>
	{
		static uint8_t min() { return 0; }
		static uint8_t max() { return 255; }
	};

	template <>
	struct Limits<float>
	{
		static float min() { return 0.f; }
		static float max() { return 1.f; }
	};


	/**
	 * \brief A very simple wrapper over a color value.
	 * Val values will be normalized (between 0 and 1).
	 */
	template <unsigned NumChannels, typename Val>
	struct Color
	{
		static_assert(NumChannels > 0 && NumChannels <= 4, "unsupported number of color channels");

		explicit Color(Val value)
		{
			std::fill_n(Rgba, NumChannels, value); // array name, size, value
			Normalize();
		}

		template<typename = std::enable_if_t<NumChannels >= 2>>
		Color(Val red, Val green) :
			Rgba{ red, green, Val(0) }
		{
			Normalize();
		}

		template<typename = std::enable_if_t<NumChannels == 3>>
		Color(Val red, Val green, Val blue) :
			Rgba{ red, green, blue }
		{
			Normalize();
		}

		template<typename = std::enable_if_t<NumChannels == 4>>
		Color(Val red, Val green, Val blue, Val alpha = Val(1)) :
			Rgba{red, green, blue, alpha}
		{
			Normalize();
		}

		/**
		 * \brief Normalizes floating point values between 0 and 1.
		 */
		void	Normalize()
		{
			for (Val& val : Rgba)
			{
				val = std::min(std::max(val, Limits<Val>::min()), Limits<Val>::max());
			}
		}

		[[nodiscard]] Val	R() const { return Rgba[0]; }

		template<typename = std::enable_if_t<NumChannels >= 2>>
		[[nodiscard]] Val	G() const { return Rgba[1]; }

		template<typename = std::enable_if_t<NumChannels >= 3>>
		[[nodiscard]] Val	B() const { return Rgba[2]; }

		template<typename = std::enable_if_t<NumChannels == 4>>
		[[nodiscard]] Val	A() const { return Rgba[3]; }



		/**
		 * \brief Convenience function to convert the color to a math vector.
		 * \return A math vector containing the color values
		 */
		[[nodiscard]] Vec<NumChannels, Val>	ToVec() const
		{
			return Vec<NumChannels, Val>(Rgba);
		}

		[[nodiscard]] const Val*	Ptr() const { return Rgba; }


		static	Color	Red()
		{
			return Color(255, 0, 0);
		}

		static	Color	Green()
		{
			return Color(0, 255);
		}

		static	Color	Blue()
		{
			return Color(0, 0, 255);
		}

		static	Color	White()
		{
			return Color(255, 255, 255);
		}

		static	Color	Black()
		{
			return Color(0);
		}


		bool operator==(const Color& rhs) const
		{
			return memcmp(Rgba, rhs.Rgba, sizeof(Val) * NumChannels) == 0;
		}

	private:

		Val	Rgba[NumChannels] { Val(1) };
	};

	typedef Color<3, uint8_t>	ColorRGB;
	typedef Color<4, uint8_t>	ColorRGBA;

	typedef Color<3, float>	ColorRGBf;
	typedef Color<4, float>	ColorRGBAf;


}

Monocle_Graphics_API moe::ColorRGB operator "" _rgb(unsigned long long hexValue);
Monocle_Graphics_API moe::ColorRGBA operator "" _rgba(unsigned long long hexValue);
Monocle_Graphics_API moe::ColorRGBf operator "" _rgbf(unsigned long long hexValue);
Monocle_Graphics_API moe::ColorRGBAf operator "" _rgbaf(unsigned long long hexValue);