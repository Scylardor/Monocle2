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

#include "Graphics/Texture/TextureHandle.h"


namespace moe
{
	struct PhongMaterial
	{
		Vec4	m_ambientColor{ 1.f };
		Vec4	m_diffuseColor{ 1.f };
		Vec4	m_specularColor{ 1.f };
		float	m_shininess{ 32 };
	};


	struct ProjectionPlanes
	{
		float m_near{ 0 };
		float m_far{ 0 };
	};


	struct VertexPositionTexture
	{
		Vec3	m_position;
		Vec2	m_texture;
	};


	Array<VertexPositionTexture, 36>	CreateCubePositionTexture(float halfExtent)
	{
		return {
			{Vec3{-halfExtent, -halfExtent, -halfExtent}, { 0.0f,  0.0f }},
			{Vec3{ halfExtent,  halfExtent, -halfExtent}, { 1.0f,  1.0f }},
			{Vec3{ halfExtent, -halfExtent, -halfExtent}, { 1.0f,  0.0f }},
			{Vec3{ halfExtent,  halfExtent, -halfExtent}, { 1.0f,  1.0f }},
			{Vec3{-halfExtent, -halfExtent, -halfExtent}, { 0.0f,  0.0f }},
			{Vec3{-halfExtent,  halfExtent, -halfExtent}, { 0.0f,  1.0f }},

			{Vec3{ -halfExtent, -halfExtent,  halfExtent }, { 0.0f,  0.0f }},
			{ Vec3{  halfExtent, -halfExtent,  halfExtent}, { 1.0f,  0.0f }},
			{ Vec3{  halfExtent,  halfExtent,  halfExtent}, { 1.0f,  1.0f }},
			{ Vec3{  halfExtent,  halfExtent,  halfExtent}, { 1.0f,  1.0f }},
			{ Vec3{ -halfExtent,  halfExtent,  halfExtent}, { 0.0f,  1.0f }},
			{ Vec3{ -halfExtent, -halfExtent,  halfExtent}, { 0.0f,  0.0f }},

			{Vec3{ -halfExtent,  halfExtent,  halfExtent }, { 1.0f,  1.0f }},
			{ Vec3{ -halfExtent,  halfExtent, -halfExtent}, { 0.0f,  1.0f } },
			{ Vec3{ -halfExtent, -halfExtent, -halfExtent}, { 0.0f,  0.0f } },
			{ Vec3{ -halfExtent, -halfExtent, -halfExtent}, { 0.0f,  0.0f } },
			{ Vec3{ -halfExtent, -halfExtent,  halfExtent}, { 1.0f,  0.0f } },
			{ Vec3{ -halfExtent,  halfExtent,  halfExtent}, { 1.0f,  1.0f } },

			{ Vec3{ halfExtent,  halfExtent,  halfExtent }, { 0.0f,  1.0f }},
			{ Vec3{ halfExtent, -halfExtent, -halfExtent}, { 1.0f,  0.0f }},
			{ Vec3{ halfExtent,  halfExtent, -halfExtent}, { 1.0f,  1.0f }},
			{ Vec3{ halfExtent, -halfExtent, -halfExtent}, { 1.0f,  0.0f }},
			{ Vec3{ halfExtent,  halfExtent,  halfExtent}, { 0.0f,  1.0f }},
			{ Vec3{ halfExtent, -halfExtent,  halfExtent}, { 0.0f,  0.0f }},

			{Vec3{ -halfExtent, -halfExtent, -halfExtent }, { 0.0f,  1.0f }},
			{ Vec3{  halfExtent, -halfExtent, -halfExtent}, { 1.0f,  1.0f } },
			{ Vec3{  halfExtent, -halfExtent,  halfExtent}, { 1.0f,  0.0f } },
			{ Vec3{  halfExtent, -halfExtent,  halfExtent}, { 1.0f,  0.0f } },
			{ Vec3{ -halfExtent, -halfExtent,  halfExtent}, { 0.0f,  0.0f } },
			{ Vec3{ -halfExtent, -halfExtent, -halfExtent}, { 0.0f,  1.0f } },

			{Vec3{ -halfExtent,  halfExtent, -halfExtent }, { 0.0f,  0.0f }},
			{ Vec3{  halfExtent,  halfExtent,  halfExtent},{ 1.0f,  1.0f } },
			{ Vec3{  halfExtent,  halfExtent, -halfExtent},{ 0.0f,  1.0f } },
			{ Vec3{  halfExtent,  halfExtent,  halfExtent},{ 1.0f,  1.0f } },
			{ Vec3{ -halfExtent,  halfExtent, -halfExtent},{ 0.0f,  0.0f } },
			{ Vec3{ -halfExtent,  halfExtent,  halfExtent},{ 1.0f,  0.0f }}
		};

	}



