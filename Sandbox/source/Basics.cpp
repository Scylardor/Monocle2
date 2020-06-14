#include "TestApplication.h"

#include <glfw3/include/GLFW/glfw3.h>
#include <Graphics/Pipeline/PipelineDescriptor.h>

#include "Math/Vec2.h"
#include <Math/Vec3.h>
#include <Math/Vec4.h>

#include "Graphics/Shader/Module/ShaderModuleDescriptor.h"
#include "Graphics/Shader/Program/ShaderProgramDescriptor.h"

#include "Graphics/Shader/ShaderStage/ShaderStage.h"

#include "Graphics/VertexLayout/VertexLayoutDescriptor.h"

#include "Graphics/Color/Color.h"

#include "Core/Misc/moeFile.h"


#include "Graphics/SceneGraph/SceneGraph.h"

#include "Graphics/Resources/ResourceLayout/ResourceLayoutDescriptor.h"
#include "Graphics/Resources/ResourceSet/ResourceSetDescriptor.h"


#include "Graphics/Camera/Camera.h"
#include "Graphics/Camera/CameraSystem.h"
#include "Graphics/Light/LightSystem.h"


#include "Graphics/Material/Material.h"

#include "Graphics/Material/MaterialLibrary.h"



namespace moe
{

	class CameraNode : public ASceneNode
	{
	public:
		CameraNode(SceneGraph& graph, std::string name) : ASceneNode(graph), grou(name)
		{
			MOE_LOG("krou krou");
		}

		~CameraNode()
		{
			MOE_LOG("krouuuuu");
		}

	private:
		std::string grou;
		Transform	m_viewMatrix;
	};


	struct PhongMaterial
	{
		Vec4	m_ambientColor{ 1.f };
		Vec4	m_diffuseColor{ 1.f };
		Vec4	m_specularColor{ 1.f };
		float	m_shininess{ 32 };
	};



	struct VertexPositionNormalTexture
	{
		Vec3	m_position;
		Vec3	m_normal;
		Vec2	m_texcoords;
	};


	Array<VertexPositionNormalTexture, 36>	CreateCubePositionNormalTexture(float halfExtent)
	{

		return {
			{Vec3{-halfExtent, -halfExtent, -halfExtent}, {0.0f,  0.0f, -1.0f}, { 0.0f,  0.0f }},
			{Vec3{ halfExtent,  halfExtent, -halfExtent}, {0.0f,  0.0f, -1.0f}, { 1.0f,  1.0f }},
			{Vec3{ halfExtent, -halfExtent, -halfExtent}, {0.0f,  0.0f, -1.0f}, { 1.0f,  0.0f }},
			{Vec3{ halfExtent,  halfExtent, -halfExtent}, {0.0f,  0.0f, -1.0f}, { 1.0f,  1.0f }},
			{Vec3{-halfExtent, -halfExtent, -halfExtent}, {0.0f,  0.0f, -1.0f}, { 0.0f,  0.0f }},
			{Vec3{-halfExtent,  halfExtent, -halfExtent}, {0.0f,  0.0f, -1.0f}, { 0.0f,  1.0f }},

			{Vec3{ -halfExtent, -halfExtent,  halfExtent }, { 0.0f,  0.0f, 1.0f }, { 0.0f,  0.0f }},
			{ Vec3{  halfExtent, -halfExtent,  halfExtent}, {0.0f,  0.0f, 1.0f}, { 1.0f,  0.0f }},
			{ Vec3{  halfExtent,  halfExtent,  halfExtent}, {0.0f,  0.0f, 1.0f}, { 1.0f,  1.0f }},
			{ Vec3{  halfExtent,  halfExtent,  halfExtent}, {0.0f,  0.0f, 1.0f}, { 1.0f,  1.0f }},
			{ Vec3{ -halfExtent,  halfExtent,  halfExtent}, {0.0f,  0.0f, 1.0f}, { 0.0f,  1.0f }},
			{ Vec3{ -halfExtent, -halfExtent,  halfExtent}, {0.0f,  0.0f, 1.0f}, { 0.0f,  0.0f }},

			{Vec3{ -halfExtent,  halfExtent,  halfExtent }, { -1.0f,  0.0f,  0.0 }, { 1.0f,  1.0f }},
			{ Vec3{ -halfExtent,  halfExtent, -halfExtent}, {-1.0f,  0.0f,  0.0}, { 0.0f,  1.0f } },
			{ Vec3{ -halfExtent, -halfExtent, -halfExtent}, {-1.0f,  0.0f,  0.0}, { 0.0f,  0.0f } },
			{ Vec3{ -halfExtent, -halfExtent, -halfExtent}, {-1.0f,  0.0f,  0.0}, { 0.0f,  0.0f } },
			{ Vec3{ -halfExtent, -halfExtent,  halfExtent}, {-1.0f,  0.0f,  0.0}, { 1.0f,  0.0f } },
			{ Vec3{ -halfExtent,  halfExtent,  halfExtent}, {-1.0f,  0.0f,  0.0}, { 1.0f,  1.0f } },

			{ Vec3{ halfExtent,  halfExtent,  halfExtent }, { 1.0f,  0.0f,  0.0f }, { 0.0f,  1.0f }},
			{ Vec3{ halfExtent, -halfExtent, -halfExtent}, {1.0f,  0.0f,  0.0f }, { 1.0f,  0.0f }},
			{ Vec3{ halfExtent,  halfExtent, -halfExtent}, {1.0f,  0.0f,  0.0f }, { 1.0f,  1.0f }},
			{ Vec3{ halfExtent, -halfExtent, -halfExtent}, {1.0f,  0.0f,  0.0f }, { 1.0f,  0.0f }},
			{ Vec3{ halfExtent,  halfExtent,  halfExtent}, {1.0f,  0.0f,  0.0f }, { 0.0f,  1.0f }},
			{ Vec3{ halfExtent, -halfExtent,  halfExtent}, {1.0f,  0.0f,  0.0f }, { 0.0f,  0.0f }},

			{Vec3{ -halfExtent, -halfExtent, -halfExtent }, { 0.0f, -1.0f,  0.0f }, { 0.0f,  1.0f }},
			{ Vec3{  halfExtent, -halfExtent, -halfExtent}, {0.0f, -1.0f,  0.0f}, { 1.0f,  1.0f } },
			{ Vec3{  halfExtent, -halfExtent,  halfExtent}, {0.0f, -1.0f,  0.0f}, { 1.0f,  0.0f } },
			{ Vec3{  halfExtent, -halfExtent,  halfExtent}, {0.0f, -1.0f,  0.0f}, { 1.0f,  0.0f } },
			{ Vec3{ -halfExtent, -halfExtent,  halfExtent}, {0.0f, -1.0f,  0.0f}, { 0.0f,  0.0f } },
			{ Vec3{ -halfExtent, -halfExtent, -halfExtent}, {0.0f, -1.0f,  0.0f}, { 0.0f,  1.0f } },

			{Vec3{ -halfExtent,  halfExtent, -halfExtent }, { 0.0f,  1.0f,  0.0f }, { 0.0f,  0.0f }},
			{ Vec3{  halfExtent,  halfExtent,  halfExtent}, {0.0f,  1.0f,  0.0f}, { 1.0f,  1.0f } },
			{ Vec3{  halfExtent,  halfExtent, -halfExtent}, {0.0f,  1.0f,  0.0f}, { 0.0f,  1.0f } },
			{ Vec3{  halfExtent,  halfExtent,  halfExtent}, {0.0f,  1.0f,  0.0f}, { 1.0f,  1.0f } },
			{ Vec3{ -halfExtent,  halfExtent, -halfExtent}, {0.0f,  1.0f,  0.0f}, { 0.0f,  0.0f } },
			{ Vec3{ -halfExtent,  halfExtent,  halfExtent}, {0.0f,  1.0f,  0.0f}, { 1.0f,  0.0f }}
		};

	}



