#ifdef MOE_VULKAN

#include "CameraSystem.h"


// helper type for visting variants
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;



namespace moe
{
	CameraRef& CameraRef::Lookat(const Vec3& worldTarget)
	{
		m_system->Lookat(m_camID, worldTarget);
		return *this;
	}

	CameraRef& CameraRef::SetPosition(const Vec3& worldPos)
	{
		m_system->SetPosition(m_camID, worldPos);
		return *this;

	}

	Vec3 CameraRef::GetPosition() const
	{
		return m_system->GetPosition(m_camID);
	}

	CameraRef& CameraRef::SetUpVector(const Vec3& upVec)
	{
		m_system->SetUpVector(m_camID, upVec);
		return *this;
	}

	const Vec3& CameraRef::GetUpVector() const
	{
		return m_system->GetUpVector(m_camID);
	}

	CameraRef& CameraRef::SetRightVector(const Vec3& rightVec)
	{
		m_system->SetRightVector(m_camID, rightVec);
		return *this;
	}

	const Vec3& CameraRef::GetRightVector() const
	{
		return m_system->GetRightVector(m_camID);
	}

	CameraRef& CameraRef::SetNearPlane(float near)
	{
		m_system->SetNearPlane(m_camID, near);
		return *this;
	}

	float CameraRef::GetNearPlane() const
	{
		return m_system->GetNearPlane(m_camID);
	}

	CameraRef& CameraRef::SetFarPlane(float far)
	{
		m_system->SetFarPlane(m_camID, far);
		return *this;
	}

	float CameraRef::GetFarPlane() const
	{
		return m_system->GetFarPlane(m_camID);
	}

	CameraRef& CameraRef::SetLeftPlane(float left)
	{
		m_system->SetLeftPlane(m_camID, left);
		return *this;
	}

	float CameraRef::GetLeftPlane() const
	{
		return m_system->GetLeftPlane(m_camID);
	}

	CameraRef& CameraRef::SetRightPlane(float right)
	{
		m_system->SetRightPlane(m_camID, right);
		return *this;
	}

	float CameraRef::GetRightPlane() const
	{
		return m_system->GetRightPlane(m_camID);
	}

	CameraRef& CameraRef::SetFieldOfViewY(Degs_f fovy)
	{
		m_system->SetFieldOfViewY(m_camID, fovy);
		return *this;
	}

	Degs_f CameraRef::GetFieldOfViewY() const
	{
		return m_system->GetFieldOfViewY(m_camID);
	}

	CameraRef& CameraRef::SetAspectRatio(float ar)
	{
		m_system->SetAspectRatio(m_camID, ar);
		return *this;
	}

	float CameraRef::GetAspectRatio() const
	{
		return m_system->GetAspectRatio(m_camID);
	}


	CameraRef& CameraRef::SetViewport(const ViewportDesc& desc)
	{
		m_system->SetViewport(m_camID, desc);
		return *this;
	}

	CameraRef& CameraRef::SetScissor(const ScissorDesc& desc)
	{
		m_system->SetScissor(m_camID, desc);
		return *this;
	}


	const Mat4& CameraRef::GetViewProjection() const
	{
		return m_system->GetViewProjection(m_camID);
	}


	void CameraRef::MoveForward(float dt)
	{
		m_system->MoveForward(m_camID, dt);
	}

	void CameraRef::MoveBackward(float dt)
	{
		m_system->MoveBackward(m_camID, dt);
	}

	void CameraRef::StrafeLeft(float dt)
	{
		m_system->StrafeLeft(m_camID, dt);
	}

	void CameraRef::StrafeRight(float dt)
	{
		m_system->StrafeRight(m_camID, dt);
	}


	VulkanCameraSystem::VulkanCameraSystem()
	{
		m_cameras.Reserve(DEFAULT_NUMBER_OF_CAMERAS);
		m_camerasHostMemory.resize(DEFAULT_NUMBER_OF_CAMERAS);
		m_cameraViewportScissor.resize(DEFAULT_NUMBER_OF_CAMERAS);
	}


