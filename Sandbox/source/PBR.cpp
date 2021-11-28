#include "TestApplication.h"

#include <glfw3/include/GLFW/glfw3.h>
#include <Graphics/Pipeline/PipelineDescriptor.h>

#include "Math/Vec2.h"
#include <Math/Vec3.h>
#include <Math/Vec4.h>


#include "Core/Resource/Material/Shader/ShaderStage/ShaderStage.h"

#include "Graphics/VertexLayout/VertexLayoutDescriptor.h"

#include "Graphics/Color/Color.h"


#include "Graphics/SceneGraph/SceneGraph.h"


#include "Graphics/Camera/Camera.h"
#include "Graphics/Camera/CameraSystem.h"
#include "Graphics/Light/LightSystem.h"


#include "Graphics/Material/Material.h"

#include "Graphics/Material/MaterialLibrary.h"

#include "Graphics/Sampler/SamplerDescriptor.h"

#include "Graphics/Model/Model.h"

#define _USE_MATH_DEFINES
#include <math.h>


namespace moe
{

	std::pair<Vector<TestApplication::VertexPositionNormalTexture>, Vector<uint32_t>>	TestApplication::CreateSphereGeometry(unsigned int xSegments, unsigned int ySegments)
	{
		// Defining some useful constants.
		// Use inverses so we can multiply by inverse instead of dividing.

		const float invMaxXSegments = 1.f / (float) xSegments;
		const float invMaxYSegments = 1.f / (float) ySegments;
		const float pi = (float) M_PI;
		const unsigned int maxXSegPlusOne = (xSegments + 1);

		// Preallocating the memory based on the wanted sphere fidelity...
		Vector<VertexPositionNormalTexture> sphereGeometry;
		sphereGeometry.Resize((xSegments + 1) * (ySegments + 1) );

		Vector<uint32_t> sphereIndices;
		sphereIndices.Reserve(ySegments * (xSegments + 1) * 2);

		// Now building geometry.
		int index = 0;

		for (unsigned int ySeg = 0; ySeg <= ySegments; ++ySeg)
		{
			for (unsigned int xSeg = 0; xSeg <= xSegments; ++xSeg)
			{
				const float xSegment = xSeg * invMaxXSegments;
				const float ySegment = ySeg * invMaxYSegments;
				const float xPos = std::cosf(xSegment * 2.0f * pi) * std::sinf(ySegment * pi);
				const float yPos = std::cosf(ySegment * pi);
				const float zPos = std::sinf(xSegment * 2.0f * pi) * std::sinf(ySegment * pi);

				sphereGeometry[index] = {
					{xPos, yPos, zPos},
					{xPos, yPos, zPos}, // normals are super easy with a sphere...
					{xSegment, ySegment}
				};

				// Now build indices
				if (ySeg != ySegments)
				{
					if (ySeg & 1) // is odd
					{
						const unsigned int stepX = xSegments - xSeg; // go "in reverse"
						sphereIndices.PushBack((ySeg + 1) * maxXSegPlusOne + stepX);
						sphereIndices.PushBack(ySeg * maxXSegPlusOne + stepX);
					}
					else
					{
						sphereIndices.PushBack(ySeg       * maxXSegPlusOne + xSeg);
						sphereIndices.PushBack((ySeg + 1) * maxXSegPlusOne + xSeg);
					}
				}

				index++;
			}
		}

		return {sphereGeometry, sphereIndices};
	}


	struct PBRParams
	{
		Vec3	m_albedo;
		float	m_metallic = 0;
		float	m_roughness = 0;
		float	m_ao = 0;
		float padding[2];
	};


	void TestApplication::TestPBRLighting()
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
		lib.AddBindingMapping("Material_PBR", { MaterialBlockBinding::MATERIAL_PBR, ResourceKind::UniformBuffer });

		lib.AddBindingMapping("Material_DiffuseMap", { MaterialTextureBinding::DIFFUSE, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_NormalMap", { MaterialTextureBinding::NORMAL, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_SpecularMap", { MaterialTextureBinding::SPECULAR, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_EmissionMap", { MaterialTextureBinding::EMISSION, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_SkyboxMap", { MaterialTextureBinding::SKYBOX, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_HeightMap", { MaterialTextureBinding::HEIGHT, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_BloomMap", { MaterialTextureBinding::BLOOM, ResourceKind::TextureReadOnly });

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

		lib.AddUniformBufferSizer(MaterialBlockBinding::MATERIAL_PBR, []() { return sizeof(PBRParams); });


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


		SamplerDescriptor blurSamplerDesc;
		blurSamplerDesc.m_magFilter = SamplerFilter::Linear;
		blurSamplerDesc.m_minFilter = SamplerFilter::Linear;
		// we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		blurSamplerDesc.m_wrap_S = SamplerWrapping::ClampToEdge;
		blurSamplerDesc.m_wrap_T = SamplerWrapping::ClampToEdge;

		SamplerHandle blurSampler = MutRenderer().MutGraphicsDevice().CreateSampler(blurSamplerDesc);


		PipelineDescriptor pipeDesc;
		pipeDesc.m_depthStencilStateDesc = DepthStencilStateDescriptor{ DepthTest::Enabled, DepthWriting::Enabled, DepthStencilComparisonFunc::Less };
		pipeDesc.m_topology = PrimitiveTopology::TriangleStrip;
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
		IGraphicsRenderer::ShaderFileList pbrFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/pbr_constant.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/pbr_constant.frag" }
		};

		ShaderProgramHandle pbrProgram = renderer.CreateShaderProgramFromSourceFiles(pbrFileList);

		RenderWorld& renderWorld = MutRenderer().CreateRenderWorld();

		// Create geometry
		auto [sphereGeom, sphereIndices] = CreateSphereGeometry(64, 64);
		Mesh* sphere = renderWorld.CreateStaticMesh(sphereGeom, sphereIndices);

		/* Create PBR material buffer */
		PBRParams params{
			Vec3{.5f, 0.f, 0.f},
			0.f,
			0.f,
			1.f
		};

		MaterialDescriptor pbrMatDesc(
			{
				{"Frame_ToneMappingParams", ShaderStage::Fragment},
				{"Material_PBR", ShaderStage::Fragment}
			}
		);
		MaterialInterface pbrInterface = lib.CreateMaterialInterface(pbrProgram, pbrMatDesc);
		MaterialInstance pbrInstance = lib.CreateMaterialInstance(pbrInterface);


		pbrInstance.UpdateUniformBlock(MaterialBlockBinding::FRAME_TONE_MAPPING, ToneMappingParams{true, 1.f, true}); // use reinhard tonemapping

		pbrInstance.UpdateUniformBlock(MaterialBlockBinding::MATERIAL_PBR, params);

		pbrInstance.CreateMaterialResourceSet();


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
			{ -10.0f,  10.0f, 10.0f, 1.f},
			{  10.0f,  10.0f, 10.0f, 1.f},
			{ -10.0f, -10.0f, 10.0f, 1.f},
			{  10.0f, -10.0f, 10.0f, 1.f}
		};

		Vec4 lightColors[] = {
			{ 300.0f, 300.0f, 300.0f, 1.f},
			{ 300.0f, 300.0f, 300.0f, 1.f},
			{ 300.0f, 300.0f, 300.0f, 1.f},
			{ 300.0f, 300.0f, 300.0f, 1.f}
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

		m_renderer.MutGraphicsDevice().SetPipeline(myPipe);

		const int nrRows = 7;
		const int nrColumns = 7;
		const float spacing = 2.5f;

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

			lightsSystem.UpdateLights();

			lightsSystem.BindLightBuffer();

			camSys.UpdateCameras();

			for (uint32_t iCam = 0; iCam < camSys.CamerasNumber(); iCam++)
			{
				camSys.BindCameraBuffer(iCam);

				renderer.UseMaterialInstance(&pbrInstance);

				// render rows*column number of spheres with varying metallic/roughness values scaled by rows and columns respectively

				for (int row = 0; row < nrRows; ++row)
				{
					params.m_metallic = row / (float)nrRows;

					for (int col = 0; col < nrColumns; ++col)
					{
						// we clamp the roughness to 0.025 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
						// on direct lighting.
						params.m_roughness = glm::clamp(col / (float)nrColumns, 0.05f, 1.f);

						pbrInstance.UpdateUniformBlock(MATERIAL_PBR, params);

						sphere->SetTransform(Transform::Identity());
						sphere->AddTransform(Transform::Translate({
							(col - (nrColumns / 2)) * spacing,
							(row - (nrRows / 2)) * spacing,
							0.0f
						}));

						sphere->UpdateObjectMatrices(camSys.GetCamera(iCam));

						renderWorld.DrawMesh(sphere, cubeVao);
					}
				}

				iLight = 0;
				for (auto& lightPos : lightPositions)
				{
					sphere->SetTransform(Transform::Identity());
					sphere->AddTransform(Transform::Translate(lightPos.xyz()));
					sphere->AddTransform(Transform::Scale(Vec3{0.5f}));


					sphere->UpdateObjectMatrices(camSys.GetCamera(iCam));

					renderWorld.DrawMesh(sphere, cubeVao);

					iLight++;
				}
			}

			SwapBuffers();

		}
	}


