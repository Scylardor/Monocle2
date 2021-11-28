#include "TestApplication.h"



#include <glfw3/include/GLFW/glfw3.h>
#include <Graphics/Pipeline/PipelineDescriptor.h>

#include "Math/Vec2.h"
#include <Math/Vec3.h>
#include <Math/Vec4.h>
#include <random> // for uniform distribution and random engine


#include "Core/Resource/Material/Shader/ShaderStage/ShaderStage.h"

#include "Graphics/VertexLayout/VertexLayoutDescriptor.h"

#include "Graphics/Color/Color.h"

#include "Core/Misc/moeFile.h"


#include "Graphics/SceneGraph/SceneGraph.h"


#include "Graphics/Camera/Camera.h"
#include "Graphics/Camera/CameraSystem.h"
#include "Graphics/Light/LightSystem.h"


#include "Graphics/Material/Material.h"

#include "Graphics/Material/MaterialLibrary.h"

#include "Graphics/Sampler/SamplerDescriptor.h"

#include "Graphics/Model/Model.h"

#include "DirectionalShadowDepthPass.h"

#include "OmnidirectionalShadowDepthPass.h"

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

		Texture2DFileDescriptor woodDesc{ "Sandbox/assets/textures/wood.png" };
		woodDesc.m_wantedMipmapLevels = 8;
		Texture2DHandle woodImg = MutRenderer().MutGraphicsDevice().CreateTexture2D(woodDesc);

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
		float	m_minShadowBias;
		float	m_maxShadowBias;
		float	m_pcfGridSize;
		Vec2	m_shadowMapTextureSize;
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


		LightObject* pointLight1 = lightsSystem.AddNewLight({ Vec4{-2.0f, 4.0f, -1.0f, 1}, Vec4::ZeroVector(),
			Vec4(0.3f), Vec4(1.f), Vec4(1.f) });
		pointLight1->SetAttenuationFactors(0.f, 0.f, 0.1f);