	void TestApplication::TestVisualizeDepthBuffer()
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
		lib.AddBindingMapping("Material_DiffuseMap", { MaterialTextureBinding::DIFFUSE, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_SpecularMap", { MaterialTextureBinding::SPECULAR, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_EmissionMap", { MaterialTextureBinding::EMISSION, ResourceKind::TextureReadOnly });

		lib.AddUniformBufferSizer(MaterialBlockBinding::FRAME_LIGHTS, []() { return sizeof(LightCastersData); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::VIEW_CAMERA, []() { return sizeof(CameraMatrices); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::VIEW_PROJECTION_PLANES, []() { return sizeof(ProjectionPlanes); });

		lib.AddUniformBufferSizer(MaterialBlockBinding::MATERIAL_PHONG, []() { return sizeof(PhongMaterial); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::MATERIAL_COLOR, []() { return sizeof(ColorRGBAf); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::OBJECT_MATRICES, []() { return sizeof(ObjectMatrices); });
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
		// always pass the depth test (same effect as glDisable(GL_DEPTH_TEST))
		pipeDesc.m_depthStencilStateDesc = DepthStencilStateDescriptor{ DepthTest::Enabled, DepthWriting::Enabled, DepthStencilComparisonFunc::Less };
		PipelineHandle myPipe = m_renderer.MutGraphicsDevice().CreatePipeline(pipeDesc);

		/* Create cube VAO */
		VertexLayoutDescriptor cubeLayout{
			{
				{"position", VertexElementFormat::Float3},
				{"texture", VertexElementFormat::Float2}
			},
			LayoutType::Interleaved
		};

		auto cubeVao = renderer.CreateVertexLayout(cubeLayout);


		/* Create cube shader */
		IGraphicsRenderer::ShaderFileList cubeFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/depth_testing.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/depth_testing.frag" }
		};

		ShaderProgramHandle cubeProgram = renderer.CreateShaderProgramFromSourceFiles(cubeFileList);


		// Create cube geometry

		RenderWorld& renderWorld = MutRenderer().CreateRenderWorld();

		auto cubeGeom = CreateCubePositionTexture(0.5f);
		Mesh* cube = renderWorld.CreateStaticMesh(cubeGeom);

		// Create plane geometry
		VertexPositionTexture planeVertices[] = {
			// positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
			{{ 5.0f, -0.5f,  5.0f },  {2.0f, 0.0f}},
			{{-5.0f, -0.5f, -5.0f },  {0.0f, 2.0f}},
			{{-5.0f, -0.5f,  5.0f },  {0.0f, 0.0f}},

			{{ 5.0f, -0.5f,  5.0f},  {2.0f, 0.0f}},
			{{ 5.0f, -0.5f, -5.0f},  {2.0f, 2.0f}},
			{{-5.0f, -0.5f, -5.0f},  {0.0f, 2.0f}}
		};

		Mesh* plane = renderWorld.CreateStaticMesh(planeVertices);

		/* Create materials */
		MaterialDescriptor materialDesc(
			{
				{"View_ProjectionPlanes", ShaderStage::Fragment},
				{"Material_DiffuseMap", ShaderStage::Fragment}
			}
		);

		MaterialInterface matInterface = lib.CreateMaterialInterface(cubeProgram, materialDesc);

		Texture2DHandle marbleTex = MutRenderer().MutGraphicsDevice().CreateTexture2D(Texture2DFileDescriptor{ "Sandbox/assets/marble.jpg" });

		MaterialInstance cubeInstance = lib.CreateMaterialInstance(matInterface);
		cubeInstance.BindTexture(MaterialTextureBinding::DIFFUSE, marbleTex);
		cubeInstance.CreateMaterialResourceSet();

		Texture2DHandle metalTex = MutRenderer().MutGraphicsDevice().CreateTexture2D(Texture2DFileDescriptor{ "Sandbox/assets/metal.png" });

		MaterialInstance planeInstance = lib.CreateMaterialInstance(matInterface);
		planeInstance.BindTexture(MaterialTextureBinding::DIFFUSE, metalTex);
		planeInstance.CreateMaterialResourceSet();

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

		m_renderer.MutGraphicsDevice().SetPipeline(myPipe);

		cubeInstance.UpdateUniformBlock(MaterialBlockBinding::VIEW_PROJECTION_PLANES, ProjectionPlanes{ 0.1f, 100.f });
		planeInstance.UpdateUniformBlock(MaterialBlockBinding::VIEW_PROJECTION_PLANES, ProjectionPlanes{ 0.1f, 100.f });


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

			//lightsSystem.UpdateLights();

			//lightsSystem.BindLightBuffer();

			camSys.UpdateCameras();

			for (uint32_t iCam = 0; iCam < camSys.CamerasNumber(); iCam++)
			{
				camSys.BindCameraBuffer(iCam);

				renderer.UseMaterialInstance(&cubeInstance);

				cube->SetTransform(Transform::Translate({ -1.0f, 0.0f, -1.0f }));
				cube->UpdateObjectMatrices(camSys.GetCamera(iCam));
				renderWorld.DrawMesh(cube, cubeVao, nullptr);

				cube->SetTransform(Transform::Translate({ 2.0f, 0.0f, 0.0f }));
				cube->UpdateObjectMatrices(camSys.GetCamera(iCam));
				renderWorld.DrawMesh(cube, cubeVao, nullptr);

				renderer.UseMaterialInstance(&planeInstance);

				plane->SetTransform(Transform::Identity());
				plane->UpdateObjectMatrices(camSys.GetCamera(iCam));
				renderWorld.DrawMesh(plane, cubeVao, nullptr);
			}

			SwapBuffers();

		}

	}

	void TestApplication::TestStencilBuffer()
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
		lib.AddBindingMapping("Material_DiffuseMap", { MaterialTextureBinding::DIFFUSE, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_SpecularMap", { MaterialTextureBinding::SPECULAR, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_EmissionMap", { MaterialTextureBinding::EMISSION, ResourceKind::TextureReadOnly });

		lib.AddUniformBufferSizer(MaterialBlockBinding::FRAME_LIGHTS, []() { return sizeof(LightCastersData); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::VIEW_CAMERA, []() { return sizeof(CameraMatrices); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::VIEW_PROJECTION_PLANES, []() { return sizeof(ProjectionPlanes); });

		lib.AddUniformBufferSizer(MaterialBlockBinding::MATERIAL_PHONG, []() { return sizeof(PhongMaterial); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::MATERIAL_COLOR, []() { return sizeof(ColorRGBAf); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::OBJECT_MATRICES, []() { return sizeof(ObjectMatrices); });
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
		pipeDesc.m_depthStencilStateDesc = DepthStencilStateDescriptor{ DepthTest::Enabled, DepthWriting::Enabled, DepthStencilComparisonFunc::Less, StencilTest::Enabled };
		pipeDesc.m_depthStencilStateDesc.m_frontFaceOps = { StencilOp::Keep, StencilOp::Keep, StencilOp::Replace, DepthStencilComparisonFunc::NotEqual };
		pipeDesc.m_depthStencilStateDesc.m_stencilRefVal = 1;
		pipeDesc.m_depthStencilStateDesc.m_stencilReadMask = 0xFF;
		pipeDesc.m_depthStencilStateDesc.m_stencilWriteMask = 0xFF;
		PipelineHandle defaultPipe = m_renderer.MutGraphicsDevice().CreatePipeline(pipeDesc);

		PipelineDescriptor floorPipeDesc = pipeDesc;
		floorPipeDesc.m_depthStencilStateDesc.m_stencilRefVal = 0;
		floorPipeDesc.m_depthStencilStateDesc.m_stencilWriteMask = 0x00; // don't write to stencil buffer while drawing floor
		floorPipeDesc.m_depthStencilStateDesc.m_frontFaceOps = { StencilOp::Keep, StencilOp::Keep, StencilOp::Replace, DepthStencilComparisonFunc::Always };

		PipelineHandle floorPipe = m_renderer.MutGraphicsDevice().CreatePipeline(floorPipeDesc);

		PipelineDescriptor cubeFirstPassPipeDesc = pipeDesc;
		cubeFirstPassPipeDesc.m_depthStencilStateDesc.m_frontFaceOps = { StencilOp::Keep, StencilOp::Keep, StencilOp::Replace, DepthStencilComparisonFunc::Always };
		PipelineHandle cubeFirstPassPipe = m_renderer.MutGraphicsDevice().CreatePipeline(cubeFirstPassPipeDesc);

		PipelineDescriptor cubeSecondPassPipeDesc = floorPipeDesc;
		cubeSecondPassPipeDesc.m_depthStencilStateDesc.m_depthTest = DepthTest::Disabled;
		cubeSecondPassPipeDesc.m_depthStencilStateDesc.m_frontFaceOps = { StencilOp::Keep, StencilOp::Keep, StencilOp::Replace, DepthStencilComparisonFunc::NotEqual };
		cubeSecondPassPipeDesc.m_depthStencilStateDesc.m_stencilRefVal = 1;
		PipelineHandle cubeSecondPassPipe = m_renderer.MutGraphicsDevice().CreatePipeline(cubeSecondPassPipeDesc);



		/* Create cube VAO */
		VertexLayoutDescriptor cubeLayout{
			{
				{"position", VertexElementFormat::Float3},
				{"texture", VertexElementFormat::Float2}
			},
			LayoutType::Interleaved
		};

		auto cubeVao = renderer.CreateVertexLayout(cubeLayout);


		/* Create cube shader */
		IGraphicsRenderer::ShaderFileList cubeFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/depth_testing.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/depth_testing.frag" }
		};

		ShaderProgramHandle cubeProgram = renderer.CreateShaderProgramFromSourceFiles(cubeFileList);

		/* Create stencil pass shader */
		IGraphicsRenderer::ShaderFileList stencilFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/stencil_testing.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/stencil_testing.frag" }
		};

		ShaderProgramHandle stencilProgram = renderer.CreateShaderProgramFromSourceFiles(stencilFileList);



		// Create cube geometry

		RenderWorld& renderWorld = MutRenderer().CreateRenderWorld();

		auto cubeGeom = CreateCubePositionTexture(0.5f);
		Mesh* cube = renderWorld.CreateStaticMesh(cubeGeom);

		// Create plane geometry
		VertexPositionTexture planeVertices[] = {
			// positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
			{{ 5.0f, -0.5f,  5.0f },  {2.0f, 0.0f}},
			{{-5.0f, -0.5f, -5.0f },  {0.0f, 2.0f}},
			{{-5.0f, -0.5f,  5.0f },  {0.0f, 0.0f}},

			{{ 5.0f, -0.5f,  5.0f},  {2.0f, 0.0f}},
			{{ 5.0f, -0.5f, -5.0f},  {2.0f, 2.0f}},
			{{-5.0f, -0.5f, -5.0f},  {0.0f, 2.0f}}
		};

		Mesh* plane = renderWorld.CreateStaticMesh(planeVertices);

		/* Create materials */
		MaterialDescriptor materialDesc(
			{
				{"View_ProjectionPlanes", ShaderStage::Fragment},
				{"Material_DiffuseMap", ShaderStage::Fragment}
			}
		);

		MaterialInterface matInterface = lib.CreateMaterialInterface(cubeProgram, materialDesc);

		Texture2DHandle marbleTex = MutRenderer().MutGraphicsDevice().CreateTexture2D(Texture2DFileDescriptor{ "Sandbox/assets/marble.jpg" });

		MaterialInstance cubeInstance = lib.CreateMaterialInstance(matInterface);
		cubeInstance.BindTexture(MaterialTextureBinding::DIFFUSE, marbleTex);
		cubeInstance.CreateMaterialResourceSet();

		Texture2DHandle metalTex = MutRenderer().MutGraphicsDevice().CreateTexture2D(Texture2DFileDescriptor{ "Sandbox/assets/metal.png" });

		MaterialInstance planeInstance = lib.CreateMaterialInstance(matInterface);
		planeInstance.BindTexture(MaterialTextureBinding::DIFFUSE, metalTex);
		planeInstance.CreateMaterialResourceSet();

		MaterialDescriptor simpleColorDesc;
		MaterialInterface simpleInterface = lib.CreateMaterialInterface(stencilProgram, simpleColorDesc);
		MaterialInstance simpleInst = lib.CreateMaterialInstance(simpleInterface);


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


		cubeInstance.UpdateUniformBlock(MaterialBlockBinding::VIEW_PROJECTION_PLANES, ProjectionPlanes{ 0.1f, 100.f });
		planeInstance.UpdateUniformBlock(MaterialBlockBinding::VIEW_PROJECTION_PLANES, ProjectionPlanes{ 0.1f, 100.f });


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

			m_renderer.MutGraphicsDevice().SetPipeline(defaultPipe);

			renderer.Clear(ColorRGBAf(0.1f, 0.1f, 0.1f, 1.0f));

			//lightsSystem.UpdateLights();

			//lightsSystem.BindLightBuffer();

			camSys.UpdateCameras();

			for (uint32_t iCam = 0; iCam < camSys.CamerasNumber(); iCam++)
			{
				camSys.BindCameraBuffer(iCam);

				// draw floor as normal, but don't write the floor to the stencil buffer, we only care about the containers.
				// We set its mask to 0x00 to not write to the stencil buffer.
				m_renderer.MutGraphicsDevice().SetPipeline(floorPipe);

				renderer.UseMaterialInstance(&planeInstance);
				plane->SetTransform(Transform::Identity());
				plane->UpdateObjectMatrices(camSys.GetCamera(iCam));
				renderWorld.DrawMesh(plane, cubeVao, nullptr);


				// 1st. render pass, draw objects as normal, writing to the stencil buffer
				m_renderer.MutGraphicsDevice().SetPipeline(cubeFirstPassPipe);

				renderer.UseMaterialInstance(&cubeInstance);

				cube->SetTransform(Transform::Translate({ -1.0f, 0.0f, -1.0f }));
				cube->UpdateObjectMatrices(camSys.GetCamera(iCam));
				renderWorld.DrawMesh(cube, cubeVao, nullptr);

				cube->SetTransform(Transform::Translate({ 2.0f, 0.0f, 0.0f }));
				cube->UpdateObjectMatrices(camSys.GetCamera(iCam));
				renderWorld.DrawMesh(cube, cubeVao, nullptr);


				// 2nd. render pass: now draw slightly scaled versions of the objects, this time disabling stencil writing.
				// The stencil buffer is now filled with several 1s: The parts of the buffer that are 1 are not drawn.
				// Thus we only draw the objects' size differences, making it look like borders.
				m_renderer.MutGraphicsDevice().SetPipeline(cubeSecondPassPipe);

				renderer.UseMaterialInstance(&simpleInst);

				float scale = 1.1f;
				cube->SetTransform(Transform::Translate({ -1.0f, 0.0f, -1.0f }));
				cube->AddTransform(Transform::Scale(Vec3{ scale }));
				cube->UpdateObjectMatrices(camSys.GetCamera(iCam));
				renderWorld.DrawMesh(cube, cubeVao, nullptr);

				cube->SetTransform(Transform::Translate({ 2.0f, 0.0f, 0.0f }));
				cube->AddTransform(Transform::Scale(Vec3{ scale }));
				cube->UpdateObjectMatrices(camSys.GetCamera(iCam));
				renderWorld.DrawMesh(cube, cubeVao, nullptr);
			}

			SwapBuffers();

		}

	}


	void TestApplication::TestBlending()
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
		lib.AddBindingMapping("Material_DiffuseMap", { MaterialTextureBinding::DIFFUSE, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_SpecularMap", { MaterialTextureBinding::SPECULAR, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_EmissionMap", { MaterialTextureBinding::EMISSION, ResourceKind::TextureReadOnly });

		lib.AddUniformBufferSizer(MaterialBlockBinding::FRAME_LIGHTS, []() { return sizeof(LightCastersData); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::VIEW_CAMERA, []() { return sizeof(CameraMatrices); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::VIEW_PROJECTION_PLANES, []() { return sizeof(ProjectionPlanes); });

		lib.AddUniformBufferSizer(MaterialBlockBinding::MATERIAL_PHONG, []() { return sizeof(PhongMaterial); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::MATERIAL_COLOR, []() { return sizeof(ColorRGBAf); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::OBJECT_MATRICES, []() { return sizeof(ObjectMatrices); });
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
		// always pass the depth test (same effect as glDisable(GL_DEPTH_TEST))
		pipeDesc.m_depthStencilStateDesc = DepthStencilStateDescriptor{ DepthTest::Enabled, DepthWriting::Enabled, DepthStencilComparisonFunc::Less };
		pipeDesc.m_blendStateDesc.m_enabled = BlendStateDescriptor::Enabled;

		PipelineHandle myPipe = m_renderer.MutGraphicsDevice().CreatePipeline(pipeDesc);

		/* Create cube VAO */
		VertexLayoutDescriptor cubeLayout{
			{
				{"position", VertexElementFormat::Float3},
				{"texture", VertexElementFormat::Float2}
			},
			LayoutType::Interleaved
		};

		auto cubeVao = renderer.CreateVertexLayout(cubeLayout);


		/* Create cube shader */
		IGraphicsRenderer::ShaderFileList cubeFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/depth_testing.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/depth_testing.frag" }
		};

		ShaderProgramHandle cubeProgram = renderer.CreateShaderProgramFromSourceFiles(cubeFileList);

		/* Create vegetation shader */
		IGraphicsRenderer::ShaderFileList vegetFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/blending.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/blending.frag" }
		};

		ShaderProgramHandle vegetProgram = renderer.CreateShaderProgramFromSourceFiles(vegetFileList);

		// Create cube geometry

		RenderWorld& renderWorld = MutRenderer().CreateRenderWorld();

		auto cubeGeom = CreateCubePositionTexture(0.5f);
		Mesh* cube = renderWorld.CreateStaticMesh(cubeGeom);

		// Create plane geometry
		VertexPositionTexture planeVertices[] = {
			// positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
			{{ 5.0f, -0.5f,  5.0f },  {2.0f, 0.0f}},
			{{-5.0f, -0.5f, -5.0f },  {0.0f, 2.0f}},
			{{-5.0f, -0.5f,  5.0f },  {0.0f, 0.0f}},

			{{ 5.0f, -0.5f,  5.0f},  {2.0f, 0.0f}},
			{{ 5.0f, -0.5f, -5.0f},  {2.0f, 2.0f}},
			{{-5.0f, -0.5f, -5.0f},  {0.0f, 2.0f}}
		};

		Mesh* plane = renderWorld.CreateStaticMesh(planeVertices);


		// Create vegetation geometry
		VertexPositionTexture vegetationVertices[] = {
			// positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
			{{0.0f,  0.5f,  0.0f},  {0.0f,  1.0f}},
			{{0.0f, -0.5f,  0.0f},  {0.0f,  0.0f}},
			{{1.0f, -0.5f,  0.0f},  {1.0f,  0.0f}},

			{{0.0f,  0.5f,  0.0f},  {0.0f,  1.0f}},
			{{1.0f, -0.5f,  0.0f},  {1.0f,  0.0f}},
			{{1.0f,  0.5f,  0.0f},  {1.0f,  1.0f}}
		};

		Mesh* vegetation = renderWorld.CreateStaticMesh(vegetationVertices);




		/* Create materials */
		MaterialDescriptor materialDesc(
			{
				{"View_ProjectionPlanes", ShaderStage::Fragment},
				{"Material_DiffuseMap", ShaderStage::Fragment}
			}
		);

		MaterialInterface matInterface = lib.CreateMaterialInterface(cubeProgram, materialDesc);

		Texture2DHandle marbleTex = MutRenderer().MutGraphicsDevice().CreateTexture2D(Texture2DFileDescriptor{ "Sandbox/assets/marble.jpg" });

		MaterialInstance cubeInstance = lib.CreateMaterialInstance(matInterface);
		cubeInstance.BindTexture(MaterialTextureBinding::DIFFUSE, marbleTex);
		cubeInstance.CreateMaterialResourceSet();

		Texture2DHandle metalTex = MutRenderer().MutGraphicsDevice().CreateTexture2D(Texture2DFileDescriptor{ "Sandbox/assets/metal.png" });

		MaterialInstance planeInstance = lib.CreateMaterialInstance(matInterface);
		planeInstance.BindTexture(MaterialTextureBinding::DIFFUSE, metalTex);
		planeInstance.CreateMaterialResourceSet();



		MaterialDescriptor vegetMatDesc(
			{
				{"Material_DiffuseMap", ShaderStage::Fragment}
			}
		);

		MaterialInterface vegetInterface = lib.CreateMaterialInterface(vegetProgram, vegetMatDesc);
		Texture2DHandle vegetTex = MutRenderer().MutGraphicsDevice().CreateTexture2D(Texture2DFileDescriptor{ "Sandbox/assets/blending_transparent_window.png" });

		MaterialInstance vegetInstance = lib.CreateMaterialInstance(vegetInterface);
		vegetInstance.BindTexture(MaterialTextureBinding::DIFFUSE, vegetTex);
		vegetInstance.CreateMaterialResourceSet();


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

		m_renderer.MutGraphicsDevice().SetPipeline(myPipe);

		cubeInstance.UpdateUniformBlock(MaterialBlockBinding::VIEW_PROJECTION_PLANES, ProjectionPlanes{ 0.1f, 100.f });
		planeInstance.UpdateUniformBlock(MaterialBlockBinding::VIEW_PROJECTION_PLANES, ProjectionPlanes{ 0.1f, 100.f });

		std::vector<Vec3> vegetPositions = {
			{-1.5f, 0.0f, -0.48f},
			{1.5f, 0.0f, 0.51f},
			{0.0f, 0.0f, 0.7f},
			{-0.3f, 0.0f, -2.3f},
			{0.5f, 0.0f, -0.6f	}
		};


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

			//lightsSystem.UpdateLights();

			//lightsSystem.BindLightBuffer();

			camSys.UpdateCameras();

			for (uint32_t iCam = 0; iCam < camSys.CamerasNumber(); iCam++)
			{
				camSys.BindCameraBuffer(iCam);

				renderer.UseMaterialInstance(&cubeInstance);

				cube->SetTransform(Transform::Translate({ -1.0f, 0.0f, -1.0f }));
				cube->UpdateObjectMatrices(camSys.GetCamera(iCam));
				renderWorld.DrawMesh(cube, cubeVao, nullptr);

				cube->SetTransform(Transform::Translate({ 2.0f, 0.0f, 0.0f }));
				cube->UpdateObjectMatrices(camSys.GetCamera(iCam));
				renderWorld.DrawMesh(cube, cubeVao, nullptr);

				renderer.UseMaterialInstance(&planeInstance);

				plane->SetTransform(Transform::Identity());
				plane->UpdateObjectMatrices(camSys.GetCamera(iCam));
				renderWorld.DrawMesh(plane, cubeVao, nullptr);

				renderer.UseMaterialInstance(&vegetInstance);

				// TODO : need to manage samplers in resource sets in order to do :
				// glTextureParameteri(texHandle.Get(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				// glTextureParameteri(texHandle.Get(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

				const Vec3 camPosition = newCam->GetTransform().Matrix().GetTranslation();

				// Sort transparent objects back to front relative to the camera
				std::sort(vegetPositions.begin(), vegetPositions.end(),
					[&camPosition](const Vec3& winPos1, const Vec3& winPos2) {
					return winPos1.Distance(camPosition) >= winPos2.Distance(camPosition);
				});

				for (auto& vegetPos : vegetPositions)
				{
					vegetation->SetTransform(Transform::Translate(vegetPos));
					vegetation->UpdateObjectMatrices(camSys.GetCamera(iCam));
					renderWorld.DrawMesh(vegetation, cubeVao, nullptr);
				}

			}

			SwapBuffers();

		}

	}


	void TestApplication::TestFramebuffer()
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
		lib.AddBindingMapping("Material_DiffuseMap", { MaterialTextureBinding::DIFFUSE, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_SpecularMap", { MaterialTextureBinding::SPECULAR, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_EmissionMap", { MaterialTextureBinding::EMISSION, ResourceKind::TextureReadOnly });

		lib.AddUniformBufferSizer(MaterialBlockBinding::FRAME_LIGHTS, []() { return sizeof(LightCastersData); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::VIEW_CAMERA, []() { return sizeof(CameraMatrices); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::VIEW_PROJECTION_PLANES, []() { return sizeof(ProjectionPlanes); });

		lib.AddUniformBufferSizer(MaterialBlockBinding::MATERIAL_PHONG, []() { return sizeof(PhongMaterial); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::MATERIAL_COLOR, []() { return sizeof(ColorRGBAf); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::OBJECT_MATRICES, []() { return sizeof(ObjectMatrices); });
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
		PipelineHandle myPipe = m_renderer.MutGraphicsDevice().CreatePipeline(pipeDesc);

		/* Create cube VAO */
		VertexLayoutDescriptor cubeLayout{
			{
				{"position", VertexElementFormat::Float3},
				{"texture", VertexElementFormat::Float2}
			},
			LayoutType::Interleaved
		};

		auto cubeVao = renderer.CreateVertexLayout(cubeLayout);


		/* Create cube shader */
		IGraphicsRenderer::ShaderFileList cubeFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/depth_testing.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/depth_testing.frag" }
		};

		ShaderProgramHandle cubeProgram = renderer.CreateShaderProgramFromSourceFiles(cubeFileList);


		// Create cube geometry

		RenderWorld& renderWorld = MutRenderer().CreateRenderWorld();

		auto cubeGeom = CreateCubePositionTexture(0.5f);
		Mesh* cube = renderWorld.CreateStaticMesh(cubeGeom);

		// Create plane geometry
		VertexPositionTexture planeVertices[] = {
			// positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
			{{ 5.0f, -0.5f,  5.0f },  {2.0f, 0.0f}},
			{{-5.0f, -0.5f, -5.0f },  {0.0f, 2.0f}},
			{{-5.0f, -0.5f,  5.0f },  {0.0f, 0.0f}},

			{{ 5.0f, -0.5f,  5.0f},  {2.0f, 0.0f}},
			{{ 5.0f, -0.5f, -5.0f},  {2.0f, 2.0f}},
			{{-5.0f, -0.5f, -5.0f},  {0.0f, 2.0f}}
		};

		Mesh* plane = renderWorld.CreateStaticMesh(planeVertices);

		/* Create materials */
		MaterialDescriptor materialDesc(
			{
				{"View_ProjectionPlanes", ShaderStage::Fragment},
				{"Material_DiffuseMap", ShaderStage::Fragment}
			}
		);

		MaterialInterface matInterface = lib.CreateMaterialInterface(cubeProgram, materialDesc);

		Texture2DFileDescriptor diffuseTexDesc{ "Sandbox/assets/kitteh.png", TextureFormat::Any, TextureUsage{Sampled}, 4 };
		Texture2DHandle marbleTex = MutRenderer().MutGraphicsDevice().CreateTexture2D(diffuseTexDesc);

		MaterialInstance cubeInstance = lib.CreateMaterialInstance(matInterface);
		cubeInstance.BindTexture(MaterialTextureBinding::DIFFUSE, marbleTex);
		cubeInstance.CreateMaterialResourceSet();

		diffuseTexDesc.m_filename = "Sandbox/assets/metal.png";
		Texture2DHandle metalTex = MutRenderer().MutGraphicsDevice().CreateTexture2D(diffuseTexDesc);

		MaterialInstance planeInstance = lib.CreateMaterialInstance(matInterface);
		planeInstance.BindTexture(MaterialTextureBinding::DIFFUSE, metalTex);
		planeInstance.CreateMaterialResourceSet();

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


		cubeInstance.UpdateUniformBlock(MaterialBlockBinding::VIEW_PROJECTION_PLANES, ProjectionPlanes{ 0.1f, 100.f });
		planeInstance.UpdateUniformBlock(MaterialBlockBinding::VIEW_PROJECTION_PLANES, ProjectionPlanes{ 0.1f, 100.f });

		Texture2DDescriptor colorAttachDesc{nullptr, Width_t(GetWindowWidth()), Height_t(GetWindowHeight()), TextureFormat::RGB32F, TextureUsage{Sampled | RenderTarget}};
		Texture2DDescriptor depthAttachDesc{ nullptr, Width_t(GetWindowWidth()), Height_t(GetWindowHeight()), TextureFormat::Depth24_Stencil8, TextureUsage{RenderTarget}};

		Texture2DHandle colorAttachTex = renderer.MutGraphicsDevice().CreateTexture2D(colorAttachDesc);
		Texture2DHandle depthAttachTex = renderer.MutGraphicsDevice().CreateTexture2D(depthAttachDesc);

		FramebufferDescriptor fbDesc{ {colorAttachTex}, depthAttachTex};
		FramebufferHandle fbHandle = renderer.MutGraphicsDevice().CreateFramebuffer(fbDesc);

		// Create framebuffer "material" (just a shader to draw a fullscreen quad)
		IGraphicsRenderer::ShaderFileList framebufferShaders =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/framebuffer.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/framebuffer.frag" }
		};

		ShaderProgramHandle framebufferProgram = renderer.CreateShaderProgramFromSourceFiles(framebufferShaders);

		MaterialDescriptor framebufferMatDesc({{"Material_DiffuseMap", ShaderStage::Fragment}});
		MaterialInterface fbMatIntf = lib.CreateMaterialInterface(framebufferProgram, framebufferMatDesc);
		MaterialInstance fbMatInst = lib.CreateMaterialInstance(fbMatIntf);
		fbMatInst.BindTexture(MaterialTextureBinding::DIFFUSE, colorAttachTex);
		fbMatInst.CreateMaterialResourceSet();

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

			// Bind the framebuffer to effectively render-to-texture
			renderer.BindFramebuffer(fbHandle);

			renderer.Clear(ColorRGBAf(0.1f, 0.1f, 0.1f, 1.0f));

			//lightsSystem.UpdateLights();

			//lightsSystem.BindLightBuffer();

			camSys.UpdateCameras();

			for (uint32_t iCam = 0; iCam < camSys.CamerasNumber(); iCam++)
			{
				camSys.BindCameraBuffer(iCam);

				renderer.UseMaterialInstance(&cubeInstance);

				cube->SetTransform(Transform::Translate({ -1.0f, 0.0f, -1.0f }));
				cube->UpdateObjectMatrices(camSys.GetCamera(iCam));
				renderWorld.DrawMesh(cube, cubeVao, nullptr);

				cube->SetTransform(Transform::Translate({ 2.0f, 0.0f, 0.0f }));
				cube->UpdateObjectMatrices(camSys.GetCamera(iCam));
				renderWorld.DrawMesh(cube, cubeVao, nullptr);

				renderer.UseMaterialInstance(&planeInstance);

				plane->SetTransform(Transform::Identity());
				plane->UpdateObjectMatrices(camSys.GetCamera(iCam));
				renderWorld.DrawMesh(plane, cubeVao, nullptr);
			}

			renderer.UnbindFramebuffer(fbHandle);

			// Finally - draw the framebuffer using a fullscreen quad

			renderer.MutGraphicsDevice().SetPipeline(fsqPipe);

			renderer.UseMaterialInstance(&fbMatInst);
			renderWorld.DrawMesh(fullscreenQuad, cubeVao, nullptr);

			SwapBuffers();

		}

	}


	struct SkyboxView
	{
		Std140Mat<3>	m_mat;
	};

	struct VertexPositionNormal
	{
		Vec3	m_position;
		Vec3	m_normal;
	};



	Array<VertexPositionNormal, 36>	CreateCubePositionNormal(float halfExtent)
	{
		return {
			{Vec3{-halfExtent, -halfExtent, -halfExtent}, {0.0f,  0.0f, -1.0f}},
			{Vec3{ halfExtent,  halfExtent, -halfExtent}, {0.0f,  0.0f, -1.0f}},
			{Vec3{ halfExtent, -halfExtent, -halfExtent}, {0.0f,  0.0f, -1.0f}},
			{Vec3{ halfExtent,  halfExtent, -halfExtent}, {0.0f,  0.0f, -1.0f}},
			{Vec3{-halfExtent, -halfExtent, -halfExtent}, {0.0f,  0.0f, -1.0f}},
			{Vec3{-halfExtent,  halfExtent, -halfExtent}, {0.0f,  0.0f, -1.0f}},

			{Vec3{ -halfExtent, -halfExtent,  halfExtent }, { 0.0f,  0.0f, 1.0f }},
			{ Vec3{  halfExtent, -halfExtent,  halfExtent}, {0.0f,  0.0f, 1.0f}},
			{ Vec3{  halfExtent,  halfExtent,  halfExtent}, {0.0f,  0.0f, 1.0f}},
			{ Vec3{  halfExtent,  halfExtent,  halfExtent}, {0.0f,  0.0f, 1.0f}},
			{ Vec3{ -halfExtent,  halfExtent,  halfExtent}, {0.0f,  0.0f, 1.0f}},
			{ Vec3{ -halfExtent, -halfExtent,  halfExtent}, {0.0f,  0.0f, 1.0f}},

			{Vec3{ -halfExtent,  halfExtent,  halfExtent }, { -1.0f,  0.0f,  0.0 }},
			{ Vec3{ -halfExtent,  halfExtent, -halfExtent}, {-1.0f,  0.0f,  0.0}},
			{ Vec3{ -halfExtent, -halfExtent, -halfExtent}, {-1.0f,  0.0f,  0.0}},
			{ Vec3{ -halfExtent, -halfExtent, -halfExtent}, {-1.0f,  0.0f,  0.0}},
			{ Vec3{ -halfExtent, -halfExtent,  halfExtent}, {-1.0f,  0.0f,  0.0}},
			{ Vec3{ -halfExtent,  halfExtent,  halfExtent}, {-1.0f,  0.0f,  0.0}},

			{ Vec3{ halfExtent,  halfExtent,  halfExtent }, { 1.0f,  0.0f,  0.0f }},
			{ Vec3{ halfExtent, -halfExtent, -halfExtent}, {1.0f,  0.0f,  0.0f }},
			{ Vec3{ halfExtent,  halfExtent, -halfExtent}, {1.0f,  0.0f,  0.0f }},
			{ Vec3{ halfExtent, -halfExtent, -halfExtent}, {1.0f,  0.0f,  0.0f }},
			{ Vec3{ halfExtent,  halfExtent,  halfExtent}, {1.0f,  0.0f,  0.0f }},
			{ Vec3{ halfExtent, -halfExtent,  halfExtent}, {1.0f,  0.0f,  0.0f }},

			{Vec3{ -halfExtent, -halfExtent, -halfExtent }, { 0.0f, -1.0f,  0.0f } },
			{ Vec3{  halfExtent, -halfExtent, -halfExtent}, {0.0f, -1.0f,  0.0f} },
			{ Vec3{  halfExtent, -halfExtent,  halfExtent}, {0.0f, -1.0f,  0.0f} },
			{ Vec3{  halfExtent, -halfExtent,  halfExtent}, {0.0f, -1.0f,  0.0f} },
			{ Vec3{ -halfExtent, -halfExtent,  halfExtent}, {0.0f, -1.0f,  0.0f} },
			{ Vec3{ -halfExtent, -halfExtent, -halfExtent}, {0.0f, -1.0f,  0.0f} },

			{Vec3{ -halfExtent,  halfExtent, -halfExtent }, { 0.0f,  1.0f,  0.0f } },
			{ Vec3{  halfExtent,  halfExtent,  halfExtent}, {0.0f,  1.0f,  0.0f} },
			{ Vec3{  halfExtent,  halfExtent, -halfExtent}, {0.0f,  1.0f,  0.0f} },
			{ Vec3{  halfExtent,  halfExtent,  halfExtent}, {0.0f,  1.0f,  0.0f} },
			{ Vec3{ -halfExtent,  halfExtent, -halfExtent}, {0.0f,  1.0f,  0.0f} },
			{ Vec3{ -halfExtent,  halfExtent,  halfExtent}, {0.0f,  1.0f,  0.0f}}
		};

	}


	void TestApplication::TestCubemap()
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
		lib.AddUniformBufferSizer(MaterialBlockBinding::VIEW_PROJECTION_PLANES, []() { return sizeof(ProjectionPlanes); });
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
		PipelineHandle myPipe = m_renderer.MutGraphicsDevice().CreatePipeline(pipeDesc);

		/* Create cube VAO */
		VertexLayoutDescriptor cubeLayout{
			{
				{"position", VertexElementFormat::Float3},
				{"texture", VertexElementFormat::Float2}
			},
			LayoutType::Interleaved
		};

		auto cubeVao = renderer.CreateVertexLayout(cubeLayout);


		/* Create cube shader */
		IGraphicsRenderer::ShaderFileList cubeFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/depth_testing.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/depth_testing.frag" }
		};

		ShaderProgramHandle cubeProgram = renderer.CreateShaderProgramFromSourceFiles(cubeFileList);


		// Create cube geometry

		RenderWorld& renderWorld = MutRenderer().CreateRenderWorld();

		auto cubeGeom = CreateCubePositionTexture(0.5f);
		Mesh* cube = renderWorld.CreateStaticMesh(cubeGeom);

		// Create plane geometry
		VertexPositionTexture planeVertices[] = {
			// positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
			{{ 5.0f, -0.5f,  5.0f },  {2.0f, 0.0f}},
			{{-5.0f, -0.5f, -5.0f },  {0.0f, 2.0f}},
			{{-5.0f, -0.5f,  5.0f },  {0.0f, 0.0f}},

			{{ 5.0f, -0.5f,  5.0f},  {2.0f, 0.0f}},
			{{ 5.0f, -0.5f, -5.0f},  {2.0f, 2.0f}},
			{{-5.0f, -0.5f, -5.0f},  {0.0f, 2.0f}}
		};

		Mesh* plane = renderWorld.CreateStaticMesh(planeVertices);

		/* Create materials */
		MaterialDescriptor materialDesc(
			{
				{"View_ProjectionPlanes", ShaderStage::Fragment},
				{"Material_DiffuseMap", ShaderStage::Fragment}
			}
		);

		MaterialInterface matInterface = lib.CreateMaterialInterface(cubeProgram, materialDesc);

		Texture2DFileDescriptor diffuseTexDesc{ "Sandbox/assets/kitteh.png", TextureFormat::Any, TextureUsage{Sampled}, 4 };
		Texture2DHandle marbleTex = MutRenderer().MutGraphicsDevice().CreateTexture2D(diffuseTexDesc);

		MaterialInstance cubeInstance = lib.CreateMaterialInstance(matInterface);
		cubeInstance.BindTexture(MaterialTextureBinding::DIFFUSE, marbleTex);
		cubeInstance.CreateMaterialResourceSet();

		diffuseTexDesc.m_filename = "Sandbox/assets/metal.png";
		Texture2DHandle metalTex = MutRenderer().MutGraphicsDevice().CreateTexture2D(diffuseTexDesc);

		MaterialInstance planeInstance = lib.CreateMaterialInstance(matInterface);
		planeInstance.BindTexture(MaterialTextureBinding::DIFFUSE, metalTex);
		planeInstance.CreateMaterialResourceSet();

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

		cubeInstance.UpdateUniformBlock(MaterialBlockBinding::VIEW_PROJECTION_PLANES, ProjectionPlanes{ 0.1f, 100.f });
		planeInstance.UpdateUniformBlock(MaterialBlockBinding::VIEW_PROJECTION_PLANES, ProjectionPlanes{ 0.1f, 100.f });

		// Create skybox

		/* Create skybox VAO */
		VertexLayoutDescriptor skyboxLayout{
			{
				{"position", VertexElementFormat::Float3}
			},
			LayoutType::Interleaved
		};

		auto skyboxVao = renderer.CreateVertexLayout(skyboxLayout);


		/* Create skybox shader */
		IGraphicsRenderer::ShaderFileList skyboxFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/skybox.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/skybox.frag" }
		};

		ShaderProgramHandle skyboxProgram = renderer.CreateShaderProgramFromSourceFiles(skyboxFileList);


		// Create a new pipeline for the skybox with depth comparison set to LessEqual
		// The depth buffer will be filled with values of 1.0 for the skybox, so we need to make sure it passes with values less than or equal to the depth buffer.
		PipelineDescriptor skyboxPipeDesc;
		skyboxPipeDesc.m_depthStencilStateDesc = DepthStencilStateDescriptor{ DepthTest::Enabled, DepthWriting::Enabled, DepthStencilComparisonFunc::LessEqual };

		PipelineHandle skyboxPipe = m_renderer.MutGraphicsDevice().CreatePipeline(skyboxPipeDesc);

		/* Create materials */
		MaterialDescriptor skyboxMatDesc(
			{
				{"SkyboxViewProjection", ShaderStage::Vertex},
				{"Material_SkyboxMap", ShaderStage::Fragment}
			}
		);
		MaterialInterface skyboxIntf = lib.CreateMaterialInterface(skyboxProgram, skyboxMatDesc);
		CubeMapTextureFilesDescriptor skyboxTexDesc{{"Sandbox/assets/textures/skybox/right.jpg",
			"Sandbox/assets/textures/skybox/left.jpg",
			"Sandbox/assets/textures/skybox/top.jpg",
			"Sandbox/assets/textures/skybox/bottom.jpg",
			"Sandbox/assets/textures/skybox/front.jpg",
			"Sandbox/assets/textures/skybox/back.jpg"},
			TextureFormat::RGBA8
		};

		MaterialInstance skyboxInst = lib.CreateMaterialInstance(skyboxIntf);
		TextureHandle skyboxTex = MutRenderer().MutGraphicsDevice().CreateCubemapTexture(skyboxTexDesc);
		skyboxInst.BindTexture(MaterialTextureBinding::SKYBOX, skyboxTex);
		skyboxInst.CreateMaterialResourceSet();

		Array<VertexPosition, 36> skyboxVertices = {
			// positions
			{{-1.0f,  1.0f, -1.0f}},
			{{-1.0f, -1.0f, -1.0f}},
			{{ 1.0f, -1.0f, -1.0f}},
			{{ 1.0f, -1.0f, -1.0f}},
			{{ 1.0f,  1.0f, -1.0f}},
			{{-1.0f,  1.0f, -1.0f}},
			{{-1.0f, -1.0f,  1.0f}},
			{{-1.0f, -1.0f, -1.0f}},
			{{-1.0f,  1.0f, -1.0f}},
			{{-1.0f,  1.0f, -1.0f}},
			{{-1.0f,  1.0f,  1.0f}},
			{{-1.0f, -1.0f,  1.0f}},
			{{ 1.0f, -1.0f, -1.0f}},
			{{ 1.0f, -1.0f,  1.0f}},
			{{ 1.0f,  1.0f,  1.0f}},
			{{ 1.0f,  1.0f,  1.0f}},
			{{ 1.0f,  1.0f, -1.0f}},
			{{ 1.0f, -1.0f, -1.0f}},
			{{-1.0f, -1.0f,  1.0f}},
			{{-1.0f,  1.0f,  1.0f}},
			{{ 1.0f,  1.0f,  1.0f}},
			{{ 1.0f,  1.0f,  1.0f}},
			{{ 1.0f, -1.0f,  1.0f}},
			{{-1.0f, -1.0f,  1.0f}},
			{{-1.0f,  1.0f, -1.0f}},
			{{ 1.0f,  1.0f, -1.0f}},
			{{ 1.0f,  1.0f,  1.0f}},
			{{ 1.0f,  1.0f,  1.0f}},
			{{-1.0f,  1.0f,  1.0f}},
			{{-1.0f,  1.0f, -1.0f}},
			{{-1.0f, -1.0f, -1.0f}},
			{{-1.0f, -1.0f,  1.0f}},
			{{ 1.0f, -1.0f, -1.0f}},
			{{ 1.0f, -1.0f, -1.0f}},
			{{-1.0f, -1.0f,  1.0f}},
			{{ 1.0f, -1.0f,  1.0f}}
		};
		//CreateCube(1.0f);

		Mesh* skybox = renderWorld.CreateStaticMesh(skyboxVertices);



		// Create reflective cube

		// VAO
		VertexLayoutDescriptor reflectiveCubeLayout{
	{
			{"position", VertexElementFormat::Float3},
			{"normal", VertexElementFormat::Float3}
			},
		LayoutType::Interleaved
		};

		auto reflCubeVao = renderer.CreateVertexLayout(reflectiveCubeLayout);

		// Shader
		IGraphicsRenderer::ShaderFileList reflCubeFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/cubemaps.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/cubemaps.frag" }
		};
		ShaderProgramHandle reflCubeProgram = renderer.CreateShaderProgramFromSourceFiles(reflCubeFileList);

		// Material

		MaterialDescriptor reflectiveMatDesc(
			{
				{ "Material_SkyboxMap", ShaderStage::Fragment }
			}
		);

		MaterialInterface reflMatIntf = lib.CreateMaterialInterface(reflCubeProgram, reflectiveMatDesc);
		MaterialInstance reflMatInst = lib.CreateMaterialInstance(reflMatIntf);
		reflMatInst.BindTexture(MaterialTextureBinding::SKYBOX, skyboxTex);
		reflMatInst.CreateMaterialResourceSet();

		// Geometry
		Array<VertexPositionNormal, 36> cubeVpnGeom = CreateCubePositionNormal(0.5f);

		Mesh* reflCube = renderWorld.CreateStaticMesh(cubeVpnGeom);

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

			//lightsSystem.UpdateLights();

			//lightsSystem.BindLightBuffer();

			camSys.UpdateCameras();

			for (uint32_t iCam = 0; iCam < camSys.CamerasNumber(); iCam++)
			{
				m_renderer.MutGraphicsDevice().SetPipeline(myPipe);

				camSys.BindCameraBuffer(iCam);

				renderer.UseMaterialInstance(&cubeInstance);
				cube->SetTransform(Transform::Translate({ -1.0f, 0.0f, -1.0f }));
				cube->UpdateObjectMatrices(camSys.GetCamera(iCam));
				renderWorld.DrawMesh(cube, cubeVao, nullptr);

				renderer.UseMaterialInstance(&reflMatInst);
				reflCube->SetTransform(Transform::Translate({ 2.0f, 0.0f, 0.0f }));
				reflCube->UpdateObjectMatrices(camSys.GetCamera(iCam));
				renderWorld.DrawMesh(reflCube, reflCubeVao, nullptr);

				renderer.UseMaterialInstance(&planeInstance);
				plane->SetTransform(Transform::Identity());
				plane->UpdateObjectMatrices(camSys.GetCamera(iCam));
				renderWorld.DrawMesh(plane, cubeVao, nullptr);

				// Render skybox last
				m_renderer.MutGraphicsDevice().SetPipeline(skyboxPipe);

				renderer.UseMaterialInstance(&skyboxInst);

				// We want the skybox to be centered around the player so that no matter how far the player moves, the skybox won't get any closer.
				// We can achieve this by converting the view matrix to a 3x3 matrix (removing translation) and converting it back to a 4x4 matrix:
				// TODO: it works, but it's a bit ugly. Find a better way to do that... (just zero translation from the view-projection matrix ?)
				Mat4 view = camSys.GetCamera(iCam).GetViewMatrix();
				view[3] = Vec4(0,0,0, 1);
				Mat4 proj = camSys.GetCamera(iCam).GetProjectionMatrix();
				Mat4 skyboxViewProj = proj * view;
				skyboxInst.UpdateUniformBlock(MaterialBlockBinding::MATERIAL_SKYBOX_VIEWPROJ, skyboxViewProj);

				renderWorld.DrawMesh(skybox, skyboxVao, nullptr);

			}

			SwapBuffers();
		}
	}


