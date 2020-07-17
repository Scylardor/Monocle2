#include "TestApplication.h"

#include <glfw3/include/GLFW/glfw3.h>
#include <Graphics/Pipeline/PipelineDescriptor.h>

#include "Math/Vec2.h"
#include <Math/Vec3.h>
#include <Math/Vec4.h>


#include "Graphics/Shader/ShaderStage/ShaderStage.h"

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


}