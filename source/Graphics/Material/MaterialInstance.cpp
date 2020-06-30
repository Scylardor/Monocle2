// Monocle Game Engine source files - Alexandre Baron


#include "MaterialInstance.h"

#include "Graphics/Resources/ResourceLayout/ResourceLayoutDescriptor.h"
#include "Graphics/Resources/ResourceSet/ResourceSetDescriptor.h"


namespace moe
{
	bool MaterialInstance::CreateMaterialResourceSet()
	{
		const auto& rscLayoutDesc = m_device->GetResourceLayoutDescriptor(m_rscLayoutHandle);

		ResourceSetDescriptor newRscSetDesc(m_rscLayoutHandle, rscLayoutDesc.NumBindings());

		for (const ResourceLayoutBindingDescriptor& rscBindingDesc : rscLayoutDesc)
		{
			switch (rscBindingDesc.m_kind)
			{
			case ResourceKind::UniformBuffer:
			{
				auto bufferIt = m_blockBindingBuffers.Find(MaterialBlockBinding(rscBindingDesc.m_bindingPoint));
				if (bufferIt != m_blockBindingBuffers.End())
				{
					DeviceBufferHandle ubHandle = bufferIt->second;
					newRscSetDesc.Emplace(ubHandle);
				}
			}
			break;

			case ResourceKind::TextureReadOnly:
			{
				auto texIt = m_textureUnitBindings.Find(MaterialTextureBinding(rscBindingDesc.m_bindingPoint));
				if (texIt != m_textureUnitBindings.End())
				{
					TextureHandle texHandle = texIt->second;
					newRscSetDesc.Emplace(texHandle);
				}
			}
			break;

			case ResourceKind::Sampler:
			{
				auto samplerIt = m_samplerBindings.Find(MaterialSamplerBinding(rscBindingDesc.m_bindingPoint));
				if (samplerIt != m_samplerBindings.End())
				{
					SamplerHandle samplHandle = samplerIt->second;
					newRscSetDesc.Emplace(samplHandle);
				}
			}
			break;

			default:
				MOE_ASSERT(false);
				MOE_ERROR(ChanGraphics, "Unmanaged ResourceKind value.");
			}
		}

		m_rscSetHandle = m_device->CreateResourceSet(newRscSetDesc);

		return m_rscSetHandle.IsNotNull();
	}


	void MaterialInstance::UpdateTexture(MaterialTextureBinding texBinding, TextureHandle texHandle)
	{
		// TODO: nasty code smell here ! This is a linear search over all resources just to update this one texture
		// because when creating the material resource set, we lose the information of what binding pt is at which index in the resource set...
		// This will have to be redone from scratch !
		BindTexture(texBinding, texHandle);

		const auto& rscLayoutDesc = m_device->GetResourceLayoutDescriptor(m_rscLayoutHandle);

		ResourceSetDescriptor newRscSetDesc(m_rscLayoutHandle, rscLayoutDesc.NumBindings());

		int iRsc = 0;

		for (const ResourceLayoutBindingDescriptor& rscBindingDesc : rscLayoutDesc)
		{
			switch (rscBindingDesc.m_kind)
			{
				case ResourceKind::TextureReadOnly:
				{
					auto texIt = m_textureUnitBindings.Find(MaterialTextureBinding(rscBindingDesc.m_bindingPoint));
					if (texIt != m_textureUnitBindings.End())
					{
						m_device->UpdateResourceSetDescriptor(m_rscSetHandle, iRsc, ResourceHandle(texHandle) );
						return;
					}
				}
				break;

				default:
				break;
			}

			iRsc++;
		}
	}
}
