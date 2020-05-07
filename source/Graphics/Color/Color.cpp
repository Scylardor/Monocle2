// Monocle Game Engine source files - Alexandre Baron

#include "Color.h"

	moe::ColorRGB operator "" _rgb(unsigned long long hexValue)
	{
		return moe::ColorRGB{ (uint8_t)(hexValue >> 16), (uint8_t)(hexValue >> 8), (uint8_t)hexValue };
	}

	moe::ColorRGBA operator "" _rgba(unsigned long long hexValue)
	{
		return moe::ColorRGBA{ (uint8_t)(hexValue >> 24), (uint8_t)(hexValue >> 16), (uint8_t)(hexValue >> 8), (uint8_t)hexValue };
	}

	moe::ColorRGBf operator "" _rgbf(unsigned long long hexValue)
	{
		return moe::ColorRGBf{ (float)((uint8_t)(hexValue >> 16) / 255.f), (float)((uint8_t)(hexValue >> 8) / 255.f), (float)((uint8_t)(hexValue) / 255.f) };
	}

	moe::ColorRGBAf operator "" _rgbaf(unsigned long long hexValue)
	{
		return moe::ColorRGBAf{ (float)((uint8_t)(hexValue >> 24) / 255.f), (float)((uint8_t)(hexValue >> 16) / 255.f), (float)((uint8_t)(hexValue >> 8) / 255.f), (float)((uint8_t)(hexValue) / 255.f) };

	}