	struct HelloQuadVertex
	{
		Vec3	m_position;
		Vec4	m_color;
		Vec2	m_texture;
	};

	struct HelloTriVertex
	{
		Vec3	m_position;
		Vec4	color;
	};




	Array<TestApplication::VertexPosition, 36>	TestApplication::CreateCube(float halfExtent)
	{
		return {
			{Vec3{-halfExtent, -halfExtent, -halfExtent}},
			{Vec3{ halfExtent,  halfExtent, -halfExtent}},
			{Vec3{ halfExtent, -halfExtent, -halfExtent}},
			{Vec3{ halfExtent,  halfExtent, -halfExtent}},
			{Vec3{-halfExtent, -halfExtent, -halfExtent}},
			{Vec3{-halfExtent,  halfExtent, -halfExtent}},

			{Vec3{ -halfExtent, -halfExtent,  halfExtent}},
			{Vec3{  halfExtent, -halfExtent,  halfExtent}},
			{Vec3{  halfExtent,  halfExtent,  halfExtent}},
			{Vec3{  halfExtent,  halfExtent,  halfExtent}},
			{Vec3{ -halfExtent,  halfExtent,  halfExtent}},
			{Vec3{ -halfExtent, -halfExtent,  halfExtent}},

			{Vec3{ -halfExtent,  halfExtent,  halfExtent}},
			{Vec3{ -halfExtent,  halfExtent, -halfExtent}},
			{Vec3{ -halfExtent, -halfExtent, -halfExtent}},
			{Vec3{ -halfExtent, -halfExtent, -halfExtent}},
			{Vec3{ -halfExtent, -halfExtent,  halfExtent}},
			{Vec3{ -halfExtent,  halfExtent,  halfExtent}},

			{Vec3{ halfExtent,  halfExtent,  halfExtent}},
			{Vec3{ halfExtent, -halfExtent, -halfExtent}},
			{Vec3{ halfExtent,  halfExtent, -halfExtent}},
			{Vec3{ halfExtent, -halfExtent, -halfExtent}},
			{Vec3{ halfExtent,  halfExtent,  halfExtent}},
			{Vec3{ halfExtent, -halfExtent,  halfExtent}},

			{Vec3{ -halfExtent, -halfExtent, -halfExtent}},
			{Vec3{  halfExtent, -halfExtent, -halfExtent}},
			{Vec3{  halfExtent, -halfExtent,  halfExtent}},
			{Vec3{  halfExtent, -halfExtent,  halfExtent}},
			{Vec3{ -halfExtent, -halfExtent,  halfExtent}},
			{Vec3{ -halfExtent, -halfExtent, -halfExtent}},

			{Vec3{ -halfExtent,  halfExtent, -halfExtent}},
			{Vec3{  halfExtent,  halfExtent,  halfExtent}},
			{Vec3{  halfExtent,  halfExtent, -halfExtent}},
			{Vec3{  halfExtent,  halfExtent,  halfExtent}},
			{Vec3{ -halfExtent,  halfExtent, -halfExtent}},
			{Vec3{ -halfExtent,  halfExtent,  halfExtent}}
		};

	}