	VulkanCameraSystem::~VulkanCameraSystem()
	{
		if (m_device)
		{
			m_cameraBuffer.Unmap(*m_device);
			m_cameraBuffer.Free(*m_device);
		}
	}


	void VulkanCameraSystem::Initialize(MyVkDevice& device, const VulkanSwapchain& swapChain, uint32_t frameCount)
	{
		m_device = &device;
		m_swapChain = &swapChain;
		m_frameCount = frameCount;

		InitializeGPUResources(device);

		InitializeCameraUpdateInfos(device);

		m_cameraDescriptors.reserve(DEFAULT_NUMBER_OF_CAMERAS * m_frameCount);
	}


	void VulkanCameraSystem::Update(uint32_t frameIndex)
	{
		if (m_uploadCounter != 0)
		{
			auto thisFrameOffset = (m_cameras.GetCapacity() * frameIndex);
			GPUCamera* thisFrameCamerasBegin = m_camerasDeviceMemory + thisFrameOffset;
			std::memcpy(thisFrameCamerasBegin, m_camerasHostMemory.data(), sizeof(GPUCamera) * m_camerasHostMemory.size());
			m_uploadCounter--;
		}
	}


	void VulkanCameraSystem::Lookat(CameraID camID, const Vec3& worldTarget)
	{
		auto& cam = m_cameras.Mut(camID);
		GPUCamera& cameraMatrices = m_camerasHostMemory[camID];
		cameraMatrices.m_view = Mat4::LookAtMatrix(cam.WorldTransform.GetTranslation(), worldTarget, cam.UpVector);
		cameraMatrices.m_viewProj = cameraMatrices.m_proj * cameraMatrices.m_view;

		cam.WorldTransform = cameraMatrices.m_view.Invert();
		cam.LookatTarget = worldTarget;

		cam.FrontVector = (worldTarget - cam.WorldTransform.GetTranslation()).GetNormalized();

		ResetGPUUploadCounter();
	}

	void VulkanCameraSystem::SetPosition(CameraID camID, const Vec3& worldPos)
	{
		auto& cam = m_cameras.Mut(camID);
		cam.WorldTransform.SetTranslation(worldPos);

		GPUCamera& cameraMatrices = m_camerasHostMemory[camID];

		if (cam.LookatTarget)
		{
			cameraMatrices.m_view = Mat4::LookAtMatrix(worldPos, cam.LookatTarget.value(), cam.UpVector);
		}
		else
		{
			cameraMatrices.m_view = Mat4::LookAtMatrix(worldPos, worldPos + cam.FrontVector, cam.UpVector);
		}

		cameraMatrices.m_viewProj = cameraMatrices.m_proj * cameraMatrices.m_view;

		ResetGPUUploadCounter();
	}

	Vec3 VulkanCameraSystem::GetPosition(CameraID camID) const
	{
		const auto& cam = m_cameras.Get(camID);
		return cam.WorldTransform.GetTranslation();
	}

	void VulkanCameraSystem::SetUpVector(CameraID camID, const Vec3& upVec)
	{
		auto& cam = m_cameras.Mut(camID);
		cam.UpVector = upVec;
	}

	const Vec3& VulkanCameraSystem::GetUpVector(CameraID camID) const
	{
		const auto& cam = m_cameras.Get(camID);
		return cam.UpVector;
	}

	void VulkanCameraSystem::SetRightVector(CameraID camID, const Vec3& rightVec)
	{
		auto& cam = m_cameras.Mut(camID);
		cam.RightVector = rightVec;
	}

	const Vec3& VulkanCameraSystem::GetRightVector(CameraID camID) const
	{
		const auto& cam = m_cameras.Get(camID);
		return cam.RightVector;
	}

	void VulkanCameraSystem::SetNearPlane(CameraID camID, float near)
	{
		auto& cam = m_cameras.Mut(camID);

		GPUCamera& cameraMatrices = m_camerasHostMemory[camID];

		std::visit(overloaded{
			[near, &cameraMatrices](OrthographicCamera& ortho)
			{
				ortho.m_near = near;
				cameraMatrices.m_proj = Mat4::Orthographic(ortho.m_left, ortho.m_right, ortho.m_bottom, ortho.m_top, ortho.m_near, ortho.m_far);
			},
			[near, &cameraMatrices](PerspectiveCameraDesc& persp)
			{
				persp.m_near = near;
				RecomputePerspectiveMatrix(cameraMatrices, persp);
			}
		}, cam.ProjectionData.m_projection);

		cameraMatrices.m_viewProj = cameraMatrices.m_proj * cameraMatrices.m_view;

		ResetGPUUploadCounter();
	}