	void TestApplication::TestGeometryShaderHouses()
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
		lib.AddUniformBufferSizer(MaterialBlockBinding::VIEW_PROJECTION_PLANES, []() { return sizeof(ProjectionPlanes); });
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
		pipeDesc.m_topology = PrimitiveTopology::PointList;
		PipelineHandle myPipe = m_renderer.MutGraphicsDevice().CreatePipeline(pipeDesc);


		RenderWorld& renderWorld = MutRenderer().CreateRenderWorld();


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


		// Create reflective cube

		// VAO
		VertexLayoutDescriptor geomLayout{
			{
			{"position", VertexElementFormat::Float2},
			{"color", VertexElementFormat::Float3}
			},
		LayoutType::Interleaved
		};

		auto geomVao = renderer.CreateVertexLayout(geomLayout);

		// Shader
		IGraphicsRenderer::ShaderFileList geometryFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/geometry_shader.vert" },
			{ ShaderStage::Geometry,	"source/Graphics/Resources/shaders/OpenGL/geometry_shader.geom" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/geometry_shader.frag" }
		};
		ShaderProgramHandle geomProgram = renderer.CreateShaderProgramFromSourceFiles(geometryFileList);

		// Material
		MaterialDescriptor emptyDesc;
		MaterialInterface pointsInf = lib.CreateMaterialInterface(geomProgram, emptyDesc);
		MaterialInstance pointsInst = lib.CreateMaterialInstance(pointsInf);