	Array<HelloQuadVertex, 36>	CreateHelloQuadVertexCube(float halfExtent)
	{
		const ColorRGBAf white = 0xffffffff_rgbaf;
		const Vec4 whiteAsVec4 = Vec4(white.R(), white.G(), white.B(), white.A());

		return {
			{Vec3{-halfExtent, -halfExtent, -halfExtent}, whiteAsVec4, Vec2{0.0f, 0.0f}},
			{Vec3{ halfExtent, -halfExtent, -halfExtent}, whiteAsVec4, Vec2{1.0f, 0.0f}},
			{Vec3{ halfExtent,  halfExtent, -halfExtent}, whiteAsVec4, Vec2{1.0f, 1.0f}},
			{Vec3{ halfExtent,  halfExtent, -halfExtent}, whiteAsVec4, Vec2{1.0f, 1.0f}},
			{Vec3{-halfExtent,  halfExtent, -halfExtent}, whiteAsVec4, Vec2{0.0f, 1.0f}},
			{Vec3{-halfExtent, -halfExtent, -halfExtent}, whiteAsVec4, Vec2{0.0f, 0.0f}},

			{Vec3{ -halfExtent, -halfExtent,  halfExtent}, whiteAsVec4,  Vec2{0.0f, 0.0f}},
			{Vec3{  halfExtent, -halfExtent,  halfExtent}, whiteAsVec4,  Vec2{1.0f, 0.0f}},
			{Vec3{  halfExtent,  halfExtent,  halfExtent}, whiteAsVec4,  Vec2{1.0f, 1.0f}},
			{Vec3{  halfExtent,  halfExtent,  halfExtent}, whiteAsVec4,  Vec2{1.0f, 1.0f}},
			{Vec3{ -halfExtent,  halfExtent,  halfExtent}, whiteAsVec4,  Vec2{0.0f, 1.0f}},
			{Vec3{ -halfExtent, -halfExtent,  halfExtent}, whiteAsVec4,  Vec2{0.0f, 0.0f}},

			{Vec3{ -halfExtent,  halfExtent,  halfExtent}, whiteAsVec4,  Vec2{1.0f, 0.0f}},
			{Vec3{ -halfExtent,  halfExtent, -halfExtent}, whiteAsVec4,  Vec2{1.0f, 1.0f}},
			{Vec3{ -halfExtent, -halfExtent, -halfExtent}, whiteAsVec4,  Vec2{0.0f, 1.0f}},
			{Vec3{ -halfExtent, -halfExtent, -halfExtent}, whiteAsVec4,  Vec2{0.0f, 1.0f}},
			{Vec3{ -halfExtent, -halfExtent,  halfExtent}, whiteAsVec4,  Vec2{0.0f, 0.0f}},
			{Vec3{ -halfExtent,  halfExtent,  halfExtent}, whiteAsVec4,  Vec2{1.0f, 0.0f}},

			{Vec3{ halfExtent,  halfExtent,  halfExtent}, whiteAsVec4, Vec2{1.0f, 0.0f}},
			{Vec3{ halfExtent,  halfExtent, -halfExtent}, whiteAsVec4, Vec2{1.0f, 1.0f}},
			{Vec3{ halfExtent, -halfExtent, -halfExtent}, whiteAsVec4, Vec2{0.0f, 1.0f}},
			{Vec3{ halfExtent, -halfExtent, -halfExtent}, whiteAsVec4, Vec2{0.0f, 1.0f}},
			{Vec3{ halfExtent, -halfExtent,  halfExtent}, whiteAsVec4, Vec2{0.0f, 0.0f}},
			{Vec3{ halfExtent,  halfExtent,  halfExtent}, whiteAsVec4, Vec2{1.0f, 0.0f}},

			{Vec3{ -halfExtent, -halfExtent, -halfExtent}, whiteAsVec4,  Vec2{0.0f, 1.0f}},
			{Vec3{  halfExtent, -halfExtent, -halfExtent}, whiteAsVec4,  Vec2{1.0f, 1.0f}},
			{Vec3{  halfExtent, -halfExtent,  halfExtent}, whiteAsVec4,  Vec2{1.0f, 0.0f}},
			{Vec3{  halfExtent, -halfExtent,  halfExtent}, whiteAsVec4,  Vec2{1.0f, 0.0f}},
			{Vec3{ -halfExtent, -halfExtent,  halfExtent}, whiteAsVec4,  Vec2{0.0f, 0.0f}},
			{Vec3{ -halfExtent, -halfExtent, -halfExtent}, whiteAsVec4,  Vec2{0.0f, 1.0f}},

			{Vec3{ -halfExtent,  halfExtent, -halfExtent}, whiteAsVec4,  Vec2{0.0f, 1.0f}},
			{Vec3{  halfExtent,  halfExtent, -halfExtent}, whiteAsVec4,  Vec2{1.0f, 1.0f}},
			{Vec3{  halfExtent,  halfExtent,  halfExtent}, whiteAsVec4,  Vec2{1.0f, 0.0f}},
			{Vec3{  halfExtent,  halfExtent,  halfExtent}, whiteAsVec4,  Vec2{1.0f, 0.0f}},
			{Vec3{ -halfExtent,  halfExtent,  halfExtent}, whiteAsVec4,  Vec2{0.0f, 0.0f}},
			{Vec3{ -halfExtent,  halfExtent, -halfExtent}, whiteAsVec4,  Vec2{0.0f, 1.0f}}
		};

	}



	const char *triVertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"layout (location = 1) in vec4 aColor;\n"
		"layout (std140) uniform CameraMatrices { mat4 view; mat4 projection; mat4 viewProjection; };\n"
		"layout (std140) uniform ObjectMatrices { mat4 model; mat4 modelView; mat4 modelViewProjection; mat3 normalMatrix; };\n"
		"out vec4 ourColor;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = model * vec4(aPos, 1.0);\n"
		" ourColor = aColor;\n"
		"}\0";

	const char *triFragShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"in vec4 ourColor;\n"
		"layout (std140) uniform Color { vec4 colorValue; };\n"
		"void main()\n"
		"{\n"
		"	FragColor = colorValue;\n"
		"}\0";


	const char *quadVertexShaderSource =
		"#version 330 core\n"
		"layout(location = 0) in vec3 aPos;\n"
		"layout(location = 1) in vec4 aColor;\n"
		"layout(location = 2) in vec2 aTexCoord;\n"
		"layout (std140) uniform CameraMatrices { mat4 view; mat4 projection; mat4 viewProjection; };\n"
		"layout (std140) uniform ObjectMatrices { mat4 model; mat4 modelView; mat4 modelViewProjection; mat3 normalMatrix; };\n"
		"out vec4 ourColor;\n"
		"out vec2 TexCoord;\n"
		"void main()\n"
		"{\n"
		"	gl_Position = modelViewProjection * vec4(aPos, 1.0);\n"
		"	ourColor = aColor;\n"
		"	TexCoord = aTexCoord;\n"
		"}\n";

