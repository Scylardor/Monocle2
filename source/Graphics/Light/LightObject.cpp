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


	void LightObject::SetDirectionalLight(const Vec3& lightDirection)
	{
		m_parentSystem->MakeDirectionalLight(m_lightIdx, lightDirection);
	}


	void LightObject::SetSpotlightDirection(const Vec3& lightDirection)
	{
		m_parentSystem->SetLightDirection(m_lightIdx, lightDirection);
	}


	void LightObject::SetConstantAttenuation(float cstAtten)
	{
		m_parentSystem->SetConstantAttenuation(m_lightIdx, cstAtten);
	}

	void LightObject::SetLinearAttenuation(float linAtten)
	{
		m_parentSystem->SetLinearAttenuation(m_lightIdx, linAtten);
	}

	void LightObject::SetQuadraticAttenuation(float quadAtten)
	{
		m_parentSystem->SetQuadraticAttenuation(m_lightIdx, quadAtten);
	}

	void LightObject::SetAttenuationFactors(float constant, float linear, float quadratic)
	{
		m_parentSystem->SetAttenuationFactors(m_lightIdx, constant, linear, quadratic);
	}


	void LightObject::SetSpotInnerCutoff(Degs_f cutoffAngle)
	{
		// To save some performance, we calculate the cosine value of the cutoff angle CPU-side beforehand and pass this to the shader.
		// This will allow the shader to do simple cos-cos comparisons (using dot product) without having to use expensive inverse cosine operations.
		Rads_f cutoffRads{cutoffAngle};
		float cutoffCos = std::cosf(cutoffRads);

		m_parentSystem->SetLightSpotInnerCutoff(m_lightIdx, cutoffCos);

	}

	void LightObject::SetSpotOuterCutoff(Degs_f cutoffAngle)
	{
		// To save some performance, we calculate the cosine value of the cutoff angle CPU-side beforehand and pass this to the shader.
// This will allow the shader to do simple cos-cos comparisons (using dot product) without having to use expensive inverse cosine operations.
		Rads_f cutoffRads{ cutoffAngle };
		float cutoffCos = std::cosf(cutoffRads);

		m_parentSystem->SetLightSpotOuterCutoff(m_lightIdx, cutoffCos);
	}
}