	float VulkanCameraSystem::GetNearPlane(CameraID camID) const
	{
		const auto& cam = m_cameras.Get(camID);
		float near = std::visit([](const auto& proj) { return proj.m_near; }, cam.ProjectionData.m_projection);
		return near;
	}

	void VulkanCameraSystem::SetFarPlane(CameraID camID, float far)
	{
		auto& cam = m_cameras.Mut(camID);

		GPUCamera& cameraMatrices = m_camerasHostMemory[camID];

		std::visit(overloaded{
			[far, &cameraMatrices](OrthographicCamera& ortho)
			{
				ortho.m_far = far;
				cameraMatrices.m_proj = Mat4::Orthographic(ortho.m_left, ortho.m_right, ortho.m_bottom, ortho.m_top, ortho.m_near, ortho.m_far);
			},
			[far, &cameraMatrices](PerspectiveCameraDesc& persp)
			{
				persp.m_far = far;
				RecomputePerspectiveMatrix(cameraMatrices, persp);
			}
			}, cam.ProjectionData.m_projection);

		cameraMatrices.m_viewProj = cameraMatrices.m_proj * cameraMatrices.m_view;

		ResetGPUUploadCounter();
	}

	float VulkanCameraSystem::GetFarPlane(CameraID camID) const
	{
		const auto& cam = m_cameras.Get(camID);
		float far = std::visit([](const auto& proj) { return proj.m_far; }, cam.ProjectionData.m_projection);
		return far;
	}

	void VulkanCameraSystem::SetLeftPlane(CameraID camID, float left)
	{
		auto& cam = m_cameras.Mut(camID);
		auto& ortho = std::get<OrthographicCamera>(cam.ProjectionData.m_projection);
		ortho.m_left = left;

		RecomputeOrthographicMatrix(camID, ortho);
		ResetGPUUploadCounter();
	}

	float VulkanCameraSystem::GetLeftPlane(CameraID camID) const
	{
		const auto& cam = m_cameras.Get(camID);
		return std::get<OrthographicCamera>(cam.ProjectionData.m_projection).m_left;
	}

	void VulkanCameraSystem::SetRightPlane(CameraID camID, float right)
	{
		auto& cam = m_cameras.Mut(camID);
		auto& ortho = std::get<OrthographicCamera>(cam.ProjectionData.m_projection);
		ortho.m_right = right;

		RecomputeOrthographicMatrix(camID, ortho);
		ResetGPUUploadCounter();
	}

	float VulkanCameraSystem::GetRightPlane(CameraID camID) const
	{
		const auto& cam = m_cameras.Get(camID);
		return std::get<OrthographicCamera>(cam.ProjectionData.m_projection).m_right;
	}

	void VulkanCameraSystem::SetTopPlane(CameraID camID, float top)
	{
		auto& cam = m_cameras.Mut(camID);
		auto& ortho = std::get<OrthographicCamera>(cam.ProjectionData.m_projection);
		ortho.m_top = top;

		RecomputeOrthographicMatrix(camID, ortho);
		ResetGPUUploadCounter();

	}

	float VulkanCameraSystem::GetTopPlane(CameraID camID) const
	{
		const auto& cam = m_cameras.Get(camID);
		return std::get<OrthographicCamera>(cam.ProjectionData.m_projection).m_top;
	}

	void VulkanCameraSystem::SetBottomPlane(CameraID camID, float bottom)
	{
		auto& cam = m_cameras.Mut(camID);
		auto& ortho = std::get<OrthographicCamera>(cam.ProjectionData.m_projection);
		ortho.m_bottom = bottom;

		RecomputeOrthographicMatrix(camID, ortho);
		ResetGPUUploadCounter();
	}

