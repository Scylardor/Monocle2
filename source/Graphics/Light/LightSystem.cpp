// Monocle Game Engine source files - Alexandre Baron

#include "LightSystem.h"

#include "Graphics/Resources/ResourceLayout/ResourceLayoutDescriptor.h"
#include "Graphics/Resources/ResourceSet/ResourceSetDescriptor.h"


namespace moe
{
	LightSystem::LightSystem(IGraphicsDevice& device) :
		m_device(device)
	{
		//m_lightDataBuffer.Resize(MAX_LIGHTS);
		m_lightObjects.Reserve(MAX_LIGHTS);

		/* TODO : Creating layout here is probably bad and what happens if we need lights in vertex shaders for Gouraud shading ? */
		ResourceLayoutDescriptor lightLayoutDesc{
			{{ "LightCastersData", ResourceKind::UniformBuffer, ShaderStage::Fragment }}
		};

		m_lightsResourceLayout = m_device.CreateResourceLayout(lightLayoutDesc);

	//	m_lightsUniformBlock = device.CreateUniformBuffer(
		//	m_lightDataBuffer.Data(), GetLightsBufferSizeBytes());

		m_lightsUniformBlock = device.CreateUniformBuffer(
			&m_lightCastersData, sizeof(m_lightCastersData));

		ResourceSetDescriptor lightSetDesc{
			m_lightsResourceLayout,
			{m_lightsUniformBlock}
		};

		m_lightsResourceSet = m_device.CreateResourceSet(lightSetDesc);
	}


	LightSystem::~LightSystem()
	{
		/* TODO : free uniform buffer */
	}


	void LightSystem::UpdateLights()
	{
		if (!NeedsUpdate())
			return; // nothing to do

		// Don't update the full buffer but only up to the number of lights currently in flight.
		//m_device.UpdateBuffer(m_lightsUniformBlock, m_lightDataBuffer.Data(), LightsNumber() * GetLightDataSizeBytes());

		// Don't update the full buffer but only up to the number of lights currently in flight.
		// Send sizeof(uint32_t) first to also send the updated number of lights !
		m_device.UpdateBuffer(m_lightsUniformBlock, &m_lightCastersData, sizeof(uint32_t) + LightsNumber() * GetLightDataSizeBytes());

		// buffer is updated
		m_needBufferUpdateFlag = false;
	}


	void LightSystem::BindLightBuffer()
	{
		m_device.BindUniformBlock(MaterialBlockBinding::FRAME_LIGHTS, m_lightsUniformBlock);
	}


	LightObject* LightSystem::AddNewLight(LightData newLightData)
	{
		MOE_ASSERT(m_lightCastersData.m_numLights != MAX_LIGHTS);

		m_lightCastersData.m_lightDataBuffer[m_lightCastersData.m_numLights] = std::move(newLightData);

		m_lightObjects.EmplaceBack(this, m_lightCastersData.m_numLights);

		m_needBufferUpdateFlag = true; // Will resend lights buffer.

		return &m_lightObjects[m_lightCastersData.m_numLights++];
	}


	void LightSystem::RemoveLight(LightObject* light)
	{
		auto swappedLightIt = m_lightObjects.EraseBySwapAt(light->GetLightIndex());

		if (swappedLightIt != m_lightObjects.End())
		{
			// Put the swapped light data inside the removed light slot
			m_lightCastersData.m_lightDataBuffer[light->GetLightIndex()] = m_lightCastersData.m_lightDataBuffer[swappedLightIt->GetLightIndex()];

			// Don't forget to update the swapped light index !
			swappedLightIt->SetLightIndex(light->GetLightIndex());
		}

		// One light has been removed : decrease the next index.
		m_lightCastersData.m_numLights--;

		m_needBufferUpdateFlag = true; // Will resend lights buffer.
	}


	void LightSystem::SetLightPosition(uint32_t lightIdx, const Vec4& pos)
	{
		m_lightCastersData.m_lightDataBuffer[lightIdx].m_position = pos;

		m_needBufferUpdateFlag = true; // light data buffer has changed: reupload lights.
	}


