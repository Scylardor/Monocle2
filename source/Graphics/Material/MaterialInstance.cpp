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

				break;
			default:
				MOE_ASSERT(false);
				MOE_ERROR(ChanGraphics, "Unmanaged ResourceKind value.");
			}
		}

		m_rscSetHandle = m_device->CreateResourceSet(newRscSetDesc);

		return m_rscSetHandle.IsNotNull();
	}
}
