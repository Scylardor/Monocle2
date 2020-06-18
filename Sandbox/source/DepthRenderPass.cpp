// Monocle Game Engine source files

#include "DepthRenderPass.h"

#include "Graphics/Device/GraphicsDevice.h"

#include "Graphics/Camera/CameraSystem.h"

namespace moe
{
	DepthRenderPass::DepthRenderPass(CameraSystem& camSys, IGraphicsDevice& device, const ShadowMapDescriptor& shadowMapDesc)
	{
		// Create the shadow map viewport
		ViewportDescriptor vpDesc{0, 0, (float)shadowMapDesc.m_textureWidth, (float)shadowMapDesc.m_textureHeight};
		m_shadowMapViewport = device.CreateViewport(vpDesc);
		MOE_DEBUG_ASSERT(m_shadowMapViewport.IsNotNull());

		// Create the shadow map camera
		// It will be a camera actually looking down the direction of the light rays, using a custom projection
		// (orthographic for a directional light for example, because the rays of a directional lights are parallel, like the orthographic projection lines)
		m_lightProjectionCam = camSys.AddNewCamera(m_shadowMapViewport, shadowMapDesc.m_projectionData);
		if (MOE_ASSERT(m_lightProjectionCam != nullptr))
		{
			m_lightProjectionCam->SetTransform(Transform::Translate(shadowMapDesc.m_lightPosition));
			m_lightProjectionCam->LookAt(shadowMapDesc.m_lightTargetPoint, Vec3(0, 1, 0)); // TODO: beware of up vector if going left-handed / changing coordinate system.
		}

		// Now create the depth map
		// TODO : is TextureUsage::DepthStencil the right usage here ?
		Texture2DDescriptor depthMapDesc{nullptr, shadowMapDesc.m_textureWidth, shadowMapDesc.m_textureHeight, shadowMapDesc.m_shadowMapFormat, TextureUsage::DepthStencil, 1 };
		m_shadowMapTexture = device.CreateTexture2D(depthMapDesc);
		MOE_DEBUG_ASSERT(m_shadowMapTexture.IsNotNull());

		// Finally create the depth map framebuffer
		FramebufferDescriptor fbDesc;
		fbDesc.m_depthAttachment = Texture2DHandle{m_shadowMapTexture}; // TODO: this Texture2DHandle is ugly, remove Texture2DHandle !
		fbDesc.m_readBuffer = TargetBuffer::None;
		fbDesc.m_drawBuffer = TargetBuffer::None;

		m_depthMapFBO = device.CreateFramebuffer(fbDesc);
		MOE_DEBUG_ASSERT(m_depthMapFBO.IsNotNull());
	}

}