	float VulkanCameraSystem::GetBottomPlane(CameraID camID) const
	{
		const auto& cam = m_cameras.Get(camID);
		return std::get<OrthographicCamera>(cam.ProjectionData.m_projection).m_bottom;
	}

	void VulkanCameraSystem::SetFieldOfViewY(CameraID camID, Degs_f fovy)
	{
		auto& cam = m_cameras.Mut(camID);
		auto& perspective = std::get<PerspectiveCamera>(cam.ProjectionData.m_projection);
		perspective.m_fovY = fovy;

		GPUCamera& cameraMatrices = m_camerasHostMemory[camID];
		RecomputePerspectiveMatrix(cameraMatrices, perspective);
		cameraMatrices.m_viewProj = cameraMatrices.m_proj * cameraMatrices.m_view;

		ResetGPUUploadCounter();
	}

	Degs_f VulkanCameraSystem::GetFieldOfViewY(CameraID camID) const
	{
		const auto& cam = m_cameras.Get(camID);
		return std::get<PerspectiveCamera>(cam.ProjectionData.m_projection).m_fovY;
	}

	void VulkanCameraSystem::SetAspectRatio(CameraID camID, float ar)
	{
		auto& cam = m_cameras.Mut(camID);
		auto& perspective = std::get<PerspectiveCamera>(cam.ProjectionData.m_projection);
		perspective.m_aspectRatio = ar;

		GPUCamera& cameraMatrices = m_camerasHostMemory[camID];
		RecomputePerspectiveMatrix(cameraMatrices, perspective);
		cameraMatrices.m_viewProj = cameraMatrices.m_proj * cameraMatrices.m_view;

		ResetGPUUploadCounter();
	}

	float VulkanCameraSystem::GetAspectRatio(CameraID camID) const
	{
		const auto& cam = m_cameras.Get(camID);
		return std::get<PerspectiveCamera>(cam.ProjectionData.m_projection).m_aspectRatio;
	}


	void VulkanCameraSystem::SetViewport(CameraID camID, const ViewportDesc& desc)
	{
		const auto fullScreenSize = m_swapChain->GetSwapchainImageExtent();

		vk::Viewport& camViewport = m_cameraViewportScissor[camID].first;
		camViewport.x = (desc.x <= 1.f ? fullScreenSize.width * desc.x : desc.x);
		camViewport.y = (desc.x <= 1.f ? fullScreenSize.height * desc.y : desc.y);
		camViewport.width = (desc.Width <= 1.f ? fullScreenSize.width * desc.Width : desc.Width);
		camViewport.height = (desc.Height <= 1.f ? fullScreenSize.height * desc.Height : desc.Height);
		camViewport.minDepth = desc.MinDepth;
		camViewport.maxDepth = desc.MaxDepth;
	}


	void VulkanCameraSystem::SetScissor(CameraID camID, const ScissorDesc& desc)
	{
		const auto fullScreenSize = m_swapChain->GetSwapchainImageExtent();

		vk::Rect2D& camScissor = m_cameraViewportScissor[camID].second;
		camScissor.offset.x = (desc.x <= 1.f ? fullScreenSize.width * desc.x : desc.x);
		camScissor.offset.x = (desc.x <= 1.f ? fullScreenSize.height * desc.y : desc.y);
		camScissor.extent.width = (desc.Width <= 1.f ? fullScreenSize.width * desc.Width : desc.Width);
		camScissor.extent.height = (desc.Height <= 1.f ? fullScreenSize.height * desc.Height : desc.Height);
	}


	const Mat4& VulkanCameraSystem::GetViewProjection(CameraID camID) const
	{
		return m_camerasHostMemory[camID].m_viewProj;
	}


	void VulkanCameraSystem::MoveForward(CameraID camID, float dt)
	{
		const auto& cam = m_cameras.Get(camID);
		Vec3 newPos = cam.WorldTransform.GetTranslation() + (cam.FrontVector * cam.CameraSpeed * dt);
		SetPosition(camID, newPos);
	}

	void VulkanCameraSystem::MoveBackward(CameraID camID, float dt)
	{
		const auto& cam = m_cameras.Get(camID);
		Vec3 newPos = cam.WorldTransform.GetTranslation() + (-cam.FrontVector * cam.CameraSpeed * dt);
		SetPosition(camID, newPos);
	}

