// Monocle Game Engine source files - Alexandre Baron

#include "MaterialLibrary.h"

#include "Graphics/Resources/ResourceLayout/ResourceLayoutDescriptor.h"

namespace moe
{
	MaterialInterface MaterialLibrary::GenerateNewMaterial(ShaderProgramHandle progHandle,
	                                                       const MaterialDescriptor& matDesc)
	{
		MaterialInterface newMaterial;

		ResourceLayoutDescriptor newDesc(matDesc.BindingsNum());

		for (const auto& [bindingName, bindingShaderStage] : matDesc)
		{
			const auto bindMapIt = m_bindingMappings.Find(bindingName);
			if (bindMapIt == m_bindingMappings.End())
			{
				/* TODO : log error ... */
				return MaterialInterface();
			}

			const MaterialBindingInfo& bindingInfo = bindMapIt->second;

			newDesc.Emplace(bindingName, bindingInfo.m_bindingPoint, bindingInfo.m_resourceKind, bindingShaderStage);

			switch (bindingInfo.m_resourceKind)
			{
			case ResourceKind::None:
				// TODO: log error...
				break;
			case ResourceKind::UniformBuffer:
				newMaterial.AddBlockBinding(MaterialBlockBinding(bindingInfo.m_bindingPoint));
				break;
			case ResourceKind::TextureReadOnly:
				newMaterial.AddTextureBinding(MaterialTextureBinding(bindingInfo.m_bindingPoint));
				break;
			case ResourceKind::Sampler:
				break;
			default:
				//TODO: log error...
				break;
			}
		}

		ResourceLayoutHandle materialLayout = m_device.CreateResourceLayout(newDesc);

		newMaterial.SetResourceLayout(materialLayout);
		newMaterial.SetShaderProgram(progHandle);

		return newMaterial;
	}


	std::size_t MaterialLibrary::GetUniformBufferSize(const std::string& uboSemantic) const
	{
		const auto bindingIt = m_bindingMappings.Find(uboSemantic);
		if (bindingIt == m_bindingMappings.End())
		{
			// TODO: log error...
			return 0;
		}

		return GetUniformBufferSize(bindingIt->second.m_bindingPoint);
	}


	std::size_t MaterialLibrary::GetUniformBufferSize(uint16_t uboBindingPoint) const
	{
		const auto sizerIt = m_bufferSizers.Find(uboBindingPoint);
		if (sizerIt == m_bufferSizers.End())
		{
			// TODO: log error...
			return 0;
		}

		UniformBufferSizer sizerFunc = sizerIt->second;

		std::size_t bufferSize = sizerFunc();
		return bufferSize;
	}


	MaterialInterface MaterialLibrary::CreateMaterialInterface(ShaderProgramHandle progHandle,
	                                                           const MaterialDescriptor& matDesc)
	{
		MaterialInterface newMaterialInterface = GenerateNewMaterial(progHandle, matDesc);

		return newMaterialInterface;
	}


	MaterialInstance MaterialLibrary::CreateMaterialInstance(const MaterialInterface& materialIntf)
	{
		MaterialInstance newMatInst(materialIntf, &m_device);

		// We can precreate the uniform buffers, but leave the texture creation to the user.
		for (MaterialBlockBinding blockBinding : materialIntf.GetBlockBindings())
		{
			auto bufferSizerIt = m_bufferSizers.Find(blockBinding);
			if (bufferSizerIt != m_bufferSizers.End())
			{
				DeviceBufferHandle ubHandle = m_device.CreateUniformBuffer(nullptr, bufferSizerIt->second());
				newMatInst.BindUniformBuffer(blockBinding, ubHandle);
			}
		}

		return newMatInst;
	}
}
