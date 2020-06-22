// Monocle Game Engine source files

#pragma once

#include "Graphics/Camera/ViewportHandle.h"
#include "Graphics/Framebuffer/FramebufferHandle.h"
#include "Graphics/Texture/TextureHandle.h"

#include "Core/Misc/Literals.h"
#include "Graphics/Camera/Camera.h"
#include "Graphics/Texture/TextureFormat.h"
#include "Math/Vec3.h"

namespace moe
{
	class IGraphicsDevice;
	class CameraSystem;

	struct OmnidirectionalShadowMapDescriptor
	{
		CameraDescriptor	m_projectionData;
		Width_t				m_textureWidth;
		Height_t			m_textureHeight;
		Vec3				m_lightPosition;
		Vec3				m_lightTargetPoint;

		TextureFormat		m_shadowMapFormat{TextureFormat::Depth24};	// The pixel format of the texture. The more bits, the better (and more expensive).
	};



	struct OmniShadowMappingInfo
	{
		Mat4	m_shadowViewProjMatrices[6];
		float	m_projectionFarPlane;
		Vec3	m_lightPosition;
	};



	class OmnidirectionalShadowDepthPass
	{

	public:
		OmnidirectionalShadowDepthPass(CameraSystem& camSys, IGraphicsDevice& device, const OmnidirectionalShadowMapDescriptor& shadowMapDesc);
		~OmnidirectionalShadowDepthPass() = default; // TODO: free allocated resources!

		uint32_t			GetShadowmapCamera() const	{ return m_lightProjectionCam->GetCameraIndex(); }

		const Vec3&			GetLightPosition() const { return m_lightProjectionCam->GetTransform().Matrix().GetTranslation(); }

		FramebufferHandle	GetFramebuffer() const		{ return m_depthMapFBO; }

		TextureHandle		GetShadowMap() const		{ return m_shadowMapTexture; }

		const OmniShadowMappingInfo&	GetShadowMappingInfo() const { return m_shadowmapInfo; }

	private:
		TextureHandle		m_shadowMapTexture;
		ViewportHandle		m_shadowMapViewport;
		FramebufferHandle	m_depthMapFBO;

		Camera*				m_lightProjectionCam = nullptr;

		Mat4				m_lightSpaceProjectionMatrix;

		OmniShadowMappingInfo	m_shadowmapInfo;
	};




}
