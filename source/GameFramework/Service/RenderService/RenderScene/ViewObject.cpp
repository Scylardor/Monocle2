#include "ViewObject.h"

#include "Graphics/ShaderCapabilities/ShaderCapabilities.h"

#include "Core/Resource/Material/MaterialResource.h"


namespace moe
{
	ViewObject::ViewObject(RenderHardwareInterface* rhi, Mat4 const& view, PerspectiveProjectionData const& perspective,
	                       std::pair<int, int> const& surfaceDimensions, Rect2Df const& clipVp, Rect2Df const& clipSc):
		m_projData(perspective),
		m_clipViewport(clipVp),
		m_clipScissor(clipSc)
	{
		InitializeDeviceData(rhi, view);

		// TODO: shouldnt we already have the aspect ratio in PerspectiveProjectionData ??
		float fwidth = (float)surfaceDimensions.first;
		float fheight = (float)surfaceDimensions.second;
		float aspectRatio = fwidth / fheight;
		UpdateViewport(fwidth, fheight, aspectRatio);
	}


	ViewObject::ViewObject(RenderHardwareInterface* rhi, Mat4 const& view, OrthographicProjectionData const& ortho,
	                       std::pair<int, int> const& surfaceDimensions, Rect2Df const& clipVp, Rect2Df const& clipSc):
		m_projData(ortho),
		m_clipViewport(clipVp),
		m_clipScissor(clipSc)
	{
		InitializeDeviceData(rhi, view);

		float fwidth = (float)surfaceDimensions.first;
		float fheight = (float)surfaceDimensions.second;
		float aspectRatio = fwidth / fheight;
		UpdateViewport(fwidth, fheight, aspectRatio);
	}


	void ViewObject::UpdateViewport(float surfaceWidth, float surfaceHeight, float aspectRatio)
	{
		if (std::holds_alternative<PerspectiveProjectionData>(m_projData))
		{
			// recompute perspective
			auto& perspective = std::get<PerspectiveProjectionData>(m_projData);
			perspective.AspectRatio = aspectRatio;

			UpdateProjection(Mat4::Perspective(perspective.FoVY, perspective.AspectRatio, perspective.Near, perspective.Far));
		}
		else
		{
			// recompute orthographic
			auto& ortho = std::get<OrthographicProjectionData>(m_projData);

			UpdateProjection(Mat4::Orthographic(-aspectRatio, aspectRatio, -1, 1, ortho.Near, ortho.Far));
		}

		m_screenViewport.x = (unsigned)( m_clipViewport.x * surfaceWidth);
		m_screenViewport.y = (unsigned)( m_clipViewport.y * surfaceHeight);
		m_screenViewport.Width  = (unsigned) (m_clipViewport.Width * surfaceWidth);
		m_screenViewport.Height = (unsigned) (m_clipViewport.Height * surfaceHeight);

		m_screenScissor.x = (unsigned)(m_clipScissor.x * surfaceWidth);
		m_screenScissor.y = (unsigned)(m_clipScissor.y * surfaceHeight);
		m_screenScissor.Width = (unsigned)(m_clipScissor.Width * surfaceWidth);
		m_screenScissor.Height = (unsigned)(m_clipScissor.Height * surfaceHeight);
	}


	float ViewObject::OffsetPerspectiveFoVy(float FoVYDelta, float min, float max)
	{
		auto* perspective = std::get_if<PerspectiveProjectionData>(&m_projData);
		if (perspective != nullptr)
		{
			perspective->FoVY += FoVYDelta;
			perspective->FoVY = Degs_f(std::clamp(perspective->FoVY(), min, max));
			UpdateProjection(Mat4::Perspective(perspective->FoVY, perspective->AspectRatio, perspective->Near, perspective->Far));
			return perspective->FoVY;
		}

		return 0.f;
	}


	void ViewObject::InitializeDeviceData(RenderHardwareInterface* rhi, Mat4 const& view)
	{
		m_viewMatrices = rhi->BufferManager().MapCoherentDeviceBuffer(sizeof(ViewMatrices));
		m_viewMatrices.MutBlock<ViewMatrices>().View = view;

		ResourceSetsDescription cameraSetDesc;
		cameraSetDesc.EmplaceBinding<BufferBinding>(m_viewMatrices.Handle(), 0, (int) ReservedCapacitySets::VIEW_MATRICES, 0, sizeof(ViewMatrices));
		m_matricesResourceHandle = rhi->MaterialManager().AllocateResourceSet(cameraSetDesc);
	}
}
