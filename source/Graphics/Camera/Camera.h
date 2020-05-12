// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Math/Matrix.h"

#include "ViewportHandle.h"

#include "Graphics/RenderWorld/GraphicsObject.h"

#include "Graphics/DeviceBuffer/UniformBufferHandle.h"

#include "Monocle_Graphics_Export.h"


namespace moe
{
	class RenderWorld;

	enum class CameraProjection : uint8_t
	{
		Orthographic,
		Perspective
	};

	struct CameraDescriptor
	{
		CameraProjection	m_proj{CameraProjection::Perspective};

	};

	struct OrthographicCameraDesc
	{
		float	m_left = 0.0f;
		float	m_right = 800.0f;
		float	m_bottom = 0.0f;
		float	m_top = 600.0f;
		float	m_near = 0.1f;
		float	m_far = 100.0f;
	};


	struct PerspectiveCameraDesc
	{
		Degs_f				m_fovY{ 90.F };	// The perspective vertical field of view (Y), in degrees.
		float				m_aspectRatio{ 1.f };	// The aspect ratio (width / height).
		float				m_near = { 0.1f };
		float				m_far = { 1000.f };
	};

	// A camera cannot be orthographic AND perspective at the same time, so let's share the data.
	union CameraData
	{
		CameraData(const CameraData& other) :
			m_ortho(other.m_ortho)
		{}

		CameraData(const OrthographicCameraDesc& orthoData) :
			m_ortho(orthoData)
		{}

		CameraData(const PerspectiveCameraDesc& persData) :
			m_perspective(persData)
		{}

		OrthographicCameraDesc	m_ortho;
		PerspectiveCameraDesc	m_perspective;
	};


	struct CameraGpuData
	{
		CameraGpuData(const Mat4& view = Mat4::Identity(), const Mat4 & proj = Mat4::Identity(), const Mat4& viewProj = Mat4::Identity()) :
			m_view(view), m_proj(proj), m_viewProj(viewProj)
		{}

		Mat4	m_view;
		Mat4	m_proj;
		Mat4	m_viewProj;
	};


	/**
	 * \brief A graphics APi-agnostic abstract class for a camera object.
	 */
	class Camera : public AGraphicObject
	{
	public:
		Camera() = default;

		Monocle_Graphics_API Camera(RenderWorld* world, const GraphicObjectData& data, ViewportHandle vpHandle, const OrthographicCameraDesc& orthoDesc, const Transform& transf = Transform::Identity());
		Monocle_Graphics_API Camera(RenderWorld* world, const GraphicObjectData& data, ViewportHandle vpHandle, const PerspectiveCameraDesc& perspecDesc, const Transform& transf = Transform::Identity());


		void	SetOrthographic(const OrthographicCameraDesc& orthoDesc);
		void	SetPerspective(const PerspectiveCameraDesc& perspecDesc);

		void	SetFoVY(Degs_f newFovY);
		void	SetAspectRatio(float newAspect);

		void	SetNear(float zNear);
		void	SetFar(float zFar);


		[[nodiscard]] const Mat4&	GetViewMatrix() const { return m_matrices.m_view; }

		[[nodiscard]] const Mat4&	GetProjectionMatrix() const { return m_matrices.m_proj; }

		[[nodiscard]] const Mat4&	GetViewProjectionMatrix() const { return m_matrices.m_viewProj; }

		[[nodiscard]] ViewportHandle	GetViewportHandle() const { return m_viewportHandle; }


		Monocle_Graphics_API void SetTransform(const Transform& transf) override final;
		Monocle_Graphics_API const Transform& AddTransform(const Transform& transf) override final;


		const CameraGpuData&	GetCameraGpuData() const
		{
			return m_matrices;
		}


		DeviceBufferHandle	GetCameraGpuHandle() const
		{
			return m_graphicData.m_uniformDataHandle;
		}


	protected:

		void	RecomputeViewMatrices();

		void	ComputeProjectionMatrix();

	private:
		CameraGpuData	m_matrices;

		CameraData	m_cameraData;

		ViewportHandle	m_viewportHandle{0};

		CameraProjection	m_projectionType{ CameraProjection::Perspective };

	};


}