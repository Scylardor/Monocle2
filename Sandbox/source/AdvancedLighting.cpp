#include "TestApplication.h"



#include <glfw3/include/GLFW/glfw3.h>
#include <Graphics/Pipeline/PipelineDescriptor.h>

#include "Math/Vec2.h"
#include <Math/Vec3.h>
#include <Math/Vec4.h>

#include "Graphics/Shader/ShaderStage/ShaderStage.h"

#include "Graphics/VertexLayout/VertexLayoutDescriptor.h"

#include "Graphics/Color/Color.h"

#include "Core/Misc/moeFile.h"


#include "Graphics/SceneGraph/SceneGraph.h"


#include "Graphics/Camera/Camera.h"
#include "Graphics/Camera/CameraSystem.h"
#include "Graphics/Light/LightSystem.h"


#include "Graphics/Material/Material.h"

#include "Graphics/Material/MaterialLibrary.h"

#include "Graphics/Framebuffer/FramebufferDescription.h"
#include "Graphics/Sampler/SamplerDescriptor.h"

#include "Graphics/Texture/TextureHandle.h"

#include "DepthRenderPass.h"


namespace moe
{
	void	TestApplication::TestBlinnPhong()
	{
		IGraphicsRenderer& renderer = MutRenderer();

		MaterialLibrary lib(MutRenderer().MutGraphicsDevice());
		lib.AddBindingMapping("Object_Matrices", { MaterialBlockBinding::OBJECT_MATRICES, ResourceKind::UniformBuffer });
		lib.AddBindingMapping("Frame_Time", { MaterialBlockBinding::FRAME_TIME, ResourceKind::UniformBuffer });
		lib.AddBindingMapping("Frame_Lights", { MaterialBlockBinding::FRAME_LIGHTS, ResourceKind::UniformBuffer });
		lib.AddBindingMapping("View_Camera", { MaterialBlockBinding::VIEW_CAMERA, ResourceKind::UniformBuffer });
		lib.AddBindingMapping("View_ProjectionPlanes", { MaterialBlockBinding::VIEW_PROJECTION_PLANES, ResourceKind::UniformBuffer });
		lib.AddBindingMapping("Material_Phong", { MaterialBlockBinding::MATERIAL_PHONG, ResourceKind::UniformBuffer });
		lib.AddBindingMapping("Material_Color", { MaterialBlockBinding::MATERIAL_COLOR, ResourceKind::UniformBuffer });
		lib.AddBindingMapping("SkyboxViewProjection", { MaterialBlockBinding::MATERIAL_SKYBOX_VIEWPROJ, ResourceKind::UniformBuffer });

		lib.AddBindingMapping("Material_DiffuseMap", { MaterialTextureBinding::DIFFUSE, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_SpecularMap", { MaterialTextureBinding::SPECULAR, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_EmissionMap", { MaterialTextureBinding::EMISSION, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_SkyboxMap", { MaterialTextureBinding::SKYBOX, ResourceKind::TextureReadOnly });

		lib.AddUniformBufferSizer(MaterialBlockBinding::FRAME_LIGHTS, []() { return sizeof(LightCastersData); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::VIEW_CAMERA, []() { return sizeof(CameraMatrices); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::MATERIAL_SKYBOX_VIEWPROJ, []() { return sizeof(Mat4); });

		lib.AddUniformBufferSizer(MaterialBlockBinding::MATERIAL_PHONG, []() { return sizeof(PhongMaterial); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::MATERIAL_COLOR, []() { return sizeof(ColorRGBAf); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::OBJECT_MATRICES, []() { return sizeof(ObjectMatrices); });

		SetInputKeyMapping(GLFW_KEY_W, GLFW_PRESS, [this]() { this->m_moveForward = true; });
		SetInputKeyMapping(GLFW_KEY_W, GLFW_RELEASE, [this]() { this->m_moveForward = false; });

		SetInputKeyMapping(GLFW_KEY_S, GLFW_PRESS, [this]() { this->m_moveBackward = true; });
		SetInputKeyMapping(GLFW_KEY_S, GLFW_RELEASE, [this]() { this->m_moveBackward = false; });

		SetInputKeyMapping(GLFW_KEY_A, GLFW_PRESS, [this]() { this->m_strafeLeft = true; });
		SetInputKeyMapping(GLFW_KEY_A, GLFW_RELEASE, [this]() { this->m_strafeLeft = false; });

		SetInputKeyMapping(GLFW_KEY_D, GLFW_PRESS, [this]() { this->m_strafeRight = true; });
		SetInputKeyMapping(GLFW_KEY_D, GLFW_RELEASE, [this]() { this->m_strafeRight = false; });

		auto[mouseX, mouseY] = GetMouseCursorPosition();
		m_lastX = mouseX;
		m_lastY = mouseY;

		SetInputMouseMoveMapping(std::bind(&TestApplication::OrientCameraWithMouse, this, std::placeholders::_1, std::placeholders::_2));

		SetInputMouseScrollMapping(std::bind(&TestApplication::CameraZoomMouseScroll, this, std::placeholders::_1, std::placeholders::_2));


		PipelineDescriptor pipeDesc;
		pipeDesc.m_depthStencilStateDesc = DepthStencilStateDescriptor{ DepthTest::Enabled, DepthWriting::Enabled, DepthStencilComparisonFunc::Less };
		pipeDesc.m_rasterizerStateDesc.m_cullMode = CullFace::None;
		PipelineHandle myPipe = m_renderer.MutGraphicsDevice().CreatePipeline(pipeDesc);

		/* Create cube VAO */
		VertexLayoutDescriptor cubeLayout{
			{
				{{"position", VertexElementFormat::Float3},
				{"normal", VertexElementFormat::Float3},
				{"texCoords", VertexElementFormat::Float2}},
			},
			LayoutType::Interleaved
		};

		auto cubeVao = renderer.CreateVertexLayout(cubeLayout);


		/* Create cube shader */
		IGraphicsRenderer::ShaderFileList blinnFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/blinn_phong.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/blinn_phong.frag" }
		};

		ShaderProgramHandle blinnProgram = renderer.CreateShaderProgramFromSourceFiles(blinnFileList);


		RenderWorld& renderWorld = MutRenderer().CreateRenderWorld();

		// Create plane geometry
		VertexPositionNormalTexture planeVertices[] = {
			// positions            // normals         // texcoords
			{{ 10.0f, -0.5f,  10.0f},  {0.0f, 1.0f, 0.0f},  {10.0f,  0.0f}},
			{{-10.0f, -0.5f,  10.0f},  {0.0f, 1.0f, 0.0f},  { 0.0f,  0.0f}},
			{{-10.0f, -0.5f, -10.0f},  {0.0f, 1.0f, 0.0f},  { 0.0f, 10.0f}},

			{{ 10.0f, -0.5f,  10.0f},  {0.0f, 1.0f, 0.0f},  {10.0f,  0.0f}},
			{{-10.0f, -0.5f, -10.0f},  {0.0f, 1.0f, 0.0f},  { 0.0f, 10.0f}},
			{{ 10.0f, -0.5f, -10.0f},  {0.0f, 1.0f, 0.0f},  {10.0f, 10.0f}}
		};

		Mesh* plane = renderWorld.CreateStaticMesh(planeVertices);

		/* Create Phong material buffer */
		MaterialDescriptor materialdesc(
			{
				{"Material_Phong", ShaderStage::Fragment},
				{"Material_DiffuseMap", ShaderStage::Fragment}
			}
		);
		MaterialInterface blinnPhongInterface = lib.CreateMaterialInterface(blinnProgram, materialdesc);
		MaterialInstance planeInst = lib.CreateMaterialInstance(blinnPhongInterface);
		planeInst.UpdateUniformBlock(MaterialBlockBinding::MATERIAL_PHONG,
			PhongMaterial{ Vec4(0.05f, 0.05f, 0.05f, 1.f),
							ColorRGBAf::White().ToVec(),
							Vec4(0.3f, 0.3f, 0.3f, 1.f),
							32 });

		Texture2DHandle woodImg = MutRenderer().MutGraphicsDevice().CreateTexture2D(Texture2DFileDescriptor{ "Sandbox/assets/textures/wood.png" });
		planeInst.BindTexture(MaterialTextureBinding::DIFFUSE, woodImg);
		planeInst.CreateMaterialResourceSet();
		/* End Phong material buffer */

		/* Create camera */
		PerspectiveCameraDesc persDesc{ 45_degf, GetWindowWidth() / (float)GetWindowHeight(), 0.1f, 100.f };

		CameraSystem camSys(renderer.MutGraphicsDevice());

		ViewportHandle vpHandle = m_renderer.MutGraphicsDevice().CreateViewport(ViewportDescriptor(0, 0, (float)GetWindowWidth(), (float)GetWindowHeight()));

		Camera* newCam = camSys.AddNewCamera(vpHandle, persDesc);

		newCam->AddTransform(Transform::Translate(Vec3(0, 0, 3)));

		m_currentCamera = newCam;

		// To make sure the camera points towards the negative z-axis by default we can give the yaw a default value of a 90 degree clockwise rotation.

		newCam->UpdateCameraVectors(0, -90);

		/* Create camera end */


		LightSystem lightsSystem(renderer.MutGraphicsDevice());

		LightObject* pointLight1 = lightsSystem.AddNewLight({ Vec4{0, 0, 0, 1}, Vec4::ZeroVector(),
			Vec4(0.05f, 0.05f, 0.05f, 1.f), Vec4(1.f), Vec4(0.3f, 0.3f, 0.3f, 1.f) });
		pointLight1->SetAttenuationFactors(1.f, 0.09f, 0.032f);



		while (WindowIsOpened())
		{
			float thisFrameTime = GetApplicationTimeSeconds();
			m_deltaTime = GetApplicationTimeSeconds() - m_lastFrame;
			m_lastFrame = thisFrameTime;

			PollInputEvents();

			if (m_moveForward)
			{
				CameraMoveForward();
			}
			else if (m_moveBackward)
			{
				CameraMoveBackwards();
			}

			if (m_strafeLeft)
			{
				CameraMoveStrafeLeft();
			}
			else if (m_strafeRight)
			{
				CameraMoveStrafeRight();
			}

			renderer.Clear(ColorRGBAf(0.1f, 0.1f, 0.1f, 1.0f));

			lightsSystem.UpdateLights();

			lightsSystem.BindLightBuffer();

			camSys.UpdateCameras();

			for (uint32_t iCam = 0; iCam < camSys.CamerasNumber(); iCam++)
			{
				m_renderer.MutGraphicsDevice().SetPipeline(myPipe);

				camSys.BindCameraBuffer(iCam);


				renderer.UseMaterialInstance(&planeInst);
				plane->SetTransform(Transform::Identity());
				plane->UpdateObjectMatrices(camSys.GetCamera(iCam));
				renderWorld.DrawMesh(plane, cubeVao, nullptr);

			}

			SwapBuffers();
		}
	}


	void	TestApplication::TestGammaCorrection()
	{
		IGraphicsRenderer& renderer = MutRenderer();

		MaterialLibrary lib(MutRenderer().MutGraphicsDevice());
		lib.AddBindingMapping("Object_Matrices", { MaterialBlockBinding::OBJECT_MATRICES, ResourceKind::UniformBuffer });
		lib.AddBindingMapping("Frame_Time", { MaterialBlockBinding::FRAME_TIME, ResourceKind::UniformBuffer });
		lib.AddBindingMapping("Frame_Lights", { MaterialBlockBinding::FRAME_LIGHTS, ResourceKind::UniformBuffer });
		lib.AddBindingMapping("View_Camera", { MaterialBlockBinding::VIEW_CAMERA, ResourceKind::UniformBuffer });
		lib.AddBindingMapping("View_ProjectionPlanes", { MaterialBlockBinding::VIEW_PROJECTION_PLANES, ResourceKind::UniformBuffer });
		lib.AddBindingMapping("Material_Phong", { MaterialBlockBinding::MATERIAL_PHONG, ResourceKind::UniformBuffer });
		lib.AddBindingMapping("Material_Color", { MaterialBlockBinding::MATERIAL_COLOR, ResourceKind::UniformBuffer });
		lib.AddBindingMapping("SkyboxViewProjection", { MaterialBlockBinding::MATERIAL_SKYBOX_VIEWPROJ, ResourceKind::UniformBuffer });

		lib.AddBindingMapping("Material_DiffuseMap", { MaterialTextureBinding::DIFFUSE, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_SpecularMap", { MaterialTextureBinding::SPECULAR, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_EmissionMap", { MaterialTextureBinding::EMISSION, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_SkyboxMap", { MaterialTextureBinding::SKYBOX, ResourceKind::TextureReadOnly });

		lib.AddBindingMapping("Material_Sampler", { MaterialSamplerBinding::SAMPLER_0, ResourceKind::Sampler });

		lib.AddUniformBufferSizer(MaterialBlockBinding::FRAME_LIGHTS, []() { return sizeof(LightCastersData); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::VIEW_CAMERA, []() { return sizeof(CameraMatrices); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::MATERIAL_SKYBOX_VIEWPROJ, []() { return sizeof(Mat4); });

		lib.AddUniformBufferSizer(MaterialBlockBinding::MATERIAL_PHONG, []() { return sizeof(PhongMaterial); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::MATERIAL_COLOR, []() { return sizeof(ColorRGBAf); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::OBJECT_MATRICES, []() { return sizeof(ObjectMatrices); });

		SetInputKeyMapping(GLFW_KEY_W, GLFW_PRESS, [this]() { this->m_moveForward = true; });
		SetInputKeyMapping(GLFW_KEY_W, GLFW_RELEASE, [this]() { this->m_moveForward = false; });

		SetInputKeyMapping(GLFW_KEY_S, GLFW_PRESS, [this]() { this->m_moveBackward = true; });
		SetInputKeyMapping(GLFW_KEY_S, GLFW_RELEASE, [this]() { this->m_moveBackward = false; });

		SetInputKeyMapping(GLFW_KEY_A, GLFW_PRESS, [this]() { this->m_strafeLeft = true; });
		SetInputKeyMapping(GLFW_KEY_A, GLFW_RELEASE, [this]() { this->m_strafeLeft = false; });

		SetInputKeyMapping(GLFW_KEY_D, GLFW_PRESS, [this]() { this->m_strafeRight = true; });
		SetInputKeyMapping(GLFW_KEY_D, GLFW_RELEASE, [this]() { this->m_strafeRight = false; });

		auto[mouseX, mouseY] = GetMouseCursorPosition();
		m_lastX = mouseX;
		m_lastY = mouseY;

		SetInputMouseMoveMapping(std::bind(&TestApplication::OrientCameraWithMouse, this, std::placeholders::_1, std::placeholders::_2));

		SetInputMouseScrollMapping(std::bind(&TestApplication::CameraZoomMouseScroll, this, std::placeholders::_1, std::placeholders::_2));


		PipelineDescriptor pipeDesc;
		pipeDesc.m_depthStencilStateDesc = DepthStencilStateDescriptor{ DepthTest::Enabled, DepthWriting::Enabled, DepthStencilComparisonFunc::Less };
		pipeDesc.m_rasterizerStateDesc.m_cullMode = CullFace::None;
		PipelineHandle myPipe = m_renderer.MutGraphicsDevice().CreatePipeline(pipeDesc);

		/* Create cube VAO */
		VertexLayoutDescriptor cubeLayout{
			{
				{{"position", VertexElementFormat::Float3},
				{"normal", VertexElementFormat::Float3},
				{"texCoords", VertexElementFormat::Float2}},
			},
			LayoutType::Interleaved
		};

		auto cubeVao = renderer.CreateVertexLayout(cubeLayout);


		/* Create cube shader */
		IGraphicsRenderer::ShaderFileList blinnFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/gamma_correction.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/gamma_correction.frag" }
		};

		ShaderProgramHandle blinnProgram = renderer.CreateShaderProgramFromSourceFiles(blinnFileList);


		RenderWorld& renderWorld = MutRenderer().CreateRenderWorld();

		// Create plane geometry
		VertexPositionNormalTexture planeVertices[] = {
			// positions            // normals         // texcoords
			{{ 10.0f, -0.5f,  10.0f},  {0.0f, 1.0f, 0.0f},  {10.0f,  0.0f}},
			{{-10.0f, -0.5f,  10.0f},  {0.0f, 1.0f, 0.0f},  { 0.0f,  0.0f}},
			{{-10.0f, -0.5f, -10.0f},  {0.0f, 1.0f, 0.0f},  { 0.0f, 10.0f}},

			{{ 10.0f, -0.5f,  10.0f},  {0.0f, 1.0f, 0.0f},  {10.0f,  0.0f}},
			{{-10.0f, -0.5f, -10.0f},  {0.0f, 1.0f, 0.0f},  { 0.0f, 10.0f}},
			{{ 10.0f, -0.5f, -10.0f},  {0.0f, 1.0f, 0.0f},  {10.0f, 10.0f}}
		};

		Mesh* plane = renderWorld.CreateStaticMesh(planeVertices);

		/* Create Phong material buffer */
		MaterialDescriptor materialdesc(
			{
				{"Material_Phong", ShaderStage::Fragment},
				{"Material_Sampler", ShaderStage::Fragment},
				{"Material_DiffuseMap", ShaderStage::Fragment}
			}
		);
		MaterialInterface blinnPhongInterface = lib.CreateMaterialInterface(blinnProgram, materialdesc);
		MaterialInstance planeInst = lib.CreateMaterialInstance(blinnPhongInterface);
		planeInst.UpdateUniformBlock(MaterialBlockBinding::MATERIAL_PHONG,
			PhongMaterial{ Vec4(0.05f, 0.05f, 0.05f, 1.f),
							ColorRGBAf::White().ToVec(),
							Vec4(0.3f, 0.3f, 0.3f, 1.f),
							64 });

		Texture2DFileDescriptor woodDesc{ "Sandbox/assets/textures/wood.png", TextureFormat::SRGB_RGBA8 };
		woodDesc.m_wantedMipmapLevels = 8;
		Texture2DHandle woodImg = MutRenderer().MutGraphicsDevice().CreateTexture2D(woodDesc);
		planeInst.BindTexture(MaterialTextureBinding::DIFFUSE, woodImg);

		SamplerDescriptor samplerDesc; // keep the default parameters
		samplerDesc.m_anisotropy = 4.f;
		SamplerHandle samplerHandle = MutRenderer().MutGraphicsDevice().CreateSampler(samplerDesc);
		planeInst.BindSampler(MaterialSamplerBinding::SAMPLER_0, samplerHandle);

		planeInst.CreateMaterialResourceSet();
		/* End Phong material buffer */

		/* Create camera */
		PerspectiveCameraDesc persDesc{ 45_degf, GetWindowWidth() / (float)GetWindowHeight(), 0.1f, 100.f };

		CameraSystem camSys(renderer.MutGraphicsDevice());

		ViewportHandle vpHandle = m_renderer.MutGraphicsDevice().CreateViewport(ViewportDescriptor(0, 0, (float)GetWindowWidth(), (float)GetWindowHeight()));

		Camera* newCam = camSys.AddNewCamera(vpHandle, persDesc);

		newCam->AddTransform(Transform::Translate(Vec3(0, 0, 3)));

		m_currentCamera = newCam;

		// To make sure the camera points towards the negative z-axis by default we can give the yaw a default value of a 90 degree clockwise rotation.

		newCam->UpdateCameraVectors(0, -90);

		/* Create camera end */


		LightSystem lightsSystem(renderer.MutGraphicsDevice());



		// lighting info
		// -------------

		LightObject* pointLight1 = lightsSystem.AddNewLight({ Vec4{-3.0f, 0.0f, 0.0f, 1}, Vec4::ZeroVector(),
			Vec4(0), Vec4(0.25f, 0.25f, 0.25f, 1.f), Vec4(0.25f, 0.25f, 0.25f, 1.f) });
		pointLight1->SetAttenuationFactors(0.f, 0.f, 1.f);
		/*
				LightObject* pointLight1 = lightsSystem.AddNewLight({ Vec4{-3.0f, 0.0f, 0.0f, 1}, Vec4::ZeroVector(),
					Vec4(0), Vec4(0.25f, 0.25f, 0.25f, 1.f), Vec4(0.25f, 0.25f, 0.25f, 1.f) });
				pointLight1->SetAttenuationFactors(0.f, 0.f, 1.f);
		*/
		LightObject* pointLight2 = lightsSystem.AddNewLight({ Vec4{-1.0f, 0.0f, 0.0f, 1}, Vec4::ZeroVector(),
	Vec4(0), Vec4(0.5f, 0.5f, 0.5f, 1.f), Vec4(0.5f, 0.5f, 0.5f, 1.f) });
		pointLight2->SetAttenuationFactors(0.f, 0.f, 1.f);

		LightObject* pointLight3 = lightsSystem.AddNewLight({ Vec4{1.0f, 0.0f, 0.0f, 1}, Vec4::ZeroVector(),
			Vec4(0), Vec4(0.75f, 0.75f, 0.75f, 1.f), Vec4(0.75f, 0.75f, 0.75f, 1.f) });
		pointLight3->SetAttenuationFactors(0.f, 0.f, 1.f);

		LightObject* pointLight4 = lightsSystem.AddNewLight({ Vec4{3.0f, 0.0f, 0.0f, 1}, Vec4::ZeroVector(),
			Vec4(0), Vec4(1.f), Vec4(1.f) });
		pointLight4->SetAttenuationFactors(0.f, 0.f, 1.f);

		while (WindowIsOpened())
		{
			float thisFrameTime = GetApplicationTimeSeconds();
			m_deltaTime = GetApplicationTimeSeconds() - m_lastFrame;
			m_lastFrame = thisFrameTime;

			PollInputEvents();

			if (m_moveForward)
			{
				CameraMoveForward();
			}
			else if (m_moveBackward)
			{
				CameraMoveBackwards();
			}

			if (m_strafeLeft)
			{
				CameraMoveStrafeLeft();
			}
			else if (m_strafeRight)
			{
				CameraMoveStrafeRight();
			}

			renderer.Clear(ColorRGBAf(0.1f, 0.1f, 0.1f, 1.0f));

			lightsSystem.UpdateLights();

			lightsSystem.BindLightBuffer();

			camSys.UpdateCameras();

			for (uint32_t iCam = 0; iCam < camSys.CamerasNumber(); iCam++)
			{
				m_renderer.MutGraphicsDevice().SetPipeline(myPipe);

				camSys.BindCameraBuffer(iCam);


				renderer.UseMaterialInstance(&planeInst);
				plane->SetTransform(Transform::Identity());
				plane->UpdateObjectMatrices(camSys.GetCamera(iCam));
				renderWorld.DrawMesh(plane, cubeVao, nullptr);

			}

			SwapBuffers();
		}
	}


	struct ShadowMappingInfo
	{
		Mat4	m_lightSpaceMatrix;
	};


	void TestApplication::TestShadowMapping()
	{
		IGraphicsRenderer& renderer = MutRenderer();

		MaterialLibrary lib(MutRenderer().MutGraphicsDevice());
		lib.AddBindingMapping("Object_Matrices", { MaterialBlockBinding::OBJECT_MATRICES, ResourceKind::UniformBuffer });
		lib.AddBindingMapping("Frame_Time", { MaterialBlockBinding::FRAME_TIME, ResourceKind::UniformBuffer });
		lib.AddBindingMapping("Frame_Lights", { MaterialBlockBinding::FRAME_LIGHTS, ResourceKind::UniformBuffer });
		lib.AddBindingMapping("View_Camera", { MaterialBlockBinding::VIEW_CAMERA, ResourceKind::UniformBuffer });
		lib.AddBindingMapping("View_ProjectionPlanes", { MaterialBlockBinding::VIEW_PROJECTION_PLANES, ResourceKind::UniformBuffer });
		lib.AddBindingMapping("Material_Phong", { MaterialBlockBinding::MATERIAL_PHONG, ResourceKind::UniformBuffer });
		lib.AddBindingMapping("Material_Color", { MaterialBlockBinding::MATERIAL_COLOR, ResourceKind::UniformBuffer });
		lib.AddBindingMapping("SkyboxViewProjection", { MaterialBlockBinding::MATERIAL_SKYBOX_VIEWPROJ, ResourceKind::UniformBuffer });
		lib.AddBindingMapping("Frame_ShadowMappingInfo", { MaterialBlockBinding::FRAME_SHADOW_MAPPING, ResourceKind::UniformBuffer });

		lib.AddBindingMapping("Material_DiffuseMap", { MaterialTextureBinding::DIFFUSE, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_SpecularMap", { MaterialTextureBinding::SPECULAR, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_EmissionMap", { MaterialTextureBinding::EMISSION, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_SkyboxMap", { MaterialTextureBinding::SKYBOX, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_ShadowMap", { MaterialTextureBinding::SHADOW, ResourceKind::TextureReadOnly });

		lib.AddBindingMapping("Material_Sampler", { MaterialSamplerBinding::SAMPLER_0, ResourceKind::Sampler });
		lib.AddBindingMapping("Material_Sampler2", { MaterialSamplerBinding::SAMPLER_1, ResourceKind::Sampler });

		lib.AddUniformBufferSizer(MaterialBlockBinding::FRAME_LIGHTS, []() { return sizeof(LightCastersData); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::VIEW_CAMERA, []() { return sizeof(CameraMatrices); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::MATERIAL_SKYBOX_VIEWPROJ, []() { return sizeof(Mat4); });

		lib.AddUniformBufferSizer(MaterialBlockBinding::MATERIAL_PHONG, []() { return sizeof(PhongMaterial); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::MATERIAL_COLOR, []() { return sizeof(ColorRGBAf); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::OBJECT_MATRICES, []() { return sizeof(ObjectMatrices); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::FRAME_SHADOW_MAPPING, []() { return sizeof(ShadowMappingInfo); });

		SetInputKeyMapping(GLFW_KEY_W, GLFW_PRESS, [this]() { this->m_moveForward = true; });
		SetInputKeyMapping(GLFW_KEY_W, GLFW_RELEASE, [this]() { this->m_moveForward = false; });

		SetInputKeyMapping(GLFW_KEY_S, GLFW_PRESS, [this]() { this->m_moveBackward = true; });
		SetInputKeyMapping(GLFW_KEY_S, GLFW_RELEASE, [this]() { this->m_moveBackward = false; });

		SetInputKeyMapping(GLFW_KEY_A, GLFW_PRESS, [this]() { this->m_strafeLeft = true; });
		SetInputKeyMapping(GLFW_KEY_A, GLFW_RELEASE, [this]() { this->m_strafeLeft = false; });

		SetInputKeyMapping(GLFW_KEY_D, GLFW_PRESS, [this]() { this->m_strafeRight = true; });
		SetInputKeyMapping(GLFW_KEY_D, GLFW_RELEASE, [this]() { this->m_strafeRight = false; });

		auto[mouseX, mouseY] = GetMouseCursorPosition();
		m_lastX = mouseX;
		m_lastY = mouseY;

		SetInputMouseMoveMapping(std::bind(&TestApplication::OrientCameraWithMouse, this, std::placeholders::_1, std::placeholders::_2));

		SetInputMouseScrollMapping(std::bind(&TestApplication::CameraZoomMouseScroll, this, std::placeholders::_1, std::placeholders::_2));


		PipelineDescriptor pipeDesc;
		pipeDesc.m_depthStencilStateDesc = DepthStencilStateDescriptor{ DepthTest::Enabled, DepthWriting::Enabled, DepthStencilComparisonFunc::Less };
		PipelineHandle myPipe = m_renderer.MutGraphicsDevice().CreatePipeline(pipeDesc);

		/* Create cube VAO */
		VertexLayoutDescriptor cubeLayout{
			{
				{{"position", VertexElementFormat::Float3},
				{"normal", VertexElementFormat::Float3},
				{"texCoords", VertexElementFormat::Float2}},
			},
			LayoutType::Interleaved
		};

		auto cubeVao = renderer.CreateVertexLayout(cubeLayout);


		/* Create cube shader */
		IGraphicsRenderer::ShaderFileList blinnFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/shadow_mapping.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/shadow_mapping.frag" }
		};

		ShaderProgramHandle blinnProgram = renderer.CreateShaderProgramFromSourceFiles(blinnFileList);


		RenderWorld& renderWorld = MutRenderer().CreateRenderWorld();

		// Create plane geometry
		VertexPositionNormalTexture planeVertices[] = {
			// positions            // normals         // texcoords
			{{-25.0f, -0.5f,  25.0f},  {0.0f, 1.0f, 0.0f},  { 0.0f,  0.0f}},
			{{ 25.0f, -0.5f,  25.0f},  {0.0f, 1.0f, 0.0f},  {25.0f,  0.0f}},
			{{-25.0f, -0.5f, -25.0f},  {0.0f, 1.0f, 0.0f},  { 0.0f, 25.0f}},

			{{-25.0f, -0.5f, -25.0f},  {0.0f, 1.0f, 0.0f},  { 0.0f, 25.0f}},
			{{ 25.0f, -0.5f,  25.0f},  {0.0f, 1.0f, 0.0f},  {25.0f,  0.0f}},
			{{ 25.0f, -0.5f, -25.0f},  {0.0f, 1.0f, 0.0f},  {25.0f, 25.0f}}
		};

		Mesh* plane = renderWorld.CreateStaticMesh(planeVertices);

		auto cubeGeom = CreateCubePositionNormalTexture(1.f);
		Mesh* cube = renderWorld.CreateStaticMesh(cubeGeom);

		/* Create camera */
		PerspectiveCameraDesc persDesc{ 45_degf, GetWindowWidth() / (float)GetWindowHeight(), 0.1f, 100.f };

		CameraSystem camSys(renderer.MutGraphicsDevice());

		ViewportHandle vpHandle = m_renderer.MutGraphicsDevice().CreateViewport(ViewportDescriptor(0, 0, (float)GetWindowWidth(), (float)GetWindowHeight()));

		Camera* newCam = camSys.AddNewCamera(vpHandle, persDesc);

		newCam->AddTransform(Transform::Translate(Vec3(0, 0, 3)));

		m_currentCamera = newCam;

		// To make sure the camera points towards the negative z-axis by default we can give the yaw a default value of a 90 degree clockwise rotation.

		newCam->UpdateCameraVectors(0, -90);

		/* Create camera end */


		LightSystem lightsSystem(renderer.MutGraphicsDevice());



		// lighting info
		// -------------


		LightObject* pointLight1 = lightsSystem.AddNewLight({ Vec4::ZeroVector(), Vec4{2.0f, -4.0f, 1.0f, 0.f},
			Vec4(0.3f, 0.3f, 0.3f, 1.f), Vec4(0.3f, 0.3f, 0.3f, 1.f), Vec4(0.3f, 0.3f, 0.3f, 1.f) });
		pointLight1->SetAttenuationFactors(0.f, 0.f, 1.f);

	//	LightObject* pointLight2 = lightsSystem.AddNewLight({ Vec4{-1.0f, 0.0f, 0.0f, 1}, Vec4::ZeroVector(),
	//Vec4(0.05f), Vec4(0.5f, 0.5f, 0.5f, 1.f), Vec4(0.5f, 0.5f, 0.5f, 1.f) });
	//	pointLight2->SetAttenuationFactors(0.f, 0.f, 1.f);

	//	LightObject* pointLight3 = lightsSystem.AddNewLight({ Vec4{1.0f, 0.0f, 0.0f, 1}, Vec4::ZeroVector(),
	//		Vec4(0.05f), Vec4(0.75f, 0.75f, 0.75f, 1.f), Vec4(0.75f, 0.75f, 0.75f, 1.f) });
	//	pointLight3->SetAttenuationFactors(0.f, 0.f, 1.f);

	//	LightObject* pointLight4 = lightsSystem.AddNewLight({ Vec4{3.0f, 0.0f, 0.0f, 1}, Vec4::ZeroVector(),
	//		Vec4(0.05f), Vec4(1.f), Vec4(1.f) });
	//	pointLight4->SetAttenuationFactors(0.f, 0.f, 1.f);

		// Depth map render pass initialization
		CameraDescriptor shadowCamDesc{CameraProjection::Orthographic, OrthographicCameraDesc{10.f, 1.f, 7.5f} };
		ShadowMapDescriptor shadowMapDesc{shadowCamDesc, 1024_width, 1024_height, Vec3(-2.0f, 4.0f, -1.0f), Vec3::ZeroVector()};
		DepthRenderPass depthRP(camSys, m_renderer.MutGraphicsDevice(), shadowMapDesc);

		// Create the depth map shader
		IGraphicsRenderer::ShaderFileList depthMapFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/depth_map.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/depth_map.frag" }
		};
		ShaderProgramHandle depthMapProgram = renderer.CreateShaderProgramFromSourceFiles(depthMapFileList);
		MaterialDescriptor depthMapDesc; // empty
		MaterialInterface depthMapInterface = lib.CreateMaterialInterface(depthMapProgram, depthMapDesc);
		MaterialInstance depthMapInstance = lib.CreateMaterialInstance(depthMapInterface);
		depthMapInstance.CreateMaterialResourceSet();

		const Camera& depthMapCam = camSys.GetCamera(depthRP.GetShadowmapCamera());

		plane->SetTransform(Transform::Identity());


		SamplerDescriptor depthMapsamplerDesc;
		depthMapsamplerDesc.m_magFilter = SamplerFilter::Nearest;
		depthMapsamplerDesc.m_minFilter = SamplerFilter::Nearest;
		SamplerHandle depthMapSamplerHandle = MutRenderer().MutGraphicsDevice().CreateSampler(depthMapsamplerDesc);


		/* Create Phong material buffer */
		MaterialDescriptor materialdesc(
			{
				{"Frame_ShadowMappingInfo", ShaderStage::Vertex},
				{"Material_Phong", ShaderStage::Fragment},
				{"Material_Sampler", ShaderStage::Fragment},
				{"Material_DiffuseMap", ShaderStage::Fragment},
				{"Material_Sampler2", ShaderStage::Fragment},
				{"Material_ShadowMap", ShaderStage::Fragment},
			}
		);
		MaterialInterface blinnPhongInterface = lib.CreateMaterialInterface(blinnProgram, materialdesc);
		MaterialInstance planeInst = lib.CreateMaterialInstance(blinnPhongInterface);

		planeInst.UpdateUniformBlock(MaterialBlockBinding::MATERIAL_PHONG,
			PhongMaterial{ Vec4(0.3f, 0.3f, 0.3f, 1.f),
							ColorRGBAf::White().ToVec(),
							ColorRGBAf::White().ToVec(),
							64 });

		// Plug the light space matrix (which is, the view projection matrix of the depth map matrix) into the shadow mapping uniform buffer
		planeInst.UpdateUniformBlock(FRAME_SHADOW_MAPPING, ShadowMappingInfo{ depthMapCam.GetViewProjectionMatrix() });

		Texture2DFileDescriptor woodDesc{ "Sandbox/assets/textures/wood.png", TextureFormat::SRGB_RGBA8 };
		woodDesc.m_wantedMipmapLevels = 8;
		Texture2DHandle woodImg = MutRenderer().MutGraphicsDevice().CreateTexture2D(woodDesc);
		planeInst.BindTexture(MaterialTextureBinding::DIFFUSE, woodImg);

		SamplerDescriptor samplerDesc;
		// for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat
		samplerDesc.m_wrap_S = SamplerWrapping::Repeat;
		samplerDesc.m_wrap_T = SamplerWrapping::Repeat;
		samplerDesc.m_anisotropy = 4.f;
		SamplerHandle samplerHandle = MutRenderer().MutGraphicsDevice().CreateSampler(samplerDesc);
		planeInst.BindSampler(MaterialSamplerBinding::SAMPLER_0, samplerHandle);

		planeInst.BindSampler(MaterialSamplerBinding::SAMPLER_1, depthMapSamplerHandle);

		planeInst.BindTexture(MaterialTextureBinding::SHADOW, depthRP.GetShadowMap());

		planeInst.CreateMaterialResourceSet();
		/* End Phong material buffer */

		// Create framebuffer "material" (just a shader to draw a fullscreen quad)
		IGraphicsRenderer::ShaderFileList framebufferShaders =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/fullscreen_quad.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/fullscreen_quad.frag" }
		};

		ShaderProgramHandle framebufferProgram = renderer.CreateShaderProgramFromSourceFiles(framebufferShaders);

		MaterialDescriptor framebufferMatDesc({
			{"Material_Sampler", ShaderStage::Fragment},
			{"Material_DiffuseMap", ShaderStage::Fragment}
		});

		MaterialInterface fbMatIntf = lib.CreateMaterialInterface(framebufferProgram, framebufferMatDesc);
		MaterialInstance fbMatInst = lib.CreateMaterialInstance(fbMatIntf);


		fbMatInst.BindSampler(MaterialSamplerBinding::SAMPLER_0, depthMapSamplerHandle);
		fbMatInst.BindTexture(MaterialTextureBinding::DIFFUSE, depthRP.GetShadowMap());
		fbMatInst.CreateMaterialResourceSet();

		/* Create fullscreen quad VAO */
		VertexLayoutDescriptor fsqLayout{
			{
				{"position", VertexElementFormat::Float3},
				{"texture", VertexElementFormat::Float2}
			},
			LayoutType::Interleaved
		};

		auto fsqVao = renderer.MutGraphicsDevice().CreateVertexLayout(fsqLayout);

		Array<VertexPositionTexture, 6> quadVertices{ // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
			// positions   // texCoords
			{{-1.0f,  1.0f, 0.f},  {0.0f, 1.0f}},
			{{-1.0f, -1.0f, 0.f},  {0.0f, 0.0f}},
			{{ 1.0f, -1.0f, 0.f},  {1.0f, 0.0f}},

			{{-1.0f,  1.0f, 0.f},  {0.0f, 1.0f}},
			{{ 1.0f, -1.0f, 0.f},  {1.0f, 0.0f}},
			{{ 1.0f,  1.0f, 0.f},  {1.0f, 1.0f}}
		};

		Mesh* fullscreenQuad = renderWorld.CreateStaticMesh(quadVertices);

		fullscreenQuad;
		PipelineDescriptor fullscreenQuadPipeDesc;
		fullscreenQuadPipeDesc.m_depthStencilStateDesc = DepthStencilStateDescriptor{ DepthTest::Disabled, DepthWriting::Enabled, DepthStencilComparisonFunc::Less };
		PipelineHandle fsqPipe = m_renderer.MutGraphicsDevice().CreatePipeline(fullscreenQuadPipeDesc);



		while (WindowIsOpened())
		{
			float thisFrameTime = GetApplicationTimeSeconds();
			m_deltaTime = GetApplicationTimeSeconds() - m_lastFrame;
			m_lastFrame = thisFrameTime;

			PollInputEvents();

			if (m_moveForward)
			{
				CameraMoveForward();
			}
			else if (m_moveBackward)
			{
				CameraMoveBackwards();
			}

			if (m_strafeLeft)
			{
				CameraMoveStrafeLeft();
			}
			else if (m_strafeRight)
			{
				CameraMoveStrafeRight();
			}

			m_renderer.MutGraphicsDevice().SetPipeline(myPipe);

			// First - render the shadow map in a texture
			renderer.BindFramebuffer(depthRP.GetFramebuffer());

			renderer.ClearDepth();

			camSys.UpdateCameras();

			camSys.BindCameraBuffer(depthRP.GetShadowmapCamera());

			renderer.UseMaterialInstance(&depthMapInstance);

			plane->UpdateObjectMatrices(depthMapCam);
			renderWorld.DrawMesh(plane, cubeVao, nullptr);

			cube->SetTransform(Transform::Translate(Vec3(0.F, 1.5f, 0.f)));
			cube->AddTransform(Transform::Scale(Vec3(0.5f)));
			cube->UpdateObjectMatrices(depthMapCam);
			renderWorld.DrawMesh(cube, cubeVao, nullptr);

			cube->SetTransform(Transform::Translate(Vec3(2.F, 0.f, 1.f)));
			cube->AddTransform(Transform::Scale(Vec3(0.5f)));
			cube->UpdateObjectMatrices(depthMapCam);
			renderWorld.DrawMesh(cube, cubeVao, nullptr);

			cube->SetTransform(Transform::Translate(Vec3(-1.F, 0.f, 2.f)));
			cube->AddTransform(Transform::Rotate(60_degf, Vec3(1.f, 0.f, 1.f).GetNormalized()));
			cube->AddTransform(Transform::Scale(Vec3(0.25f)));
			cube->UpdateObjectMatrices(depthMapCam);
			renderWorld.DrawMesh(cube, cubeVao, nullptr);

			renderer.UnbindFramebuffer(depthRP.GetFramebuffer());

			renderer.Clear(ColorRGBAf(0.1f, 0.1f, 0.1f, 1.0f));


			// Finally - draw the framebuffer using a fullscreen quad

			//renderer.MutGraphicsDevice().SetPipeline(fsqPipe);

			//camSys.BindCameraBuffer(m_currentCamera->GetCameraIndex());

			//renderer.UseMaterialInstance(&fbMatInst);
			//renderWorld.DrawMesh(fullscreenQuad, fsqVao, nullptr);



			lightsSystem.UpdateLights();

			lightsSystem.BindLightBuffer();

			camSys.UpdateCameras();

			camSys.BindCameraBuffer(m_currentCamera->GetCameraIndex());

			renderer.UseMaterialInstance(&planeInst);

			plane->UpdateObjectMatrices(*m_currentCamera);
			renderWorld.DrawMesh(plane, cubeVao, nullptr);

			cube->SetTransform(Transform::Translate(Vec3(0.F, 1.5f, 0.f)));
			cube->AddTransform(Transform::Scale(Vec3(0.5f)));
			cube->UpdateObjectMatrices(*m_currentCamera);
			renderWorld.DrawMesh(cube, cubeVao, nullptr);

			cube->SetTransform(Transform::Translate(Vec3(2.F, 0.f, 1.f)));
			cube->AddTransform(Transform::Scale(Vec3(0.5f)));
			cube->UpdateObjectMatrices(*m_currentCamera);
			renderWorld.DrawMesh(cube, cubeVao, nullptr);

			cube->SetTransform(Transform::Translate(Vec3(-1.F, 0.f, 2.f)));
			cube->AddTransform(Transform::Rotate(60_degf, Vec3(1.f, 0.f, 1.f).GetNormalized()));
			cube->AddTransform(Transform::Scale(Vec3(0.25f)));
			cube->UpdateObjectMatrices(*m_currentCamera);
			renderWorld.DrawMesh(cube, cubeVao, nullptr);

			SwapBuffers();
		}
	}

}