	void TestApplication::TestPBRTextured()
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
		lib.AddBindingMapping("Material_PBR", { MaterialBlockBinding::MATERIAL_PBR, ResourceKind::UniformBuffer });

		lib.AddBindingMapping("Material_DiffuseMap", { MaterialTextureBinding::DIFFUSE, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_NormalMap", { MaterialTextureBinding::NORMAL, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_SpecularMap", { MaterialTextureBinding::SPECULAR, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_EmissionMap", { MaterialTextureBinding::EMISSION, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_SkyboxMap", { MaterialTextureBinding::SKYBOX, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_HeightMap", { MaterialTextureBinding::HEIGHT, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_BloomMap", { MaterialTextureBinding::BLOOM, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_AmbientOcclusion", { MaterialTextureBinding::AMBIENT_OCCLUSION, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_MetallicMap", { MaterialTextureBinding::METALLIC, ResourceKind::TextureReadOnly });

		lib.AddBindingMapping("Material_Sampler", { MaterialSamplerBinding::SAMPLER_0, ResourceKind::Sampler });

		lib.AddUniformBufferSizer(MaterialBlockBinding::FRAME_LIGHTS, []() { return sizeof(LightCastersData); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::VIEW_CAMERA, []() { return sizeof(CameraMatrices); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::MATERIAL_SKYBOX_VIEWPROJ, []() { return sizeof(Mat4); });

		lib.AddUniformBufferSizer(MaterialBlockBinding::MATERIAL_PHONG, []() { return sizeof(PhongMaterial); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::MATERIAL_COLOR, []() { return sizeof(ColorRGBAf); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::OBJECT_MATRICES, []() { return sizeof(ObjectMatrices); });

		lib.AddUniformBufferSizer(MaterialBlockBinding::FRAME_TONE_MAPPING, []() { return sizeof(ToneMappingParams); });

		lib.AddUniformBufferSizer(MaterialBlockBinding::FRAME_GAUSSIAN_BLUR, []() { return sizeof(TwoPassGaussianBlurParams); });

		lib.AddUniformBufferSizer(MaterialBlockBinding::MATERIAL_PBR, []() { return sizeof(PBRParams); });


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


		SamplerDescriptor blurSamplerDesc;
		blurSamplerDesc.m_magFilter = SamplerFilter::Linear;
		blurSamplerDesc.m_minFilter = SamplerFilter::Linear;
		// we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		blurSamplerDesc.m_wrap_S = SamplerWrapping::ClampToEdge;
		blurSamplerDesc.m_wrap_T = SamplerWrapping::ClampToEdge;

		SamplerHandle blurSampler = MutRenderer().MutGraphicsDevice().CreateSampler(blurSamplerDesc);


		PipelineDescriptor pipeDesc;
		pipeDesc.m_depthStencilStateDesc = DepthStencilStateDescriptor{ DepthTest::Enabled, DepthWriting::Enabled, DepthStencilComparisonFunc::Less };
		pipeDesc.m_topology = PrimitiveTopology::TriangleStrip;
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
		IGraphicsRenderer::ShaderFileList pbrFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/pbr_textured.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/pbr_textured.frag" }
		};

		ShaderProgramHandle pbrProgram = renderer.CreateShaderProgramFromSourceFiles(pbrFileList);

		RenderWorld& renderWorld = MutRenderer().CreateRenderWorld();

		// Create geometry
		auto[sphereGeom, sphereIndices] = CreateSphereGeometry(64, 64);
		Mesh* sphere = renderWorld.CreateStaticMesh(sphereGeom, sphereIndices);

		/* Create PBR material buffer */
		PBRParams params{
			Vec3{.5f, 0.f, 0.f},
			0.f,
			0.f,
			1.f
		};

		// Albedo is SRGBA, normals/AO are RGB, roughness/metalness are R8.
		Texture2DFileDescriptor albedoDesc{ "Sandbox/assets/textures/pbr/rusted_iron/albedo.png" };
		albedoDesc.m_targetFormat = TextureFormat::SRGB_RGBA8;
		albedoDesc.m_wantedMipmapLevels = 12;
		Texture2DHandle albedoImg = MutRenderer().MutGraphicsDevice().CreateTexture2D(albedoDesc);

		Texture2DFileDescriptor texDesc{ "Sandbox/assets/textures/pbr/rusted_iron/normal.png" };
		texDesc.m_targetFormat = TextureFormat::RGB8;
		texDesc.m_wantedMipmapLevels = 12;
		Texture2DHandle normalsImg = MutRenderer().MutGraphicsDevice().CreateTexture2D(texDesc);

		// I don't know why but the AO texture *specifically* has to have 10 mips. 12 mips makes OpenGL balk, although it works for all the other textures. Why ???
		texDesc.m_wantedMipmapLevels = 10;
		texDesc.m_filename = "Sandbox/assets/textures/pbr/rusted_iron/ao.png";
		Texture2DHandle aoImg = MutRenderer().MutGraphicsDevice().CreateTexture2D(texDesc);

		texDesc.m_wantedMipmapLevels = 12;
		texDesc.m_targetFormat = TextureFormat::R8;

		texDesc.m_filename = "Sandbox/assets/textures/pbr/rusted_iron/roughness.png";
		Texture2DHandle roughImg = MutRenderer().MutGraphicsDevice().CreateTexture2D(texDesc);

		texDesc.m_filename = "Sandbox/assets/textures/pbr/rusted_iron/metallic.png";
		Texture2DHandle metalImg = MutRenderer().MutGraphicsDevice().CreateTexture2D(texDesc);

		SamplerDescriptor mySamplerDesc;
		mySamplerDesc.m_magFilter = SamplerFilter::Linear;
		mySamplerDesc.m_minFilter = SamplerFilter::LinearMipmapLinear;
		mySamplerDesc.m_wrap_S = SamplerWrapping::Repeat;
		mySamplerDesc.m_wrap_T = SamplerWrapping::Repeat;

		SamplerHandle mySampler = MutRenderer().MutGraphicsDevice().CreateSampler(mySamplerDesc);


		MaterialDescriptor pbrMatDesc(
			{
				{"Frame_ToneMappingParams", ShaderStage::Fragment},
				{"Material_Sampler", ShaderStage::Fragment},
				{"Material_DiffuseMap", ShaderStage::Fragment},
				{"Material_NormalMap", ShaderStage::Fragment},
				{"Material_SpecularMap", ShaderStage::Fragment},
				{"Material_MetallicMap", ShaderStage::Fragment},
				{"Material_AmbientOcclusion", ShaderStage::Fragment}
			}
		);
		MaterialInterface pbrInterface = lib.CreateMaterialInterface(pbrProgram, pbrMatDesc);
		MaterialInstance pbrInstance = lib.CreateMaterialInstance(pbrInterface);


		pbrInstance.UpdateUniformBlock(MaterialBlockBinding::FRAME_TONE_MAPPING, ToneMappingParams{ true, 1.f, true }); // use reinhard tonemapping

		pbrInstance.BindSampler(MaterialSamplerBinding::SAMPLER_0, mySampler);

		pbrInstance.BindTexture(DIFFUSE, albedoImg);
		pbrInstance.BindTexture(NORMAL, normalsImg);
		pbrInstance.BindTexture(SPECULAR, roughImg);
		pbrInstance.BindTexture(METALLIC, metalImg);
		pbrInstance.BindTexture(AMBIENT_OCCLUSION, aoImg);

		pbrInstance.CreateMaterialResourceSet();


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
			{ 0,  0, 10.0f, 1.f},
		//	{  10.0f,  10.0f, 10.0f, 1.f},
		//	{ -10.0f, -10.0f, 10.0f, 1.f},
		//	{  10.0f, -10.0f, 10.0f, 1.f}
		};

		Vec4 lightColors[] = {
			{ 150.f, 150.f, 150.f, 1.f},
		//	{ 300.0f, 300.0f, 300.0f, 1.f},
		//	{ 300.0f, 300.0f, 300.0f, 1.f},
		//	{ 300.0f, 300.0f, 300.0f, 1.f}
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

		m_renderer.MutGraphicsDevice().SetPipeline(myPipe);

		const int nrRows = 7;
		const int nrColumns = 7;
		const float spacing = 2.5f;

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

			lightsSystem.UpdateLights();

			lightsSystem.BindLightBuffer();

			camSys.UpdateCameras();

			for (uint32_t iCam = 0; iCam < camSys.CamerasNumber(); iCam++)
			{
				camSys.BindCameraBuffer(iCam);

				renderer.UseMaterialInstance(&pbrInstance);

				// render rows*column number of spheres with varying metallic/roughness values scaled by rows and columns respectively

				for (int row = 0; row < nrRows; ++row)
				{
					for (int col = 0; col < nrColumns; ++col)
					{
						sphere->SetTransform(Transform::Identity());
						sphere->AddTransform(Transform::Translate({
							(col - (nrColumns / 2)) * spacing,
							(row - (nrRows / 2)) * spacing,
							0.0f
							}));

						sphere->UpdateObjectMatrices(camSys.GetCamera(iCam));

						renderWorld.DrawMesh(sphere, cubeVao);
					}
				}

				iLight = 0;
				for (auto& lightPos : lightPositions)
				{
					sphere->SetTransform(Transform::Identity());
					sphere->AddTransform(Transform::Translate(lightPos.xyz()));
					sphere->AddTransform(Transform::Scale(Vec3{ 0.5f }));


					sphere->UpdateObjectMatrices(camSys.GetCamera(iCam));

					renderWorld.DrawMesh(sphere, cubeVao);

					iLight++;
				}
			}

			SwapBuffers();

		}
	}


	void TestApplication::TestPBRIrradianceConversion()
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
		lib.AddBindingMapping("Material_PBR", { MaterialBlockBinding::MATERIAL_PBR, ResourceKind::UniformBuffer });

		lib.AddBindingMapping("Material_DiffuseMap", { MaterialTextureBinding::DIFFUSE, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_NormalMap", { MaterialTextureBinding::NORMAL, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_SpecularMap", { MaterialTextureBinding::SPECULAR, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_EmissionMap", { MaterialTextureBinding::EMISSION, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_SkyboxMap", { MaterialTextureBinding::SKYBOX, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_HeightMap", { MaterialTextureBinding::HEIGHT, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_BloomMap", { MaterialTextureBinding::BLOOM, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_AmbientOcclusion", { MaterialTextureBinding::AMBIENT_OCCLUSION, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_MetallicMap", { MaterialTextureBinding::METALLIC, ResourceKind::TextureReadOnly });
		lib.AddBindingMapping("Material_RoughnessMap", { MaterialTextureBinding::ROUGHNESS, ResourceKind::TextureReadOnly });

		lib.AddBindingMapping("Material_Sampler", { MaterialSamplerBinding::SAMPLER_0, ResourceKind::Sampler });
		lib.AddBindingMapping("Material_Sampler2", { MaterialSamplerBinding::SAMPLER_1, ResourceKind::Sampler });
		lib.AddBindingMapping("Material_Sampler3", { MaterialSamplerBinding::SAMPLER_2, ResourceKind::Sampler });

		lib.AddUniformBufferSizer(MaterialBlockBinding::FRAME_LIGHTS, []() { return sizeof(LightCastersData); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::VIEW_CAMERA, []() { return sizeof(CameraMatrices); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::MATERIAL_SKYBOX_VIEWPROJ, []() { return sizeof(Mat4); });

		lib.AddUniformBufferSizer(MaterialBlockBinding::MATERIAL_PHONG, []() { return sizeof(PhongMaterial); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::MATERIAL_COLOR, []() { return sizeof(ColorRGBAf); });
		lib.AddUniformBufferSizer(MaterialBlockBinding::OBJECT_MATRICES, []() { return sizeof(ObjectMatrices); });

		lib.AddUniformBufferSizer(MaterialBlockBinding::FRAME_TONE_MAPPING, []() { return sizeof(ToneMappingParams); });

		lib.AddUniformBufferSizer(MaterialBlockBinding::FRAME_GAUSSIAN_BLUR, []() { return sizeof(TwoPassGaussianBlurParams); });

		lib.AddUniformBufferSizer(MaterialBlockBinding::MATERIAL_PBR, []() { return sizeof(PBRParams); });


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


		SamplerDescriptor radianceSamplerDesc;
		radianceSamplerDesc.m_magFilter = SamplerFilter::Linear;
		radianceSamplerDesc.m_minFilter = SamplerFilter::Linear;
		// we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		radianceSamplerDesc.m_wrap_S = SamplerWrapping::ClampToEdge;
		radianceSamplerDesc.m_wrap_T = SamplerWrapping::ClampToEdge;
		radianceSamplerDesc.m_wrap_R = SamplerWrapping::ClampToEdge;

		SamplerHandle radianceSampler = MutRenderer().MutGraphicsDevice().CreateSampler(radianceSamplerDesc);


		PipelineDescriptor pipeDesc;
		pipeDesc.m_depthStencilStateDesc = DepthStencilStateDescriptor{ DepthTest::Enabled, DepthWriting::Enabled, DepthStencilComparisonFunc::Less };
		pipeDesc.m_topology = PrimitiveTopology::TriangleStrip;
		pipeDesc.m_rasterizerStateDesc.m_cullMode = CullFace::None;
		PipelineHandle myPipe = m_renderer.MutGraphicsDevice().CreatePipeline(pipeDesc);

		pipeDesc.m_depthStencilStateDesc.m_depthFunc = DepthStencilComparisonFunc::LessEqual;
		PipelineHandle scenePipe = m_renderer.MutGraphicsDevice().CreatePipeline(pipeDesc);

		pipeDesc.m_topology = PrimitiveTopology::TriangleList;
		PipelineHandle skyboxPipe = m_renderer.MutGraphicsDevice().CreatePipeline(pipeDesc);

		/* Create  VAOs */
		VertexLayoutDescriptor sphereLayout{
			{
				{{"position", VertexElementFormat::Float3},
				{"normal", VertexElementFormat::Float3},
				{"texCoords", VertexElementFormat::Float2}},
			},
			LayoutType::Interleaved
		};
		auto sphereVao = renderer.CreateVertexLayout(sphereLayout);

		VertexLayoutDescriptor cubeLayout{
			{{"position", VertexElementFormat::Float3}},
			LayoutType::Interleaved
		};
		auto cubeVao = renderer.CreateVertexLayout(cubeLayout);


		VertexLayoutDescriptor quadLayout{
	{
		{	{"position", VertexElementFormat::Float3},
			{"texCoords", VertexElementFormat::Float2}},
			},
			LayoutType::Interleaved
		};
		auto quadVao = renderer.CreateVertexLayout(quadLayout);

		const uint32_t environmentMapSize = 512;

		/* Load the HDR Radiance map */
		Texture2DFileDescriptor radianceMapDesc{ "Sandbox/assets/textures/hdr/newport_loft.hdr" };
		radianceMapDesc.m_targetFormat = TextureFormat::RGBE;
		Texture2DHandle radianceImg = MutRenderer().MutGraphicsDevice().CreateTexture2D(radianceMapDesc);

		/* Create the cube map that will host the environment map */
		CubeMapTextureDescriptor envMapTexDesc{nullptr, Width_t(environmentMapSize), Height_t(environmentMapSize)};
		envMapTexDesc.m_targetFormat = TextureFormat::RGBE;
		envMapTexDesc.m_wantedMipmapLevels = 10;
		TextureHandle envMapTex = MutRenderer().MutGraphicsDevice().CreateCubemapTexture(envMapTexDesc);

		// TODO: The up vector is flipped because we told the STBI image importer to vertical flip the images on import.
		// TODO: would there be a better way to do this than to render the cube six times using each face as color attachment ? Maybe attach all 6 faces as color attachments at once ?
		// cf. : https://stackoverflow.com/questions/462721/rendering-to-cube-map
		// https://www.khronos.org/opengl/wiki/Geometry_Shader#Layered_rendering
		struct LookatDesc
		{
			Vec3 position;
			Vec3 target;
			Vec3 up;
		};

		const LookatDesc hdrCaptureViewMatrices[6] = {
			{Vec3::ZeroVector(), { 1.0f,  0.0f,  0.0f}, {0.0f, -1.0f,  0.0f}},
			{Vec3::ZeroVector(), {-1.0f,  0.0f,  0.0f}, {0.0f, -1.0f,  0.0f}},
			{Vec3::ZeroVector(), { 0.0f,  1.0f,  0.0f}, {0.0f,  0.0f,  1.0f}},
			{Vec3::ZeroVector(), { 0.0f, -1.0f,  0.0f}, {0.0f,  0.0f, -1.0f}},
			{Vec3::ZeroVector(), { 0.0f,  0.0f,  1.0f}, {0.0f, -1.0f,  0.0f}},
			{Vec3::ZeroVector(), { 0.0f,  0.0f, -1.0f}, {0.0f, -1.0f,  0.0f}}
		};


		/* Create all necessary shaders */

		IGraphicsRenderer::ShaderFileList pbrFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/pbr_constant.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/pbr_textured_IBL.frag" }
		};

		ShaderProgramHandle pbrProgram = renderer.CreateShaderProgramFromSourceFiles(pbrFileList);


		IGraphicsRenderer::ShaderFileList irradianceConvolutionFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/pbr_cubemap.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/pbr_irradiance_convolution.frag" }
		};

		ShaderProgramHandle irradianceConvolutionProgram = renderer.CreateShaderProgramFromSourceFiles(irradianceConvolutionFileList);


		IGraphicsRenderer::ShaderFileList equiRectToCubeMapFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/pbr_cubemap.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/pbr_equirectangular_to_cubemap.frag" }
		};

		ShaderProgramHandle equiRectToCubeMapProgram = renderer.CreateShaderProgramFromSourceFiles(equiRectToCubeMapFileList);

		MaterialDescriptor equiRectToCubeMapDesc(
			{
				{"Material_Sampler", ShaderStage::Fragment},
				{"Material_DiffuseMap", ShaderStage::Fragment}
			}
		);

		MaterialInterface equiRectToCubeMapInterface = lib.CreateMaterialInterface(equiRectToCubeMapProgram, equiRectToCubeMapDesc);
		MaterialInstance equiRectToCubeMapInstance = lib.CreateMaterialInstance(equiRectToCubeMapInterface);

		equiRectToCubeMapInstance.BindSampler(SAMPLER_0, radianceSampler);
		equiRectToCubeMapInstance.BindTexture(DIFFUSE, radianceImg);
		equiRectToCubeMapInstance.CreateMaterialResourceSet();


		IGraphicsRenderer::ShaderFileList backgroundFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/pbr_hdr_background.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/pbr_hdr_background.frag" }
		};

		ShaderProgramHandle backgroundProgram = renderer.CreateShaderProgramFromSourceFiles(backgroundFileList);


		RenderWorld& renderWorld = MutRenderer().CreateRenderWorld();

		// Create geometry
		auto[sphereGeom, sphereIndices] = CreateSphereGeometry(64, 64);
		Mesh* sphere = renderWorld.CreateStaticMesh(sphereGeom, sphereIndices);

		auto cubeGeom = CreateCube(1.f);
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

		Vec4 lightPositions[] = {
			{ -10.0f,  10.0f, 10.0f, 1.f},
			{  10.0f,  10.0f, 10.0f, 1.f},
			{ -10.0f, -10.0f, 10.0f, 1.f},
			{  10.0f, -10.0f, 10.0f, 1.f}
		};

		Vec4 lightColors[] = {
			{ 300.0f, 300.0f, 300.0f, 1.f},
			{ 300.0f, 300.0f, 300.0f, 1.f},
			{ 300.0f, 300.0f, 300.0f, 1.f},
			{ 300.0f, 300.0f, 300.0f, 1.f}
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

		m_renderer.MutGraphicsDevice().SetPipeline(myPipe);

		// First generate the skybox cubemap
		ViewportHandle conversionViewportHandle = m_renderer.MutGraphicsDevice().CreateViewport(ViewportDescriptor(0, 0, environmentMapSize, environmentMapSize));
		PerspectiveCameraDesc conversionCapturePerspective{ 90_degf, 1, 0.1f, 10.f };

		Camera* conversionCam = camSys.AddNewCamera(conversionViewportHandle, conversionCapturePerspective);

		/* Create the framebuffer to do the equirectangular texture-to-cubemap conversion */
		Texture2DDescriptor depthAttachDesc{ nullptr, Width_t(environmentMapSize), Height_t(environmentMapSize), TextureFormat::Depth24, TextureUsage{RenderTarget} };
		Texture2DHandle depthAttachTex = renderer.MutGraphicsDevice().CreateTexture2D(depthAttachDesc);

		FramebufferDescriptor fbDesc;
		fbDesc.m_depthAttachment = depthAttachTex;
		fbDesc.m_doCompletenessCheck = CompleteCheck::Disabled; // otherwise it will yell the framebuffer is not complete upon creation
		FramebufferHandle fbHandle = renderer.MutGraphicsDevice().CreateFramebuffer(fbDesc);

		AFramebuffer* conversionFramebuffer = renderer.MutGraphicsDevice().MutFramebuffer(fbHandle);

		renderer.BindFramebuffer(fbHandle);

		for (unsigned int iCapture = 0; iCapture < 6; ++iCapture)
		{
			// Change the view matrix to turn to a new face
			conversionCam->LookAt(hdrCaptureViewMatrices[iCapture].target, hdrCaptureViewMatrices[iCapture].up);

			camSys.UpdateCameras();

			camSys.BindCameraBuffer(conversionCam->GetCameraIndex());

			// Render to a different face of the cube map now
			conversionFramebuffer->BindColorAttachment(0, envMapTex, 0, true, iCapture);

			renderer.Clear(ColorRGBAf(0.1f, 0.1f, 0.1f, 1.0f));

			renderer.UseMaterialInstance(&equiRectToCubeMapInstance);

			cube->UpdateObjectMatrices(*conversionCam);

			renderWorld.DrawMesh(cube, cubeVao);
		}

		// then let the renderer generate mipmaps from first mip face (to generate prefiltered env map)
		MutRenderer().MutGraphicsDevice().GenerateTextureMipmaps(envMapTex);


		// Create the BRDF convolution LUT (while the framebuffer is still at the right size)
		// --------------------------------------------------------------------------------

		const uint32_t brdfLUTsize = 512;

		Texture2DDescriptor brdfLutTexDesc;
		brdfLutTexDesc.m_width = Width_t(brdfLUTsize);
		brdfLutTexDesc.m_height = Height_t(brdfLUTsize);
		brdfLutTexDesc.m_targetFormat = TextureFormat::RG16F;
		Texture2DHandle brdfLutTex = MutRenderer().MutGraphicsDevice().CreateTexture2D(brdfLutTexDesc);

		IGraphicsRenderer::ShaderFileList brdfLutShaderFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/brdf_lut.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/brdf_lut.frag" }
		};

		ShaderProgramHandle brdfLUTProgram = renderer.CreateShaderProgramFromSourceFiles(brdfLutShaderFileList);

		MaterialDescriptor brdfLutMatDesc; // empty

		MaterialInterface brdfLutMatInterface = lib.CreateMaterialInterface(brdfLUTProgram, brdfLutMatDesc);
		MaterialInstance brdfLutMatInstance = lib.CreateMaterialInstance(brdfLutMatInterface);

		brdfLutMatInstance.CreateMaterialResourceSet();

		renderer.BindFramebuffer(fbHandle);

		conversionFramebuffer->BindColorAttachment(0, brdfLutTex);

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

		renderer.Clear(ColorRGBAf(0.1f, 0.1f, 0.1f, 1.0f));

		camSys.BindCameraBuffer(conversionCam->GetCameraIndex());

		renderer.UseMaterialInstance(&brdfLutMatInstance);

		renderWorld.DrawMesh(fullscreenQuad, quadVao);

		// Now, create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
		// --------------------------------------------------------------------------------

		const uint32_t irradianceMapSize = 32;

		/* Create the irradiance cube map */
		CubeMapTextureDescriptor irrMapTexDesc{ nullptr, Width_t(irradianceMapSize), Height_t(irradianceMapSize) };
		irrMapTexDesc.m_targetFormat = TextureFormat::RGBE;
		TextureHandle irrMapTex = MutRenderer().MutGraphicsDevice().CreateCubemapTexture(irrMapTexDesc);

		// TODO: Just update the frame buffer depth texture to resize it. Yes, this is a bit ugly... The previous depth attachment gets leaked... help !!!
		depthAttachDesc.m_width = Width_t(irradianceMapSize);
		depthAttachDesc.m_height = Height_t(irradianceMapSize);
		Texture2DHandle depthAttachTex32 = renderer.MutGraphicsDevice().CreateTexture2D(depthAttachDesc);

		conversionFramebuffer->BindDepthAttachment(depthAttachTex32);

		MaterialDescriptor irradianceConvoDesc(
			{
				{"Material_Sampler", ShaderStage::Fragment},
				{"Material_DiffuseMap", ShaderStage::Fragment}
			}
		);

		MaterialInterface irradianceConvoInterface = lib.CreateMaterialInterface(irradianceConvolutionProgram, irradianceConvoDesc);
		MaterialInstance irradianceConvoInstance = lib.CreateMaterialInstance(irradianceConvoInterface);


		irradianceConvoInstance.BindSampler(SAMPLER_0, radianceSampler);
		irradianceConvoInstance.BindTexture(DIFFUSE, envMapTex);
		irradianceConvoInstance.CreateMaterialResourceSet();

		ViewportHandle irradianceViewportHandle = m_renderer.MutGraphicsDevice().CreateViewport(ViewportDescriptor(0, 0, irradianceMapSize, irradianceMapSize));

		Camera* irradianceCam = camSys.AddNewCamera(irradianceViewportHandle, conversionCapturePerspective);


		for (unsigned int iCapture = 0; iCapture < 6; ++iCapture)
		{
			// Change the view matrix to turn to a new face
			irradianceCam->LookAt(hdrCaptureViewMatrices[iCapture].target, hdrCaptureViewMatrices[iCapture].up);

			camSys.UpdateCameras();

			camSys.BindCameraBuffer(irradianceCam->GetCameraIndex());

			// Render to a different face of the cube map now
			conversionFramebuffer->BindColorAttachment(0, irrMapTex, 0, true, iCapture);

			renderer.Clear(ColorRGBAf(0.1f, 0.1f, 0.1f, 1.0f));

			renderer.UseMaterialInstance(&irradianceConvoInstance);

			cube->UpdateObjectMatrices(*irradianceCam);

			renderWorld.DrawMesh(cube, cubeVao);
		}

		/* Create the prefiltered environment map for PBR specular reflections. We want mipmaps for this one */
		// --------------------------------------------------------------------------------

		const uint32_t prefilterMapSize = 128;

		irrMapTexDesc.m_width = 128_width;
		irrMapTexDesc.m_height = 128_height;
		irrMapTexDesc.m_wantedMipmapLevels = 8;
		TextureHandle prefilterMapTex = MutRenderer().MutGraphicsDevice().CreateCubemapTexture(irrMapTexDesc);

		SamplerDescriptor envMapSamplerDesc;
		envMapSamplerDesc.m_magFilter = SamplerFilter::Linear;
		envMapSamplerDesc.m_minFilter = SamplerFilter::LinearMipmapLinear; // ensures maximum fidelity when sampling mipmaps.
		// we clamp to the edge as the filter would otherwise sample repeated texture values!
		envMapSamplerDesc.m_wrap_S = SamplerWrapping::ClampToEdge;
		envMapSamplerDesc.m_wrap_T = SamplerWrapping::ClampToEdge;
		envMapSamplerDesc.m_wrap_R = SamplerWrapping::ClampToEdge;

		SamplerHandle envMapSampler = MutRenderer().MutGraphicsDevice().CreateSampler(envMapSamplerDesc);

		IGraphicsRenderer::ShaderFileList prefilterShaderFileList =
		{
			{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/pbr_cubemap.vert" },
			{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/pbr_prefilter_environment.frag" }
		};

		ShaderProgramHandle prefilterProgram = renderer.CreateShaderProgramFromSourceFiles(prefilterShaderFileList);

		/* Create PBR material buffer */
		PBRParams prefilterParams{
			Vec3{.5f, 0.f, 0.f},
			0.f,
			0.f,
			1.f
		};

		MaterialDescriptor prefilterEnvMatDesc(
			{
				{"Material_Sampler", ShaderStage::Fragment},
				{"Material_DiffuseMap", ShaderStage::Fragment},
				{"Material_PBR", ShaderStage::Fragment}
			}
		);

		MaterialInterface prefilterEnvMatInterface = lib.CreateMaterialInterface(prefilterProgram, prefilterEnvMatDesc);
		MaterialInstance prefilterEnvMatInstance = lib.CreateMaterialInstance(prefilterEnvMatInterface);

		prefilterEnvMatInstance.BindSampler(SAMPLER_0, envMapSampler);

		prefilterEnvMatInstance.UpdateUniformBlock(MATERIAL_PBR, prefilterParams);

		prefilterEnvMatInstance.BindTexture(DIFFUSE, envMapTex);

		prefilterEnvMatInstance.CreateMaterialResourceSet();

		const uint32_t maxPrefilteredMips = 5;
		for (int iMip = 0; iMip < maxPrefilteredMips; ++iMip)
		{
			// TODO: warning, created viewports get leaked here... erf. Our current API basically only allows to create them, not removing them.
			// And we cannot reassign the viewport of a camera, so we have to create a new camera each time.

			// reisze framebuffer according to mip-level size.
			unsigned int mipWidth  = unsigned int(128.0 * std::pow(0.5, iMip));
			unsigned int mipHeight = unsigned int(128.0 * std::pow(0.5, iMip));
			ViewportHandle prefilterViewportHandle = m_renderer.MutGraphicsDevice().CreateViewport(ViewportDescriptor(0, 0, (float)mipWidth, (float)mipHeight));

			depthAttachDesc.m_width = Width_t(mipWidth);
			depthAttachDesc.m_height = Height_t(mipHeight);
			Texture2DHandle mipDepthAttachment = renderer.MutGraphicsDevice().CreateTexture2D(depthAttachDesc);

			conversionFramebuffer->BindDepthAttachment(mipDepthAttachment);

			Camera* prefilterCam = camSys.AddNewCamera(prefilterViewportHandle, conversionCapturePerspective);

			const float mipRoughness = (float)iMip / (float)(maxPrefilteredMips - 1);

			prefilterParams.m_roughness = mipRoughness;
			prefilterEnvMatInstance.UpdateUniformBlock(MATERIAL_PBR, prefilterParams);

			for (unsigned int iCapture = 0; iCapture < 6; ++iCapture)
			{
				// Change the view matrix to turn to a new face
				prefilterCam->LookAt(hdrCaptureViewMatrices[iCapture].target, hdrCaptureViewMatrices[iCapture].up);

				camSys.UpdateCameras();

				camSys.BindCameraBuffer(prefilterCam->GetCameraIndex());

				// Render to a different face of the cube map now
				conversionFramebuffer->BindColorAttachment(0, prefilterMapTex, iMip, true, iCapture);

				renderer.Clear(ColorRGBAf(0.1f, 0.1f, 0.1f, 1.0f));

				renderer.UseMaterialInstance(&prefilterEnvMatInstance);

				cube->UpdateObjectMatrices(*prefilterCam);

				renderWorld.DrawMesh(cube, cubeVao);
			}

			// We don't need these cameras afterwards ; just remove them
			camSys.RemoveCamera(prefilterCam);
		}



		renderer.UnbindFramebuffer(fbHandle);

		// We don't need these cameras afterwards ; just remove them
		camSys.RemoveCamera(conversionCam);
		camSys.RemoveCamera(irradianceCam);


		MaterialDescriptor backgroundMatDesc(
			{
				{"Material_DiffuseMap", ShaderStage::Fragment}
			}
		);

		MaterialInterface backgroundMatInterface = lib.CreateMaterialInterface(backgroundProgram, backgroundMatDesc);
		MaterialInstance backgroundMatInstance = lib.CreateMaterialInstance(backgroundMatInterface);

		backgroundMatInstance.BindTexture(DIFFUSE, envMapTex);
		backgroundMatInstance.CreateMaterialResourceSet();



		SamplerDescriptor pbrMatSamplerDesc;
		pbrMatSamplerDesc.m_magFilter = SamplerFilter::Linear;
		pbrMatSamplerDesc.m_minFilter = SamplerFilter::LinearMipmapLinear; // ensures maximum fidelity when sampling mipmaps.
		// we clamp to the edge as the filter would otherwise sample repeated texture values!
		pbrMatSamplerDesc.m_wrap_S = SamplerWrapping::Repeat;
		pbrMatSamplerDesc.m_wrap_T = SamplerWrapping::Repeat;
		pbrMatSamplerDesc.m_wrap_R = SamplerWrapping::Repeat;

		SamplerHandle pbrMatSampler = MutRenderer().MutGraphicsDevice().CreateSampler(pbrMatSamplerDesc);


		/* Create PBR material buffer */
		PBRParams params{
			Vec3{.5f, 0.f, 0.f},
			0.f,
			0.f,
			1.f
		};

		MaterialDescriptor pbrMatDesc(
			{
				{"Frame_ToneMappingParams", ShaderStage::Fragment},
				{"Material_PBR", ShaderStage::Fragment},
				{"Material_Sampler",  ShaderStage::Fragment},
				{"Material_DiffuseMap", ShaderStage::Fragment},
				{"Material_SpecularMap", ShaderStage::Fragment},
				{"Material_Sampler2",  ShaderStage::Fragment},
				{"Material_SkyboxMap", ShaderStage::Fragment},
				{"Material_Sampler3",  ShaderStage::Fragment},
				{"Material_EmissionMap", ShaderStage::Fragment},
				{"Material_NormalMap", ShaderStage::Fragment},
				{"Material_MetallicMap", ShaderStage::Fragment},
				{"Material_RoughnessMap", ShaderStage::Fragment},
				{"Material_AmbientOcclusion", ShaderStage::Fragment},
			}
		);
		MaterialInterface pbrInterface = lib.CreateMaterialInterface(pbrProgram, pbrMatDesc);

		MaterialInstance pbrInstance = lib.CreateMaterialInstance(pbrInterface);

		// rusty iron

		MaterialInstance rustyIronInstance = lib.CreateMaterialInstance(pbrInterface);

		rustyIronInstance.UpdateUniformBlock(MaterialBlockBinding::FRAME_TONE_MAPPING, ToneMappingParams{ true, 1.f, true }); // use reinhard tonemapping

		rustyIronInstance.UpdateUniformBlock(MaterialBlockBinding::MATERIAL_PBR, params);

		rustyIronInstance.BindSampler(SAMPLER_0, radianceSampler);

		rustyIronInstance.BindTexture(DIFFUSE, irrMapTex);
		rustyIronInstance.BindTexture(SPECULAR, brdfLutTex);

		rustyIronInstance.BindSampler(SAMPLER_1, envMapSampler);

		rustyIronInstance.BindTexture(SKYBOX, prefilterMapTex);

		rustyIronInstance.BindSampler(SAMPLER_2, pbrMatSampler);

		Texture2DFileDescriptor pbrAlbedoTexDesc;
		pbrAlbedoTexDesc.m_targetFormat = TextureFormat::SRGB_RGBA8;

		Texture2DFileDescriptor pbrMatTexDesc;
		pbrAlbedoTexDesc.m_filename = "Sandbox/assets/textures/pbr/rusted_iron/albedo.png";
		Texture2DHandle albedoIron = m_renderer.MutGraphicsDevice().CreateTexture2D(pbrAlbedoTexDesc);

		pbrMatTexDesc.m_filename = "Sandbox/assets/textures/pbr/rusted_iron/normal.png";
		Texture2DHandle normalIron = m_renderer.MutGraphicsDevice().CreateTexture2D(pbrMatTexDesc);

		pbrMatTexDesc.m_filename = "Sandbox/assets/textures/pbr/rusted_iron/metallic.png";
		Texture2DHandle metallicIron = m_renderer.MutGraphicsDevice().CreateTexture2D(pbrMatTexDesc);

		pbrMatTexDesc.m_filename = "Sandbox/assets/textures/pbr/rusted_iron/roughness.png";
		Texture2DHandle roughnessIron = m_renderer.MutGraphicsDevice().CreateTexture2D(pbrMatTexDesc);

		pbrMatTexDesc.m_filename = "Sandbox/assets/textures/pbr/rusted_iron/ao.png";
		Texture2DHandle aoIron = m_renderer.MutGraphicsDevice().CreateTexture2D(pbrMatTexDesc);

		rustyIronInstance.BindTexture(EMISSION, albedoIron);

		rustyIronInstance.BindTexture(NORMAL, normalIron);

		rustyIronInstance.BindTexture(METALLIC, metallicIron);

		rustyIronInstance.BindTexture(ROUGHNESS, roughnessIron);

		rustyIronInstance.BindTexture(AMBIENT_OCCLUSION, aoIron);

		rustyIronInstance.CreateMaterialResourceSet();


		// gold

		MaterialInstance goldInstance = lib.CreateMaterialInstance(pbrInterface);

		goldInstance.UpdateUniformBlock(MaterialBlockBinding::FRAME_TONE_MAPPING, ToneMappingParams{ true, 1.f, true }); // use reinhard tonemapping

		goldInstance.UpdateUniformBlock(MaterialBlockBinding::MATERIAL_PBR, params);

		goldInstance.BindSampler(SAMPLER_0, radianceSampler);

		goldInstance.BindTexture(DIFFUSE, irrMapTex);
		goldInstance.BindTexture(SPECULAR, brdfLutTex);

		goldInstance.BindSampler(SAMPLER_1, envMapSampler);

		goldInstance.BindTexture(SKYBOX, prefilterMapTex);

		goldInstance.BindSampler(SAMPLER_2, pbrMatSampler);

		pbrAlbedoTexDesc.m_filename = "Sandbox/assets/textures/pbr/gold/albedo.png";
		Texture2DHandle albedoGold = m_renderer.MutGraphicsDevice().CreateTexture2D(pbrAlbedoTexDesc);

		pbrMatTexDesc.m_filename = "Sandbox/assets/textures/pbr/gold/normal.png";
		Texture2DHandle normalGold = m_renderer.MutGraphicsDevice().CreateTexture2D(pbrMatTexDesc);

		pbrMatTexDesc.m_filename = "Sandbox/assets/textures/pbr/gold/metallic.png";
		Texture2DHandle metallicGold = m_renderer.MutGraphicsDevice().CreateTexture2D(pbrMatTexDesc);

		pbrMatTexDesc.m_filename = "Sandbox/assets/textures/pbr/gold/roughness.png";
		Texture2DHandle roughnessGold = m_renderer.MutGraphicsDevice().CreateTexture2D(pbrMatTexDesc);

		pbrMatTexDesc.m_filename = "Sandbox/assets/textures/pbr/gold/ao.png";
		Texture2DHandle aoGold = m_renderer.MutGraphicsDevice().CreateTexture2D(pbrMatTexDesc);

		goldInstance.BindTexture(EMISSION, albedoGold);

		goldInstance.BindTexture(NORMAL, normalGold);

		goldInstance.BindTexture(METALLIC, metallicGold);

		goldInstance.BindTexture(ROUGHNESS, roughnessGold);

		goldInstance.BindTexture(AMBIENT_OCCLUSION, aoGold);

		goldInstance.CreateMaterialResourceSet();

		// grass

		MaterialInstance grassInstance = lib.CreateMaterialInstance(pbrInterface);

		grassInstance.UpdateUniformBlock(MaterialBlockBinding::FRAME_TONE_MAPPING, ToneMappingParams{ true, 1.f, true }); // use reinhard tonemapping

		grassInstance.UpdateUniformBlock(MaterialBlockBinding::MATERIAL_PBR, params);

		grassInstance.BindSampler(SAMPLER_0, radianceSampler);

		grassInstance.BindTexture(DIFFUSE, irrMapTex);
		grassInstance.BindTexture(SPECULAR, brdfLutTex);

		grassInstance.BindSampler(SAMPLER_1, envMapSampler);

		grassInstance.BindTexture(SKYBOX, prefilterMapTex);

		grassInstance.BindSampler(SAMPLER_2, pbrMatSampler);

		pbrAlbedoTexDesc.m_filename = "Sandbox/assets/textures/pbr/grass/albedo.png";
		Texture2DHandle albedoGrass = m_renderer.MutGraphicsDevice().CreateTexture2D(pbrAlbedoTexDesc);

		pbrMatTexDesc.m_filename = "Sandbox/assets/textures/pbr/grass/normal.png";
		Texture2DHandle normalGrass = m_renderer.MutGraphicsDevice().CreateTexture2D(pbrMatTexDesc);

		pbrMatTexDesc.m_filename = "Sandbox/assets/textures/pbr/grass/metallic.png";
		Texture2DHandle metallicGrass = m_renderer.MutGraphicsDevice().CreateTexture2D(pbrMatTexDesc);

		pbrMatTexDesc.m_filename = "Sandbox/assets/textures/pbr/grass/roughness.png";
		Texture2DHandle roughnessGrass = m_renderer.MutGraphicsDevice().CreateTexture2D(pbrMatTexDesc);

		pbrMatTexDesc.m_filename = "Sandbox/assets/textures/pbr/grass/ao.png";
		Texture2DHandle aoGrass = m_renderer.MutGraphicsDevice().CreateTexture2D(pbrMatTexDesc);


		grassInstance.BindTexture(EMISSION, albedoGrass);

		grassInstance.BindTexture(NORMAL, normalGrass);

		grassInstance.BindTexture(METALLIC, metallicGrass);

		grassInstance.BindTexture(ROUGHNESS, roughnessGrass);

		grassInstance.BindTexture(AMBIENT_OCCLUSION, aoGrass);

		grassInstance.CreateMaterialResourceSet();

		// plastic

		MaterialInstance plasticInstance = lib.CreateMaterialInstance(pbrInterface);

		plasticInstance.UpdateUniformBlock(MaterialBlockBinding::FRAME_TONE_MAPPING, ToneMappingParams{ true, 1.f, true }); // use reinhard tonemapping

		plasticInstance.UpdateUniformBlock(MaterialBlockBinding::MATERIAL_PBR, params);

		plasticInstance.BindSampler(SAMPLER_0, radianceSampler);

		plasticInstance.BindTexture(DIFFUSE, irrMapTex);
		plasticInstance.BindTexture(SPECULAR, brdfLutTex);

		plasticInstance.BindSampler(SAMPLER_1, envMapSampler);

		plasticInstance.BindTexture(SKYBOX, prefilterMapTex);

		plasticInstance.BindSampler(SAMPLER_2, pbrMatSampler);

		pbrAlbedoTexDesc.m_filename = "Sandbox/assets/textures/pbr/plastic/albedo.png";
		Texture2DHandle albedoPlastic = m_renderer.MutGraphicsDevice().CreateTexture2D(pbrAlbedoTexDesc);

		pbrMatTexDesc.m_filename = "Sandbox/assets/textures/pbr/plastic/normal.png";
		Texture2DHandle normalPlastic = m_renderer.MutGraphicsDevice().CreateTexture2D(pbrMatTexDesc);

		pbrMatTexDesc.m_filename = "Sandbox/assets/textures/pbr/plastic/metallic.png";
		Texture2DHandle metallicPlastic = m_renderer.MutGraphicsDevice().CreateTexture2D(pbrMatTexDesc);

		pbrMatTexDesc.m_filename = "Sandbox/assets/textures/pbr/plastic/roughness.png";
		Texture2DHandle roughnessPlastic = m_renderer.MutGraphicsDevice().CreateTexture2D(pbrMatTexDesc);

		pbrMatTexDesc.m_filename = "Sandbox/assets/textures/pbr/plastic/ao.png";
		Texture2DHandle aoPlastic = m_renderer.MutGraphicsDevice().CreateTexture2D(pbrMatTexDesc);

		plasticInstance.BindTexture(EMISSION, albedoPlastic);

		plasticInstance.BindTexture(NORMAL, normalPlastic);

		plasticInstance.BindTexture(METALLIC, metallicPlastic);

		plasticInstance.BindTexture(ROUGHNESS, roughnessPlastic);

		plasticInstance.BindTexture(AMBIENT_OCCLUSION, aoPlastic);

		plasticInstance.CreateMaterialResourceSet();

		// wall

		MaterialInstance wallInstance = lib.CreateMaterialInstance(pbrInterface);

		wallInstance.UpdateUniformBlock(MaterialBlockBinding::FRAME_TONE_MAPPING, ToneMappingParams{ true, 1.f, true }); // use reinhard tonemapping

		wallInstance.UpdateUniformBlock(MaterialBlockBinding::MATERIAL_PBR, params);

		wallInstance.BindSampler(SAMPLER_0, radianceSampler);

		wallInstance.BindTexture(DIFFUSE, irrMapTex);
		wallInstance.BindTexture(SPECULAR, brdfLutTex);

		wallInstance.BindSampler(SAMPLER_1, envMapSampler);

		wallInstance.BindTexture(SKYBOX, prefilterMapTex);

		wallInstance.BindSampler(SAMPLER_2, pbrMatSampler);

		pbrAlbedoTexDesc.m_filename = "Sandbox/assets/textures/pbr/wall/albedo.png";
		Texture2DHandle albedoWall = m_renderer.MutGraphicsDevice().CreateTexture2D(pbrAlbedoTexDesc);

		pbrMatTexDesc.m_filename = "Sandbox/assets/textures/pbr/wall/normal.png";
		Texture2DHandle normalWall = m_renderer.MutGraphicsDevice().CreateTexture2D(pbrMatTexDesc);

		pbrMatTexDesc.m_filename = "Sandbox/assets/textures/pbr/wall/metallic.png";
		Texture2DHandle metallicWall = m_renderer.MutGraphicsDevice().CreateTexture2D(pbrMatTexDesc);

		pbrMatTexDesc.m_filename = "Sandbox/assets/textures/pbr/wall/roughness.png";
		Texture2DHandle roughnessWall = m_renderer.MutGraphicsDevice().CreateTexture2D(pbrMatTexDesc);

		pbrMatTexDesc.m_filename = "Sandbox/assets/textures/pbr/wall/ao.png";
		Texture2DHandle aoWall = m_renderer.MutGraphicsDevice().CreateTexture2D(pbrMatTexDesc);

		wallInstance.BindTexture(EMISSION, albedoWall);

		wallInstance.BindTexture(NORMAL, normalWall);

		wallInstance.BindTexture(METALLIC, metallicWall);

		wallInstance.BindTexture(ROUGHNESS, roughnessWall);

		wallInstance.BindTexture(AMBIENT_OCCLUSION, aoWall);

		wallInstance.CreateMaterialResourceSet();


		const int nrRows = 7;
		const int nrColumns = 7;
		const float spacing = 2.5f;



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


			m_renderer.MutGraphicsDevice().SetPipeline(scenePipe);

			renderer.Clear(ColorRGBAf(0.1f, 0.1f, 0.1f, 1.0f));

			lightsSystem.UpdateLights();

			lightsSystem.BindLightBuffer();

			camSys.UpdateCameras();

			for (uint32_t iCam = 0; iCam < camSys.CamerasNumber(); iCam++)
			{
				camSys.BindCameraBuffer(iCam);

				//renderer.UseMaterialInstance(&pbrInstance);

				// render rows*column number of spheres with varying metallic/roughness values scaled by rows and columns respectively

				//for (int row = 0; row < nrRows; ++row)
				//{
				//	params.m_metallic = row / (float)nrRows;

				//	for (int col = 0; col < nrColumns; ++col)
				//	{
				//		// we clamp the roughness to 0.025 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
				//		// on direct lighting.
				//		params.m_roughness = glm::clamp(col / (float)nrColumns, 0.05f, 1.f);

				//		pbrInstance.UpdateUniformBlock(MATERIAL_PBR, params);

				//		sphere->SetTransform(Transform::Identity());
				//		sphere->AddTransform(Transform::Translate({
				//			(col - (nrColumns / 2)) * spacing,
				//			(row - (nrRows / 2)) * spacing,
				//			0.0f
				//			}));

				//		sphere->UpdateObjectMatrices(camSys.GetCamera(iCam));

				//		renderWorld.DrawMesh(sphere, sphereVao);
				//	}
				//}

				renderer.UseMaterialInstance(&rustyIronInstance);

				rustyIronInstance.UpdateUniformBlock(MATERIAL_PBR, params);

				sphere->SetTransform(Transform::Identity());
				sphere->AddTransform(Transform::Translate({ -5.0f, 0.0f, 2.0f }));

				sphere->UpdateObjectMatrices(camSys.GetCamera(iCam));

				renderWorld.DrawMesh(sphere, sphereVao);



				renderer.UseMaterialInstance(&goldInstance);

				goldInstance.UpdateUniformBlock(MATERIAL_PBR, params);

				sphere->SetTransform(Transform::Identity());
				sphere->AddTransform(Transform::Translate({ -3.0f, 0.0f, 2.0f }));

				sphere->UpdateObjectMatrices(camSys.GetCamera(iCam));

				renderWorld.DrawMesh(sphere, sphereVao);



				renderer.UseMaterialInstance(&grassInstance);

				grassInstance.UpdateUniformBlock(MATERIAL_PBR, params);

				sphere->SetTransform(Transform::Identity());
				sphere->AddTransform(Transform::Translate({ -1.0, 0.0, 2.0 }));

				sphere->UpdateObjectMatrices(camSys.GetCamera(iCam));

				renderWorld.DrawMesh(sphere, sphereVao);



				renderer.UseMaterialInstance(&plasticInstance);

				plasticInstance.UpdateUniformBlock(MATERIAL_PBR, params);

				sphere->SetTransform(Transform::Identity());
				sphere->AddTransform(Transform::Translate({ 1.0f, 0.0f, 2.0f }));

				sphere->UpdateObjectMatrices(camSys.GetCamera(iCam));

				renderWorld.DrawMesh(sphere, sphereVao);



				renderer.UseMaterialInstance(&wallInstance);

				wallInstance.UpdateUniformBlock(MATERIAL_PBR, params);

				sphere->SetTransform(Transform::Identity());
				sphere->AddTransform(Transform::Translate({ 3.0f, 0.0f, 2.0f }));

				sphere->UpdateObjectMatrices(camSys.GetCamera(iCam));

				renderWorld.DrawMesh(sphere, sphereVao);

				iLight = 0;
				for (auto& lightPos : lightPositions)
				{
					sphere->SetTransform(Transform::Identity());
					sphere->AddTransform(Transform::Translate(lightPos.xyz()));
					sphere->AddTransform(Transform::Scale(Vec3{ 0.5f }));


					sphere->UpdateObjectMatrices(camSys.GetCamera(iCam));

					renderWorld.DrawMesh(sphere, sphereVao);

					iLight++;
				}

				// Render the skybox last to avoid overdrawing

				m_renderer.MutGraphicsDevice().SetPipeline(skyboxPipe);

				renderer.UseMaterialInstance(&backgroundMatInstance);

				cube->UpdateObjectMatrices(camSys.GetCamera(iCam));

				renderWorld.DrawMesh(cube, cubeVao);
			}

			SwapBuffers();

		}
	}


}