		// Geometry
		struct VertexPositionColor2
		{
			Vec2 m_position;
			Vec3 m_color;
		};

		Array<VertexPositionColor2, 4> geomTest = {
			{{-0.5f,  0.5f}, Vec3(1, 0, 0)}, // top-left
			{{ 0.5f,  0.5f}, Vec3(0, 1, 0)}, // top-right
			{{ 0.5f, -0.5f}, Vec3(0, 0, 1)}, // bottom-right
			{{-0.5f, -0.5f}, Vec3(1, 1, 0)}  // bottom-left
		};

		Mesh* points = renderWorld.CreateStaticMesh(geomTest);

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

			renderer.Clear(ColorRGBAf(0.1f, 0.1f, 0.1f, 1.0f));

			//lightsSystem.UpdateLights();

			//lightsSystem.BindLightBuffer();

			camSys.UpdateCameras();

			for (uint32_t iCam = 0; iCam < camSys.CamerasNumber(); iCam++)
			{

				camSys.BindCameraBuffer(iCam);

				renderer.UseMaterialInstance(&pointsInst);
				//cube->SetTransform(Transform::Translate({ -1.0f, 0.0f, -1.0f }));
				//cube->UpdateObjectMatrices(camSys.GetCamera(iCam));
				renderWorld.DrawMesh(points, geomVao, nullptr);

			}