	void VulkanCameraSystem::StrafeLeft(CameraID camID, float dt)
	{
		const auto& cam = m_cameras.Get(camID);
		Vec3 newPos = cam.WorldTransform.GetTranslation() + (-cam.RightVector * cam.CameraSpeed * dt);
		SetPosition(camID, newPos);
	}

	void VulkanCameraSystem::StrafeRight(CameraID camID, float dt)
	{
		const auto& cam = m_cameras.Get(camID);
		Vec3 newPos = cam.WorldTransform.GetTranslation() + (cam.RightVector * cam.CameraSpeed * dt);
		SetPosition(camID, newPos);
	}


	void VulkanCameraSystem::InitializeGPUResources(MyVkDevice& device)
	{
		// Create the cameras buffer.
		m_cameraBuffer = VulkanBuffer::NewUniformBuffer(device, sizeof(GPUCamera) * m_cameras.GetCapacity() * m_frameCount,
			nullptr, VulkanBuffer::NoTransfer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

		// Leave the buffer mapped forever.
		m_camerasDeviceMemory = (GPUCamera*)m_cameraBuffer.Map(device);

		vk::DescriptorSetLayoutBinding uboBinding{
			0,
			vk::DescriptorType::eUniformBuffer,
			1,
			vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment
		};

		vk::DescriptorSetLayoutCreateInfo layoutCreateInfo{
			vk::DescriptorSetLayoutCreateFlags(),
			1,
			&uboBinding
		};

		m_setAllocator.InitializePool(device, layoutCreateInfo, DEFAULT_NUMBER_OF_CAMERAS * m_frameCount);
	}


	void VulkanCameraSystem::InitializeCameraUpdateInfos(MyVkDevice& device)
	{
		auto allocatedSets = m_setAllocator.Allocate(m_frameCount);

		std::vector<vk::WriteDescriptorSet> m_setUpdates;
		m_setUpdates.reserve(allocatedSets.size());

		std::vector <vk::DescriptorBufferInfo> m_buffInfos;
		m_buffInfos.reserve(allocatedSets.size());

		uint32_t camOffset = 0;
		for (auto vkSet : allocatedSets)
		{
			m_buffInfos.emplace_back(
				m_cameraBuffer.Handle(),
				camOffset,
				sizeof(GPUCamera)
			);

			camOffset += (uint32_t)(sizeof(GPUCamera) * m_camerasHostMemory.capacity());

			m_setUpdates.emplace_back(vkSet,
				0, 0, 1,
				vk::DescriptorType::eUniformBuffer,
				nullptr,
				&m_buffInfos.back());
		}

		device->updateDescriptorSets((uint32_t)m_setUpdates.size(), m_setUpdates.data(), 0, nullptr);
	}


	void VulkanCameraSystem::AllocateCameraDescriptors(CameraID camID)
	{
		const auto minWantedDescriptors = std::max(m_cameraDescriptors.size(), m_cameraDescriptors.size() + (camID + 1) * m_frameCount);
		m_cameraDescriptors.resize(minWantedDescriptors);
		auto cameraSets = m_setAllocator.Allocate(m_frameCount);

		std::memcpy(&m_cameraDescriptors[camID * m_frameCount], cameraSets.data(), sizeof(vk::DescriptorSet) * m_frameCount);

		const auto minWantedCameras = std::max(m_camerasHostMemory.size(), (size_t)camID + 1);
		m_camerasHostMemory.resize(minWantedCameras);
	}


	void VulkanCameraSystem::InitializeViewportScissor(CameraID camID, const ViewportDesc& vpDesc, const ScissorDesc& scDesc)
	{
		const auto minWantedCameras = std::max(m_cameraViewportScissor.size(), (size_t)camID + 1);

		m_cameraViewportScissor.resize(minWantedCameras);

		const auto fullScreenSize = m_swapChain->GetSwapchainImageExtent();

		vk::Viewport& camViewport = m_cameraViewportScissor[camID].first;
		camViewport.x = (vpDesc.x <= 1.f ? (float)fullScreenSize.width * vpDesc.x : vpDesc.x);
		camViewport.y = (vpDesc.x <= 1.f ? (float)fullScreenSize.height * vpDesc.y : vpDesc.y);
		camViewport.width = (vpDesc.Width <= 1.f ? (float)fullScreenSize.width * vpDesc.Width : vpDesc.Width);
		camViewport.height = (vpDesc.Height <= 1.f ? (float)fullScreenSize.height * vpDesc.Height : vpDesc.Height);
		camViewport.minDepth = vpDesc.MinDepth;
		camViewport.maxDepth = vpDesc.MaxDepth;

		vk::Rect2D& camScissor = m_cameraViewportScissor[camID].second;
		camScissor.offset.x = int32_t(scDesc.x <= 1.f ? (float)fullScreenSize.width * scDesc.x : scDesc.x);
		camScissor.offset.x = int32_t(scDesc.x <= 1.f ? (float)fullScreenSize.height * scDesc.y : scDesc.y);
		camScissor.extent.width = uint32_t(scDesc.Width <= 1.f ? (float)fullScreenSize.width * scDesc.Width : scDesc.Width);
		camScissor.extent.height = uint32_t(scDesc.Height <= 1.f ? (float)fullScreenSize.height * scDesc.Height : scDesc.Height);
	}


	void VulkanCameraSystem::RecomputeGPUCamera(CameraID camID)
	{
		auto& cam = m_cameras.Mut(camID);

		GPUCamera& cameraMatrices = m_camerasHostMemory[camID];

		cameraMatrices.m_view = cam.WorldTransform.GetInverse();

		std::visit(overloaded{
			[&cameraMatrices](OrthographicCamera& ortho)
			{
				cameraMatrices.m_proj = Mat4::Orthographic(ortho.m_left, ortho.m_right, ortho.m_bottom, ortho.m_top, ortho.m_near, ortho.m_far);
			},
			[ &cameraMatrices](PerspectiveCameraDesc& persp)
			{
				RecomputePerspectiveMatrix(cameraMatrices, persp);
			}
			}, cam.ProjectionData.m_projection);

		cameraMatrices.m_viewProj = cameraMatrices.m_proj * cameraMatrices.m_view;

		cameraMatrices.m_cameraPos = Vec4{ cam.WorldTransform.GetTranslation(), 1.f };

		ResetGPUUploadCounter();
	}

	void VulkanCameraSystem::RecomputeOrthographicMatrix(CameraID camID, const OrthographicCamera& orthoData)
	{
		GPUCamera& cameraMatrices = m_camerasHostMemory[camID];
		cameraMatrices.m_proj = Mat4::Orthographic(orthoData.m_left, orthoData.m_right, orthoData.m_bottom, orthoData.m_top, orthoData.m_near, orthoData.m_far);
		cameraMatrices.m_viewProj = cameraMatrices.m_proj * cameraMatrices.m_view;

	}


	void VulkanCameraSystem::RecomputePerspectiveMatrix(GPUCamera& cameraMatrices, const PerspectiveCameraDesc& perspectiveData)
	{
		switch (perspectiveData.m_projType)
		{
		case CameraProjection::Perspective_MinusOneToOne:
			cameraMatrices.m_proj = Mat4::Perspective(perspectiveData.m_fovY, perspectiveData.m_aspectRatio, perspectiveData.m_near, perspectiveData.m_far);
			break;
		case CameraProjection::Perspective_ZeroToOne:
			cameraMatrices.m_proj = Mat4::PerspectiveZeroOne(perspectiveData.m_fovY, perspectiveData.m_aspectRatio, perspectiveData.m_near, perspectiveData.m_far);
			break;
		case CameraProjection::Perspective_ZeroToOne_FlippedY:
			cameraMatrices.m_proj = Mat4::PerspectiveZeroOne(perspectiveData.m_fovY, perspectiveData.m_aspectRatio, perspectiveData.m_near, perspectiveData.m_far);
			cameraMatrices.m_proj[1][1] *= -1;
			break;
		default:
			MOE_ASSERT(false);
			break;
		}
	}

}


#endif // MOE_VULKAN
