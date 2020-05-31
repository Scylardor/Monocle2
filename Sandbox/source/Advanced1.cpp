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
			VertexLayoutDescriptor::Interleaved
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
			VertexLayoutDescriptor::Interleaved
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
			VertexLayoutDescriptor::Interleaved
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
}
