// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Graphics/Material/MaterialInterface.h"
#include "Graphics/DeviceBuffer/DeviceBufferHandle.h"
#include "Graphics/Resources/ResourceSet/ResourceSetHandle.h"
#include "Graphics/Device/GraphicsDevice.h"

#include "Math/Vec4.h"

#include "Core/Containers/HashMap/HashMap.h"

#include "Monocle_Graphics_Export.h"


namespace moe
{

	class MaterialInstance
	{
	public:
		MaterialInstance() = default;

		MaterialInstance(const MaterialInterface& matIntf, IGraphicsDevice* device) :
			m_device(device), m_rscLayoutHandle(matIntf.GetResourceLayout()), m_programHandle(matIntf.GetShaderProgram())
		{}


		void	BindUniformBuffer(MaterialBlockBinding blockBinding, DeviceBufferHandle ubHandle)
		{
			// Use operator[] and not Insert to replace the existing handle if there was already one.
			m_blockBindingBuffers[blockBinding] = ubHandle;
		}

		void	BindTexture(MaterialTextureBinding texBinding, const TextureHandle& texHandle)
		{
			m_textureUnitBindings[texBinding] = texHandle;
		}

		void	BindSampler(MaterialSamplerBinding samplerBinding, SamplerHandle samplerHandle)
		{
			m_samplerBindings[samplerBinding] = samplerHandle;
		}

		Monocle_Graphics_API bool	CreateMaterialResourceSet();

		template <typename T>
		void UpdateUniformBlock(MaterialBlockBinding bufferBinding, const T& value);

		template <typename T>
		void UpdateUniformBlockVariable(MaterialBlockBinding bufferBinding, const std::string& variableName,
		                                const T& value);


		void	SetVec4(MaterialBlockBinding bufferBinding, const std::string& variableName, const Vec4& value)
		{
			UpdateUniformBlockVariable(bufferBinding, variableName, value);
		}

		void	SetFloat(MaterialBlockBinding bufferBinding, const std::string& variableName, float value)
		{
			UpdateUniformBlockVariable(bufferBinding, variableName, value);
		}

		Monocle_Graphics_API void	UpdateTexture(MaterialTextureBinding texBinding, TextureHandle texHandle);

		ShaderProgramHandle	GetShaderHandle() const { return m_programHandle; }

		ResourceSetHandle	GetResourceSetHandle() const { return m_rscSetHandle; }

	private:

		IGraphicsDevice*		m_device = nullptr;

		ShaderProgramHandle		m_programHandle;
		ResourceLayoutHandle	m_rscLayoutHandle;
		ResourceSetHandle		m_rscSetHandle;

		HashMap<MaterialBlockBinding, DeviceBufferHandle>	m_blockBindingBuffers;
		HashMap<MaterialTextureBinding, TextureHandle>		m_textureUnitBindings;
		HashMap<MaterialSamplerBinding, SamplerHandle >		m_samplerBindings;
	};


	template <typename T>
	void MaterialInstance::UpdateUniformBlock(MaterialBlockBinding bufferBinding, const T& value)
	{
		if (!MOE_ASSERT(m_device != nullptr))
		{
			// TODO: log error...
			return;
		}

		auto bufferIt = m_blockBindingBuffers.Find(bufferBinding);

		if (!MOE_ASSERT(bufferIt != m_blockBindingBuffers.End()))
		{
			// TODO: log error...
			return;
		}

		DeviceBufferHandle bufferHandle = bufferIt->second;

		m_device->UpdateBuffer(bufferHandle, (const void*)&value, sizeof(T));
	}


	template <typename T>
	void MaterialInstance::UpdateUniformBlockVariable(MaterialBlockBinding bufferBinding,
	                                                  const std::string& variableName, const T& value)
	{
		if (!MOE_ASSERT(m_device != nullptr))
		{
			// TODO: log error...
			return;
		}

		auto bufferIt = m_blockBindingBuffers.Find(bufferBinding);

		if (!MOE_ASSERT(bufferIt != m_blockBindingBuffers.End()))
		{
			// TODO: log error...
			return;
		}

		DeviceBufferHandle bufferHandle = bufferIt->second;

		m_device->UpdateUniformBlockVariable(m_programHandle, bufferHandle, variableName, &value, sizeof(T));
	}
}
