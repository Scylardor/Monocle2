// Monocle Game Engine source files - Alexandre Baron

#include "LightSystem.h"

#include "Graphics/Resources/ResourceLayout/ResourceLayoutDescriptor.h"
#include "Graphics/Resources/ResourceSet/ResourceSetDescriptor.h"


namespace moe
{
	LightSystem::LightSystem(IGraphicsDevice& device) :
		m_device(device)
	{
		m_lightDataBuffer.Resize(ms_MAX_LIGHTS);
		m_lightObjects.Reserve(ms_MAX_LIGHTS);

		/* TODO : Creating layout here is probably bad and what happens if we need lights in vertex shaders for Gouraud shading ? */
		ResourceLayoutDescriptor lightLayoutDesc{
			{{ "LightCastersData", ResourceKind::UniformBuffer, ShaderStage::Fragment }}
		};

		m_lightsResourceLayout = m_device.CreateResourceLayout(lightLayoutDesc);

		m_lightsUniformBlock = device.CreateUniformBuffer(
			m_lightDataBuffer.Data(), GetLightsBufferSizeBytes());

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
		m_device.UpdateBuffer(m_lightsUniformBlock, m_lightDataBuffer.Data(), LightsNumber() * GetLightDataSizeBytes());

		// buffer is updated
		m_needBufferUpdateFlag = false;
	}


	void LightSystem::BindLightBuffer()
	{
		m_device.BindUniformBlock(MaterialBlockBinding::FRAME_LIGHTS, m_lightsUniformBlock);
	}


	LightObject* LightSystem::AddNewLight(const LightData& newLightData)
	{
		m_lightDataBuffer[m_nextLightIdx] = newLightData;

		m_lightObjects.EmplaceBack(this, m_nextLightIdx);

		m_needBufferUpdateFlag = true; // Will resend lights buffer.

		return &m_lightObjects[m_nextLightIdx++];
	}


	void LightSystem::RemoveLight(LightObject* light)
	{
		auto swappedLightIt = m_lightObjects.EraseBySwapAt(light->GetLightIndex());

		if (swappedLightIt != m_lightObjects.End())
		{
			// Put the swapped light data inside the removed light slot
			m_lightDataBuffer[light->GetLightIndex()] = m_lightDataBuffer[swappedLightIt->GetLightIndex()];

			// Don't forget to update the swapped light index !
			swappedLightIt->SetLightIndex(light->GetLightIndex());
		}

		m_nextLightIdx--; // One light has been removed : decrease the next index.

		m_needBufferUpdateFlag = true; // Will resend lights buffer.
	}


	void LightSystem::SetLightPosition(uint32_t lightIdx, const Vec4& pos)
	{
		m_lightDataBuffer[lightIdx].m_position = pos;
		m_needBufferUpdateFlag = true; // light data buffer has changed: reupload lights.
	}


	void LightSystem::SetLightAmbientColor(uint32_t lightIdx, const ColorRGBAf& ambient)
	{
		m_lightDataBuffer[lightIdx].m_ambientColor = ambient.ToVec();
		m_needBufferUpdateFlag = true; // light data buffer has changed: reupload lights.
	}


	void LightSystem::SetLightDiffuseColor(uint32_t lightIdx, const ColorRGBAf& diffuse)
	{
		m_lightDataBuffer[lightIdx].m_diffuseColor = diffuse.ToVec();
		m_needBufferUpdateFlag = true; // light data buffer has changed: reupload lights.
	}


	void LightSystem::SetLightSpecularColor(uint32_t lightIdx, const ColorRGBAf& specular)
	{
		m_lightDataBuffer[lightIdx].m_specularColor = specular.ToVec();
		m_needBufferUpdateFlag = true; // light data buffer has changed: reupload lights.
	}
}