			SwapBuffers();
		}
	}



	void TestApplication::TestGeometryShaderExplode()
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

		lib.AddUniformBufferSizer(MaterialBlockBinding::FRAME_TIME, []() { return sizeof(float); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::FRAME_LIGHTS, []() { return sizeof(LightCastersData); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::VIEW_CAMERA, []() { return sizeof(CameraMatrices); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::VIEW_PROJECTION_PLANES, []() { return sizeof(ProjectionPlanes); });
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
		PipelineHandle myPipe = m_renderer.MutGraphicsDevice().CreatePipeline(pipeDesc);

		/* Create cube VAO */
		VertexLayoutDescriptor cubeLayout{
			{
				{"position", VertexElementFormat::Float3},
				{"texture", VertexElementFormat::Float2}
			},
			LayoutType::Interleaved
		};

		auto cubeVao = renderer.CreateVertexLayout(cubeLayout);


		/* Create cube shader */
		IGraphicsRenderer::ShaderFileList cubeFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/explode.vert" },
			{ ShaderStage::Geometry,	"source/Graphics/Resources/shaders/OpenGL/explode.geom" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/explode.frag" }
		};

		ShaderProgramHandle cubeProgram = renderer.CreateShaderProgramFromSourceFiles(cubeFileList);


		// Create cube geometry

		RenderWorld& renderWorld = MutRenderer().CreateRenderWorld();

		auto cubeGeom = CreateCubePositionTexture(0.5f);
		Mesh* cube = renderWorld.CreateStaticMesh(cubeGeom);

		// Create plane geometry
		VertexPositionTexture planeVertices[] = {
			// positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
			{{ 5.0f, -0.5f,  5.0f },  {2.0f, 0.0f}},
			{{-5.0f, -0.5f, -5.0f },  {0.0f, 2.0f}},
			{{-5.0f, -0.5f,  5.0f },  {0.0f, 0.0f}},

			{{ 5.0f, -0.5f,  5.0f},  {2.0f, 0.0f}},
			{{ 5.0f, -0.5f, -5.0f},  {2.0f, 2.0f}},
			{{-5.0f, -0.5f, -5.0f},  {0.0f, 2.0f}}
		};

		Mesh* plane = renderWorld.CreateStaticMesh(planeVertices);

		/* Create materials */
		MaterialDescriptor materialDesc(
			{
				{"View_ProjectionPlanes", ShaderStage::Fragment},
				{"Material_DiffuseMap", ShaderStage::Fragment},
				{"Frame_Time", ShaderStage::Geometry}
			}
		);

		MaterialInterface matInterface = lib.CreateMaterialInterface(cubeProgram, materialDesc);

		Texture2DFileDescriptor diffuseTexDesc{ "Sandbox/assets/kitteh.png", TextureFormat::Any, TextureUsage{Sampled}, 4 };
		Texture2DHandle marbleTex = MutRenderer().MutGraphicsDevice().CreateTexture2D(diffuseTexDesc);

		MaterialInstance cubeInstance = lib.CreateMaterialInstance(matInterface);
		cubeInstance.BindTexture(MaterialTextureBinding::DIFFUSE, marbleTex);
		cubeInstance.CreateMaterialResourceSet();

		diffuseTexDesc.m_filename = "Sandbox/assets/metal.png";
		Texture2DHandle metalTex = MutRenderer().MutGraphicsDevice().CreateTexture2D(diffuseTexDesc);

		MaterialInstance planeInstance = lib.CreateMaterialInstance(matInterface);
		planeInstance.BindTexture(MaterialTextureBinding::DIFFUSE, metalTex);
		planeInstance.CreateMaterialResourceSet();

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

		cubeInstance.UpdateUniformBlock(MaterialBlockBinding::VIEW_PROJECTION_PLANES, ProjectionPlanes{ 0.1f, 100.f });
		planeInstance.UpdateUniformBlock(MaterialBlockBinding::VIEW_PROJECTION_PLANES, ProjectionPlanes{ 0.1f, 100.f });

		// Create skybox

		/* Create skybox VAO */
		VertexLayoutDescriptor skyboxLayout{
			{
				{"position", VertexElementFormat::Float3}
			},
			LayoutType::Interleaved
		};

		auto skyboxVao = renderer.CreateVertexLayout(skyboxLayout);


		/* Create skybox shader */
		IGraphicsRenderer::ShaderFileList skyboxFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/skybox.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/skybox.frag" }
		};

		ShaderProgramHandle skyboxProgram = renderer.CreateShaderProgramFromSourceFiles(skyboxFileList);


		// Create a new pipeline for the skybox with depth comparison set to LessEqual
		// The depth buffer will be filled with values of 1.0 for the skybox, so we need to make sure it passes with values less than or equal to the depth buffer.
		PipelineDescriptor skyboxPipeDesc;
		skyboxPipeDesc.m_depthStencilStateDesc = DepthStencilStateDescriptor{ DepthTest::Enabled, DepthWriting::Enabled, DepthStencilComparisonFunc::LessEqual };

		PipelineHandle skyboxPipe = m_renderer.MutGraphicsDevice().CreatePipeline(skyboxPipeDesc);

		/* Create materials */
		MaterialDescriptor skyboxMatDesc(
			{
				{"SkyboxViewProjection", ShaderStage::Vertex},
				{"Material_SkyboxMap", ShaderStage::Fragment}
			}
		);
		MaterialInterface skyboxIntf = lib.CreateMaterialInterface(skyboxProgram, skyboxMatDesc);
		CubeMapTextureFilesDescriptor skyboxTexDesc{ {"Sandbox/assets/textures/skybox/right.jpg",
			"Sandbox/assets/textures/skybox/left.jpg",
			"Sandbox/assets/textures/skybox/top.jpg",
			"Sandbox/assets/textures/skybox/bottom.jpg",
			"Sandbox/assets/textures/skybox/front.jpg",
			"Sandbox/assets/textures/skybox/back.jpg"},
			TextureFormat::RGBA8
		};

		MaterialInstance skyboxInst = lib.CreateMaterialInstance(skyboxIntf);
		TextureHandle skyboxTex = MutRenderer().MutGraphicsDevice().CreateCubemapTexture(skyboxTexDesc);
		skyboxInst.BindTexture(MaterialTextureBinding::SKYBOX, skyboxTex);
		skyboxInst.CreateMaterialResourceSet();

		Array<VertexPosition, 36> skyboxVertices = {
			// positions
			{{-1.0f,  1.0f, -1.0f}},
			{{-1.0f, -1.0f, -1.0f}},
			{{ 1.0f, -1.0f, -1.0f}},
			{{ 1.0f, -1.0f, -1.0f}},
			{{ 1.0f,  1.0f, -1.0f}},
			{{-1.0f,  1.0f, -1.0f}},
			{{-1.0f, -1.0f,  1.0f}},
			{{-1.0f, -1.0f, -1.0f}},
			{{-1.0f,  1.0f, -1.0f}},
			{{-1.0f,  1.0f, -1.0f}},
			{{-1.0f,  1.0f,  1.0f}},
			{{-1.0f, -1.0f,  1.0f}},
			{{ 1.0f, -1.0f, -1.0f}},
			{{ 1.0f, -1.0f,  1.0f}},
			{{ 1.0f,  1.0f,  1.0f}},
			{{ 1.0f,  1.0f,  1.0f}},
			{{ 1.0f,  1.0f, -1.0f}},
			{{ 1.0f, -1.0f, -1.0f}},
			{{-1.0f, -1.0f,  1.0f}},
			{{-1.0f,  1.0f,  1.0f}},
			{{ 1.0f,  1.0f,  1.0f}},
			{{ 1.0f,  1.0f,  1.0f}},
			{{ 1.0f, -1.0f,  1.0f}},
			{{-1.0f, -1.0f,  1.0f}},
			{{-1.0f,  1.0f, -1.0f}},
			{{ 1.0f,  1.0f, -1.0f}},
			{{ 1.0f,  1.0f,  1.0f}},
			{{ 1.0f,  1.0f,  1.0f}},
			{{-1.0f,  1.0f,  1.0f}},
			{{-1.0f,  1.0f, -1.0f}},
			{{-1.0f, -1.0f, -1.0f}},
			{{-1.0f, -1.0f,  1.0f}},
			{{ 1.0f, -1.0f, -1.0f}},
			{{ 1.0f, -1.0f, -1.0f}},
			{{-1.0f, -1.0f,  1.0f}},
			{{ 1.0f, -1.0f,  1.0f}}
		};
		//CreateCube(1.0f);

		Mesh* skybox = renderWorld.CreateStaticMesh(skyboxVertices);



		// Create reflective cube

		// VAO
		VertexLayoutDescriptor reflectiveCubeLayout{
	{
			{"position", VertexElementFormat::Float3},
			{"normal", VertexElementFormat::Float3}
			},
		LayoutType::Interleaved
		};

		auto reflCubeVao = renderer.CreateVertexLayout(reflectiveCubeLayout);

		// Shader
		IGraphicsRenderer::ShaderFileList reflCubeFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/cubemaps.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/cubemaps.frag" }
		};
		ShaderProgramHandle reflCubeProgram = renderer.CreateShaderProgramFromSourceFiles(reflCubeFileList);

		// Material

		MaterialDescriptor reflectiveMatDesc(
			{
				{ "Material_SkyboxMap", ShaderStage::Fragment }
			}
		);

		MaterialInterface reflMatIntf = lib.CreateMaterialInterface(reflCubeProgram, reflectiveMatDesc);
		MaterialInstance reflMatInst = lib.CreateMaterialInstance(reflMatIntf);
		reflMatInst.BindTexture(MaterialTextureBinding::SKYBOX, skyboxTex);
		reflMatInst.CreateMaterialResourceSet();

		IGraphicsRenderer::ShaderFileList normalVizFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/visualize_normals.vert" },
			{ ShaderStage::Geometry,	"source/Graphics/Resources/shaders/OpenGL/visualize_normals.geom" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/visualize_normals.frag" }
		};
		ShaderProgramHandle normalVizProgram = renderer.CreateShaderProgramFromSourceFiles(normalVizFileList);

		MaterialDescriptor normalVizDesc;
		MaterialInterface normalVizIntf = lib.CreateMaterialInterface(normalVizProgram, normalVizDesc);
		MaterialInstance normalVizInst = lib.CreateMaterialInstance(normalVizIntf);

		// Geometry
		Array<VertexPositionNormal, 36> cubeVpnGeom = CreateCubePositionNormal(0.5f);

		Mesh* reflCube = renderWorld.CreateStaticMesh(cubeVpnGeom);

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

			// The sin function returns a value between -1.0 and 1.0. Because we don't want to implode the object we transform the sin value to the [0,1] range.
			float sinTime = (sinf(thisFrameTime) + 1.f) / 2.f;
			cubeInstance.UpdateUniformBlock(MaterialBlockBinding::FRAME_TIME, sinTime);


			renderer.Clear(ColorRGBAf(0.1f, 0.1f, 0.1f, 1.0f));

			//lightsSystem.UpdateLights();

			//lightsSystem.BindLightBuffer();

			camSys.UpdateCameras();

			for (uint32_t iCam = 0; iCam < camSys.CamerasNumber(); iCam++)
			{
				m_renderer.MutGraphicsDevice().SetPipeline(myPipe);

				camSys.BindCameraBuffer(iCam);

				renderer.UseMaterialInstance(&reflMatInst);
				reflCube->SetTransform(Transform::Translate({ 2.0f, 0.0f, 0.0f }));
				reflCube->UpdateObjectMatrices(camSys.GetCamera(iCam));
				renderWorld.DrawMesh(reflCube, reflCubeVao, nullptr);

				// Draw the normals of the reflective cube
				renderer.UseMaterialInstance(&normalVizInst);
				renderWorld.DrawMesh(reflCube, reflCubeVao, nullptr);


				renderer.UseMaterialInstance(&cubeInstance);
				cube->SetTransform(Transform::Translate({ -1.0f, 0.0f, -1.0f }));
				cube->UpdateObjectMatrices(camSys.GetCamera(iCam));
				renderWorld.DrawMesh(cube, cubeVao, nullptr);

				renderer.UseMaterialInstance(&planeInstance);
				plane->SetTransform(Transform::Identity());
				plane->UpdateObjectMatrices(camSys.GetCamera(iCam));
				renderWorld.DrawMesh(plane, cubeVao, nullptr);

				// Render skybox last
				m_renderer.MutGraphicsDevice().SetPipeline(skyboxPipe);

				renderer.UseMaterialInstance(&skyboxInst);

				// We want the skybox to be centered around the player so that no matter how far the player moves, the skybox won't get any closer.
				// We can achieve this by converting the view matrix to a 3x3 matrix (removing translation) and converting it back to a 4x4 matrix:
				// TODO: it works, but it's a bit ugly. Find a better way to do that... (just zero translation from the view-projection matrix ?)
				Mat4 view = camSys.GetCamera(iCam).GetViewMatrix();
				view[3] = Vec4(0, 0, 0, 1);
				Mat4 proj = camSys.GetCamera(iCam).GetProjectionMatrix();
				Mat4 skyboxViewProj = proj * view;
				skyboxInst.UpdateUniformBlock(MaterialBlockBinding::MATERIAL_SKYBOX_VIEWPROJ, skyboxViewProj);

				renderWorld.DrawMesh(skybox, skyboxVao, nullptr);
			}

			SwapBuffers();
		}
	}


	void TestApplication::TestInstancedAsteroids()
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

		lib.AddUniformBufferSizer(MaterialBlockBinding::FRAME_TIME, []() { return sizeof(float); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::FRAME_LIGHTS, []() { return sizeof(LightCastersData); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::VIEW_CAMERA, []() { return sizeof(CameraMatrices); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::VIEW_PROJECTION_PLANES, []() { return sizeof(ProjectionPlanes); });
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
				{"position", VertexElementFormat::Float3},
				{"texture", VertexElementFormat::Float2}
			},
			LayoutType::Interleaved
		};

		auto cubeVao = renderer.CreateVertexLayout(cubeLayout);


		/* Create cube shader */
		IGraphicsRenderer::ShaderFileList cubeFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/depth_testing.vert" },
			//{ ShaderStage::Geometry,	"source/Graphics/Resources/shaders/OpenGL/explode.geom" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/depth_testing.frag" }
		};

		ShaderProgramHandle cubeProgram = renderer.CreateShaderProgramFromSourceFiles(cubeFileList);


		// Create cube geometry

		RenderWorld& renderWorld = MutRenderer().CreateRenderWorld();

		/* Create materials */
		MaterialDescriptor materialDesc(
			{
				{"View_ProjectionPlanes", ShaderStage::Fragment},
				{"Material_DiffuseMap", ShaderStage::Fragment},
				{"Frame_Time", ShaderStage::Geometry}
			}
		);

		MaterialInterface matInterface = lib.CreateMaterialInterface(cubeProgram, materialDesc);

		Texture2DFileDescriptor diffuseTexDesc{ "Sandbox/assets/kitteh.png", TextureFormat::Any, TextureUsage{Sampled}, 4 };
		Texture2DHandle marbleTex = MutRenderer().MutGraphicsDevice().CreateTexture2D(diffuseTexDesc);

		MaterialInstance cubeInstance = lib.CreateMaterialInstance(matInterface);
		cubeInstance.BindTexture(MaterialTextureBinding::DIFFUSE, marbleTex);
		cubeInstance.CreateMaterialResourceSet();

		diffuseTexDesc.m_filename = "Sandbox/assets/metal.png";
		Texture2DHandle metalTex = MutRenderer().MutGraphicsDevice().CreateTexture2D(diffuseTexDesc);

		MaterialInstance planeInstance = lib.CreateMaterialInstance(matInterface);
		planeInstance.BindTexture(MaterialTextureBinding::DIFFUSE, metalTex);
		planeInstance.CreateMaterialResourceSet();

		/* Create camera */
		PerspectiveCameraDesc persDesc{ 45_degf, GetWindowWidth() / (float)GetWindowHeight(), 0.1f, 1000.f };

		CameraSystem camSys(renderer.MutGraphicsDevice());

		ViewportHandle vpHandle = m_renderer.MutGraphicsDevice().CreateViewport(ViewportDescriptor(0, 0, (float)GetWindowWidth(), (float)GetWindowHeight()));

		Camera* newCam = camSys.AddNewCamera(vpHandle, persDesc);

		newCam->AddTransform(Transform::Translate(Vec3(0, 0, 3)));

		m_currentCamera = newCam;

		// To make sure the camera points towards the negative z-axis by default we can give the yaw a default value of a 90 degree clockwise rotation.

		newCam->UpdateCameraVectors(0, -90);

		/* Create camera end */

		cubeInstance.UpdateUniformBlock(MaterialBlockBinding::VIEW_PROJECTION_PLANES, ProjectionPlanes{ 0.1f, 100.f });
		planeInstance.UpdateUniformBlock(MaterialBlockBinding::VIEW_PROJECTION_PLANES, ProjectionPlanes{ 0.1f, 100.f });

		// Create skybox

		/* Create skybox VAO */
		VertexLayoutDescriptor skyboxLayout{
			{
				{"position", VertexElementFormat::Float3}
			},
			LayoutType::Interleaved
		};

		auto skyboxVao = renderer.CreateVertexLayout(skyboxLayout);


		/* Create skybox shader */
		IGraphicsRenderer::ShaderFileList skyboxFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/skybox.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/skybox.frag" }
		};

		ShaderProgramHandle skyboxProgram = renderer.CreateShaderProgramFromSourceFiles(skyboxFileList);


		// Create a new pipeline for the skybox with depth comparison set to LessEqual
		// The depth buffer will be filled with values of 1.0 for the skybox, so we need to make sure it passes with values less than or equal to the depth buffer.
		PipelineDescriptor skyboxPipeDesc;
		skyboxPipeDesc.m_depthStencilStateDesc = DepthStencilStateDescriptor{ DepthTest::Enabled, DepthWriting::Enabled, DepthStencilComparisonFunc::LessEqual };

		PipelineHandle skyboxPipe = m_renderer.MutGraphicsDevice().CreatePipeline(skyboxPipeDesc);

		/* Create materials */
		MaterialDescriptor skyboxMatDesc(
			{
				{"SkyboxViewProjection", ShaderStage::Vertex},
				{"Material_SkyboxMap", ShaderStage::Fragment}
			}
		);
		MaterialInterface skyboxIntf = lib.CreateMaterialInterface(skyboxProgram, skyboxMatDesc);
		CubeMapTextureFilesDescriptor skyboxTexDesc{ {"Sandbox/assets/textures/skybox/right.jpg",
			"Sandbox/assets/textures/skybox/left.jpg",
			"Sandbox/assets/textures/skybox/top.jpg",
			"Sandbox/assets/textures/skybox/bottom.jpg",
			"Sandbox/assets/textures/skybox/front.jpg",
			"Sandbox/assets/textures/skybox/back.jpg"},
			TextureFormat::RGBA8
		};

		MaterialInstance skyboxInst = lib.CreateMaterialInstance(skyboxIntf);
		TextureHandle skyboxTex = MutRenderer().MutGraphicsDevice().CreateCubemapTexture(skyboxTexDesc);
		skyboxInst.BindTexture(MaterialTextureBinding::SKYBOX, skyboxTex);
		skyboxInst.CreateMaterialResourceSet();

		Array<VertexPosition, 36> skyboxVertices = {
			// positions
			{{-1.0f,  1.0f, -1.0f}},
			{{-1.0f, -1.0f, -1.0f}},
			{{ 1.0f, -1.0f, -1.0f}},
			{{ 1.0f, -1.0f, -1.0f}},
			{{ 1.0f,  1.0f, -1.0f}},
			{{-1.0f,  1.0f, -1.0f}},
			{{-1.0f, -1.0f,  1.0f}},
			{{-1.0f, -1.0f, -1.0f}},
			{{-1.0f,  1.0f, -1.0f}},
			{{-1.0f,  1.0f, -1.0f}},
			{{-1.0f,  1.0f,  1.0f}},
			{{-1.0f, -1.0f,  1.0f}},
			{{ 1.0f, -1.0f, -1.0f}},
			{{ 1.0f, -1.0f,  1.0f}},
			{{ 1.0f,  1.0f,  1.0f}},
			{{ 1.0f,  1.0f,  1.0f}},
			{{ 1.0f,  1.0f, -1.0f}},
			{{ 1.0f, -1.0f, -1.0f}},
			{{-1.0f, -1.0f,  1.0f}},
			{{-1.0f,  1.0f,  1.0f}},
			{{ 1.0f,  1.0f,  1.0f}},
			{{ 1.0f,  1.0f,  1.0f}},
			{{ 1.0f, -1.0f,  1.0f}},
			{{-1.0f, -1.0f,  1.0f}},
			{{-1.0f,  1.0f, -1.0f}},
			{{ 1.0f,  1.0f, -1.0f}},
			{{ 1.0f,  1.0f,  1.0f}},
			{{ 1.0f,  1.0f,  1.0f}},
			{{-1.0f,  1.0f,  1.0f}},
			{{-1.0f,  1.0f, -1.0f}},
			{{-1.0f, -1.0f, -1.0f}},
			{{-1.0f, -1.0f,  1.0f}},
			{{ 1.0f, -1.0f, -1.0f}},
			{{ 1.0f, -1.0f, -1.0f}},
			{{-1.0f, -1.0f,  1.0f}},
			{{ 1.0f, -1.0f,  1.0f}}
		};

		Mesh* skybox = renderWorld.CreateStaticMesh(skyboxVertices);


		// Create instanced cubes

		/* Create instanced VAO */

		InstancedVertexLayoutDescriptor instancedObjectLayout{
			{{{"position", VertexElementFormat::Float3}, {"texture", VertexElementFormat::Float2}}, LayoutType::Interleaved, 0},
			 {{{"modelMatrices", VertexElementFormat::Mat4}}, LayoutType::Interleaved, 1, 1}
		};

		auto instancedVao = renderer.CreateInstancedVertexLayout(instancedObjectLayout);

		// Create instancing shader
		IGraphicsRenderer::ShaderFileList instCubeFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/instancing.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/instancing.frag" }
		};

		ShaderProgramHandle instancedCubeProgram = renderer.CreateShaderProgramFromSourceFiles(instCubeFileList);

		/* Create materials */
		MaterialDescriptor instancedMatDesc(
			{
				{"Material_DiffuseMap", ShaderStage::Fragment}
			}
		);
		MaterialInterface instanceMatIntf = lib.CreateMaterialInterface(instancedCubeProgram, instancedMatDesc);
		MaterialInstance instanceMatInst = lib.CreateMaterialInstance(instanceMatIntf);
		instanceMatInst.BindTexture(MaterialTextureBinding::DIFFUSE, marbleTex);
		instanceMatInst.CreateMaterialResourceSet();

		auto cubeGeom = CreateCubePositionTexture(0.5f);
		Mesh* cube = renderWorld.CreateStaticMesh(cubeGeom);

		InstancedMesh* instCube = renderWorld.CreateInstancedMesh(cubeGeom);


		// generate a large list of semi-random model transformation matrices
		// ------------------------------------------------------------------
		constexpr unsigned int amount = 50;
		Mat4 modelMatrices[amount];
		srand((unsigned)GetApplicationTimeSeconds()); // initialize random seed
		float radius = 15.f;
		float offset = 25.0f;
		for (unsigned int iMat = 0; iMat < amount; iMat++)
		{
			modelMatrices[iMat].SetIdentity();

			// 1. translation: displace along circle with 'radius' in range [-offset, offset]
			float angle = (float)iMat / (float)amount * 360.0f;
			float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float x = sin(angle) * radius + displacement; x;
			displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float y = displacement * 0.4f; y; // keep height of asteroid field smaller compared to width of x and z
			displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float z = cos(angle) * radius + displacement; z;
			modelMatrices[iMat].Translate(Vec3(x, y, z));

			// 2. scale: Scale between 0.05 and 0.25f
			//float scale = (rand() % 20) / 100.0f + 0.05f;
			//modelMatrices[iMat].Scale(Vec3(scale));

			// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
			float rotAngle = float(rand() % 360);
			modelMatrices[iMat].Rotate(Degs_f(rotAngle), Vec3(0.4f, 0.6f, 0.8f));
		}

		// Now create a GPU buffer to copy this data into
		DeviceBufferHandle instancingBuffer = m_renderer.MutGraphicsDevice().CreateStaticVertexBufferFromData(modelMatrices);

		// Finally, associate this data to the instancing buffer of the instanced cube
		instCube->SetInstancingBufferBinding(instancingBuffer, amount);

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

			// The sin function returns a value between -1.0 and 1.0. Because we don't want to implode the object we transform the sin value to the [0,1] range.
			float sinTime = (sinf(thisFrameTime) + 1.f) / 2.f;
			cubeInstance.UpdateUniformBlock(MaterialBlockBinding::FRAME_TIME, sinTime);


			renderer.Clear(ColorRGBAf(0.1f, 0.1f, 0.1f, 1.0f));

			//lightsSystem.UpdateLights();

			//lightsSystem.BindLightBuffer();

			camSys.UpdateCameras();

			for (uint32_t iCam = 0; iCam < camSys.CamerasNumber(); iCam++)
			{
				m_renderer.MutGraphicsDevice().SetPipeline(myPipe);

				camSys.BindCameraBuffer(iCam);

				renderer.UseMaterialInstance(&cubeInstance);
				cube->SetTransform(Transform::Translate({ -1.0f, 0.0f, -1.0f }));
				cube->UpdateObjectMatrices(camSys.GetCamera(iCam));
				renderWorld.DrawMesh(cube, cubeVao, nullptr);

				renderer.UseMaterialInstance(&instanceMatInst);
				renderWorld.DrawInstancedMesh(instCube, instancedVao);


				// Render skybox last
				m_renderer.MutGraphicsDevice().SetPipeline(skyboxPipe);

				renderer.UseMaterialInstance(&skyboxInst);

				// We want the skybox to be centered around the player so that no matter how far the player moves, the skybox won't get any closer.
				// We can achieve this by converting the view matrix to a 3x3 matrix (removing translation) and converting it back to a 4x4 matrix:
				// TODO: it works, but it's a bit ugly. Find a better way to do that... (just zero translation from the view-projection matrix ?)
				Mat4 view = camSys.GetCamera(iCam).GetViewMatrix();
				view[3] = Vec4(0, 0, 0, 1);
				Mat4 proj = camSys.GetCamera(iCam).GetProjectionMatrix();
				Mat4 skyboxViewProj = proj * view;
				skyboxInst.UpdateUniformBlock(MaterialBlockBinding::MATERIAL_SKYBOX_VIEWPROJ, skyboxViewProj);

				renderWorld.DrawMesh(skybox, skyboxVao, nullptr);
			}

			SwapBuffers();
		}
	}
}