	void LightSystem::SetLightDirection(uint32_t lightIdx, const Vec3& dir)
	{
		m_lightCastersData.m_lightDataBuffer[lightIdx].m_direction = Vec4(dir, 0);

		m_needBufferUpdateFlag = true; // light data buffer has changed: reupload lights.
	}


	void LightSystem::SetLightAmbientColor(uint32_t lightIdx, const ColorRGBAf& ambient)
	{
		m_lightCastersData.m_lightDataBuffer[lightIdx].m_ambientColor = ambient.ToVec();

		m_needBufferUpdateFlag = true; // light data buffer has changed: reupload lights.
	}


	void LightSystem::SetLightDiffuseColor(uint32_t lightIdx, const ColorRGBAf& diffuse)
	{
		m_lightCastersData.m_lightDataBuffer[lightIdx].m_diffuseColor = diffuse.ToVec();

		m_needBufferUpdateFlag = true; // light data buffer has changed: reupload lights.
	}


	void LightSystem::SetLightSpecularColor(uint32_t lightIdx, const ColorRGBAf& specular)
	{
		m_lightCastersData.m_lightDataBuffer[lightIdx].m_specularColor = specular.ToVec();

		m_needBufferUpdateFlag = true; // light data buffer has changed: reupload lights.
	}


	void LightSystem::MakeDirectionalLight(uint32_t lightIdx, const Vec3& direction)
	{
		m_lightCastersData.m_lightDataBuffer[lightIdx].m_position = Vec4::ZeroVector();
		m_lightCastersData.m_lightDataBuffer[lightIdx].m_direction = Vec4(direction, 0); // Put 0 in w because this is a direction, not a position

		m_needBufferUpdateFlag = true; // light data buffer has changed: reupload lights.
	}


	void LightSystem::SetConstantAttenuation(uint32_t lightIdx, float cstAtten)
	{
		m_lightCastersData.m_lightDataBuffer[lightIdx].m_constantAttenuation = cstAtten;

		m_needBufferUpdateFlag = true; // light data buffer has changed: reupload lights.
	}


	void LightSystem::SetLinearAttenuation(uint32_t lightIdx, float linAtten)
	{
		m_lightCastersData.m_lightDataBuffer[lightIdx].m_linearAttenuation = linAtten;

		m_needBufferUpdateFlag = true; // light data buffer has changed: reupload lights.
	}


	void LightSystem::SetQuadraticAttenuation(uint32_t lightIdx, float quadAtten)
	{
		m_lightCastersData.m_lightDataBuffer[lightIdx].m_quadraticAttenuation = quadAtten;

		m_needBufferUpdateFlag = true; // light data buffer has changed: reupload lights.
	}


	void LightSystem::SetAttenuationFactors(uint32_t lightIdx, float constant, float linear, float quadratic)
	{
		m_lightCastersData.m_lightDataBuffer[lightIdx].m_constantAttenuation = constant;
		m_lightCastersData.m_lightDataBuffer[lightIdx].m_linearAttenuation = linear;
		m_lightCastersData.m_lightDataBuffer[lightIdx].m_quadraticAttenuation = quadratic;

		m_needBufferUpdateFlag = true; // light data buffer has changed: reupload lights.

	}


	void LightSystem::SetLightSpotInnerCutoff(uint32_t lightIdx, float cutoff)
	{
		m_lightCastersData.m_lightDataBuffer[lightIdx].m_spotLightInnerCutoff = cutoff;

		m_needBufferUpdateFlag = true; // light data buffer has changed: reupload lights.

	}


	void LightSystem::SetLightSpotOuterCutoff(uint32_t lightIdx, float cutoff)
	{
		m_lightCastersData.m_lightDataBuffer[lightIdx].m_spotLightOuterCutoff = cutoff;

		m_needBufferUpdateFlag = true; // light data buffer has changed: reupload lights.
	}
}
