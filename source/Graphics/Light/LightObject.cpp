// Monocle Game Engine source files - Alexandre Baron

#include "LightObject.h"

#include "LightSystem.h"


namespace moe
{
	void	LightObject::SetPosition(const Vec4& position)
	{
		m_parentSystem->SetLightPosition(m_lightIdx, position);
	}


	void LightObject::SetAmbientColor(const ColorRGBAf& ambient)
	{
		m_parentSystem->SetLightAmbientColor(m_lightIdx, ambient);
	}

	void LightObject::SetDiffuseColor(const ColorRGBAf& diffuse)
	{
		m_parentSystem->SetLightDiffuseColor(m_lightIdx, diffuse);
	}

	void LightObject::SetSpecularColor(const ColorRGBAf& specular)
	{
		m_parentSystem->SetLightSpecularColor(m_lightIdx, specular);
	}
}