/*
		LightObject* pointLight1 = lightsSystem.AddNewLight({ Vec4::ZeroVector(), Vec4{2.0f, -4.0f, 1.0f, 0.f},
			Vec4(0.3f, 0.3f, 0.3f, 1.f), Vec4(0.3f, 0.3f, 0.3f, 1.f), Vec4(0.3f, 0.3f, 0.3f, 1.f) });
		pointLight1->SetAttenuationFactors(0.f, 0.f, 1.f);*/

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
		//CameraDescriptor shadowCamDesc{ CameraProjection::Perspective, PerspectiveCameraDesc{45_radf, 1024 / 1024, 1.f, 7.5f} };

		ShadowMapDescriptor shadowMapDesc{shadowCamDesc, 1024_width, 1024_height, Vec3{-2.0f, 4.0f, -1.0f}, Vec3::ZeroVector()};
		DirectionalShadowDepthPass depthRP(camSys, m_renderer.MutGraphicsDevice(), shadowMapDesc);

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
		depthMapsamplerDesc.m_wrap_S = SamplerWrapping::ClampToBorder;
		depthMapsamplerDesc.m_wrap_T = SamplerWrapping::ClampToBorder;
		depthMapsamplerDesc.m_borderColor = ColorRGBAf::White();

		SamplerHandle depthMapSamplerHandle = MutRenderer().MutGraphicsDevice().CreateSampler(depthMapsamplerDesc);


		/* Create Phong material buffer */
		MaterialDescriptor materialdesc(
			{
				{"Frame_ShadowMappingInfo", ShaderStage(Vertex | Fragment) },
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
			PhongMaterial{ Vec4(0.05f, 0.05f, 0.05f, 1.f),
							ColorRGBAf::White().ToVec(),
							Vec4(0.3f, 0.3f, 0.3f, 1.f),
							64 });

		// Plug the light space matrix (which is, the view projection matrix of the depth map matrix) into the shadow mapping uniform buffer
		planeInst.UpdateUniformBlock(FRAME_SHADOW_MAPPING, ShadowMappingInfo{
			depthMapCam.GetViewProjectionMatrix(), 0.005f, 0.05f,

			4.f, Vec2{1024, 1024}
		});

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


	void TestApplication::TestOmnidirectionalShadowMapping()
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
		lib.AddBindingMapping("Frame_OmniShadowMappingInfo", { MaterialBlockBinding::FRAME_OMNI_SHADOW_MAPPING, ResourceKind::UniformBuffer });

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
		lib.AddUniformBufferSizer(MaterialBlockBinding::FRAME_OMNI_SHADOW_MAPPING, []() { return sizeof(OmniShadowMappingInfo); });

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

		PipelineDescriptor bigCubePipeDesc;
		bigCubePipeDesc.m_depthStencilStateDesc = DepthStencilStateDescriptor{ DepthTest::Enabled, DepthWriting::Enabled, DepthStencilComparisonFunc::Less };
		bigCubePipeDesc.m_rasterizerStateDesc.m_cullMode = CullFace::None;
		PipelineHandle bigCubePipe = m_renderer.MutGraphicsDevice().CreatePipeline(bigCubePipeDesc);

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
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/omnidirectional_shadow_mapping.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/omnidirectional_shadow_mapping.frag" }
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
		cube;

		auto bigCubeGeom = CreateCubePositionNormalTexture_ReversedNormals(1.f);
		Mesh* bigCube = renderWorld.CreateStaticMesh(bigCubeGeom);

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


		LightObject* pointLight1 = lightsSystem.AddNewLight({ Vec4{0.f, 0.f, 0.f, 1}, Vec4::ZeroVector(),
			Vec4(0.3f), Vec4(1.f), Vec4(1.f) });
		pointLight1->SetAttenuationFactors(0.f, 0.f, 0.1f);
		/*
				LightObject* pointLight1 = lightsSystem.AddNewLight({ Vec4::ZeroVector(), Vec4{2.0f, -4.0f, 1.0f, 0.f},
					Vec4(0.3f, 0.3f, 0.3f, 1.f), Vec4(0.3f, 0.3f, 0.3f, 1.f), Vec4(0.3f, 0.3f, 0.3f, 1.f) });
				pointLight1->SetAttenuationFactors(0.f, 0.f, 1.f);*/

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
		//CameraDescriptor shadowCamDesc{ CameraProjection::Orthographic, OrthographicCameraDesc{10.f, 1.f, 7.5f} };
		CameraDescriptor shadowCamDesc{ CameraProjection::Perspective, PerspectiveCameraDesc{90_degf, 1024 / 1024, 1.f, 25.f} };

		OmnidirectionalShadowMapDescriptor shadowMapDesc{ shadowCamDesc, 1024_width, 1024_height, Vec3::ZeroVector(), Vec3::ZeroVector(), TextureFormat::Depth32F };
		OmnidirectionalShadowDepthPass depthRP(camSys, m_renderer.MutGraphicsDevice(), shadowMapDesc);

		// Create the depth map shader
		IGraphicsRenderer::ShaderFileList depthMapFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/depth_map_omnidirectional.vert" },
			{ ShaderStage::Geometry,	"source/Graphics/Resources/shaders/OpenGL/depth_map_omnidirectional.geom" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/depth_map_omnidirectional.frag" }
		};
		ShaderProgramHandle depthMapProgram = renderer.CreateShaderProgramFromSourceFiles(depthMapFileList);

		MaterialDescriptor depthMapDesc{
			{"Frame_OmniShadowMappingInfo", ShaderStage(Geometry | Fragment)}
		};
		MaterialInterface depthMapInterface = lib.CreateMaterialInterface(depthMapProgram, depthMapDesc);

		MaterialInstance depthMapInstance = lib.CreateMaterialInstance(depthMapInterface);

		depthMapInstance.UpdateUniformBlock(MaterialBlockBinding::FRAME_OMNI_SHADOW_MAPPING, depthRP.GetShadowMappingInfo());

		depthMapInstance.CreateMaterialResourceSet();

		const Camera& depthMapCam = camSys.GetCamera(depthRP.GetShadowmapCamera());

		plane->SetTransform(Transform::Identity());

		SamplerDescriptor depthMapsamplerDesc;
		depthMapsamplerDesc.m_magFilter = SamplerFilter::Nearest;
		depthMapsamplerDesc.m_minFilter = SamplerFilter::Nearest;
		depthMapsamplerDesc.m_wrap_S = SamplerWrapping::ClampToEdge;
		depthMapsamplerDesc.m_wrap_T = SamplerWrapping::ClampToEdge;
		depthMapsamplerDesc.m_wrap_R = SamplerWrapping::ClampToEdge;

		SamplerHandle depthMapSamplerHandle = MutRenderer().MutGraphicsDevice().CreateSampler(depthMapsamplerDesc);


		/* Create Phong material buffer */
		MaterialDescriptor materialdesc(
			{
				{"Frame_ShadowMappingInfo", ShaderStage(Vertex | Fragment) },
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
			PhongMaterial{ Vec4(0.05f, 0.05f, 0.05f, 1.f),
							ColorRGBAf::White().ToVec(),
							Vec4(0.3f, 0.3f, 0.3f, 1.f),
							64 });

		// Plug the light space matrix (which is, the view projection matrix of the depth map matrix) into the shadow mapping uniform buffer
		planeInst.UpdateUniformBlock(FRAME_SHADOW_MAPPING, ShadowMappingInfo{
			depthMapCam.GetViewProjectionMatrix(), 0.005f, 0.05f,

			4.f, Vec2{1024, 1024}
			});

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

			m_renderer.MutGraphicsDevice().SetPipeline(bigCubePipe);

			bigCube->SetTransform(Transform::Identity());
			bigCube->AddTransform(Transform::Scale(Vec3(5.f)));
			bigCube->UpdateObjectMatrices(depthMapCam);
			renderWorld.DrawMesh(bigCube, cubeVao, nullptr);

			m_renderer.MutGraphicsDevice().SetPipeline(myPipe);

			cube->SetTransform(Transform::Identity());
			cube->AddTransform(Transform::Translate(Vec3(4.0f, -3.5f, 0.0f)));
			cube->AddTransform(Transform::Scale(Vec3(0.5f)));
			cube->UpdateObjectMatrices(depthMapCam);
			renderWorld.DrawMesh(cube, cubeVao, nullptr);

			cube->SetTransform(Transform::Identity());
			cube->AddTransform(Transform::Translate(Vec3(2.0f, 3.0f, 1.0f)));
			cube->AddTransform(Transform::Scale(Vec3(0.75f)));
			cube->UpdateObjectMatrices(depthMapCam);
			renderWorld.DrawMesh(cube, cubeVao, nullptr);

			cube->SetTransform(Transform::Identity());
			cube->AddTransform(Transform::Translate(Vec3(-3.0f, -1.0f, 0.0f)));
			cube->AddTransform(Transform::Scale(Vec3(0.5f)));
			cube->UpdateObjectMatrices(depthMapCam);
			renderWorld.DrawMesh(cube, cubeVao, nullptr);

			cube->SetTransform(Transform::Identity());
			cube->AddTransform(Transform::Translate(Vec3(-1.5f, 1.0f, 1.5f)));
			cube->AddTransform(Transform::Scale(Vec3(0.5f)));
			cube->UpdateObjectMatrices(depthMapCam);
			renderWorld.DrawMesh(cube, cubeVao, nullptr);

			cube->SetTransform(Transform::Identity());
			cube->AddTransform(Transform::Translate(Vec3(-1.5f, 2.0f, -3.0f)));
			cube->AddTransform(Transform::Rotate(60_degf, Vec3(1.0f, 0.0f, 1.0f).GetNormalized()));
			cube->AddTransform(Transform::Scale(Vec3(0.75f)));
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


			m_renderer.MutGraphicsDevice().SetPipeline(bigCubePipe);

			bigCube->SetTransform(Transform::Identity());
			bigCube->AddTransform(Transform::Scale(Vec3(5.f)));
			bigCube->UpdateObjectMatrices(*m_currentCamera);
			renderWorld.DrawMesh(bigCube, cubeVao, nullptr);

			m_renderer.MutGraphicsDevice().SetPipeline(myPipe);

			cube->SetTransform(Transform::Identity());
			cube->AddTransform(Transform::Translate(Vec3(4.0f, -3.5f, 0.0f)));
			cube->AddTransform(Transform::Scale(Vec3(0.5f)));
			cube->UpdateObjectMatrices(*m_currentCamera);
			renderWorld.DrawMesh(cube, cubeVao, nullptr);

			cube->SetTransform(Transform::Identity());
			cube->AddTransform(Transform::Translate(Vec3(2.0f, 3.0f, 1.0f)));
			cube->AddTransform(Transform::Scale(Vec3(0.75f)));
			cube->UpdateObjectMatrices(*m_currentCamera);
			renderWorld.DrawMesh(cube, cubeVao, nullptr);

			cube->SetTransform(Transform::Identity());
			cube->AddTransform(Transform::Translate(Vec3(-3.0f, -1.0f, 0.0f)));
			cube->AddTransform(Transform::Scale(Vec3(0.5f)));
			cube->UpdateObjectMatrices(*m_currentCamera);
			renderWorld.DrawMesh(cube, cubeVao, nullptr);

			cube->SetTransform(Transform::Identity());
			cube->AddTransform(Transform::Translate(Vec3(-1.5f, 1.0f, 1.5f)));
			cube->AddTransform(Transform::Scale(Vec3(0.5f)));
			cube->UpdateObjectMatrices(*m_currentCamera);
			renderWorld.DrawMesh(cube, cubeVao, nullptr);

			cube->SetTransform(Transform::Identity());
			cube->AddTransform(Transform::Translate(Vec3(-1.5f, 2.0f, -3.0f)));
			cube->AddTransform(Transform::Rotate(60_degf, Vec3(1.0f, 0.0f, 1.0f).GetNormalized()));
			cube->AddTransform(Transform::Scale(Vec3(0.75f)));
			cube->UpdateObjectMatrices(*m_currentCamera);
			renderWorld.DrawMesh(cube, cubeVao, nullptr);

			SwapBuffers();
		}
	}



	std::pair<Vec3, Vec3>	ComputeTriangleTangentBitangent(const Vec3& v1Pos, const Vec2& v1Tex, const Vec3& v2Pos, const Vec2& v2Tex, const Vec3& v3Pos, const Vec2& v3Tex)
	{
		const Vec3 edge1 = v2Pos - v1Pos;
		const Vec3 edge2 = v3Pos - v1Pos;
		const Vec2 deltaUV1 = v2Tex - v1Tex;
		const Vec2 deltaUV2 = v3Tex - v1Tex;

		const float f = 1.0f / (deltaUV1.x() * deltaUV2.y() - deltaUV2.x() * deltaUV1.y());

		Vec3 tangent, bitangent;

		tangent.x() = f * (deltaUV2.y() * edge1.x() - deltaUV1.y() * edge2.x());
		tangent.y() = f * (deltaUV2.y() * edge1.y() - deltaUV1.y() * edge2.y());
		tangent.z() = f * (deltaUV2.y() * edge1.z() - deltaUV1.y() * edge2.z());
		tangent.Normalize();

		bitangent.x() = f * (-deltaUV2.x() * edge1.x() + deltaUV1.x() * edge2.x());
		bitangent.y() = f * (-deltaUV2.x() * edge1.y() + deltaUV1.x() * edge2.y());
		bitangent.z() = f * (-deltaUV2.x() * edge1.z() + deltaUV1.x() * edge2.z());
		bitangent.Normalize();

		return {tangent, bitangent};
	}

	void	TestApplication::TestNormalMapping()
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
		lib.AddBindingMapping("Material_NormalMap", { MaterialTextureBinding::NORMAL, ResourceKind::TextureReadOnly });
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
				{"texCoords", VertexElementFormat::Float2},
				{"tangent", VertexElementFormat::Float3},
				{"bitangent", VertexElementFormat::Float3} },
			},
			LayoutType::Interleaved
		};

		auto cubeVao = renderer.CreateVertexLayout(cubeLayout);


		/* Create cube shader */
		IGraphicsRenderer::ShaderFileList blinnFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/blinn_phong_normal_mapping.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/blinn_phong_normal_mapping.frag" }
		};

		ShaderProgramHandle blinnProgram = renderer.CreateShaderProgramFromSourceFiles(blinnFileList);

		RenderWorld& renderWorld = MutRenderer().CreateRenderWorld();

		// Create plane geometry
		Vec3 normal = { 0.0f, 0.0f, 1.0f };

		auto [tangent1, bitangent1] = ComputeTriangleTangentBitangent(
			{ -1.f, 1.f,  0.f }, { 0.f,  1.f }, { -1.f, -1.F,  0.f }, { 0.0f,  0.0f }, { 1.f, -1.f, 0.f }, { 1.f, 0.0f });

		auto[tangent2, bitangent2] = ComputeTriangleTangentBitangent(
			{ -1.f, 1.f,  0.f }, { 0.f,  1.f }, { 1.f, -1.f, 0.f }, { 1.f, 0.0f }, { 1.f, 1.f, 0.f }, { 1.f, 1.f });

		VertexPositionNormalTextureTangentBitangent planeVertices[] = {
			// positions            // normals         // texcoords
			{{ -1.f, 1.f,  0.f},  normal,		{0.f,  1.f}, tangent1, bitangent1},
			{{-1.f, -1.F,  0.f},  normal,		{ 0.0f,  0.0f}, tangent1, bitangent1},
			{{1.f, -1.f, 0.f},  normal,	{ 1.f, 0.0f}, tangent1, bitangent1},
			{{ -1.f, 1.f,  0.f}, normal,	{0.f,  1.f}, tangent2, bitangent2},
			{{1.f, -1.f, 0.f},  normal,	{ 1.f, 0.0f}, tangent2, bitangent2},
			{{ 1.f, 1.f, 0.f},  normal,	{1.f, 1.f}, tangent2, bitangent2}
		};

		Mesh* plane = renderWorld.CreateStaticMesh(planeVertices);

		/* Create Phong material buffer */
		MaterialDescriptor materialdesc(
			{
				{"Material_Phong", ShaderStage::Fragment},
				{"Material_Sampler", ShaderStage::Fragment},
				{"Material_DiffuseMap", ShaderStage::Fragment},
				{"Material_NormalMap", ShaderStage::Fragment}
			}
		);
		MaterialInterface blinnPhongInterface = lib.CreateMaterialInterface(blinnProgram, materialdesc);
		MaterialInstance planeInst = lib.CreateMaterialInstance(blinnPhongInterface);
		planeInst.UpdateUniformBlock(MaterialBlockBinding::MATERIAL_PHONG,
			PhongMaterial{ Vec4(0.1f, 0.1f, 0.1f, 1.f),
							ColorRGBAf::White().ToVec(),
							Vec4(0.2f, 0.2f, 0.2f, 1.f),
							32 });


		SamplerDescriptor mySamplerDesc;
		mySamplerDesc.m_magFilter = SamplerFilter::Linear;
		mySamplerDesc.m_minFilter = SamplerFilter::LinearMipmapLinear;
		mySamplerDesc.m_wrap_S = SamplerWrapping::Repeat;
		mySamplerDesc.m_wrap_T = SamplerWrapping::Repeat;

		SamplerHandle mySampler = MutRenderer().MutGraphicsDevice().CreateSampler(mySamplerDesc);

		planeInst.BindSampler(MaterialSamplerBinding::SAMPLER_0, mySampler);

		Texture2DFileDescriptor brickDesc{ "Sandbox/assets/textures/brickwall.jpg" };
		brickDesc.m_wantedMipmapLevels = 8;
		Texture2DHandle brickImg = MutRenderer().MutGraphicsDevice().CreateTexture2D(brickDesc);
		planeInst.BindTexture(MaterialTextureBinding::DIFFUSE, brickImg);

		Texture2DFileDescriptor brickNormalDesc{ "Sandbox/assets/textures/brickwall_normal.jpg" };
		brickDesc.m_wantedMipmapLevels = 8;
		Texture2DHandle brickNormals = MutRenderer().MutGraphicsDevice().CreateTexture2D(brickNormalDesc);
		planeInst.BindTexture(MaterialTextureBinding::NORMAL, brickNormals);


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

		lightsSystem.AddNewLight({ Vec4{0.5f, 0.f, 0.3f, 1}, Vec4::ZeroVector(),
			Vec4(1.f), Vec4(1.f), Vec4(1.f) });
		//pointLight1->SetAttenuationFactors(1.f, 0.09f, 0.032f);

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

				// render normal-mapped quad rotated to show normal mapping from multiple directions
				plane->SetTransform(Transform::Identity());
				//plane->AddTransform(Transform::Rotate(Degs_f(thisFrameTime * -10.f), Vec3(1, 0, 1).GetNormalized()));
				plane->UpdateObjectMatrices(camSys.GetCamera(iCam));
				renderWorld.DrawMesh(plane, cubeVao, nullptr);

				// render light source (simply re-renders a smaller plane at the light's position for debugging/visualization)
				plane->SetTransform(Transform::Identity());
				plane->AddTransform(Transform::Translate(Vec3{ 0.5f, 0.f, 0.3f})); // the light position
				plane->AddTransform(Transform::Scale(Vec3(0.1f)));
				plane->UpdateObjectMatrices(camSys.GetCamera(iCam));
				renderWorld.DrawMesh(plane, cubeVao, nullptr);

			}

			SwapBuffers();
		}
	}



	void	TestApplication::TestParallaxMapping()
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
		lib.AddBindingMapping("Material_NormalMap", { MaterialTextureBinding::NORMAL, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_SpecularMap", { MaterialTextureBinding::SPECULAR, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_EmissionMap", { MaterialTextureBinding::EMISSION, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_SkyboxMap", { MaterialTextureBinding::SKYBOX, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_HeightMap", { MaterialTextureBinding::HEIGHT, ResourceKind::TextureReadOnly });

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
				{"texCoords", VertexElementFormat::Float2},
				{"tangent", VertexElementFormat::Float3},
				{"bitangent", VertexElementFormat::Float3} },
			},
			LayoutType::Interleaved
		};

		auto cubeVao = renderer.CreateVertexLayout(cubeLayout);


		/* Create cube shader */
		IGraphicsRenderer::ShaderFileList blinnFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/blinn_phong_parallax_mapping.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/blinn_phong_parallax_mapping.frag" }
		};

		ShaderProgramHandle blinnProgram = renderer.CreateShaderProgramFromSourceFiles(blinnFileList);

		RenderWorld& renderWorld = MutRenderer().CreateRenderWorld();

		// Create plane geometry
		Vec3 normal = { 0.0f, 0.0f, 1.0f };

		auto[tangent1, bitangent1] = ComputeTriangleTangentBitangent(
			{ -1.f, 1.f,  0.f }, { 0.f,  1.f }, { -1.f, -1.F,  0.f }, { 0.0f,  0.0f }, { 1.f, -1.f, 0.f }, { 1.f, 0.0f });

		auto[tangent2, bitangent2] = ComputeTriangleTangentBitangent(
			{ -1.f, 1.f,  0.f }, { 0.f,  1.f }, { 1.f, -1.f, 0.f }, { 1.f, 0.0f }, { 1.f, 1.f, 0.f }, { 1.f, 1.f });

		VertexPositionNormalTextureTangentBitangent planeVertices[] = {
			// positions            // normals         // texcoords
			{{ -1.f, 1.f,  0.f},  normal,		{0.f,  1.f}, tangent1, bitangent1},
			{{-1.f, -1.F,  0.f},  normal,		{ 0.0f,  0.0f}, tangent1, bitangent1},
			{{1.f, -1.f, 0.f},  normal,	{ 1.f, 0.0f}, tangent1, bitangent1},
			{{ -1.f, 1.f,  0.f}, normal,	{0.f,  1.f}, tangent2, bitangent2},
			{{1.f, -1.f, 0.f},  normal,	{ 1.f, 0.0f}, tangent2, bitangent2},
			{{ 1.f, 1.f, 0.f},  normal,	{1.f, 1.f}, tangent2, bitangent2}
		};

		Mesh* plane = renderWorld.CreateStaticMesh(planeVertices);

		/* Create Phong material buffer */
		MaterialDescriptor materialdesc(
			{
				{"Material_Phong", ShaderStage::Fragment},
				{"Material_Sampler", ShaderStage::Fragment},
				{"Material_DiffuseMap", ShaderStage::Fragment},
				{"Material_NormalMap", ShaderStage::Fragment},
				{"Material_HeightMap", ShaderStage::Fragment}
			}
		);
		MaterialInterface blinnPhongInterface = lib.CreateMaterialInterface(blinnProgram, materialdesc);
		MaterialInstance planeInst = lib.CreateMaterialInstance(blinnPhongInterface);
		planeInst.UpdateUniformBlock(MaterialBlockBinding::MATERIAL_PHONG,
			PhongMaterial{ Vec4(0.1f, 0.1f, 0.1f, 1.f),
							ColorRGBAf::White().ToVec(),
							Vec4(0.2f, 0.2f, 0.2f, 1.f),
							32 });


		SamplerDescriptor mySamplerDesc;
		mySamplerDesc.m_magFilter = SamplerFilter::Linear;
		mySamplerDesc.m_minFilter = SamplerFilter::LinearMipmapLinear;
		mySamplerDesc.m_wrap_S = SamplerWrapping::Repeat;
		mySamplerDesc.m_wrap_T = SamplerWrapping::Repeat;

		SamplerHandle mySampler = MutRenderer().MutGraphicsDevice().CreateSampler(mySamplerDesc);

		planeInst.BindSampler(MaterialSamplerBinding::SAMPLER_0, mySampler);

		Texture2DFileDescriptor brickDesc{ "Sandbox/assets/textures/toy_box_diffuse.png" };
		brickDesc.m_wantedMipmapLevels = 8;
		Texture2DHandle brickImg = MutRenderer().MutGraphicsDevice().CreateTexture2D(brickDesc);
		planeInst.BindTexture(MaterialTextureBinding::DIFFUSE, brickImg);

		Texture2DFileDescriptor brickNormalDesc{ "Sandbox/assets/textures/toy_box_normal.png" };
		brickDesc.m_wantedMipmapLevels = 8;
		Texture2DHandle brickNormals = MutRenderer().MutGraphicsDevice().CreateTexture2D(brickNormalDesc);
		planeInst.BindTexture(MaterialTextureBinding::NORMAL, brickNormals);

		Texture2DFileDescriptor brickHeightDesc{ "Sandbox/assets/textures/toy_box_disp.png" };
		brickDesc.m_wantedMipmapLevels = 8;
		Texture2DHandle brickHeight = MutRenderer().MutGraphicsDevice().CreateTexture2D(brickHeightDesc);
		planeInst.BindTexture(MaterialTextureBinding::HEIGHT, brickHeight);

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

		lightsSystem.AddNewLight({ Vec4{0.5f, 1.f, 0.3f, 1}, Vec4::ZeroVector(),
			Vec4(1.f), Vec4(1.f), Vec4(1.f) });
		//pointLight1->SetAttenuationFactors(1.f, 0.09f, 0.032f);

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

				// render normal-mapped quad rotated to show normal mapping from multiple directions
				plane->SetTransform(Transform::Identity());
				plane->AddTransform(Transform::Rotate(Degs_f(thisFrameTime * -10.f), Vec3(1, 0, 1).GetNormalized()));
				plane->UpdateObjectMatrices(camSys.GetCamera(iCam));
				renderWorld.DrawMesh(plane, cubeVao, nullptr);

				// render light source (simply re-renders a smaller plane at the light's position for debugging/visualization)
				plane->SetTransform(Transform::Identity());
				plane->AddTransform(Transform::Translate(Vec3{ 0.5f, 1.f, 0.3f })); // the light position
				plane->AddTransform(Transform::Scale(Vec3(0.1f)));
				plane->UpdateObjectMatrices(camSys.GetCamera(iCam));
				renderWorld.DrawMesh(plane, cubeVao, nullptr);

			}

			SwapBuffers();
		}
	}


	void	TestApplication::TestHDR()
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
		lib.AddBindingMapping("Frame_ToneMappingParams", { MaterialBlockBinding::FRAME_TONE_MAPPING, ResourceKind::UniformBuffer });

		lib.AddBindingMapping("Material_DiffuseMap", { MaterialTextureBinding::DIFFUSE, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_NormalMap", { MaterialTextureBinding::NORMAL, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_SpecularMap", { MaterialTextureBinding::SPECULAR, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_EmissionMap", { MaterialTextureBinding::EMISSION, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_SkyboxMap", { MaterialTextureBinding::SKYBOX, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_HeightMap", { MaterialTextureBinding::HEIGHT, ResourceKind::TextureReadOnly });

		lib.AddBindingMapping("Material_Sampler", { MaterialSamplerBinding::SAMPLER_0, ResourceKind::Sampler });

		lib.AddUniformBufferSizer(MaterialBlockBinding::FRAME_LIGHTS, []() { return sizeof(LightCastersData); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::VIEW_CAMERA, []() { return sizeof(CameraMatrices); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::MATERIAL_SKYBOX_VIEWPROJ, []() { return sizeof(Mat4); });

		lib.AddUniformBufferSizer(MaterialBlockBinding::MATERIAL_PHONG, []() { return sizeof(PhongMaterial); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::MATERIAL_COLOR, []() { return sizeof(ColorRGBAf); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::OBJECT_MATRICES, []() { return sizeof(ObjectMatrices); });

		lib.AddUniformBufferSizer(MaterialBlockBinding::FRAME_TONE_MAPPING, []() { return sizeof(ToneMappingParams); });

		SetInputKeyMapping(GLFW_KEY_W, GLFW_PRESS, [this]() { this->m_moveForward = true; });
		SetInputKeyMapping(GLFW_KEY_W, GLFW_RELEASE, [this]() { this->m_moveForward = false; });

		SetInputKeyMapping(GLFW_KEY_S, GLFW_PRESS, [this]() { this->m_moveBackward = true; });
		SetInputKeyMapping(GLFW_KEY_S, GLFW_RELEASE, [this]() { this->m_moveBackward = false; });

		SetInputKeyMapping(GLFW_KEY_A, GLFW_PRESS, [this]() { this->m_strafeLeft = true; });
		SetInputKeyMapping(GLFW_KEY_A, GLFW_RELEASE, [this]() { this->m_strafeLeft = false; });

		SetInputKeyMapping(GLFW_KEY_D, GLFW_PRESS, [this]() { this->m_strafeRight = true; });
		SetInputKeyMapping(GLFW_KEY_D, GLFW_RELEASE, [this]() { this->m_strafeRight = false; });

		SetInputKeyMapping(GLFW_KEY_SPACE, GLFW_RELEASE, [this]() { this->m_toneMappingParams.m_enabled = !this->m_toneMappingParams.m_enabled; });
		SetInputKeyMapping(GLFW_KEY_Q, GLFW_PRESS, [this]()
		{
			this->m_toneMappingParams.m_exposure -= 0.01f;
		});
		SetInputKeyMapping(GLFW_KEY_Q, GLFW_REPEAT, [this]()
		{
			this->m_toneMappingParams.m_exposure -= 0.01f;
		});
		SetInputKeyMapping(GLFW_KEY_E, GLFW_PRESS, [this]() { this->m_toneMappingParams.m_exposure += 0.01f; });

		SetInputKeyMapping(GLFW_KEY_E, GLFW_REPEAT, [this]() { this->m_toneMappingParams.m_exposure += 0.01f; });
		SetInputKeyMapping(GLFW_KEY_R, GLFW_RELEASE, [this]() { this->m_toneMappingParams.m_useReinhardToneMapping = !this->m_toneMappingParams.m_useReinhardToneMapping; });



		auto[mouseX, mouseY] = GetMouseCursorPosition();
		m_lastX = mouseX;
		m_lastY = mouseY;

		SetInputMouseMoveMapping(std::bind(&TestApplication::OrientCameraWithMouse, this, std::placeholders::_1, std::placeholders::_2));

		SetInputMouseScrollMapping(std::bind(&TestApplication::CameraZoomMouseScroll, this, std::placeholders::_1, std::placeholders::_2));


		Texture2DDescriptor colorAttachDesc{ nullptr, Width_t(GetWindowWidth()), Height_t(GetWindowHeight()), TextureFormat::RGBA16F, TextureUsage{Sampled} };
		Texture2DDescriptor depthAttachDesc{ nullptr, Width_t(GetWindowWidth()), Height_t(GetWindowHeight()), TextureFormat::Depth24_Stencil8, TextureUsage{RenderTarget} };

		Texture2DHandle colorAttachTex = renderer.MutGraphicsDevice().CreateTexture2D(colorAttachDesc);
		Texture2DHandle depthAttachTex = renderer.MutGraphicsDevice().CreateTexture2D(depthAttachDesc);

		FramebufferDescriptor fbDesc{ {colorAttachTex}, depthAttachTex };
		FramebufferHandle fbHandle = renderer.MutGraphicsDevice().CreateFramebuffer(fbDesc);


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


		/* Create cube VAO */
		VertexLayoutDescriptor quadLayout{
			{
				{{"position", VertexElementFormat::Float3},
				{"texCoords", VertexElementFormat::Float2}},
			},
			LayoutType::Interleaved
		};

		auto quadVao = renderer.CreateVertexLayout(quadLayout);


		/* Create cube shader */
		IGraphicsRenderer::ShaderFileList hdrLightingFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/hdr_blinn_phong.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/hdr_blinn_phong.frag" }
		};

		ShaderProgramHandle hdrLightingProgram = renderer.CreateShaderProgramFromSourceFiles(hdrLightingFileList);


		IGraphicsRenderer::ShaderFileList hdrFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/hdr_blit.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/hdr_blit.frag" }
		};

		ShaderProgramHandle hdrProgram = renderer.CreateShaderProgramFromSourceFiles(hdrFileList);

		RenderWorld& renderWorld = MutRenderer().CreateRenderWorld();

		// Create geometry
		auto cubeGeom = CreateCubePositionNormalTexture(1.f, true);

		Mesh* cube = renderWorld.CreateStaticMesh(cubeGeom);

		Array<VertexPositionTexture, 6> quadVertices{ // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
			// positions   // texCoords
			{{-1.0f,  1.0f, 0.f},  {0.0f, 1.0f}},
			{{-1.0f, -1.0f, 0.f},  {0.0f, 0.0f}},
			{{ 1.0f, -1.0f, 0.f},  {1.0f, 0.0f}},

			{{-1.0f,  1.0f, 0.f},  {0.0f, 1.0f}},
			{{ 1.0f, -1.0f, 0.f},  {1.0f, 0.0f}},
			{{ 1.0f,  1.0f, 0.f},  {1.0f, 1.0f}}
		};


		Mesh* plane = renderWorld.CreateStaticMesh(quadVertices);

		/* Create Phong material buffer */
		MaterialDescriptor materialdesc(
			{
				{"Material_Phong", ShaderStage::Fragment},
				{"Material_Sampler", ShaderStage::Fragment},
				{"Material_DiffuseMap", ShaderStage::Fragment}
			}
		);
		MaterialInterface hdrInterface = lib.CreateMaterialInterface(hdrLightingProgram, materialdesc);
		MaterialInstance planeInst = lib.CreateMaterialInstance(hdrInterface);
		planeInst.UpdateUniformBlock(MaterialBlockBinding::MATERIAL_PHONG,
			PhongMaterial{ ColorRGBAf::White().ToVec(),
							ColorRGBAf::White().ToVec(),
							ColorRGBAf::White().ToVec(),
							32 });


		SamplerDescriptor mySamplerDesc;
		mySamplerDesc.m_magFilter = SamplerFilter::Linear;
		mySamplerDesc.m_minFilter = SamplerFilter::LinearMipmapLinear;
		mySamplerDesc.m_wrap_S = SamplerWrapping::Repeat;
		mySamplerDesc.m_wrap_T = SamplerWrapping::Repeat;

		SamplerHandle mySampler = MutRenderer().MutGraphicsDevice().CreateSampler(mySamplerDesc);

		planeInst.BindSampler(MaterialSamplerBinding::SAMPLER_0, mySampler);

		Texture2DFileDescriptor woodDesc{ "Sandbox/assets/textures/wood.png", TextureFormat::SRGB_RGBA8 };
		woodDesc.m_wantedMipmapLevels = 8;
		Texture2DHandle brickImg = MutRenderer().MutGraphicsDevice().CreateTexture2D(woodDesc);
		planeInst.BindTexture(MaterialTextureBinding::DIFFUSE, brickImg);

		planeInst.CreateMaterialResourceSet();
		/* End Phong material buffer */

		/* Create full screen quad material */
		MaterialDescriptor framebufferMatDesc({
			{"Material_DiffuseMap", ShaderStage::Fragment},
			{"Frame_ToneMappingParams", ShaderStage::Fragment}
		});

		MaterialInterface fbMatIntf = lib.CreateMaterialInterface(hdrProgram, framebufferMatDesc);
		MaterialInstance fbMatInst = lib.CreateMaterialInstance(fbMatIntf);

		fbMatInst.UpdateUniformBlock(MaterialBlockBinding::FRAME_TONE_MAPPING, m_toneMappingParams);

		fbMatInst.BindTexture(MaterialTextureBinding::DIFFUSE, colorAttachTex);

		fbMatInst.CreateMaterialResourceSet();

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

		auto pointLight =
		lightsSystem.AddNewLight({ Vec4{0.0f,  0.0f, 49.5f, 1}, Vec4::ZeroVector(),
			Vec4(0.f), Vec4(200.0f, 200.0f, 200.0f, 1.f), Vec4(0.f) });
		pointLight->SetAttenuationFactors(0.f, 0.0f, 1.f);

		pointLight =
		lightsSystem.AddNewLight({ Vec4{-1.4f, -1.9f, 9.0f, 1}, Vec4::ZeroVector(),
	Vec4(0.f), Vec4(0.1f, 0.0f, 0.0f, 1), Vec4(0.f) });
		pointLight->SetAttenuationFactors(0.f, 0.0f, 1.f);

		pointLight =
		lightsSystem.AddNewLight({ Vec4{0.0f, -1.8f, 4.0f, 1}, Vec4::ZeroVector(),
	Vec4(0.f), Vec4(0.0f, 0.0f, 0.2f, 1.f), Vec4(0.f) });
		pointLight->SetAttenuationFactors(0.f, 0.0f, 1.f);

		pointLight =
		lightsSystem.AddNewLight({ Vec4{0.8f, -1.7f, 6.0f, 1}, Vec4::ZeroVector(),
	Vec4(0.f), Vec4(0.0f, 0.1f, 0.0f, 1.f), Vec4(0.f) });
		pointLight->SetAttenuationFactors(0.f, 0.0f, 1.f);

		m_renderer.MutGraphicsDevice().SetPipeline(myPipe);



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

			// First, render tunnel in framebuffer

			m_renderer.MutGraphicsDevice().SetPipeline(myPipe);

			// Bind the framebuffer to effectively render-to-texture
			renderer.BindFramebuffer(fbHandle);

			renderer.Clear(ColorRGBAf(0.1f, 0.1f, 0.1f, 1.0f));

			lightsSystem.UpdateLights();

			lightsSystem.BindLightBuffer();

			camSys.UpdateCameras();

			for (uint32_t iCam = 0; iCam < camSys.CamerasNumber(); iCam++)
			{
				camSys.BindCameraBuffer(iCam);

				renderer.UseMaterialInstance(&planeInst);

				cube->SetTransform(Transform::Identity());
				cube->AddTransform(Transform::Translate(Vec3(0.0f, 0.0f, 25.0f)));
				cube->AddTransform(Transform::Scale(Vec3(2.5f, 2.5f, 27.5f)));
				cube->UpdateObjectMatrices(camSys.GetCamera(iCam));
				renderWorld.DrawMesh(cube, cubeVao, nullptr);
			}

			renderer.UnbindFramebuffer(fbHandle);

			// Finally - draw the framebuffer using a fullscreen quad

			renderer.MutGraphicsDevice().SetPipeline(fsqPipe);

			fbMatInst.UpdateUniformBlock(MaterialBlockBinding::FRAME_TONE_MAPPING, m_toneMappingParams);

			renderer.UseMaterialInstance(&fbMatInst);
			renderWorld.DrawMesh(plane, quadVao, nullptr);

			SwapBuffers();

		}
	}

	void	TestApplication::TestBloom()
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
		lib.AddBindingMapping("Frame_ToneMappingParams", { MaterialBlockBinding::FRAME_TONE_MAPPING, ResourceKind::UniformBuffer });
		lib.AddBindingMapping("Frame_TwoPassGaussianBlur", { MaterialBlockBinding::FRAME_GAUSSIAN_BLUR, ResourceKind::UniformBuffer });

		lib.AddBindingMapping("Material_DiffuseMap", { MaterialTextureBinding::DIFFUSE, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_NormalMap", { MaterialTextureBinding::NORMAL, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_SpecularMap", { MaterialTextureBinding::SPECULAR, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_EmissionMap", { MaterialTextureBinding::EMISSION, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_SkyboxMap", { MaterialTextureBinding::SKYBOX, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_HeightMap", { MaterialTextureBinding::HEIGHT, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_BloomMap", { MaterialTextureBinding::BLOOM, ResourceKind::TextureReadOnly });

		lib.AddBindingMapping("Material_Sampler", { MaterialSamplerBinding::SAMPLER_0, ResourceKind::Sampler });

		lib.AddUniformBufferSizer(MaterialBlockBinding::FRAME_LIGHTS, []() { return sizeof(LightCastersData); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::VIEW_CAMERA, []() { return sizeof(CameraMatrices); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::MATERIAL_SKYBOX_VIEWPROJ, []() { return sizeof(Mat4); });

		lib.AddUniformBufferSizer(MaterialBlockBinding::MATERIAL_PHONG, []() { return sizeof(PhongMaterial); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::MATERIAL_COLOR, []() { return sizeof(ColorRGBAf); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::OBJECT_MATRICES, []() { return sizeof(ObjectMatrices); });

		lib.AddUniformBufferSizer(MaterialBlockBinding::FRAME_TONE_MAPPING, []() { return sizeof(ToneMappingParams); });

		lib.AddUniformBufferSizer(MaterialBlockBinding::FRAME_GAUSSIAN_BLUR, []() { return sizeof(TwoPassGaussianBlurParams); });


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


		Texture2DDescriptor colorAttachDesc{ nullptr, Width_t(GetWindowWidth()), Height_t(GetWindowHeight()), TextureFormat::RGBA16F, TextureUsage{Sampled} };

		// Our color texture to render the scene into
		Texture2DHandle colorAttachTex = renderer.MutGraphicsDevice().CreateTexture2D(colorAttachDesc);

		// Our texture to store the brightness information into
		Texture2DHandle brightnessTexture = renderer.MutGraphicsDevice().CreateTexture2D(colorAttachDesc);

		Texture2DDescriptor depthAttachDesc{ nullptr, Width_t(GetWindowWidth()), Height_t(GetWindowHeight()), TextureFormat::Depth24_Stencil8, TextureUsage{RenderTarget} };
		Texture2DHandle depthAttachTex = renderer.MutGraphicsDevice().CreateTexture2D(depthAttachDesc);

		FramebufferDescriptor fbDesc{ {colorAttachTex, brightnessTexture}, depthAttachTex };
		fbDesc.m_drawBuffer = TargetBuffer::AllColorAttachments; // Tell the framebuffer to draw to ALL color buffers (not just the first one).
		FramebufferHandle fbHandle = renderer.MutGraphicsDevice().CreateFramebuffer(fbDesc);

		/* Create gaussian blur ping pong framebuffers */
		Texture2DHandle pingpongColorTex1 = renderer.MutGraphicsDevice().CreateTexture2D(colorAttachDesc);
		FramebufferDescriptor pingpongDesc1{ {pingpongColorTex1} };
		FramebufferHandle pingpongFb1 = renderer.MutGraphicsDevice().CreateFramebuffer(pingpongDesc1);

		Texture2DHandle pingpongColorTex2 = renderer.MutGraphicsDevice().CreateTexture2D(colorAttachDesc);
		FramebufferDescriptor pingpongDesc2{ {pingpongColorTex2} };
		FramebufferHandle pingpongFb2 = renderer.MutGraphicsDevice().CreateFramebuffer(pingpongDesc2);

		Texture2DHandle blurColorAttachments[2]{pingpongColorTex1, pingpongColorTex2};
		FramebufferHandle blurFramebuffers[2]{pingpongFb1, pingpongFb2};

		SamplerDescriptor blurSamplerDesc;
		blurSamplerDesc.m_magFilter = SamplerFilter::Linear;
		blurSamplerDesc.m_minFilter = SamplerFilter::Linear;
		// we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		blurSamplerDesc.m_wrap_S = SamplerWrapping::ClampToEdge;
		blurSamplerDesc.m_wrap_T = SamplerWrapping::ClampToEdge;

		SamplerHandle blurSampler = MutRenderer().MutGraphicsDevice().CreateSampler(blurSamplerDesc);

		/* Create blur quad material */
		IGraphicsRenderer::ShaderFileList bloomBlurFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/bloom_blur.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/bloom_blur.frag" }
		};

		ShaderProgramHandle bloomBlurProgram = renderer.CreateShaderProgramFromSourceFiles(bloomBlurFileList);

		MaterialDescriptor blurMatDesc({
			{"Frame_TwoPassGaussianBlur", ShaderStage::Fragment},
			{"Material_Sampler", ShaderStage::Fragment},
			{"Material_DiffuseMap", ShaderStage::Fragment}
			});

		MaterialInterface blurMatIntf = lib.CreateMaterialInterface(bloomBlurProgram, blurMatDesc);
		MaterialInstance blurMatInst = lib.CreateMaterialInstance(blurMatIntf);

		TwoPassGaussianBlurParams gaussianBlurParams{ true, (int)GetWindowWidth(), (int)GetWindowHeight(), 5, {0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f} };
		blurMatInst.UpdateUniformBlock(MaterialBlockBinding::FRAME_GAUSSIAN_BLUR,
			gaussianBlurParams);

		blurMatInst.BindSampler(SAMPLER_0, blurSampler);
		blurMatInst.BindTexture(MaterialTextureBinding::DIFFUSE, brightnessTexture);

		blurMatInst.CreateMaterialResourceSet();

		const int gaussianBlurAmount = 10;

		/* End blur material */

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


		/* Create cube VAO */
		VertexLayoutDescriptor quadLayout{
			{
				{{"position", VertexElementFormat::Float3},
				{"texCoords", VertexElementFormat::Float2}},
			},
			LayoutType::Interleaved
		};

		auto quadVao = renderer.CreateVertexLayout(quadLayout);


		/* Create cube shader */
		IGraphicsRenderer::ShaderFileList bloomLightingFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/bloom_blinn_phong.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/bloom_blinn_phong.frag" }
		};

		ShaderProgramHandle hdrLightingProgram = renderer.CreateShaderProgramFromSourceFiles(bloomLightingFileList);

		IGraphicsRenderer::ShaderFileList lightBoxFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/bloom_blinn_phong.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/bloom_light_box.frag" }
		};

		ShaderProgramHandle lightBoxProgram = renderer.CreateShaderProgramFromSourceFiles(lightBoxFileList);

		IGraphicsRenderer::ShaderFileList bloomFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/bloom_final.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/bloom_final.frag" }
		};

		ShaderProgramHandle bloomProgram = renderer.CreateShaderProgramFromSourceFiles(bloomFileList);

		RenderWorld& renderWorld = MutRenderer().CreateRenderWorld();

		// Create geometry
		auto cubeGeom = CreateCubePositionNormalTexture(1.f, false);

		Mesh* cube = renderWorld.CreateStaticMesh(cubeGeom);

		Array<VertexPositionTexture, 6> quadVertices{ // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
			// positions   // texCoords
			{{-1.0f,  1.0f, 0.f},  {0.0f, 1.0f}},
			{{-1.0f, -1.0f, 0.f},  {0.0f, 0.0f}},
			{{ 1.0f, -1.0f, 0.f},  {1.0f, 0.0f}},

			{{-1.0f,  1.0f, 0.f},  {0.0f, 1.0f}},
			{{ 1.0f, -1.0f, 0.f},  {1.0f, 0.0f}},
			{{ 1.0f,  1.0f, 0.f},  {1.0f, 1.0f}}
		};


		Mesh* plane = renderWorld.CreateStaticMesh(quadVertices);

		/* Create Phong material buffer */
		MaterialDescriptor materialdesc(
			{
				{"Material_Phong", ShaderStage::Fragment},
				{"Material_Sampler", ShaderStage::Fragment},
				{"Material_DiffuseMap", ShaderStage::Fragment}
			}
		);
		MaterialInterface hdrInterface = lib.CreateMaterialInterface(hdrLightingProgram, materialdesc);
		MaterialInstance planeInst = lib.CreateMaterialInstance(hdrInterface);
		planeInst.UpdateUniformBlock(MaterialBlockBinding::MATERIAL_PHONG,
			PhongMaterial{ ColorRGBAf::White().ToVec(),
							ColorRGBAf::White().ToVec(),
							ColorRGBAf::White().ToVec(),
							32 });


		SamplerDescriptor mySamplerDesc;
		mySamplerDesc.m_magFilter = SamplerFilter::Linear;
		mySamplerDesc.m_minFilter = SamplerFilter::LinearMipmapLinear;
		mySamplerDesc.m_wrap_S = SamplerWrapping::Repeat;
		mySamplerDesc.m_wrap_T = SamplerWrapping::Repeat;

		SamplerHandle mySampler = MutRenderer().MutGraphicsDevice().CreateSampler(mySamplerDesc);

		planeInst.BindSampler(MaterialSamplerBinding::SAMPLER_0, mySampler);

		Texture2DFileDescriptor woodDesc{ "Sandbox/assets/textures/wood.png", TextureFormat::SRGB_RGBA8 };
		woodDesc.m_wantedMipmapLevels = 8;
		Texture2DHandle brickImg = MutRenderer().MutGraphicsDevice().CreateTexture2D(woodDesc);
		planeInst.BindTexture(MaterialTextureBinding::DIFFUSE, brickImg);

		planeInst.CreateMaterialResourceSet();

		MaterialInstance boxInst = lib.CreateMaterialInstance(hdrInterface);

		boxInst.UpdateUniformBlock(MaterialBlockBinding::MATERIAL_PHONG,
			PhongMaterial{ ColorRGBAf::White().ToVec(),
							ColorRGBAf::White().ToVec(),
							ColorRGBAf::White().ToVec(),
							32 });

		boxInst.BindSampler(MaterialSamplerBinding::SAMPLER_0, mySampler);

		Texture2DFileDescriptor containerDesc{ "Sandbox/assets/textures/container2.png", TextureFormat::SRGB_RGBA8 };
		woodDesc.m_wantedMipmapLevels = 8;
		Texture2DHandle containerImg = MutRenderer().MutGraphicsDevice().CreateTexture2D(containerDesc);
		boxInst.BindTexture(MaterialTextureBinding::DIFFUSE, containerImg);

		boxInst.CreateMaterialResourceSet();
		/* End Phong material buffer */

		/* Create full screen quad material */
		MaterialDescriptor framebufferMatDesc({
			{"Material_DiffuseMap", ShaderStage::Fragment},
			{"Material_BloomMap", ShaderStage::Fragment},
			{"Frame_ToneMappingParams", ShaderStage::Fragment}
			});

		MaterialInterface fbMatIntf = lib.CreateMaterialInterface(bloomProgram, framebufferMatDesc);
		MaterialInstance fbMatInst = lib.CreateMaterialInstance(fbMatIntf);

		fbMatInst.UpdateUniformBlock(MaterialBlockBinding::FRAME_TONE_MAPPING, m_toneMappingParams);

		fbMatInst.BindTexture(MaterialTextureBinding::DIFFUSE, colorAttachTex);

		// Attach to the last bloom pingpong FBO color attachment that will have been drawn
		// To know which FBO to bind with is simple: even number of gaussian blurs = first FBO.
		fbMatInst.BindTexture(MaterialTextureBinding::BLOOM, blurColorAttachments[gaussianBlurAmount % 2]);

		fbMatInst.CreateMaterialResourceSet();

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

		Vec4 lightPositions[] = {
			{ 0.0f, 0.5f,  1.5f, 1.f},
			{ -4.0f, 0.5f, -3.0f, 1.f},
			{  3.0f, 0.5f,  1.0f, 1.f},
			{ -.8f,  2.4f, -1.0f, 1.f}
		};

		Vec4 lightColors[] = {
			{ 5.0f,   5.0f,  5.0f, 1.f},
			{ 10.0f,  0.0f,  0.0f, 1.f},
			{ 0.0f,   0.0f,  15.f, 1.f},
			{ 0.0f,   5.0f,  0.0f, 1.f}
		};

		int iLight = 0;
		for (const Vec4& lightPos : lightPositions)
		{
			auto pointLight =
				lightsSystem.AddNewLight({ lightPos, Vec4::ZeroVector(),
					Vec4(0.f), lightColors[iLight], Vec4(0.f) });
			pointLight->SetAttenuationFactors(0.f, 0.0f, 1.f);
			iLight++;
		}


		MaterialDescriptor lightMatDesc(
			{
				{"Material_Color", ShaderStage::Fragment} }
		);

		MaterialInterface lightMatInterface = lib.CreateMaterialInterface(lightBoxProgram, lightMatDesc);

		MaterialInstance lightMatInstance = lib.CreateMaterialInstance(lightMatInterface);

		lightMatInstance.UpdateUniformBlock(MaterialBlockBinding::MATERIAL_COLOR, ColorRGBAf::White());

		lightMatInstance.CreateMaterialResourceSet();


		m_renderer.MutGraphicsDevice().SetPipeline(myPipe);



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

			// First, render tunnel in framebuffer

			m_renderer.MutGraphicsDevice().SetPipeline(myPipe);

			// Bind the framebuffer to effectively render-to-texture
			renderer.BindFramebuffer(fbHandle);

			renderer.Clear(ColorRGBAf(0.1f, 0.1f, 0.1f, 1.0f));

			lightsSystem.UpdateLights();

			lightsSystem.BindLightBuffer();

			camSys.UpdateCameras();

			for (uint32_t iCam = 0; iCam < camSys.CamerasNumber(); iCam++)
			{
				camSys.BindCameraBuffer(iCam);

				renderer.UseMaterialInstance(&planeInst);

				cube->SetTransform(Transform::Identity());
				cube->AddTransform(Transform::Translate(Vec3(0.0f, -1.0f, 0.0f)));
				cube->AddTransform(Transform::Scale(Vec3(12.5f, 0.5f, 12.5f)));
				cube->UpdateObjectMatrices(camSys.GetCamera(iCam));
				renderWorld.DrawMesh(cube, cubeVao, nullptr);

				renderer.UseMaterialInstance(&boxInst);

				cube->SetTransform(Transform::Identity());
				cube->AddTransform(Transform::Translate(Vec3(0.0f, 1.5f, 0.0f)));
				cube->AddTransform(Transform::Scale(Vec3(0.5f)));
				cube->UpdateObjectMatrices(camSys.GetCamera(iCam));
				renderWorld.DrawMesh(cube, cubeVao, nullptr);

				cube->SetTransform(Transform::Identity());
				cube->AddTransform(Transform::Translate(Vec3(2.0f, 0.0f, 1.0f)));
				cube->AddTransform(Transform::Scale(Vec3(0.5f)));
				cube->UpdateObjectMatrices(camSys.GetCamera(iCam));
				renderWorld.DrawMesh(cube, cubeVao, nullptr);

				cube->SetTransform(Transform::Identity());
				cube->AddTransform(Transform::Translate(Vec3(-1.0f, -1.0f, 2.0f)));
				cube->AddTransform(Transform::Rotate(60_degf, Vec3(1.0, 0.0, 1.0).GetNormalized()));
				cube->UpdateObjectMatrices(camSys.GetCamera(iCam));
				renderWorld.DrawMesh(cube, cubeVao, nullptr);

				cube->SetTransform(Transform::Identity());
				cube->AddTransform(Transform::Translate(Vec3(0.0f, 2.7f, 4.0f)));
				cube->AddTransform(Transform::Rotate(23_degf, Vec3(1.0, 0.0, 1.0).GetNormalized()));
				cube->UpdateObjectMatrices(camSys.GetCamera(iCam));
				renderWorld.DrawMesh(cube, cubeVao, nullptr);

				cube->SetTransform(Transform::Identity());
				cube->AddTransform(Transform::Translate(Vec3(-2.0f, 1.0f, -3.0f)));
				cube->AddTransform(Transform::Rotate(124_degf, Vec3(1.0, 0.0, 1.0).GetNormalized()));
				cube->UpdateObjectMatrices(camSys.GetCamera(iCam));
				renderWorld.DrawMesh(cube, cubeVao, nullptr);

				cube->SetTransform(Transform::Identity());
				cube->AddTransform(Transform::Translate(Vec3(-3.0f, 0.0f, 0.0f)));
				cube->AddTransform(Transform::Scale(Vec3(0.5f)));
				cube->UpdateObjectMatrices(camSys.GetCamera(iCam));
				renderWorld.DrawMesh(cube, cubeVao, nullptr);


				iLight = 0;
				for (auto& lightPos : lightPositions)
				{
					lightMatInstance.UpdateUniformBlock(MaterialBlockBinding::MATERIAL_COLOR, lightColors[iLight]);
					renderer.UseMaterialInstance(&lightMatInstance);

					cube->SetTransform(Transform::Identity());
					cube->AddTransform(Transform::Translate(lightPos.xyz()));
					cube->AddTransform(Transform::Scale(Vec3(0.25f)));
					cube->UpdateObjectMatrices(camSys.GetCamera(iCam));
					renderWorld.DrawMesh(cube, cubeVao, nullptr);

					iLight++;
				}
			}

			renderer.UnbindFramebuffer(fbHandle);

			// Step two - draw the gaussian-blurred brightness buffer using a fullscreen quad

			renderer.MutGraphicsDevice().SetPipeline(fsqPipe);

			for (int iBlur = 0; iBlur < gaussianBlurAmount; iBlur++)
			{
				gaussianBlurParams.m_horizontalBlur = !gaussianBlurParams.m_horizontalBlur;

				blurMatInst.UpdateUniformBlock(MaterialBlockBinding::FRAME_GAUSSIAN_BLUR,
					gaussianBlurParams);

				const Texture2DHandle& sourceAttachment = (iBlur == 0 ? brightnessTexture : blurColorAttachments[iBlur % 2]);

				blurMatInst.UpdateTexture(DIFFUSE, sourceAttachment);

				renderer.UseMaterialInstance(&blurMatInst);

				const int destFramebuffer = (iBlur + 1) % 2;

				renderer.BindFramebuffer(blurFramebuffers[destFramebuffer]);

				renderWorld.DrawMesh(plane, quadVao, nullptr);

				renderer.UnbindFramebuffer(blurFramebuffers[destFramebuffer]);
			}

			// Finally - draw the framebuffer using a fullscreen quad


			fbMatInst.UpdateUniformBlock(MaterialBlockBinding::FRAME_TONE_MAPPING, m_toneMappingParams);

			renderer.UseMaterialInstance(&fbMatInst);
			renderWorld.DrawMesh(plane, quadVao, nullptr);

			SwapBuffers();

		}
	}


	void	TestApplication::TestDeferredRendering()
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
		lib.AddBindingMapping("Frame_ToneMappingParams", { MaterialBlockBinding::FRAME_TONE_MAPPING, ResourceKind::UniformBuffer });
		lib.AddBindingMapping("Frame_TwoPassGaussianBlur", { MaterialBlockBinding::FRAME_GAUSSIAN_BLUR, ResourceKind::UniformBuffer });

		lib.AddBindingMapping("Material_DiffuseMap", { MaterialTextureBinding::DIFFUSE, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_NormalMap", { MaterialTextureBinding::NORMAL, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_SpecularMap", { MaterialTextureBinding::SPECULAR, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_EmissionMap", { MaterialTextureBinding::EMISSION, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_SkyboxMap", { MaterialTextureBinding::SKYBOX, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_HeightMap", { MaterialTextureBinding::HEIGHT, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_BloomMap", { MaterialTextureBinding::BLOOM, ResourceKind::TextureReadOnly });

		lib.AddBindingMapping("Material_Sampler", { MaterialSamplerBinding::SAMPLER_0, ResourceKind::Sampler });

		lib.AddUniformBufferSizer(MaterialBlockBinding::FRAME_LIGHTS, []() { return sizeof(LightCastersData); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::VIEW_CAMERA, []() { return sizeof(CameraMatrices); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::MATERIAL_SKYBOX_VIEWPROJ, []() { return sizeof(Mat4); });

		lib.AddUniformBufferSizer(MaterialBlockBinding::MATERIAL_PHONG, []() { return sizeof(PhongMaterial); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::MATERIAL_COLOR, []() { return sizeof(ColorRGBAf); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::OBJECT_MATRICES, []() { return sizeof(ObjectMatrices); });

		lib.AddUniformBufferSizer(MaterialBlockBinding::FRAME_TONE_MAPPING, []() { return sizeof(ToneMappingParams); });

		lib.AddUniformBufferSizer(MaterialBlockBinding::FRAME_GAUSSIAN_BLUR, []() { return sizeof(TwoPassGaussianBlurParams); });


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


		// Create our G-buffer using position, normal, albedo + specular color buffers
		// (store specular as a component of albedo buffer as an optimization)
		Texture2DDescriptor hiPrecisionColorAttachDesc{ nullptr, Width_t(GetWindowWidth()), Height_t(GetWindowHeight()), TextureFormat::RGBA16F, TextureUsage{Sampled} };
		Texture2DDescriptor normalPrecisionColorAttachDesc{ nullptr, Width_t(GetWindowWidth()), Height_t(GetWindowHeight()), TextureFormat::RGBA8, TextureUsage{Sampled} };
		Texture2DDescriptor depthAttachDesc{ nullptr, Width_t(GetWindowWidth()), Height_t(GetWindowHeight()), TextureFormat::Depth24_Stencil8, TextureUsage{RenderTarget} };


		Texture2DHandle positionTexture = renderer.MutGraphicsDevice().CreateTexture2D(hiPrecisionColorAttachDesc);
		Texture2DHandle normalsTexture = renderer.MutGraphicsDevice().CreateTexture2D(hiPrecisionColorAttachDesc);
		Texture2DHandle albedoSpecularTexture = renderer.MutGraphicsDevice().CreateTexture2D(normalPrecisionColorAttachDesc);
		Texture2DHandle gBufDepthAttachTex = renderer.MutGraphicsDevice().CreateTexture2D(depthAttachDesc);

		FramebufferDescriptor gBufferDesc{ {positionTexture, normalsTexture, albedoSpecularTexture}, gBufDepthAttachTex };
		gBufferDesc.m_drawBuffer = TargetBuffer::AllColorAttachments; // Tell the framebuffer to draw to ALL color buffers (not just the first one) to enable MRT rendering.
		FramebufferHandle gBufferHandle = renderer.MutGraphicsDevice().CreateFramebuffer(gBufferDesc);

		IGraphicsRenderer::ShaderFileList deferredGBufferFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/deferred_gbuffer.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/deferred_gbuffer.frag" }
		};

		ShaderProgramHandle deferredGBufferProgram = renderer.CreateShaderProgramFromSourceFiles(deferredGBufferFileList);


		Texture2DDescriptor colorAttachDesc{ nullptr, Width_t(GetWindowWidth()), Height_t(GetWindowHeight()), TextureFormat::RGBA16F, TextureUsage{Sampled} };

		// Our color texture to render the scene into
		Texture2DHandle colorAttachTex = renderer.MutGraphicsDevice().CreateTexture2D(colorAttachDesc);

		// Our texture to store the brightness information into
		Texture2DHandle brightnessTexture = renderer.MutGraphicsDevice().CreateTexture2D(colorAttachDesc);

		Texture2DHandle depthAttachTex = renderer.MutGraphicsDevice().CreateTexture2D(depthAttachDesc);

		FramebufferDescriptor fbDesc{ {colorAttachTex, brightnessTexture}, depthAttachTex };
		fbDesc.m_drawBuffer = TargetBuffer::AllColorAttachments; // Tell the framebuffer to draw to ALL color buffers (not just the first one).
		FramebufferHandle fbHandle = renderer.MutGraphicsDevice().CreateFramebuffer(fbDesc);

		/* Create gaussian blur ping pong framebuffers */
		Texture2DHandle pingpongColorTex1 = renderer.MutGraphicsDevice().CreateTexture2D(colorAttachDesc);
		FramebufferDescriptor pingpongDesc1{ {pingpongColorTex1} };
		FramebufferHandle pingpongFb1 = renderer.MutGraphicsDevice().CreateFramebuffer(pingpongDesc1);

		Texture2DHandle pingpongColorTex2 = renderer.MutGraphicsDevice().CreateTexture2D(colorAttachDesc);
		FramebufferDescriptor pingpongDesc2{ {pingpongColorTex2} };
		FramebufferHandle pingpongFb2 = renderer.MutGraphicsDevice().CreateFramebuffer(pingpongDesc2);

		Texture2DHandle blurColorAttachments[2]{ pingpongColorTex1, pingpongColorTex2 };
		FramebufferHandle blurFramebuffers[2]{ pingpongFb1, pingpongFb2 };

		SamplerDescriptor blurSamplerDesc;
		blurSamplerDesc.m_magFilter = SamplerFilter::Linear;
		blurSamplerDesc.m_minFilter = SamplerFilter::Linear;
		// we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		blurSamplerDesc.m_wrap_S = SamplerWrapping::ClampToEdge;
		blurSamplerDesc.m_wrap_T = SamplerWrapping::ClampToEdge;

		SamplerHandle blurSampler = MutRenderer().MutGraphicsDevice().CreateSampler(blurSamplerDesc);

		/* Create blur quad material */
		IGraphicsRenderer::ShaderFileList bloomBlurFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/bloom_blur.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/bloom_blur.frag" }
		};

		ShaderProgramHandle bloomBlurProgram = renderer.CreateShaderProgramFromSourceFiles(bloomBlurFileList);

		MaterialDescriptor blurMatDesc({
			{"Frame_TwoPassGaussianBlur", ShaderStage::Fragment},
			{"Material_Sampler", ShaderStage::Fragment},
			{"Material_DiffuseMap", ShaderStage::Fragment}
			});

		MaterialInterface blurMatIntf = lib.CreateMaterialInterface(bloomBlurProgram, blurMatDesc);
		MaterialInstance blurMatInst = lib.CreateMaterialInstance(blurMatIntf);

		TwoPassGaussianBlurParams gaussianBlurParams{ true, (int)GetWindowWidth(), (int)GetWindowHeight(), 5, {0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f} };
		blurMatInst.UpdateUniformBlock(MaterialBlockBinding::FRAME_GAUSSIAN_BLUR,
			gaussianBlurParams);

		blurMatInst.BindSampler(SAMPLER_0, blurSampler);
		blurMatInst.BindTexture(MaterialTextureBinding::DIFFUSE, brightnessTexture);

		blurMatInst.CreateMaterialResourceSet();

		const int gaussianBlurAmount = 10;

		/* End blur material */

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


		/* Create cube VAO */
		VertexLayoutDescriptor quadLayout{
			{
				{{"position", VertexElementFormat::Float3},
				{"texCoords", VertexElementFormat::Float2}},
			},
			LayoutType::Interleaved
		};

		auto quadVao = renderer.CreateVertexLayout(quadLayout);


		/* Create cube shader */
		IGraphicsRenderer::ShaderFileList bloomLightingFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/bloom_blinn_phong.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/bloom_blinn_phong.frag" }
		};

		ShaderProgramHandle hdrLightingProgram = renderer.CreateShaderProgramFromSourceFiles(bloomLightingFileList);

		IGraphicsRenderer::ShaderFileList lightBoxFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/bloom_blinn_phong.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/bloom_light_box.frag" }
		};

		ShaderProgramHandle lightBoxProgram = renderer.CreateShaderProgramFromSourceFiles(lightBoxFileList);

		IGraphicsRenderer::ShaderFileList bloomFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/bloom_final.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/bloom_final.frag" }
		};

		ShaderProgramHandle bloomProgram = renderer.CreateShaderProgramFromSourceFiles(bloomFileList);

		RenderWorld& renderWorld = MutRenderer().CreateRenderWorld();

		// Create geometry
		auto cubeGeom = CreateCubePositionNormalTexture(1.f, false);


		Mesh* cube = renderWorld.CreateStaticMesh(cubeGeom);
		cube;

		Array<VertexPositionTexture, 6> quadVertices{ // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
			// positions   // texCoords
			{{-1.0f,  1.0f, 0.f},  {0.0f, 1.0f}},
			{{-1.0f, -1.0f, 0.f},  {0.0f, 0.0f}},
			{{ 1.0f, -1.0f, 0.f},  {1.0f, 0.0f}},

			{{-1.0f,  1.0f, 0.f},  {0.0f, 1.0f}},
			{{ 1.0f, -1.0f, 0.f},  {1.0f, 0.0f}},
			{{ 1.0f,  1.0f, 0.f},  {1.0f, 1.0f}}
		};


		Mesh* plane = renderWorld.CreateStaticMesh(quadVertices);

		/* Create Phong material buffer */
		MaterialDescriptor materialdesc(
			{
				{"Material_Phong", ShaderStage::Fragment},
				{"Material_Sampler", ShaderStage::Fragment},
				{"Material_DiffuseMap", ShaderStage::Fragment}
			}
		);
		MaterialInterface hdrInterface = lib.CreateMaterialInterface(hdrLightingProgram, materialdesc);
		MaterialInstance planeInst = lib.CreateMaterialInstance(hdrInterface);
		planeInst.UpdateUniformBlock(MaterialBlockBinding::MATERIAL_PHONG,
			PhongMaterial{ ColorRGBAf::White().ToVec(),
							ColorRGBAf::White().ToVec(),
							ColorRGBAf::White().ToVec(),
							32 });


		SamplerDescriptor mySamplerDesc;
		mySamplerDesc.m_magFilter = SamplerFilter::Linear;
		mySamplerDesc.m_minFilter = SamplerFilter::LinearMipmapLinear;
		mySamplerDesc.m_wrap_S = SamplerWrapping::Repeat;
		mySamplerDesc.m_wrap_T = SamplerWrapping::Repeat;

		SamplerHandle mySampler = MutRenderer().MutGraphicsDevice().CreateSampler(mySamplerDesc);

		planeInst.BindSampler(MaterialSamplerBinding::SAMPLER_0, mySampler);

		Texture2DFileDescriptor woodDesc{ "Sandbox/assets/textures/wood.png", TextureFormat::SRGB_RGBA8 };
		woodDesc.m_wantedMipmapLevels = 8;
		Texture2DHandle brickImg = MutRenderer().MutGraphicsDevice().CreateTexture2D(woodDesc);
		planeInst.BindTexture(MaterialTextureBinding::DIFFUSE, brickImg);

		planeInst.CreateMaterialResourceSet();

		MaterialInstance boxInst = lib.CreateMaterialInstance(hdrInterface);

		boxInst.UpdateUniformBlock(MaterialBlockBinding::MATERIAL_PHONG,
			PhongMaterial{ ColorRGBAf::White().ToVec(),
							ColorRGBAf::White().ToVec(),
							ColorRGBAf::White().ToVec(),
							32 });

		boxInst.BindSampler(MaterialSamplerBinding::SAMPLER_0, mySampler);

		Texture2DFileDescriptor containerDesc{ "Sandbox/assets/textures/container2.png", TextureFormat::SRGB_RGBA8 };
		woodDesc.m_wantedMipmapLevels = 8;
		Texture2DHandle containerImg = MutRenderer().MutGraphicsDevice().CreateTexture2D(containerDesc);
		boxInst.BindTexture(MaterialTextureBinding::DIFFUSE, containerImg);

		boxInst.CreateMaterialResourceSet();
		/* End Phong material buffer */

		/* Create full screen quad material */
		MaterialDescriptor framebufferMatDesc({
			{"Material_DiffuseMap", ShaderStage::Fragment},
			{"Material_BloomMap", ShaderStage::Fragment},
			{"Frame_ToneMappingParams", ShaderStage::Fragment}
			});

		MaterialInterface fbMatIntf = lib.CreateMaterialInterface(bloomProgram, framebufferMatDesc);
		MaterialInstance fbMatInst = lib.CreateMaterialInstance(fbMatIntf);

		fbMatInst.UpdateUniformBlock(MaterialBlockBinding::FRAME_TONE_MAPPING, m_toneMappingParams);

		fbMatInst.BindTexture(MaterialTextureBinding::DIFFUSE, colorAttachTex);

		// Attach to the last bloom pingpong FBO color attachment that will have been drawn
		// To know which FBO to bind with is simple: even number of gaussian blurs = first FBO.
		fbMatInst.BindTexture(MaterialTextureBinding::BLOOM, blurColorAttachments[gaussianBlurAmount % 2]);

		fbMatInst.CreateMaterialResourceSet();

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

		srand(13);

		Vec4 lightPositions[MAX_LIGHTS];
		for (Vec4& lightPos : lightPositions)
		{
			// calculate slightly random offsets
			float xPos = ((rand() % 100) / 100.f) * 6.0f - 3.f;
			float yPos = ((rand() % 100) / 100.f) * 6.0f - 4.f;
			float zPos = ((rand() % 100) / 100.f) * 6.0f - 3.f;
			lightPos = {xPos, yPos, zPos, 1.f};
		}

		Vec4 lightColors[MAX_LIGHTS];
		for (Vec4& lightClr : lightColors)
		{
			// also calculate random color
			float rColor = ((rand() % 100) / 200.0f) + 0.5f; // between 0.5 and 1.0
			float gColor = ((rand() % 100) / 200.0f) + 0.5f; // between 0.5 and 1.0
			float bColor = ((rand() % 100) / 200.0f) + 0.5f; // between 0.5 and 1.0
			lightClr = { rColor, gColor, bColor, 1.f };
		}

		int iLight = 0;
		for (const Vec4& lightPos : lightPositions)
		{
			auto pointLight =
				lightsSystem.AddNewLight({ lightPos, Vec4::ZeroVector(),
					Vec4(0.f), lightColors[iLight], lightColors[iLight] });
			pointLight->SetAttenuationFactors(1.f, 0.7f, 1.8f);
			iLight++;
		}


		MaterialDescriptor lightMatDesc(
			{
				{"Material_Color", ShaderStage::Fragment} }
		);

		MaterialInterface lightMatInterface = lib.CreateMaterialInterface(lightBoxProgram, lightMatDesc);

		MaterialInstance lightMatInstance = lib.CreateMaterialInstance(lightMatInterface);

		lightMatInstance.UpdateUniformBlock(MaterialBlockBinding::MATERIAL_COLOR, ColorRGBAf::White());

		lightMatInstance.CreateMaterialResourceSet();


		m_renderer.MutGraphicsDevice().SetPipeline(myPipe);



		PipelineDescriptor fullscreenQuadPipeDesc;
		fullscreenQuadPipeDesc.m_depthStencilStateDesc = DepthStencilStateDescriptor{ DepthTest::Enabled, DepthWriting::Enabled, DepthStencilComparisonFunc::Less };
		PipelineHandle fsqPipe = m_renderer.MutGraphicsDevice().CreatePipeline(fullscreenQuadPipeDesc);

		ModelDescriptor modelDesc;
		modelDesc.m_modelFilename = "Sandbox/assets/objects/backpack/backpack.obj";
		modelDesc.m_shaderProgram = hdrLightingProgram;

		Model testModel(renderWorld, lib, modelDesc);

		Vec3 modelPositions[] = {
			{-3.0,  -0.5, -3.0 },
			{ 0.0,  -0.5, -3.0 },
			{ 3.0,  -0.5, -3.0 },
			{- 3.0,  -0.5,  0.0},
			{ 0.0,  -0.5,  0.0 },
			{ 3.0,  -0.5,  0.0 },
			{- 3.0,  -0.5,  3.0},
			{ 0.0,  -0.5,  3.0 },
			{ 3.0,  -0.5,  3.0 }
		};


		IGraphicsRenderer::ShaderFileList deferredLightingPassDesc =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/deferred_lighting_pass.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/deferred_lighting_pass.frag" }
		};

		ShaderProgramHandle deferredLightingPassProgram = renderer.CreateShaderProgramFromSourceFiles(deferredLightingPassDesc);

		// Create the material used to render the full screen quad of the lighting pass
		// (we piggyback on diffuse map slot for position texture, normal map for normals texture, and specular map for albedo+specular
		MaterialDescriptor lightingPassMatDesc(
			{
				{"Material_DiffuseMap", ShaderStage::Fragment},
				{"Material_NormalMap", ShaderStage::Fragment},
				{"Material_SpecularMap", ShaderStage::Fragment},
				{"Frame_ToneMappingParams", ShaderStage::Fragment}
			}
		);



		MaterialInterface lightingPassMatInterface = lib.CreateMaterialInterface(deferredLightingPassProgram, lightingPassMatDesc);

		MaterialInstance lightingPassMatInstance = lib.CreateMaterialInstance(lightingPassMatInterface);

		lightingPassMatInstance.BindTexture(DIFFUSE, positionTexture);
		lightingPassMatInstance.BindTexture(NORMAL, normalsTexture);
		lightingPassMatInstance.BindTexture(SPECULAR, albedoSpecularTexture);

		lightingPassMatInstance.UpdateUniformBlock(MaterialBlockBinding::FRAME_TONE_MAPPING, m_toneMappingParams);

		lightingPassMatInstance.CreateMaterialResourceSet();

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

			// First, render tunnel in framebuffer

			m_renderer.MutGraphicsDevice().SetPipeline(myPipe);

			// Bind the framebuffer to effectively render-to-texture
			renderer.BindFramebuffer(gBufferHandle);

			renderer.Clear(ColorRGBAf(0.1f, 0.1f, 0.1f, 1.0f));

			lightsSystem.UpdateLights();

			lightsSystem.BindLightBuffer();

			camSys.UpdateCameras();

			for (uint32_t iCam = 0; iCam < camSys.CamerasNumber(); iCam++)
			{
				camSys.BindCameraBuffer(iCam);

				for (auto& pos : modelPositions)
				{
					for (Mesh* modelMesh : testModel)
					{

						// TODO: think of a better alternative :
						// here, if we have 1000 subobjects, it means 1000 uniform buffer updates
						// whereas it's the same transform for all meshes ?! Crazy...
						renderer.UseMaterialInstance(&modelMesh->GetBoundMaterial());

						renderer.UseShaderProgram(deferredGBufferProgram);

						modelMesh->SetTransform(Transform::Identity());
						modelMesh->AddTransform(Transform::Translate(pos));
						modelMesh->AddTransform(Transform::Scale(Vec3{0.5f}));
						modelMesh->UpdateObjectMatrices(camSys.GetCamera(iCam));
						renderWorld.DrawMesh(modelMesh, cubeVao, nullptr);
					}
				}

			}

			renderer.UnbindFramebuffer(gBufferHandle);

			// Step two - lighting pass using a fullscreen quad


			renderer.Clear(ColorRGBAf(0.1f, 0.1f, 0.1f, 1.0f));

			renderer.MutGraphicsDevice().SetPipeline(fsqPipe);

			renderer.UseMaterialInstance(&lightingPassMatInstance);

			renderWorld.DrawMesh(plane, quadVao, nullptr);


			// Step 3 - draw lights using forward rendering after copying the depth buffer of the G-buffer into the default framebuffer one
			const Rect2Di blittedArea{0, 0, (int)GetWindowWidth(), (int)GetWindowHeight()};
			MutRenderer().MutGraphicsDevice().BlitFramebuffer(gBufferHandle, FramebufferHandle::Null(), TargetBuffer::Depth, blittedArea, blittedArea, SamplerFilter::Nearest);

			iLight = 0;
			for (auto& lightPos : lightPositions)
			{
				lightMatInstance.UpdateUniformBlock(MaterialBlockBinding::MATERIAL_COLOR, lightColors[iLight]);
				renderer.UseMaterialInstance(&lightMatInstance);

				cube->SetTransform(Transform::Identity());
				cube->AddTransform(Transform::Translate(lightPos.xyz()));
				cube->AddTransform(Transform::Scale(Vec3(0.125f)));
				cube->UpdateObjectMatrices(camSys.GetCamera(0));
				renderWorld.DrawMesh(cube, cubeVao, nullptr);

				iLight++;
			}

			SwapBuffers();
		}
	}


	float Lerp(float a, float b, float f)
	{
		return a + f * (b - a);
	}


	struct SSAOParams
	{
		static const int MAX_SSAO_KERNEL_SIZE = 64;
		float	m_radius{0.5f};
		float	m_bias{0.025f};

		Vec2	m_noiseScale{1.f};

		int		m_kernelSize{ MAX_SSAO_KERNEL_SIZE };

		unsigned char padding[12];
		// Storing vec4's for std140 alignment reasons but really we only use the vec3 part
		Vec4	m_kernel[MAX_SSAO_KERNEL_SIZE];
	};


	void	TestApplication::TestSSAO()
	{
		constexpr auto ssaosize = sizeof(SSAOParams);

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
		lib.AddBindingMapping("Frame_ToneMappingParams", { MaterialBlockBinding::FRAME_TONE_MAPPING, ResourceKind::UniformBuffer });
		lib.AddBindingMapping("Frame_TwoPassGaussianBlur", { MaterialBlockBinding::FRAME_GAUSSIAN_BLUR, ResourceKind::UniformBuffer });
		lib.AddBindingMapping("Frame_SSAOParams", { MaterialBlockBinding::FRAME_SSAO_PARAMS, ResourceKind::UniformBuffer });

		lib.AddBindingMapping("Material_DiffuseMap", { MaterialTextureBinding::DIFFUSE, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_NormalMap", { MaterialTextureBinding::NORMAL, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_SpecularMap", { MaterialTextureBinding::SPECULAR, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_EmissionMap", { MaterialTextureBinding::EMISSION, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_SkyboxMap", { MaterialTextureBinding::SKYBOX, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_HeightMap", { MaterialTextureBinding::HEIGHT, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_BloomMap", { MaterialTextureBinding::BLOOM, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_AmbientOcclusion", { MaterialTextureBinding::AMBIENT_OCCLUSION, ResourceKind::TextureReadOnly });

		lib.AddBindingMapping("Material_Sampler", { MaterialSamplerBinding::SAMPLER_0, ResourceKind::Sampler });

		lib.AddBindingMapping("Material_Sampler2", { MaterialSamplerBinding::SAMPLER_1, ResourceKind::Sampler });

		lib.AddUniformBufferSizer(MaterialBlockBinding::FRAME_LIGHTS, []() { return sizeof(LightCastersData); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::VIEW_CAMERA, []() { return sizeof(CameraMatrices); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::MATERIAL_SKYBOX_VIEWPROJ, []() { return sizeof(Mat4); });

		lib.AddUniformBufferSizer(MaterialBlockBinding::MATERIAL_PHONG, []() { return sizeof(PhongMaterial); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::MATERIAL_COLOR, []() { return sizeof(ColorRGBAf); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::OBJECT_MATRICES, []() { return sizeof(ObjectMatrices); });

		lib.AddUniformBufferSizer(MaterialBlockBinding::FRAME_TONE_MAPPING, []() { return sizeof(ToneMappingParams); });

		lib.AddUniformBufferSizer(MaterialBlockBinding::FRAME_GAUSSIAN_BLUR, []() { return sizeof(TwoPassGaussianBlurParams); });

		lib.AddUniformBufferSizer(MaterialBlockBinding::FRAME_SSAO_PARAMS, []() { return sizeof(SSAOParams); });


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


		// Create our G-buffer using position, normal, albedo + specular color buffers
		// (store specular as a component of albedo buffer as an optimization)
		Texture2DDescriptor hiPrecisionColorAttachDesc{ nullptr, Width_t(GetWindowWidth()), Height_t(GetWindowHeight()), TextureFormat::RGBA16F, TextureUsage{Sampled} };
		Texture2DDescriptor normalPrecisionColorAttachDesc{ nullptr, Width_t(GetWindowWidth()), Height_t(GetWindowHeight()), TextureFormat::RGBA8, TextureUsage{Sampled} };
		Texture2DDescriptor depthAttachDesc{ nullptr, Width_t(GetWindowWidth()), Height_t(GetWindowHeight()), TextureFormat::Depth24_Stencil8, TextureUsage{RenderTarget} };

		Texture2DHandle positionTexture = renderer.MutGraphicsDevice().CreateTexture2D(hiPrecisionColorAttachDesc);
		Texture2DHandle normalsTexture = renderer.MutGraphicsDevice().CreateTexture2D(hiPrecisionColorAttachDesc);
		Texture2DHandle albedoSpecularTexture = renderer.MutGraphicsDevice().CreateTexture2D(normalPrecisionColorAttachDesc);
		Texture2DHandle gBufDepthAttachTex = renderer.MutGraphicsDevice().CreateTexture2D(depthAttachDesc);

		FramebufferDescriptor gBufferDesc{ {positionTexture, normalsTexture, albedoSpecularTexture}, gBufDepthAttachTex };
		gBufferDesc.m_drawBuffer = TargetBuffer::AllColorAttachments; // Tell the framebuffer to draw to ALL color buffers (not just the first one) to enable MRT rendering.
		FramebufferHandle gBufferHandle = renderer.MutGraphicsDevice().CreateFramebuffer(gBufferDesc);

		/* SSAO stuff */
		IGraphicsRenderer::ShaderFileList ssaoGeomFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/ssao_geometry.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/ssao_geometry.frag" }
		};

		ShaderProgramHandle ssaoGeometryProgram = renderer.CreateShaderProgramFromSourceFiles(ssaoGeomFileList);

		IGraphicsRenderer::ShaderFileList ssaoFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/ssao.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/ssao.frag" }
		};

		ShaderProgramHandle ssaoProgram = renderer.CreateShaderProgramFromSourceFiles(ssaoFileList);

		IGraphicsRenderer::ShaderFileList ssaoBlurFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/ssao_blur.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/ssao_blur.frag" }
		};

		ShaderProgramHandle ssaoBlurProgram = renderer.CreateShaderProgramFromSourceFiles(ssaoBlurFileList);

		IGraphicsRenderer::ShaderFileList ssaoFinalPassFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/ssao_lighting_pass.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/ssao_lighting_pass.frag" }
		};

		ShaderProgramHandle ssaoFinalPassProgram = renderer.CreateShaderProgramFromSourceFiles(ssaoFinalPassFileList);


		// Creating a small SSAO kernel of m_kernel in tangent space.
		// we want to generate m_kernel that form a hemisphere, otherwise half of the kernel m_kernel end up being in the surrounding geometry.
		const auto ssaoKernelSize = 64;
		const float ssaoInterpolationNearVal = 0.1f;
		const auto ssaoNoiseTextureWidth = 4;
		const auto ssaoNoiseTextureHeight = 4;

		SSAOParams ssaoParms;
		ssaoParms.m_kernelSize = ssaoKernelSize;

		std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // random floats between [0.0, 1.0]
		std::default_random_engine generator;

		const float invKernelSize = 1.f / ssaoKernelSize;

		for (unsigned int iSample = 0; iSample < ssaoKernelSize; ++iSample)
		{
			float randomFloat1 = randomFloats(generator);
			float randomFloat2 = randomFloats(generator);
			float randomFloat3 = randomFloats(generator);

			// Handedness warning : Careful as this assumes "positive Z" is "up"!
			ssaoParms.m_kernel[iSample].x() = randomFloat1 * 2.0f - 1.0f;
			ssaoParms.m_kernel[iSample].y() = randomFloat2 * 2.0f - 1.0f;
			ssaoParms.m_kernel[iSample].z() = randomFloat3; // (if we varied the z direction between -1.0 and 1.0 as well, we'd have a sphere sample kernel)
			ssaoParms.m_kernel[iSample].w() = 0.f;

			ssaoParms.m_kernel[iSample].Normalize();
			ssaoParms.m_kernel[iSample] *= randomFloats(generator);

			// We'd rather place a larger weight on occlusions close to the actual fragment. We want to distribute more kernel m_kernel closer to the origin.
			// We can do this with an accelerating interpolation function: This gives us a kernel distribution that places most m_kernel closer to its origin.
			float scale = iSample * invKernelSize;
			scale = Lerp(ssaoInterpolationNearVal, 1.f, scale * scale);

			ssaoParms.m_kernel[iSample] *= scale;

			MOE_LOG("Kernel sample %i : %.3f %.3f %.3f", iSample, ssaoParms.m_kernel[iSample].x(), ssaoParms.m_kernel[iSample].y(), ssaoParms.m_kernel[iSample].z());
		}

		ssaoParms.m_noiseScale = Vec2{GetWindowWidth() / (float)ssaoNoiseTextureWidth, GetWindowHeight() / (float)ssaoNoiseTextureHeight };

		// Then, we introduce some semi-random rotation / noise to our sample kernel in order to reduce the amount of necessary m_kernel.
		// Use a small 4x4 texture tiled over the screen to store the rotation vectors. z = 0 so we rotate around the tangent-space surface normal.
		const auto ssaoRandomVectorsNumber = ssaoNoiseTextureWidth * ssaoNoiseTextureHeight;

		Vector<Vec3> ssaoNoise;
		ssaoNoise.Reserve(ssaoRandomVectorsNumber);

		for (unsigned int iVec = 0; iVec < ssaoRandomVectorsNumber; iVec++)
		{
			const Vec3 noise(
				randomFloats(generator) * 2.0f - 1.0f,
				randomFloats(generator) * 2.0f - 1.0f,
				0.0f);

			ssaoNoise.PushBack(noise);
		}

		Texture2DDescriptor ssaoNoiseTexDesc{ (char*)ssaoNoise.Data(), Width_t(ssaoNoiseTextureWidth), Height_t(ssaoNoiseTextureHeight), TextureFormat::RGBA32F, TextureUsage{Sampled}, 1, TextureFormat::RGB32F };
		Texture2DHandle ssaoNoiseTex = renderer.MutGraphicsDevice().CreateTexture2D(ssaoNoiseTexDesc);

		SamplerDescriptor ssaoTexSamplerDesc = { SamplerFilter::Nearest, SamplerFilter::Nearest, 1.f, SamplerWrapping::Repeat, SamplerWrapping::Repeat };
		SamplerHandle ssaoTexSampler = renderer.MutGraphicsDevice().CreateSampler(ssaoTexSamplerDesc);

		SamplerDescriptor ssaoPositionTexSamplerDesc = { SamplerFilter::Nearest, SamplerFilter::Nearest, 1.f, SamplerWrapping::ClampToEdge, SamplerWrapping::ClampToEdge };
		SamplerHandle ssaoPositionTexSampler = renderer.MutGraphicsDevice().CreateSampler(ssaoPositionTexSamplerDesc);


		Texture2DDescriptor ssaoColorTexDesc{ nullptr, Width_t(GetWindowWidth()), Height_t(GetWindowHeight()), TextureFormat::R8, TextureUsage{Sampled}, 1, TextureFormat::R8 };
		Texture2DHandle  ssaoColorTex = renderer.MutGraphicsDevice().CreateTexture2D(ssaoColorTexDesc);
		FramebufferDescriptor ssaoFramebufferDesc{ {ssaoColorTex} };
		FramebufferHandle ssaoFbHandle = renderer.MutGraphicsDevice().CreateFramebuffer(ssaoFramebufferDesc);

		Texture2DHandle  ssaoBlurColorTex = renderer.MutGraphicsDevice().CreateTexture2D(ssaoColorTexDesc);
		FramebufferDescriptor ssaoFramebufferDesc2{ {ssaoBlurColorTex} };
		FramebufferHandle ssaoBlurFbHandle = renderer.MutGraphicsDevice().CreateFramebuffer(ssaoFramebufferDesc2);

		/* SSAO stuff END */

		// Create geometry
		RenderWorld& renderWorld = MutRenderer().CreateRenderWorld();


		ModelDescriptor modelDesc;
		modelDesc.m_modelFilename = "Sandbox/assets/objects/backpack/backpack.obj";
		modelDesc.m_shaderProgram = ssaoGeometryProgram;

		Model testModel(renderWorld, lib, modelDesc);


		MaterialDescriptor ssaoGeomMatDesc;
		MaterialInterface ssaoGeomMatIntf = lib.CreateMaterialInterface(ssaoGeometryProgram, ssaoGeomMatDesc);
		MaterialInstance ssaoGeomMatInst = lib.CreateMaterialInstance(ssaoGeomMatIntf);
		ssaoGeomMatInst.CreateMaterialResourceSet();

		MaterialDescriptor ssaoBlurMatDesc{
			{ "Material_Sampler", ShaderStage::Fragment},
			{ "Material_DiffuseMap", ShaderStage::Fragment}
		};
		MaterialInterface ssaoBlurMatIntf = lib.CreateMaterialInterface(ssaoBlurProgram, ssaoBlurMatDesc);
		MaterialInstance ssaoBlurMatInst = lib.CreateMaterialInstance(ssaoBlurMatIntf);

		ssaoBlurMatInst.BindSampler(SAMPLER_0, ssaoTexSampler);
		ssaoBlurMatInst.BindTexture(MaterialTextureBinding::DIFFUSE, ssaoColorTex);
		ssaoBlurMatInst.CreateMaterialResourceSet();

		PipelineDescriptor pipeDesc;
		pipeDesc.m_depthStencilStateDesc = DepthStencilStateDescriptor{ DepthTest::Enabled, DepthWriting::Enabled, DepthStencilComparisonFunc::Less };
		PipelineHandle myPipe = m_renderer.MutGraphicsDevice().CreatePipeline(pipeDesc);


		PipelineDescriptor cubeDesc;
		cubeDesc.m_depthStencilStateDesc = DepthStencilStateDescriptor{ DepthTest::Enabled, DepthWriting::Enabled, DepthStencilComparisonFunc::Less };
		cubeDesc.m_rasterizerStateDesc.m_cullMode = CullFace::Front;
		PipelineHandle cubePipe = m_renderer.MutGraphicsDevice().CreatePipeline(cubeDesc);


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


		/* Create cube VAO */
		VertexLayoutDescriptor quadLayout{
			{
				{{"position", VertexElementFormat::Float3},
				{"texCoords", VertexElementFormat::Float2}},
			},
			LayoutType::Interleaved
		};

		auto quadVao = renderer.CreateVertexLayout(quadLayout);

		// Create geometry

		Array<VertexPositionTexture, 6> quadVertices{ // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
			// positions   // texCoords
			{{-1.0f,  1.0f, 0.f},  {0.0f, 1.0f}},
			{{-1.0f, -1.0f, 0.f},  {0.0f, 0.0f}},
			{{ 1.0f, -1.0f, 0.f},  {1.0f, 0.0f}},

			{{-1.0f,  1.0f, 0.f},  {0.0f, 1.0f}},
			{{ 1.0f, -1.0f, 0.f},  {1.0f, 0.0f}},
			{{ 1.0f,  1.0f, 0.f},  {1.0f, 1.0f}}
		};


		Mesh* plane = renderWorld.CreateStaticMesh(quadVertices);

		auto cubeGeom = CreateCubePositionNormalTexture(1.f, true);
		Mesh* cube = renderWorld.CreateStaticMesh(cubeGeom);

		/* Create full screen quad materials */

		MaterialDescriptor ssaoQuadDesc(
			{
				{"Material_Sampler", ShaderStage::Fragment},
				{"Material_DiffuseMap", ShaderStage::Fragment},
				{"Material_Sampler2", ShaderStage::Fragment},
				{"Material_NormalMap", ShaderStage::Fragment},
				{"Material_SpecularMap", ShaderStage::Fragment},
				{"Frame_SSAOParams", ShaderStage::Fragment}
			}
		);

		MaterialInterface ssaoQuadInterface = lib.CreateMaterialInterface(ssaoProgram, ssaoQuadDesc);

		MaterialInstance ssaoQuadInstance = lib.CreateMaterialInstance(ssaoQuadInterface);

		ssaoQuadInstance.UpdateUniformBlock(MaterialBlockBinding::FRAME_SSAO_PARAMS, ssaoParms);

		ssaoQuadInstance.BindSampler(SAMPLER_0, ssaoPositionTexSampler);
		ssaoQuadInstance.BindTexture(MaterialTextureBinding::DIFFUSE, positionTexture);
		ssaoQuadInstance.BindSampler(SAMPLER_1, ssaoTexSampler);
		ssaoQuadInstance.BindTexture(MaterialTextureBinding::NORMAL, normalsTexture);
		ssaoQuadInstance.BindTexture(MaterialTextureBinding::SPECULAR, ssaoNoiseTex);

		ssaoQuadInstance.CreateMaterialResourceSet();

		MaterialDescriptor ssaoFinalPassDesc(
			{
				{"Material_Sampler", ShaderStage::Fragment},
				{"Material_DiffuseMap", ShaderStage::Fragment},
				{"Material_Sampler2", ShaderStage::Fragment},
				{"Material_NormalMap", ShaderStage::Fragment},
				{"Material_SpecularMap", ShaderStage::Fragment},
				{"Material_AmbientOcclusion", ShaderStage::Fragment},
				{"Frame_ToneMappingParams", ShaderStage::Fragment}
			}
		);

		MaterialInterface ssaoFinalPassIntf = lib.CreateMaterialInterface(ssaoFinalPassProgram, ssaoFinalPassDesc);

		MaterialInstance ssaoFinalPassInst = lib.CreateMaterialInstance(ssaoFinalPassIntf);

		ssaoFinalPassInst.UpdateUniformBlock(MaterialBlockBinding::FRAME_TONE_MAPPING, m_toneMappingParams);

		ssaoFinalPassInst.BindSampler(SAMPLER_0, ssaoPositionTexSampler);
		ssaoFinalPassInst.BindTexture(MaterialTextureBinding::DIFFUSE, positionTexture);

		ssaoFinalPassInst.BindSampler(SAMPLER_1, ssaoTexSampler);
		ssaoFinalPassInst.BindTexture(MaterialTextureBinding::NORMAL, normalsTexture);
		ssaoFinalPassInst.BindTexture(MaterialTextureBinding::SPECULAR, albedoSpecularTexture);
		ssaoFinalPassInst.BindTexture(MaterialTextureBinding::AMBIENT_OCCLUSION, ssaoBlurColorTex);

		ssaoFinalPassInst.CreateMaterialResourceSet();


		/* Create camera */
		PerspectiveCameraDesc persDesc{ 45_degf, GetWindowWidth() / (float)GetWindowHeight(), 0.1f, 100.f };

		CameraSystem camSys(renderer.MutGraphicsDevice());

		ViewportHandle vpHandle = m_renderer.MutGraphicsDevice().CreateViewport(ViewportDescriptor(0, 0, (float)GetWindowWidth(), (float)GetWindowHeight()));

		Camera* newCam = camSys.AddNewCamera(vpHandle, persDesc);

		newCam->AddTransform(Transform::Translate(Vec3(0, 0, 5)));

		m_currentCamera = newCam;

		// To make sure the camera points towards the negative z-axis by default we can give the yaw a default value of a 90 degree clockwise rotation.

		newCam->UpdateCameraVectors(0, -90);

		/* Create camera end */


		LightSystem lightsSystem(renderer.MutGraphicsDevice());

		auto pointLight =
			lightsSystem.AddNewLight({ {2.0f, 4.0f, -2.0f, 1.f}, Vec4::ZeroVector(),
				Vec4(0.3f, 0.3f, 0.3f, 1.f), {0.2f, 0.2f, 0.7f, 1.f}, {0.2f, 0.2f, 0.7f, 1.f} });
		pointLight->SetAttenuationFactors(1.f, 0.09f, 0.032f);

		m_renderer.MutGraphicsDevice().SetPipeline(myPipe);



		PipelineDescriptor fullscreenQuadPipeDesc;
		fullscreenQuadPipeDesc.m_depthStencilStateDesc = DepthStencilStateDescriptor{ DepthTest::Disabled, DepthWriting::Enabled, DepthStencilComparisonFunc::Less };
		PipelineHandle fsqPipe = m_renderer.MutGraphicsDevice().CreatePipeline(fullscreenQuadPipeDesc);


		// Create the material used to render the full screen quad of the lighting pass
		// (we piggyback on diffuse map slot for position texture, normal map for normals texture, and specular map for albedo+specular
		MaterialDescriptor lightingPassMatDesc(
			{
				{"Material_DiffuseMap", ShaderStage::Fragment},
				{"Material_NormalMap", ShaderStage::Fragment},
				{"Material_SpecularMap", ShaderStage::Fragment},
				{"Frame_ToneMappingParams", ShaderStage::Fragment}
			}
		);



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

			// First, render tunnel in framebuffer

			m_renderer.MutGraphicsDevice().SetPipeline(myPipe);

			// Bind the framebuffer to effectively render-to-texture
			renderer.BindFramebuffer(gBufferHandle);

			renderer.Clear(ColorRGBAf(0.1f, 0.1f, 0.1f, 1.0f));

			lightsSystem.UpdateLights();

			lightsSystem.BindLightBuffer();

			camSys.UpdateCameras();

			/* Step 1 : geometry pass */
			for (uint32_t iCam = 0; iCam < camSys.CamerasNumber(); iCam++)
			{

				m_renderer.MutGraphicsDevice().SetPipeline(cubePipe);

				camSys.BindCameraBuffer(iCam);

				renderer.UseMaterialInstance(&ssaoGeomMatInst);

				// Room cube
				cube->SetTransform(Transform::Identity());
				cube->AddTransform(Transform::Translate({0, 7, 0}));
				cube->AddTransform(Transform::Scale({7.5F, 7.5F, 7.5F}));
				cube->UpdateObjectMatrices(camSys.GetCamera(iCam));

				renderWorld.DrawMesh(cube, cubeVao);


				m_renderer.MutGraphicsDevice().SetPipeline(myPipe);

				// Backpack on the floor
				for (Mesh* subMesh : testModel)
				{
					subMesh->SetTransform(Transform::Identity());
					subMesh->AddTransform(Transform::Translate({ 0, 0.5f, 0 }));
					subMesh->AddTransform(Transform::Rotate(-90_degf, Vec3{1.0f, 0.0f, 0.0f}));
					subMesh->AddTransform(Transform::Scale(Vec3{ 1.F }));
					subMesh->UpdateObjectMatrices(camSys.GetCamera(iCam));

					renderWorld.DrawMesh(subMesh, cubeVao);
				}
			}

			renderer.UnbindFramebuffer(gBufferHandle);

			m_renderer.MutGraphicsDevice().SetPipeline(fsqPipe);

			/* Step 2 : creating the occlusion texture */

			camSys.BindCameraBuffer(0); // Needed to get the camera projection matrix

			renderer.BindFramebuffer(ssaoFbHandle);

			renderer.UseMaterialInstance(&ssaoQuadInstance);

			renderWorld.DrawMesh(plane, quadVao);

			renderer.UnbindFramebuffer(ssaoFbHandle);


			/* Step 3 : blurring the occlusion texture */
			renderer.BindFramebuffer(ssaoBlurFbHandle);

			renderer.UseMaterialInstance(&ssaoBlurMatInst);

			renderWorld.DrawMesh(plane, quadVao);

			renderer.UnbindFramebuffer(ssaoBlurFbHandle);


			// Step 4 : final lighting pass using our occlusion texture


			renderer.Clear(ColorRGBAf(0.1f, 0.1f, 0.1f, 1.0f));

			renderer.UseMaterialInstance(&ssaoFinalPassInst);

			renderWorld.DrawMesh(plane, quadVao);

			SwapBuffers();
		}
	}

}
