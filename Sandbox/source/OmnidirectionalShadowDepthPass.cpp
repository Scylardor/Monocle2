// Monocle Game Engine source files

#include "OmnidirectionalShadowDepthPass.h"

#include "Graphics/Device/GraphicsDevice.h"

#include "Graphics/Camera/CameraSystem.h"

namespace moe
{
	OmnidirectionalShadowDepthPass::OmnidirectionalShadowDepthPass(CameraSystem& camSys, IGraphicsDevice& device, const OmnidirectionalShadowMapDescriptor& shadowMapDesc)
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
			m_shadowmapInfo.m_lightPosition = shadowMapDesc.m_lightPosition;

			const Vec3& lightPos = m_shadowmapInfo.m_lightPosition;
			m_lightProjectionCam->SetTransform(Transform::Translate(lightPos));

			// Construct a different view-projection matrix for each of the cubemap's faces.
			// TODO: beware of vector directions and order of operations if going left-handed / changing coordinate system.
			std::pair<Vec3, Vec3> lookatTargetAndUpVectors[6] = {
				{Vec3(1.0f,  0.0f,  0.0f),			Vec3(0.0f, -1.0f,  0.0f)},
				{Vec3(-1.0f,  0.0f,  0.0f),			Vec3(0.0f, -1.0f,  0.0f)},
				{Vec3(0.0f,  1.0f,  0.0f),			Vec3(0.0f,  0.0f,  1.0f)},
				{Vec3(0.0f, -1.0f,  0.0f),		Vec3(0.0f,  0.0f, -1.0f)},
				{Vec3(0.0f,  0.0f,  1.0f),		Vec3(0.0f, -1.0f,  0.0f)},
				{Vec3(0.0f,  0.0f, -1.0f),		Vec3(0.0f, -1.0f,  0.0f)}
			};

			const Mat4& projection = m_lightProjectionCam->GetProjectionMatrix();
			for (auto iFace = 0; iFace < 6; iFace++)
			{
				const auto& lookatTargetUpVectorPair = lookatTargetAndUpVectors[iFace];
				m_shadowmapInfo.m_shadowViewProjMatrices[iFace] = projection *
					Mat4::LookAtMatrix(lightPos, lightPos + lookatTargetUpVectorPair.first, lookatTargetUpVectorPair.second);
			}

			m_shadowmapInfo.m_projectionFarPlane = m_lightProjectionCam->GetFar();
		}

		// Now create the depth map
		// TODO : is TextureUsage::DepthStencil the right usage here ?
		CubeMapTextureDescriptor depthMapDesc{nullptr,
			shadowMapDesc.m_textureWidth, shadowMapDesc.m_textureHeight,
			shadowMapDesc.m_shadowMapFormat, shadowMapDesc.m_shadowMapFormat, TextureUsage::DepthStencil, 1 };
		m_shadowMapTexture = device.CreateCubemapTexture(depthMapDesc);
		MOE_DEBUG_ASSERT(m_shadowMapTexture.IsNotNull());

		// Finally create the depth map framebuffer
		FramebufferDescriptor fbDesc;
		fbDesc.m_depthAttachment = TextureHandle{m_shadowMapTexture};
		fbDesc.m_readBuffer = TargetBuffer::None;
		fbDesc.m_drawBuffer = TargetBuffer::None;

		m_depthMapFBO = device.CreateFramebuffer(fbDesc);
		MOE_DEBUG_ASSERT(m_depthMapFBO.IsNotNull());
	}

}
