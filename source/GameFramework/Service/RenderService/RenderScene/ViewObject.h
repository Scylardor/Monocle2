#pragma once

#include <variant>


#include "Graphics/RHI/RenderHardwareInterface.h"
#include "Math/Matrix.h"
#include "Math/Rect2D.h"

namespace moe
{

	struct ViewProjectionData
	{
		ViewProjectionData() = default;

		ViewProjectionData(float n, float f) :
			Near(n),
			Far(f)
		{}

		inline static const float DEFAULT_NEAR = 0.1f;
		inline static const float DEFAULT_FAR = 1000.f;

		float Near = DEFAULT_NEAR;
		float Far = DEFAULT_FAR;

	};


	struct OrthographicProjectionData : ViewProjectionData
	{
		OrthographicProjectionData() = default;

		OrthographicProjectionData(float cubeExtent, float near = DEFAULT_NEAR, float far = DEFAULT_FAR) :
			ViewProjectionData(near, far),
			Left(-cubeExtent), Right(cubeExtent), Bottom(-cubeExtent), Top(cubeExtent)
		{}

		float	Left = 0.0f;
		float	Right = 800.0f;
		float	Bottom = 0.0f;
		float	Top = 600.0f;
	};


	struct PerspectiveProjectionData : ViewProjectionData
	{
		PerspectiveProjectionData() = default;

		PerspectiveProjectionData(Degs_f fovy, float ar, float near = DEFAULT_NEAR, float far = DEFAULT_FAR) :
			ViewProjectionData(near, far),
			FoVY(fovy),
			AspectRatio(ar)
		{}

		Degs_f	FoVY{ 90.F };	// The perspective vertical field of view (Y), in degrees.
		float	AspectRatio{ 1.f };	// The aspect ratio (width / height).
	};


	struct ViewMatrices
	{
		Mat4	View;			// The "world to view" matrix
		Mat4	Projection;		// The projection matrix (usually orthographic or perspective)
		Mat4	ViewProjection;	// projection * view (has to be kept uptodate at all times)
	};


	/* A description of a projection object (usually a camera).
	 * Contains a projection matrix, viewport dimensions, and a scissor region.
	 * Used by a render pass to define where it is currently drawing. */
	class ViewObject
	{
	public:

		inline static Rect2Df FULLSCREEN_CLIP = Rect2Df{ 0, 0, 1, 1 };

		ViewObject(RenderHardwareInterface* rhi, Mat4 const& view, PerspectiveProjectionData const& perspective,
			std::pair<int, int> const& surfaceDimensions, Rect2Df const& clipVp = FULLSCREEN_CLIP,
			Rect2Df const& clipSc = FULLSCREEN_CLIP);

		ViewObject(RenderHardwareInterface* rhi, Mat4 const& view, OrthographicProjectionData const& ortho,
			std::pair<int, int> const& surfaceDimensions, Rect2Df const& clipVp = FULLSCREEN_CLIP,
			Rect2Df const& clipSc = FULLSCREEN_CLIP);

		void	UpdateViewport(float surfaceWidth, float surfaceHeight, float aspectRatio);



		void	UpdateView(Mat4 const& newViewMat)
		{
			ViewMatrices& matrices = m_viewMatrices.MutBlock<ViewMatrices>();
			matrices.View = newViewMat;
			matrices.ViewProjection = matrices.Projection * matrices.View;
		}


		void	UpdateProjection(Mat4 const& newProj)
		{
			ViewMatrices& matrices = m_viewMatrices.MutBlock<ViewMatrices>();
			matrices.Projection = newProj;
			matrices.ViewProjection = matrices.Projection * matrices.View;
		}


		DeviceResourceSetHandle	GetMatricesResourceSetHandle() const
		{
			return m_matricesResourceHandle;
		}


		Mat4 const& GetViewProjectionMatrix() const
		{
			return m_viewMatrices.GetBlock<ViewMatrices>().ViewProjection;
		}


		Rect2Du const& ScreenViewport() const
		{
			return m_screenViewport;
		}

		Rect2Du const&	ScreenScissor() const
		{
			return m_screenScissor;
		}


		float	OffsetPerspectiveFoVy(float FoVYDelta, float min, float max);


	private:

		void	InitializeDeviceData(RenderHardwareInterface* rhi, Mat4 const& view);

		using ProjectionDataVariant = std::variant<OrthographicProjectionData, PerspectiveProjectionData>;

		ProjectionDataVariant	m_projData{};

		DeviceBufferMapping			m_viewMatrices;
		DeviceResourceSetHandle		m_matricesResourceHandle;

		// Coefficients, in clip coordinates, of the position and extent of the viewport and scissors.
		Rect2Df	m_clipViewport;
		Rect2Df	m_clipScissor;

		// Actual coordinates and extent of the viewport and scissor in window coordinates.
		Rect2Du	m_screenViewport;
		Rect2Du	m_screenScissor;
	};

}
