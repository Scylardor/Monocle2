// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Graphics/Color/Color.h"

#include "Math/Vec4.h"

#include "Monocle_Graphics_Export.h"


namespace moe
{
	class LightSystem;

	class LightObject
	{
	public:
		LightObject(LightSystem* parentSystem, uint32_t lightIdx) :
			m_parentSystem(parentSystem), m_lightIdx(lightIdx)
		{}


		uint32_t	GetLightIndex() const				{ return m_lightIdx; }
		void		SetLightIndex(uint32_t newIndex)	{ m_lightIdx = newIndex; }

		Monocle_Graphics_API void	SetPosition(const Vec4& position);
		Monocle_Graphics_API void	SetAmbientColor(const ColorRGBAf& ambient);
		Monocle_Graphics_API void	SetDiffuseColor(const ColorRGBAf& diffuse);
		Monocle_Graphics_API void	SetSpecularColor(const ColorRGBAf& specular);

	private:
		LightSystem*	m_parentSystem = nullptr;

		uint32_t	m_lightIdx = 0;
	};

}