	const char *quadFragShaderSource =
		"#version 330 core\n"
		"out vec4 FragColor;\n"
		"in vec4 ourColor;\n"
		"in vec2 TexCoord;\n"
		"layout (std140) uniform Color { vec4 colorValue; };\n"
		"uniform sampler2D DiffuseMap1;"
		"uniform sampler2D DiffuseMap2;"
		"void main()\n"
		"{\n"
		" FragColor = mix(texture(DiffuseMap1, TexCoord), texture(DiffuseMap2, TexCoord), 0.6);"
		"}\n";



void TestApplication::TestSceneGraph()
{

	SceneGraph graph(12);

	SceneNodeHandle bodyHandle = graph.CreateNode<ASceneNode>(SceneGraph::GetRootHandle(), Transform::Translate({ 0, 35, 0 }));
	SceneNodeHandle headHandle = graph.CreateNode<ASceneNode>(bodyHandle, Transform::Translate({ 0, 30, 0 }));
	SceneNodeHandle leftArmHandle = graph.CreateNode<ASceneNode>(bodyHandle, Transform::Translate({ -12, 30, -1 }));
	SceneNodeHandle rightArmHandle = graph.CreateNode<ASceneNode>(bodyHandle, Transform::Translate({ 12, 30, -1 }));
	SceneNodeHandle leftLegHandle = graph.CreateNode<ASceneNode>(bodyHandle, Transform::Translate({ -8, 0,  0 }));
	SceneNodeHandle rightLegHandle = graph.CreateNode<ASceneNode>(bodyHandle, Transform::Translate({ 8, 0, 0 }));
	SceneNodeHandle leftFootHandle = graph.CreateNode<ASceneNode>(leftLegHandle, Transform::Translate({ 0, -35, 0 }));
	SceneNodeHandle rightFootHandle = graph.CreateNode<ASceneNode>(rightLegHandle, Transform::Translate({ 0, -35, 0 }));


	ASceneNode* body = graph.MutSceneNode(bodyHandle);
	body->SetWorldTransform(Transform::Translate({ 10, 0, 0 }));


	SceneNodeHandle cameraHandle = graph.CreateNode<CameraNode>(SceneGraph::GetRootHandle(), Transform::Identity(), "krou");

	graph.RemoveNode(cameraHandle);

	graph.RemoveNode(bodyHandle);


	SceneNodeHandle nodeHandle3 = graph.CreateNode<ASceneNode>(SceneGraph::GetRootHandle(), Transform::Identity());

}


void TestApplication::TestMultiLights()
{
	IGraphicsRenderer& renderer = MutRenderer();


	/* Create phong object shader */
	IGraphicsRenderer::ShaderFileList phongFileList =
	{
		{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/phong_maps.vert" },
		{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/phong_maps.frag" }
	};

	ShaderProgramHandle phongProgram = renderer.CreateShaderProgramFromSourceFiles(phongFileList);



	MaterialLibrary lib(MutRenderer().MutGraphicsDevice());
	lib.AddBindingMapping("Object_Matrices", { MaterialBlockBinding::OBJECT_MATRICES, ResourceKind::UniformBuffer });
	lib.AddBindingMapping("Frame_Time", { MaterialBlockBinding::FRAME_TIME, ResourceKind::UniformBuffer });
	lib.AddBindingMapping("Frame_Lights", { MaterialBlockBinding::FRAME_LIGHTS, ResourceKind::UniformBuffer });
	lib.AddBindingMapping("View_Camera", { MaterialBlockBinding::VIEW_CAMERA, ResourceKind::UniformBuffer });
	lib.AddBindingMapping("Material_Phong", { MaterialBlockBinding::MATERIAL_PHONG, ResourceKind::UniformBuffer });
	lib.AddBindingMapping("Material_Color", { MaterialBlockBinding::MATERIAL_COLOR, ResourceKind::UniformBuffer });
	lib.AddBindingMapping("Material_DiffuseMap", { MaterialTextureBinding::DIFFUSE, ResourceKind::TextureReadOnly });
	lib.AddBindingMapping("Material_SpecularMap", { MaterialTextureBinding::SPECULAR, ResourceKind::TextureReadOnly });
	lib.AddBindingMapping("Material_EmissionMap", { MaterialTextureBinding::EMISSION, ResourceKind::TextureReadOnly });

	lib.AddUniformBufferSizer(MaterialBlockBinding::FRAME_LIGHTS, []() { return sizeof(LightCastersData); });
	lib.AddUniformBufferSizer(MaterialBlockBinding::VIEW_CAMERA, []() { return sizeof(CameraMatrices); });
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


	BlendStateDescriptor desc = BlendStateDescriptor::ms_Disabled;
	PipelineDescriptor desc2;


	VertexLayoutDescriptor layout{
		{{"position", VertexElementFormat::Float3},
				{"color", VertexElementFormat::Float3},
				{"texture", VertexElementFormat::Float2}},
		LayoutType::Interleaved
	};


	/* Create light VAO */
	VertexLayoutDescriptor lightLayout{
	{{"position", VertexElementFormat::Float3}},
		LayoutType::Interleaved
	};

	auto lightVao = renderer.CreateVertexLayout(lightLayout);


	/* Create light shader */
	IGraphicsRenderer::ShaderFileList lightFileList =
	{
		{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/light.vert" },
		{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/light.frag" }
	};

	ShaderProgramHandle lightProgram = renderer.CreateShaderProgramFromSourceFiles(lightFileList);


	/* Create phong object VAO */
	VertexLayoutDescriptor phongLayout{
{{"position", VertexElementFormat::Float3},
		{"normal", VertexElementFormat::Float3},
		{"texCoords", VertexElementFormat::Float2}},
	LayoutType::Interleaved
	};

	auto phongVao = renderer.CreateVertexLayout(phongLayout);


	/* Create Light uniform buffer */

	LightSystem lightsSystem(renderer.MutGraphicsDevice());

	m_flashLight = lightsSystem.AddNewLight({ Vec4::ZeroVector(), Vec4::ZeroVector(),
	Vec4(0.0f, 0.0f, 0.0f, 1.f), Vec4(1.f, 1.f, 0.f, 1.f), Vec4(1.f) });

	//newLight->SetDirectionalLight({ -0.2f, -1.0f, -0.3f });

	m_flashLight->SetAttenuationFactors(1.f, 0.09f, 0.032f);
	m_flashLight->SetSpotInnerCutoff(Degs_f{ 12.5 });
	m_flashLight->SetSpotOuterCutoff(Degs_f{ 15.F });

	LightObject* pointLight1 = lightsSystem.AddNewLight({ Vec4::ZeroVector(), Vec4::ZeroVector(),
		Vec4(0.05f, 0.05f, 0.05f, 1.f), Vec4(0.8f, 0.8f, 0.8f, 1.f), Vec4(1.f) });
	pointLight1->SetAttenuationFactors(1.f, 0.09f, 0.032f);

	LightObject* pointLight2 = lightsSystem.AddNewLight({ Vec4::ZeroVector(), Vec4::ZeroVector(),
	Vec4(0.05f, 0.05f, 0.05f, 1.f), Vec4(0.8f, 0.8f, 0.8f, 1.f), Vec4(1.f) });
	pointLight2->SetAttenuationFactors(1.f, 0.09f, 0.032f);

	LightObject* pointLight3 = lightsSystem.AddNewLight({ Vec4::ZeroVector(), Vec4::ZeroVector(),
	Vec4(0.05f, 0.05f, 0.05f, 1.f), Vec4(0.8f, 0.8f, 0.8f, 1.f), Vec4(1.f) });
	pointLight3->SetAttenuationFactors(1.f, 0.09f, 0.032f);

	LightObject* pointLight4 = lightsSystem.AddNewLight({ Vec4::ZeroVector(), Vec4::ZeroVector(),
	Vec4(0.05f, 0.05f, 0.05f, 1.f), Vec4(0.8f, 0.8f, 0.8f, 1.f), Vec4(1.f) });
	pointLight4->SetAttenuationFactors(1.f, 0.09f, 0.032f);

	LightObject* dirLight1 = lightsSystem.AddNewLight({ Vec4::ZeroVector(), Vec4{-0.2f, -1.0f, -0.3f, 0.f},
	Vec4(0.05f, 0.05f, 0.05f, 1.f), Vec4(0.4f, 0.4f, 0.4f, 1.f), Vec4(0.5f, 0.5f, 0.5f, 1.f) });

	(void)dirLight1;


	MaterialDescriptor lightMatDesc(
		{
			{"Material_Color", ShaderStage::Fragment} }
	);

	MaterialInterface lightMatInterface = lib.CreateMaterialInterface(lightProgram, lightMatDesc);

	MaterialInstance lightMatInstance = lib.CreateMaterialInstance(lightMatInterface);

	lightMatInstance.UpdateUniformBlock(MaterialBlockBinding::MATERIAL_COLOR, ColorRGBAf::White());

	lightMatInstance.CreateMaterialResourceSet();

	/* End Light uniform buffer */


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

	m_flashLight->SetPosition(Vec4(newCam->GetTransform().Matrix().GetTranslation(), 1.F));
	m_flashLight->SetSpotlightDirection(newCam->GetFrontVector());

	/* Create camera end */


	/* Create Phong material buffer */

	MaterialDescriptor materialdesc(
		{
			{"Material_Phong", ShaderStage::Fragment},
			{"Material_DiffuseMap", ShaderStage::Fragment},
			{"Material_SpecularMap", ShaderStage::Fragment},
			{"Material_EmissionMap", ShaderStage::Fragment},
		}
	);

	MaterialInterface phongInterface = lib.CreateMaterialInterface(phongProgram, materialdesc);

	MaterialInstance containerInstance = lib.CreateMaterialInstance(phongInterface);
	containerInstance.UpdateUniformBlock(MaterialBlockBinding::MATERIAL_PHONG,
		PhongMaterial{ ColorRGBAf::White().ToVec(),
						ColorRGBAf::White().ToVec(),
						ColorRGBAf{0.5f}.ToVec(),
						64 });

	Texture2DHandle containerImg = MutRenderer().MutGraphicsDevice().CreateTexture2D(Texture2DFileDescriptor{ "Sandbox/assets/container2.png" });
	Texture2DHandle containerSpecularImg = MutRenderer().MutGraphicsDevice().CreateTexture2D(Texture2DFileDescriptor{ "Sandbox/assets/container2_specular.png" });
	Texture2DHandle emissionImg = MutRenderer().MutGraphicsDevice().CreateTexture2D(Texture2DFileDescriptor{ "Sandbox/assets/matrix.jpg" });


	containerInstance.BindTexture(MaterialTextureBinding::DIFFUSE, containerImg);
	containerInstance.BindTexture(MaterialTextureBinding::SPECULAR, containerSpecularImg);
	containerInstance.BindTexture(MaterialTextureBinding::EMISSION, emissionImg);

	containerInstance.CreateMaterialResourceSet();

	/* End Phong material buffer */


	auto phongCubeGeom = CreateCubePositionNormalTexture(0.5f);
	Mesh* phongCube = renderWorld.CreateStaticMesh(phongCubeGeom);


	auto lightCubeGeom = CreateCube(0.5f);
	Mesh* lightCube = renderWorld.CreateStaticMesh(lightCubeGeom);

	Vec3 cubePositions[] = {
	Vec3(0.0f,  0.0f,  0.0f),
	Vec3(2.0f,  5.0f, -15.0f),
	Vec3(-1.5f, -2.2f, -2.5f),
	Vec3(-3.8f, -2.0f, -12.3f),
	Vec3(2.4f, -0.4f, -3.5f),
	Vec3(-1.7f,  3.0f, -7.5f),
	Vec3(1.3f, -2.0f, -2.5f),
	Vec3(1.5f,  2.0f, -2.5f),
	Vec3(1.5f,  0.2f, -1.5f),
	Vec3(-1.3f,  1.0f, -1.5f)
	};

	Vec3 pointLightPositions[] = {
		Vec3(0.7f,  0.2f,  2.0f),
		Vec3(2.3f, -3.3f, -4.0f),
		Vec3(-4.0f,  2.0f, -12.0f),
		Vec3(0.0f,  0.0f, -3.0f)
	};

	LightObject* pointLights[] = {
		pointLight1,
		pointLight2,
		pointLight3,
		pointLight4
	};

	int iLight = 0;
	for (auto& lightPos : pointLightPositions)
	{
		pointLights[iLight]->SetPosition(Vec4(lightPos, 1.f));
		iLight++;
	}

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

		//phongCube->SetTransform(Mat4::Rotation(Degs_f(thisFrameTime * 20), Vec3(0.0f, 1.0f, 0.f)));

		Transform transf;
		transf *= Mat4::Rotation(Degs_f(thisFrameTime * 20), Vec3(0.0f, 1.0f, 0.f));
		transf *= Transform::Translate({ 2.0f, 0.5f, 0.F });
		transf *= Transform::Scale(Vec3{ 0.2f });

		lightCube->SetTransform(transf);



		//Vec3 lightColor(1);
		//lightColor.x() = sinf(thisFrameTime * 2.0f);
		//lightColor.y() = sinf(thisFrameTime * 0.7f);
		//lightColor.z() = sinf(thisFrameTime * 1.3f);

		//Vec3 diffuseColor = lightColor * Vec3(0.5f);
		//Vec3 ambientColor = diffuseColor * Vec3(0.2f);


		//pointLight1->SetPosition(Vec4(lightCube->GetTransform().Matrix().GetTranslation(), 1.F));

		//newLight->SetDiffuseColor(ColorRGBAf(diffuseColor.x(), diffuseColor.y(), diffuseColor.z()));
		//newLight->SetAmbientColor(ColorRGBAf(ambientColor.x(), ambientColor.y(), ambientColor.z()));
		//
		//lightMatInstance.SetVec4(MATERIAL_COLOR, "Color.colorValue", ColorRGBAf(diffuseColor.x(), diffuseColor.y(), diffuseColor.z()).ToVec());


		renderer.Clear(ColorRGBAf(0.1f, 0.1f, 0.1f, 1.0f));

		lightsSystem.UpdateLights();

		lightsSystem.BindLightBuffer();

		camSys.UpdateCameras();

		for (uint32_t iCam = 0; iCam < camSys.CamerasNumber(); iCam++)
		{
			camSys.BindCameraBuffer(iCam);

			renderer.UseMaterialInstance(&containerInstance);

			int iCube = 0;
			for (auto& pos : cubePositions)
			{

				float angle = 20.0f * iCube;

				phongCube->SetTransform(Transform::Translate(pos));
				phongCube->AddTransform(Mat4::Rotation(Degs_f(angle), Vec3(1.0f, 0.3f, 0.5f)));

				phongCube->UpdateObjectMatrices(camSys.GetCamera(iCam));
				renderWorld.DrawMesh(phongCube, phongVao, nullptr);

				iCube++;
			}

			renderer.UseMaterialInstance(&lightMatInstance);

			for (auto& lightPos : pointLightPositions)
			{
				lightCube->SetTransform(Transform::Translate(lightPos));
				lightCube->AddTransform(Transform::Scale(Vec3(0.2f)));
				lightCube->UpdateObjectMatrices(camSys.GetCamera(iCam));
				renderWorld.DrawMesh(lightCube, lightVao, nullptr);
			}

		}

		SwapBuffers();

	}

}

void TestApplication::Test3DPlane()
{
	Std140Mat<3> test{ Mat3::Identity() };

	MOE_LOG("addr test[0] : %x [1] : %x [2]: %x sizeof : %lu", &test.m_vec1, &test.m_vec2, &test.m_vec3, sizeof(test));

	BlendStateDescriptor desc = BlendStateDescriptor::ms_Disabled;
	PipelineDescriptor desc2;

	IGraphicsRenderer::ShaderFileList fileList =
	{
		{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/basic.vert" },
		{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/basic.frag" }
	};

	IGraphicsRenderer& renderer = MutRenderer();
	ShaderProgramHandle programHandle = renderer.CreateShaderProgramFromSourceFiles(fileList);

	VertexLayoutDescriptor layout{
		{{"position", VertexElementFormat::Float3},
				{"color", VertexElementFormat::Float3},
				{"texture", VertexElementFormat::Float2}},
		LayoutType::Interleaved
	};
	//
	//auto bim = MutRenderer().CreateVertexLayout(layout);
	//layout.SetType(LayoutType::Packed);
	//auto bam = MutRenderer().CreateVertexLayout(layout);
	//
	////Array<VertexPositionColor> myVertices = {
	//
	////}
	//
	//glm::mat4 test = glm::perspective(glm::radians(45.f), 1.f, 10.f, 1000.f);
	//
	//ColorRGB testcol = 0xff002a_rgb;
	//ColorRGBA testcola = 0xff000000_rgba;
	//ColorRGBf testcolf = 0xff0080_rgbf;
	//ColorRGBAf testcolaf = 0xff0080ff_rgbaf;
	//
	//Array<VertexPositionColor, 24> cubeGeom = CreateCube(0.5f);
	//
	//MeshHandle meshHandle = MutRenderer().CreateStaticMesh(cubeGeom);
	//
	//MutRenderer().DeleteStaticMesh(meshHandle);

	//ViewportHandle handle{0};
	//OrthographicCameraDesc orthodesc;
	PerspectiveCameraDesc persDesc{ 45_degf, GetWindowWidth() / (float)GetWindowHeight(), 0.1f, 100.f };
	//
	//CameraHandle camHandle = MutRenderer().CreateCamera(persDesc, ViewportDescriptor());



	ShaderProgramHandle helloQuadProgram = renderer.CreateShaderProgramFromSource(ShaderProgramDescriptor{
		{ ShaderStage::Vertex, quadVertexShaderSource },
		{  ShaderStage::Fragment, quadFragShaderSource }
		});

	ShaderProgramHandle helloTriProgram = renderer.CreateShaderProgramFromSource(ShaderProgramDescriptor{
		{ ShaderStage::Vertex, triVertexShaderSource },
		{  ShaderStage::Fragment, triFragShaderSource }
		});


	VertexLayoutDescriptor helloTriLayout{
		{
			{"position", VertexElementFormat::Float3},
			{"color", VertexElementFormat::Float4}
		},
		LayoutType::Packed
	};

	VertexLayoutDescriptor helloQuadLayout{
	{{"position", VertexElementFormat::Float3},
			{"color", VertexElementFormat::Float4},
			{"texture", VertexElementFormat::Float2}},
	LayoutType::Interleaved
	};

	auto helloTriLayoutHandle = MutRenderer().CreateVertexLayout(helloTriLayout);

	auto helloQuadLayoutHandle = MutRenderer().CreateVertexLayout(helloQuadLayout);

	Array<HelloTriVertex, 3> helloTri{
	{ {-0.8f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f} },
	{ {-0.6f, -0.5f, 0.0f}, {0.f, 1.f, 0.0f, 1.0f} },
	{ {-0.7f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f} }
	};

	float triPackedData[] = {
		0.0f, 0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		0.5f,  -0.5f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.f, 1.f, 0.0f,
		0.0f, 0.0f, 1.0f
	};


	Array<HelloQuadVertex, 4> helloQuad{
	{{0.5f, 0.5f, 0.0f } , {1.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f} },   // top right
	{{0.5f, -0.5f, 0.0f } , {0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},   // bottom right
	{{-0.5f, -0.5f, 0.0f} , {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},   // bottom left
	{{-0.5f, 0.5f, 0.0f } , {1.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}    // top left

	};

	Array<uint32_t, 6> helloIndices = {  // note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};


	DeviceBufferHandle uniformColorBuffer = MutRenderer().CreateUniformBufferFrom(ColorRGBAf::Green());


	ResourceLayoutDescriptor layoutDesc{
		{ "Color", ResourceKind::UniformBuffer, ShaderStage::Fragment },
		{ "DiffuseMap1", ResourceKind::TextureReadOnly, ShaderStage::Fragment },
		{ "DiffuseMap2", ResourceKind::TextureReadOnly, ShaderStage::Fragment }
	};

	auto resourceLayoutHandle = MutRenderer().CreateResourceLayout(layoutDesc);

	Texture2DHandle containerImg = MutRenderer().MutGraphicsDevice().CreateTexture2D(Texture2DFileDescriptor{ "Sandbox/assets/container.jpg" });
	Texture2DHandle kittehImg = MutRenderer().MutGraphicsDevice().CreateTexture2D(Texture2DFileDescriptor{ "Sandbox/assets/kitteh.png" });

	Texture2DHandle puppyImg = MutRenderer().MutGraphicsDevice().CreateTexture2D(Texture2DFileDescriptor{ "Sandbox/assets/puppy.png" });

	ResourceSetDescriptor rscSetDesc{
		resourceLayoutHandle,
		{uniformColorBuffer, kittehImg, puppyImg}
	};

	auto resourceSetHandle = MutRenderer().CreateResourceSet(rscSetDesc);

	RenderWorld& renderWorld = MutRenderer().CreateRenderWorld();

	//Mesh* helloTriMesh = renderWorld.CreateStaticMesh(helloTri);

	Mesh* helloPackedTriMeshHandle = renderWorld.CreateStaticMeshFromBuffer({ triPackedData, sizeof(triPackedData), 3 }, {});

	Mesh* helloQuadMeshHandle = renderWorld.CreateStaticMesh(helloQuad, helloIndices);

	Camera* helloCamera = renderWorld.CreateCamera(persDesc, ViewportDescriptor(0, 0, (float)GetWindowWidth(), (float)GetWindowHeight()));

	renderWorld.UseCamera(helloCamera);

	helloCamera->AddTransform(Transform::Translate(Vec3(0, 0, 3)));

	Transform transf = helloCamera->GetTransform();

	// Create a resource layout for ObjectMatrices.
	ResourceLayoutDescriptor objMatLayoutDesc{
	{ "CameraMatrices", ResourceKind::UniformBuffer, ShaderStage::Vertex },
	{ "ObjectMatrices", ResourceKind::UniformBuffer, ShaderStage::Vertex } };

	auto objMatLayoutHandle = MutRenderer().CreateResourceLayout(objMatLayoutDesc);

	DeviceBufferHandle cameraMatBuf = MutRenderer().CreateUniformBufferFrom(CameraMatrices());
	m_renderer.UpdateUniformBufferFrom(cameraMatBuf, helloCamera->GetCameraMatrices());

	DeviceBufferHandle matricesUniformBuffer = MutRenderer().CreateUniformBufferFrom(ObjectMatrices());

	ResourceSetDescriptor matricesSetDesc{ objMatLayoutHandle, {cameraMatBuf, matricesUniformBuffer} };

	auto objMatSetHandle = MutRenderer().CreateResourceSet(matricesSetDesc);


	Material quadMat(MutRenderer(), helloQuadProgram);
	quadMat.AddPerMaterialResourceSet(resourceSetHandle);
	quadMat.AddPerObjectResourceSet(objMatSetHandle);

	Material triMat(MutRenderer(), helloTriProgram);
	triMat.AddPerMaterialResourceSet(resourceSetHandle);
	triMat.AddPerObjectResourceSet(objMatSetHandle);

	helloQuadMeshHandle->SetTransform(Mat4::Rotation(-55_degf, Vec3(1, 0, 0)));

	while (WindowIsOpened())
	{
		PollInputEvents();

		float timeValue = GetApplicationTimeSeconds();

		// make the quad rotate
		helloPackedTriMeshHandle->SetTransform(Mat4::Translation(-0.5f, 0.f, 0.f));
		helloPackedTriMeshHandle->AddTransform(Mat4::Rotation(Degs_f(timeValue), Vec3(0.0f, 0.0f, -1.f)));


		//helloQuadMeshHandle->SetTransform(Mat4::Rotation(Degs_f(timeValue), Vec3(0.0f, 0.0f, 1.0f)));

		// update the uniform color
		float greenValue = sinf(timeValue) / 2.0f + 0.5f;
		m_renderer.UpdateUniformBufferFrom(uniformColorBuffer, ColorRGBAf(0.0f, greenValue, 0.0f, 1.0f));

		renderer.Clear(ColorRGBAf(0.2f, 0.3f, 0.3f, 1.0f));

		renderer.UseMaterial(&triMat);
		renderWorld.DrawMesh(helloPackedTriMeshHandle, helloTriLayoutHandle, &triMat);

		renderer.UseMaterial(&quadMat);
		renderWorld.DrawMesh(helloQuadMeshHandle, helloQuadLayoutHandle, &quadMat);

		SwapBuffers();

	}
}


void TestApplication::TestCubeWorld()
{
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

	BlendStateDescriptor desc = BlendStateDescriptor::ms_Disabled;
	PipelineDescriptor desc2;

	IGraphicsRenderer::ShaderFileList fileList =
	{
		{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/basic.vert" },
		{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/basic.frag" }
	};

	IGraphicsRenderer& renderer = MutRenderer();
	ShaderProgramHandle programHandle = renderer.CreateShaderProgramFromSourceFiles(fileList);

	VertexLayoutDescriptor layout{
		{{"position", VertexElementFormat::Float3},
				{"color", VertexElementFormat::Float3},
				{"texture", VertexElementFormat::Float2}},
		LayoutType::Interleaved
	};
	//
	//auto bim = MutRenderer().CreateVertexLayout(layout);
	//layout.SetType(LayoutType::Packed);
	//auto bam = MutRenderer().CreateVertexLayout(layout);
	//
	////Array<VertexPositionColor> myVertices = {
	//
	////}
	//
	//glm::mat4 test = glm::perspective(glm::radians(45.f), 1.f, 10.f, 1000.f);
	//
	//ColorRGB testcol = 0xff002a_rgb;
	//ColorRGBA testcola = 0xff000000_rgba;
	//ColorRGBf testcolf = 0xff0080_rgbf;
	//ColorRGBAf testcolaf = 0xff0080ff_rgbaf;
	//
	//Array<VertexPositionColor, 24> cubeGeom = CreateCube(0.5f);
	//
	//MeshHandle meshHandle = MutRenderer().CreateStaticMesh(cubeGeom);
	//
	//MutRenderer().DeleteStaticMesh(meshHandle);

	//ViewportHandle handle{0};
	//OrthographicCameraDesc orthodesc;
	PerspectiveCameraDesc persDesc{ 45_degf, GetWindowWidth() / (float)GetWindowHeight(), 0.1f, 100.f };
	//
	//CameraHandle camHandle = MutRenderer().CreateCamera(persDesc, ViewportDescriptor());



	ShaderProgramHandle helloQuadProgram = renderer.CreateShaderProgramFromSource(ShaderProgramDescriptor{
		{ ShaderStage::Vertex, quadVertexShaderSource },
		{  ShaderStage::Fragment, quadFragShaderSource }
		});

	ShaderProgramHandle helloTriProgram = renderer.CreateShaderProgramFromSource(ShaderProgramDescriptor{
		{ ShaderStage::Vertex, triVertexShaderSource },
		{  ShaderStage::Fragment, triFragShaderSource }
		});


	VertexLayoutDescriptor helloTriLayout{
		{
			{"position", VertexElementFormat::Float3},
			{"color", VertexElementFormat::Float4}
		},
		LayoutType::Packed
	};

	VertexLayoutDescriptor helloQuadLayout{
	{{"position", VertexElementFormat::Float3},
			{"color", VertexElementFormat::Float4},
			{"texture", VertexElementFormat::Float2}},
	LayoutType::Interleaved
	};

	auto helloTriLayoutHandle = MutRenderer().CreateVertexLayout(helloTriLayout);

	auto helloQuadLayoutHandle = MutRenderer().CreateVertexLayout(helloQuadLayout);

	Array<HelloTriVertex, 3> helloTri{
	{ {-0.8f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f} },
	{ {-0.6f, -0.5f, 0.0f}, {0.f, 1.f, 0.0f, 1.0f} },
	{ {-0.7f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f} }
	};

	float triPackedData[] = {
		0.0f, 0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		0.5f,  -0.5f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.f, 1.f, 0.0f,
		0.0f, 0.0f, 1.0f
	};


	auto helloQuad = CreateHelloQuadVertexCube(0.5f);

	Array<uint32_t, 6> helloIndices = {  // note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};


	DeviceBufferHandle uniformColorBuffer = MutRenderer().CreateUniformBufferFrom(ColorRGBAf::Green());


	ResourceLayoutDescriptor layoutDesc{
		{ "Color", ResourceKind::UniformBuffer, ShaderStage::Fragment },
		{ "DiffuseMap1", ResourceKind::TextureReadOnly, ShaderStage::Fragment },
		{ "DiffuseMap2", ResourceKind::TextureReadOnly, ShaderStage::Fragment }
	};

	auto resourceLayoutHandle = MutRenderer().CreateResourceLayout(layoutDesc);

	Texture2DHandle containerImg = MutRenderer().MutGraphicsDevice().CreateTexture2D(Texture2DFileDescriptor{ "Sandbox/assets/container.jpg" });
	Texture2DHandle kittehImg = MutRenderer().MutGraphicsDevice().CreateTexture2D(Texture2DFileDescriptor{ "Sandbox/assets/kitteh.png" });

	Texture2DHandle puppyImg = MutRenderer().MutGraphicsDevice().CreateTexture2D(Texture2DFileDescriptor{ "Sandbox/assets/puppy.png" });

	ResourceSetDescriptor rscSetDesc{
		resourceLayoutHandle,
		{uniformColorBuffer, kittehImg, puppyImg}
	};

	auto resourceSetHandle = MutRenderer().CreateResourceSet(rscSetDesc);

	RenderWorld& renderWorld = MutRenderer().CreateRenderWorld();

	//Mesh* helloTriMesh = renderWorld.CreateStaticMesh(helloTri);

//	Mesh* helloPackedTriMeshHandle = renderWorld.CreateStaticMeshFromBuffer({triPackedData, sizeof(triPackedData), 3}, {});

	Mesh* helloQuadMeshHandle = renderWorld.CreateStaticMesh(helloQuad);

	Camera* helloCamera = renderWorld.CreateCamera(persDesc, ViewportDescriptor(0, 0, (float)GetWindowWidth(), (float)GetWindowHeight()));

	renderWorld.UseCamera(helloCamera);

	m_currentCamera = helloCamera;
	// To make sure the camera points towards the negative z-axis by default we can give the yaw a default value of a 90 degree clockwise rotation.
	m_currentCamera->UpdateCameraVectors(0, -90);

	helloCamera->AddTransform(Transform::Translate(Vec3(0, 0, 3)));

	// Create a resource layout for ObjectMatrices.
	ResourceLayoutDescriptor objMatLayoutDesc{
	{ "CameraMatrices", ResourceKind::UniformBuffer, ShaderStage::Vertex },
	{ "ObjectMatrices", ResourceKind::UniformBuffer, ShaderStage::Vertex } };

	auto objMatLayoutHandle = MutRenderer().CreateResourceLayout(objMatLayoutDesc);

	DeviceBufferHandle cameraMatBuf = MutRenderer().CreateUniformBufferFrom(CameraMatrices());
	m_renderer.UpdateUniformBufferFrom(cameraMatBuf, helloCamera->GetCameraMatrices());

	DeviceBufferHandle matricesUniformBuffer = MutRenderer().CreateUniformBufferFrom(ObjectMatrices());

	ResourceSetDescriptor matricesSetDesc{ objMatLayoutHandle, {cameraMatBuf, matricesUniformBuffer} };

	auto objMatSetHandle = MutRenderer().CreateResourceSet(matricesSetDesc);


	Material quadMat(MutRenderer(), helloQuadProgram);
	quadMat.AddPerMaterialResourceSet(resourceSetHandle);
	quadMat.AddPerObjectResourceSet(objMatSetHandle);

	Material triMat(MutRenderer(), helloTriProgram);
	triMat.AddPerMaterialResourceSet(resourceSetHandle);
	triMat.AddPerObjectResourceSet(objMatSetHandle);


	Vec3 cubePositions[] = {
		Vec3(0.0f,  0.0f,  0.0f),
		Vec3(2.0f,  5.0f, -15.0f),
		Vec3(-1.5f, -2.2f, -2.5f),
		Vec3(-3.8f, -2.0f, -12.3f),
		Vec3(2.4f, -0.4f, -3.5f),
		Vec3(-1.7f,  3.0f, -7.5f),
		Vec3(1.3f, -2.0f, -2.5f),
		Vec3(1.5f,  2.0f, -2.5f),
		Vec3(1.5f,  0.2f, -1.5f),
		Vec3(-1.3f,  1.0f, -1.5f)
	};


	const float radius = 10.0f;


	while (WindowIsOpened())
	{
		PollInputEvents();

		float thisFrameTime = GetApplicationTimeSeconds();
		m_deltaTime = GetApplicationTimeSeconds() - m_lastFrame;
		m_lastFrame = thisFrameTime;

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

		//float camX = sinf(thisFrameTime) * radius;
		//float camZ = cosf(thisFrameTime) * radius;
		//helloCamera->LookAt(Vec3(camX, 0, camZ), Vec3::ZeroVector(), Vec3(0, 1, 0));

		// make the quad rotate
		//helloPackedTriMeshHandle->SetTransform(Mat4::Translation(-0.5f, 0.f, 0.f));
		//helloPackedTriMeshHandle->AddTransform(Mat4::Rotation(Degs_f(timeValue), Vec3(0.0f, 0.0f, -1.f)));


		//helloQuadMeshHandle->SetTransform(Mat4::Rotation(Degs_f(timeValue), Vec3(0.0f, 0.0f, 1.0f)));

		// update the uniform color
		float greenValue = sinf(thisFrameTime) / 2.0f + 0.5f;
		m_renderer.UpdateUniformBufferFrom(uniformColorBuffer, ColorRGBAf(0.0f, greenValue, 0.0f, 1.0f));

		renderer.Clear(ColorRGBAf(0.2f, 0.3f, 0.3f, 1.0f));

		//renderer.UseMaterial(&triMat);
		//renderWorld.DrawMesh(helloPackedTriMeshHandle, helloTriLayoutHandle, &triMat);

		renderer.UseMaterial(&quadMat);

		for (unsigned int i = 0; i < 10; i++)
		{
			helloQuadMeshHandle->SetTransform(Transform::Translate(cubePositions[i]));

			if (i % 2 == 0)
			{
				helloQuadMeshHandle->AddTransform(Mat4::Rotation(Degs(10 * i + thisFrameTime * 50_degf), Vec3(0.5f, 1.f, 0.f)));
			}
			else if (i % 3 == 0)
			{
				helloQuadMeshHandle->AddTransform(Mat4::Rotation(Degs_f(20.f * i), Vec3(1.f, 0.3f, 0.5f)));
			}
			else
			{
				helloQuadMeshHandle->AddTransform(Mat4::Rotation(Degs_f(10.f * i), Vec3(0.f, 0.5f, 0.7f)));
			}


			renderWorld.DrawMesh(helloQuadMeshHandle, helloQuadLayoutHandle, &quadMat);
		}


		SwapBuffers();

	}
}

}
