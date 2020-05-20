// Monocle Game Engine source files - Alexandre Baron

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

	class ObjectMatricesBlock : public IMaterialObjectBlock
	{
	public:

		uint32_t	GetObjectBlockDataSize() const override final
		{
			return (uint32_t)sizeof ObjectMatrices;
		}


		std::string GetObjectBlockName() const override
		{
			return "ObjectMatrices";
		}

	};



	class Texture2DBlock : public IMaterialBlock
	{
	public:
		Texture2DBlock(IGraphicsRenderer& renderer, Texture2DFileDescriptor& texDesc) :
			m_texture(renderer.MutGraphicsDevice().CreateTexture2D(texDesc))
		{}

	private:

		Texture2DHandle	m_texture;
	};





TestApplication::TestApplication(const OpenGLGlfwAppDescriptor& appDesc) :
	OpenGLGlfwApplication(appDesc)
{
	moe::GetLogChainSingleton().LinkTo(&m_logger);

}

struct VertexPositionColor
{
	Vec3	m_position;
	Vec4	m_color;
};

struct VertexPositionNormal
{
	Vec3	m_position;
	Vec3	m_normal;
};

struct VertexPosition
{
	Vec3	m_position;
};


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


Array<VertexPositionNormal, 36>	CreateCubePositionNormal(float halfExtent)
{

	return {
		{Vec3{-halfExtent, -halfExtent, -halfExtent}, {0.0f,  0.0f, -1.0f}},
		{Vec3{ halfExtent,  halfExtent, -halfExtent}, {0.0f,  0.0f, -1.0f}},
		{Vec3{ halfExtent, -halfExtent, -halfExtent}, {0.0f,  0.0f, -1.0f}},
		{Vec3{ halfExtent,  halfExtent, -halfExtent}, {0.0f,  0.0f, -1.0f}},
		{Vec3{-halfExtent, -halfExtent, -halfExtent}, {0.0f,  0.0f, -1.0f}},
		{Vec3{-halfExtent,  halfExtent, -halfExtent}, {0.0f,  0.0f, -1.0f}},

		{Vec3{ -halfExtent, -halfExtent,  halfExtent}, {0.0f,  0.0f, 1.0f}},
		{Vec3{  halfExtent, -halfExtent,  halfExtent}, {0.0f,  0.0f, 1.0f}},
		{Vec3{  halfExtent,  halfExtent,  halfExtent}, {0.0f,  0.0f, 1.0f}},
		{Vec3{  halfExtent,  halfExtent,  halfExtent}, {0.0f,  0.0f, 1.0f}},
		{Vec3{ -halfExtent,  halfExtent,  halfExtent}, {0.0f,  0.0f, 1.0f}},
		{Vec3{ -halfExtent, -halfExtent,  halfExtent}, {0.0f,  0.0f, 1.0f}},

		{Vec3{ -halfExtent,  halfExtent,  halfExtent}, {-1.0f,  0.0f,  0.0f}},
		{Vec3{ -halfExtent,  halfExtent, -halfExtent}, {-1.0f,  0.0f,  0.0f}},
		{Vec3{ -halfExtent, -halfExtent, -halfExtent}, {-1.0f,  0.0f,  0.0f}},
		{Vec3{ -halfExtent, -halfExtent, -halfExtent}, {-1.0f,  0.0f,  0.0f}},
		{Vec3{ -halfExtent, -halfExtent,  halfExtent}, {-1.0f,  0.0f,  0.0f}},
		{Vec3{ -halfExtent,  halfExtent,  halfExtent}, {-1.0f,  0.0f,  0.0f}},

		{Vec3{ halfExtent,  halfExtent,  halfExtent}, {1.0f,  0.0f,  0.0f}},
		{Vec3{ halfExtent, -halfExtent, -halfExtent}, {1.0f,  0.0f,  0.0f}},
		{Vec3{ halfExtent,  halfExtent, -halfExtent}, {1.0f,  0.0f,  0.0f}},
		{Vec3{ halfExtent, -halfExtent, -halfExtent}, {1.0f,  0.0f,  0.0f}},
		{Vec3{ halfExtent,  halfExtent,  halfExtent}, {1.0f,  0.0f,  0.0f}},
		{Vec3{ halfExtent, -halfExtent,  halfExtent}, {1.0f,  0.0f,  0.0f}},

		{Vec3{ -halfExtent, -halfExtent, -halfExtent}, {0.0f, -1.0f,  0.0f}},
		{Vec3{  halfExtent, -halfExtent, -halfExtent}, {0.0f, -1.0f,  0.0f}},
		{Vec3{  halfExtent, -halfExtent,  halfExtent}, {0.0f, -1.0f,  0.0f}},
		{Vec3{  halfExtent, -halfExtent,  halfExtent}, {0.0f, -1.0f,  0.0f}},
		{Vec3{ -halfExtent, -halfExtent,  halfExtent}, {0.0f, -1.0f,  0.0f}},
		{Vec3{ -halfExtent, -halfExtent, -halfExtent}, {0.0f, -1.0f,  0.0f}},

		{Vec3{ -halfExtent,  halfExtent, -halfExtent}, {0.0f,  1.0f,  0.0f}},
		{Vec3{  halfExtent,  halfExtent,  halfExtent}, {0.0f,  1.0f,  0.0f}},
		{Vec3{  halfExtent,  halfExtent, -halfExtent}, {0.0f,  1.0f,  0.0f}},
		{Vec3{  halfExtent,  halfExtent,  halfExtent}, {0.0f,  1.0f,  0.0f}},
		{Vec3{ -halfExtent,  halfExtent, -halfExtent}, {0.0f,  1.0f,  0.0f}},
		{Vec3{ -halfExtent,  halfExtent,  halfExtent}, {0.0f,  1.0f,  0.0f}}
	};

}


Array<VertexPosition, 36>	CreateCube(float halfExtent)
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


Array<VertexPositionColor, 36>	CreatePositionColorVertexCube(float halfExtent)
{
	const ColorRGBAf white = 0xffffffff_rgbaf;
	const Vec4 whiteAsVec4 = Vec4(white.R(), white.G(), white.B(), white.A());

	return {
		{Vec3{-halfExtent, -halfExtent, -halfExtent}, whiteAsVec4},
		{Vec3{ halfExtent, -halfExtent, -halfExtent}, whiteAsVec4},
		{Vec3{ halfExtent,  halfExtent, -halfExtent}, whiteAsVec4},
		{Vec3{ halfExtent,  halfExtent, -halfExtent}, whiteAsVec4},
		{Vec3{-halfExtent,  halfExtent, -halfExtent}, whiteAsVec4},
		{Vec3{-halfExtent, -halfExtent, -halfExtent}, whiteAsVec4},

		{Vec3{ -halfExtent, -halfExtent,  halfExtent}, whiteAsVec4},
		{Vec3{  halfExtent, -halfExtent,  halfExtent}, whiteAsVec4},
		{Vec3{  halfExtent,  halfExtent,  halfExtent}, whiteAsVec4},
		{Vec3{  halfExtent,  halfExtent,  halfExtent}, whiteAsVec4},
		{Vec3{ -halfExtent,  halfExtent,  halfExtent}, whiteAsVec4},
		{Vec3{ -halfExtent, -halfExtent,  halfExtent}, whiteAsVec4},

		{Vec3{ -halfExtent,  halfExtent,  halfExtent}, whiteAsVec4},
		{Vec3{ -halfExtent,  halfExtent, -halfExtent}, whiteAsVec4},
		{Vec3{ -halfExtent, -halfExtent, -halfExtent}, whiteAsVec4},
		{Vec3{ -halfExtent, -halfExtent, -halfExtent}, whiteAsVec4},
		{Vec3{ -halfExtent, -halfExtent,  halfExtent}, whiteAsVec4},
		{Vec3{ -halfExtent,  halfExtent,  halfExtent}, whiteAsVec4},

		{Vec3{ halfExtent,  halfExtent,  halfExtent}, whiteAsVec4},
		{Vec3{ halfExtent,  halfExtent, -halfExtent}, whiteAsVec4},
		{Vec3{ halfExtent, -halfExtent, -halfExtent}, whiteAsVec4},
		{Vec3{ halfExtent, -halfExtent, -halfExtent}, whiteAsVec4},
		{Vec3{ halfExtent, -halfExtent,  halfExtent}, whiteAsVec4},
		{Vec3{ halfExtent,  halfExtent,  halfExtent}, whiteAsVec4},

		{Vec3{ -halfExtent, -halfExtent, -halfExtent}, whiteAsVec4},
		{Vec3{  halfExtent, -halfExtent, -halfExtent}, whiteAsVec4},
		{Vec3{  halfExtent, -halfExtent,  halfExtent}, whiteAsVec4},
		{Vec3{  halfExtent, -halfExtent,  halfExtent}, whiteAsVec4},
		{Vec3{ -halfExtent, -halfExtent,  halfExtent}, whiteAsVec4},
		{Vec3{ -halfExtent, -halfExtent, -halfExtent}, whiteAsVec4},

		{Vec3{ -halfExtent,  halfExtent, -halfExtent}, whiteAsVec4},
		{Vec3{  halfExtent,  halfExtent, -halfExtent}, whiteAsVec4},
		{Vec3{  halfExtent,  halfExtent,  halfExtent}, whiteAsVec4},
		{Vec3{  halfExtent,  halfExtent,  halfExtent}, whiteAsVec4},
		{Vec3{ -halfExtent,  halfExtent,  halfExtent}, whiteAsVec4},
		{Vec3{ -halfExtent,  halfExtent, -halfExtent}, whiteAsVec4}
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
	Vec4	m_ambientColor{1.f};
	Vec4	m_diffuseColor{1.f};
	Vec4	m_specularColor{1.f};
	float	m_shininess{32};
};


struct LightCastersData
{
	Vec4	m_ambientColor{ColorRGBAf::White().ToVec()};
	Vec4	m_diffuseColor{ ColorRGBAf::White().ToVec() };
	Vec4	m_specularColor{ ColorRGBAf::White().ToVec() };
	Vec4	m_lightPos{ 0.f };
};





void TestApplication::Run()
{
	IGraphicsRenderer& renderer = MutRenderer();


	/* Create phong object shader */
	IGraphicsRenderer::ShaderFileList phongFileList =
	{
		{ ShaderStage::Vertex,		"source/Graphics/Resources/shaders/OpenGL/phong.vert" },
		{ ShaderStage::Fragment,	"source/Graphics/Resources/shaders/OpenGL/phong.frag" }
	};

	ShaderProgramHandle phongProgram = renderer.CreateShaderProgramFromSourceFiles(phongFileList);


	MaterialDescriptor materialdesc(
	{
		{"Material_Phong", ShaderStage::Fragment} }
	);


	MaterialLibrary lib(MutRenderer().MutGraphicsDevice());
	lib.AddBindingMapping("Object_Matrices", {MaterialBlockBinding::OBJECT_MATRICES, ResourceKind::UniformBuffer});
	lib.AddBindingMapping("Frame_Time", { MaterialBlockBinding::FRAME_TIME, ResourceKind::UniformBuffer });
	lib.AddBindingMapping("Frame_Lights", { MaterialBlockBinding::FRAME_LIGHTS, ResourceKind::UniformBuffer });
	lib.AddBindingMapping("View_Camera", { MaterialBlockBinding::VIEW_CAMERA, ResourceKind::UniformBuffer });
	lib.AddBindingMapping("Material_Phong", { MaterialBlockBinding::MATERIAL_PHONG, ResourceKind::UniformBuffer });
	lib.AddBindingMapping("Material_Color", { MaterialBlockBinding::MATERIAL_COLOR, ResourceKind::UniformBuffer });
	lib.AddBindingMapping("Material_SpecularMap", { MaterialTextureBinding::SPECULAR, ResourceKind::TextureReadOnly });

	lib.AddUniformBufferSizer(MaterialBlockBinding::FRAME_LIGHTS, []() { return sizeof(LightCastersData); });
	lib.AddUniformBufferSizer(MaterialBlockBinding::VIEW_CAMERA, []() { return sizeof(CameraMatrices); });
	lib.AddUniformBufferSizer(MaterialBlockBinding::MATERIAL_PHONG, []() { return sizeof(PhongMaterial); });
	lib.AddUniformBufferSizer(MaterialBlockBinding::MATERIAL_COLOR, []() { return sizeof(ColorRGBAf); });

	lib.AddUniformBufferSizer(MaterialBlockBinding::OBJECT_MATRICES, []() { return sizeof(ObjectMatrices); });


	SetInputKeyMapping(GLFW_KEY_W, GLFW_PRESS, [this]() { this->m_moveForward = true;});
	SetInputKeyMapping(GLFW_KEY_W, GLFW_RELEASE, [this]() { this->m_moveForward = false; });

	SetInputKeyMapping(GLFW_KEY_S, GLFW_PRESS, [this]() { this->m_moveBackward = true; });
	SetInputKeyMapping(GLFW_KEY_S, GLFW_RELEASE, [this]() { this->m_moveBackward = false; });

	SetInputKeyMapping(GLFW_KEY_A, GLFW_PRESS, [this]() { this->m_strafeLeft = true; });
	SetInputKeyMapping(GLFW_KEY_A, GLFW_RELEASE, [this]() { this->m_strafeLeft = false; });

	SetInputKeyMapping(GLFW_KEY_D, GLFW_PRESS, [this]() { this->m_strafeRight = true; });
	SetInputKeyMapping(GLFW_KEY_D, GLFW_RELEASE, [this]() { this->m_strafeRight = false; });

	auto [mouseX, mouseY] = GetMouseCursorPosition();
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
		VertexLayoutDescriptor::Interleaved
	};


	/* Create light VAO */
	VertexLayoutDescriptor lightLayout{
	{{"position", VertexElementFormat::Float3}},
		VertexLayoutDescriptor::Interleaved
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
		{"normal", VertexElementFormat::Float3}},
	VertexLayoutDescriptor::Interleaved
	};

	auto phongVao = renderer.CreateVertexLayout(phongLayout);


	/* Create Light uniform buffer */

	LightSystem lightsSystem(renderer.MutGraphicsDevice());

	LightObject* newLight = lightsSystem.AddNewLight({ Vec4::ZeroVector(),
	Vec4(1.f), Vec4(1.f), Vec4(1.f) });
//	Vec4(0.2f, 0.2f, 0.2f, 1.f), Vec4(0.5f, 0.5f, 0.5f, 1.f), Vec4(1.f) });

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


	/* Create camera end */


	/* Create Phong material buffer */

	MaterialInterface phongInterface = lib.CreateMaterialInterface(phongProgram, materialdesc);

	MaterialInstance emeraldInstance = lib.CreateMaterialInstance(phongInterface);
	emeraldInstance.UpdateUniformBlock(MaterialBlockBinding::MATERIAL_PHONG,
		PhongMaterial{	ColorRGBAf{0.0215f, 0.1745f, 0.0215f}.ToVec(),
						ColorRGBAf{0.07568f, 0.61424f, 0.07568f }.ToVec(),
						ColorRGBAf{0.633f, 0.727811f, 0.633f}.ToVec(),
						76.8f });

	emeraldInstance.CreateMaterialResourceSet();

	MaterialInstance pearlInstance = lib.CreateMaterialInstance(phongInterface);
	pearlInstance.UpdateUniformBlock(MaterialBlockBinding::MATERIAL_PHONG,
		PhongMaterial{ ColorRGBAf{0.25f, 	0.20725f, 	0.20725f}.ToVec(),
						ColorRGBAf{1.f, 	0.829f, 	0.829f, }.ToVec(),
						ColorRGBAf{0.296648f, 	0.296648f, 	0.296648f}.ToVec(),
						11.264f });

	pearlInstance.CreateMaterialResourceSet();

	MaterialInstance bronzeInstance = lib.CreateMaterialInstance(phongInterface);
	bronzeInstance.UpdateUniformBlock(MaterialBlockBinding::MATERIAL_PHONG,
		PhongMaterial{ ColorRGBAf{0.2125f, 	0.1275f, 	0.054f}.ToVec(),
						ColorRGBAf{0.714f, 	0.4284f, 	0.18144f }.ToVec(),
						ColorRGBAf{0.393548f, 	0.271906f, 	0.166721f}.ToVec(),
						25.6f });

	bronzeInstance.CreateMaterialResourceSet();

	MaterialInstance goldInstance = lib.CreateMaterialInstance(phongInterface);
	goldInstance.UpdateUniformBlock(MaterialBlockBinding::MATERIAL_PHONG,
		PhongMaterial{ ColorRGBAf{0.24725f, 	0.1995f, 	0.0745f}.ToVec(),
						ColorRGBAf{0.75164f, 	0.60648f, 	0.22648f 	 }.ToVec(),
						ColorRGBAf{0.628281f, 	0.555802f, 	0.366065f}.ToVec(),
						51.2f });

	goldInstance.CreateMaterialResourceSet();

	MaterialInstance cyanPlastic = lib.CreateMaterialInstance(phongInterface);
	cyanPlastic.UpdateUniformBlock(MaterialBlockBinding::MATERIAL_PHONG,
		PhongMaterial{ ColorRGBAf{0.0f, 	0.1f, 	0.06f}.ToVec(),
						ColorRGBAf{0.0f, 	0.50980392f, 	0.50980392f }.ToVec(),
						ColorRGBAf{0.50196078f}.ToVec(),
						32.f });

	cyanPlastic.CreateMaterialResourceSet();

	MaterialInstance redPlastic = lib.CreateMaterialInstance(phongInterface);
	redPlastic.UpdateUniformBlock(MaterialBlockBinding::MATERIAL_PHONG,
		PhongMaterial{ ColorRGBAf{0.f}.ToVec(),
						ColorRGBAf{ 	0.5f, 	0.0f, 	0.0f }.ToVec(),
						ColorRGBAf{ 0.7f, 	0.6f, 	0.6f}.ToVec(),
						32.f});

	redPlastic.CreateMaterialResourceSet();

	MaterialInstance greenRubberInstance  = lib.CreateMaterialInstance(phongInterface);
	greenRubberInstance.UpdateUniformBlock(MaterialBlockBinding::MATERIAL_PHONG,
		PhongMaterial{ ColorRGBAf{0.0f, 	0.05f, 	0.0f}.ToVec(),
						ColorRGBAf{0.4f, 	0.5f, 	0.4f }.ToVec(),
						ColorRGBAf{0.04f, 	0.7f, 	0.04f}.ToVec(),
						10.f });

	greenRubberInstance.CreateMaterialResourceSet();


	MaterialInstance yellowRubberInstance = lib.CreateMaterialInstance(phongInterface);
	yellowRubberInstance.UpdateUniformBlock(MaterialBlockBinding::MATERIAL_PHONG,
		PhongMaterial{ ColorRGBAf{0.05f, 	0.05f, 	0.0f}.ToVec(),
						ColorRGBAf{0.5f, 	0.5f, 	0.4f }.ToVec(),
						ColorRGBAf{0.7f, 	0.7f, 	0.04f}.ToVec(),
						10.f });

	yellowRubberInstance.CreateMaterialResourceSet();

	/* End Phong material buffer */


	auto phongCubeGeom = CreateCubePositionNormal(0.5f);
	Mesh* phongCube = renderWorld.CreateStaticMesh(phongCubeGeom);


	auto lightCubeGeom = CreateCube(0.5f);
	Mesh* lightCube = renderWorld.CreateStaticMesh(lightCubeGeom);


	Vec3 positions[8] = {
		{-4, 0, 0},
		{-2, 0, 0},
		{0, 0, 0},
		{2, 0, 0},
		{-4, 2, 0},
		{-2, 2, 0},
		{0, 2, 0},
		{2, 2, 0}
	};
	MaterialInstance* materials[8]
	{
		&emeraldInstance, &pearlInstance, &bronzeInstance, &goldInstance, &cyanPlastic, &redPlastic, &greenRubberInstance, &yellowRubberInstance
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

		//phongCube->SetTransform(Mat4::Rotation(Degs_f(thisFrameTime * 20), Vec3(0.0f, 1.0f, 0.f)));

		Transform transf;
		transf *= Mat4::Rotation(Degs_f(thisFrameTime * 20), Vec3(0.0f, 1.0f, 0.f));
		transf *= Transform::Translate({ 10.0f, 1.5f, 0.F });
		transf *= Transform::Scale(Vec3{ 0.2f });

		lightCube->SetTransform(transf);

		Vec3 lightColor(1);
		lightColor.x() = sinf(thisFrameTime * 2.0f);
		lightColor.y() = sinf(thisFrameTime * 0.7f);
		lightColor.z() = sinf(thisFrameTime * 1.3f);

		Vec3 diffuseColor = lightColor * Vec3(0.5f);
		Vec3 ambientColor = diffuseColor * Vec3(0.2f);


		newLight->SetPosition(Vec4(lightCube->GetTransform().Matrix().GetTranslation(), 1.F));
		newLight->SetDiffuseColor(ColorRGBAf(diffuseColor.x(), diffuseColor.y(), diffuseColor.z()));
		newLight->SetAmbientColor(ColorRGBAf(ambientColor.x(), ambientColor.y(), ambientColor.z()));

		lightMatInstance.SetVec4(MATERIAL_COLOR, "Color.colorValue", ColorRGBAf(diffuseColor.x(), diffuseColor.y(), diffuseColor.z()).ToVec());


		renderer.Clear(ColorRGBAf(0.1f, 0.1f, 0.1f, 1.0f));

		lightsSystem.UpdateLights();

		lightsSystem.BindLightBuffer();

		camSys.UpdateCameras();

		for (uint32_t iCam = 0; iCam < camSys.CamerasNumber(); iCam++)
		{
			camSys.BindCameraBuffer(iCam);

			for (int iObj = 0; iObj < 8; iObj++)
			{
				renderer.UseMaterialInstance(materials[iObj]);

				phongCube->SetTransform(Transform::Translate(positions[iObj]));
				phongCube->UpdateObjectMatrices(camSys.GetCamera(iCam));
				renderWorld.DrawMesh(phongCube, phongVao, nullptr);
			}

			renderer.UseMaterialInstance(&lightMatInstance);
			lightCube->UpdateObjectMatrices(camSys.GetCamera(iCam));
			renderWorld.DrawMesh(lightCube, lightVao, nullptr);
		}

		SwapBuffers();

	}

}


	void TestApplication::TestSceneGraph()
	{

		SceneGraph graph(12);

		SceneNodeHandle bodyHandle = graph.CreateNode<ASceneNode>(SceneGraph::GetRootHandle(), Transform::Translate({ 0, 35, 0 }));
		SceneNodeHandle headHandle = graph.CreateNode<ASceneNode>(bodyHandle, Transform::Translate({ 0, 30, 0 }));
		SceneNodeHandle leftArmHandle = graph.CreateNode<ASceneNode>(bodyHandle, Transform::Translate({ -12, 30, -1 }));
		SceneNodeHandle rightArmHandle = graph.CreateNode<ASceneNode>(bodyHandle, Transform::Translate({ 12, 30, -1 }));
		SceneNodeHandle leftLegHandle = graph.CreateNode<ASceneNode>(bodyHandle, Transform::Translate({ -8, 0,  0}));
		SceneNodeHandle rightLegHandle = graph.CreateNode<ASceneNode>(bodyHandle, Transform::Translate({ 8, 0, 0 }));
		SceneNodeHandle leftFootHandle = graph.CreateNode<ASceneNode>(leftLegHandle, Transform::Translate({ 0, -35, 0 }));
		SceneNodeHandle rightFootHandle = graph.CreateNode<ASceneNode>(rightLegHandle, Transform::Translate({ 0, -35, 0 }));


		ASceneNode* body = graph.MutSceneNode(bodyHandle);
		body->SetWorldTransform(Transform::Translate({10, 0, 0}));


		SceneNodeHandle cameraHandle = graph.CreateNode<CameraNode>(SceneGraph::GetRootHandle(), Transform::Identity(), "krou");

		graph.RemoveNode(cameraHandle);

		graph.RemoveNode(bodyHandle);


		SceneNodeHandle nodeHandle3 = graph.CreateNode<ASceneNode>(SceneGraph::GetRootHandle(), Transform::Identity());

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
			VertexLayoutDescriptor::Interleaved
		};
		//
		//auto bim = MutRenderer().CreateVertexLayout(layout);
		//layout.SetType(VertexLayoutDescriptor::Packed);
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
			VertexLayoutDescriptor::Packed
		};

		VertexLayoutDescriptor helloQuadLayout{
		{{"position", VertexElementFormat::Float3},
				{"color", VertexElementFormat::Float4},
				{"texture", VertexElementFormat::Float2}},
		VertexLayoutDescriptor::Interleaved
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
			VertexLayoutDescriptor::Interleaved
		};
		//
		//auto bim = MutRenderer().CreateVertexLayout(layout);
		//layout.SetType(VertexLayoutDescriptor::Packed);
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
			VertexLayoutDescriptor::Packed
		};

		VertexLayoutDescriptor helloQuadLayout{
		{{"position", VertexElementFormat::Float3},
				{"color", VertexElementFormat::Float4},
				{"texture", VertexElementFormat::Float2}},
		VertexLayoutDescriptor::Interleaved
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


	void TestApplication::CameraMoveForward()
	{
		if (m_currentCamera != nullptr)
		{
			Vec3 camPos = m_currentCamera->GetTransform().Matrix().GetTranslation();
			camPos += (m_deltaTime * m_cameraSpeed) * m_currentCamera->GetFrontVector();

			m_currentCamera->LookAt(camPos, camPos + m_currentCamera->GetFrontVector(), m_currentCamera->GetUpVector());
		}
	}


	void TestApplication::CameraMoveBackwards()
	{
		if (m_currentCamera != nullptr)
		{
			Vec3 camPos = m_currentCamera->GetTransform().Matrix().GetTranslation();
			camPos -= (m_deltaTime * m_cameraSpeed) * m_currentCamera->GetFrontVector();

			m_currentCamera->LookAt(camPos, camPos + m_currentCamera->GetFrontVector(), m_currentCamera->GetUpVector());
		}
	}


	void TestApplication::CameraMoveStrafeLeft()
	{
		if (m_currentCamera != nullptr)
		{
			Vec3 camPos = m_currentCamera->GetTransform().Matrix().GetTranslation();
			camPos -= (m_deltaTime * m_cameraSpeed) * m_currentCamera->GetRightVector();

			m_currentCamera->LookAt(camPos, camPos + m_currentCamera->GetFrontVector(), m_currentCamera->GetUpVector());
		}
	}


	void TestApplication::CameraMoveStrafeRight()
	{
		if (m_currentCamera != nullptr)
		{
			Vec3 camPos = m_currentCamera->GetTransform().Matrix().GetTranslation();
			camPos += (m_deltaTime * m_cameraSpeed) * m_currentCamera->GetRightVector();

			m_currentCamera->LookAt(camPos, camPos + m_currentCamera->GetFrontVector(), m_currentCamera->GetUpVector());
		}

	}


	void TestApplication::OrientCameraWithMouse(double xpos, double ypos)
	{
		if (m_currentCamera == nullptr)
		{
			return;
		}

		const float xoffset = ((float)xpos - m_lastX) * m_mouseSensitivity;
		const float yoffset = (m_lastY - (float)ypos) * m_mouseSensitivity; // reversed since y-coordinates range from bottom to top

		m_lastX = (float)xpos;
		m_lastY = (float)ypos;

		float newCamYaw = m_currentCamera->GetYaw() + xoffset;
		float newCamPitch = m_currentCamera->GetPitch() + yoffset;

		m_currentCamera->UpdateCameraVectors(newCamPitch, newCamYaw);

		const Vec3 camPos = m_currentCamera->GetTransform().Matrix().GetTranslation();

		m_currentCamera->LookAt(camPos + m_currentCamera->GetFrontVector(), m_currentCamera->GetUpVector());
	}



	void TestApplication::CameraZoomMouseScroll(double /*xoffset*/, double yoffset)
	{
		if (m_currentCamera == nullptr)
		{
			return;
		}

		//Degs_f fov = m_currentCamera->GetFovY();

		//if (fov >= 1.0f && fov <= 45.0f)
		//	fov -= (float)yoffset;
		//else if (fov <= 1.0f)
		//	fov = 1_degf;
		//else if (fov >= 45.0f)
		//{
		//	fov = 45_degf;
		//}


		m_currentCamera->AddFoVY(-Degs_f((float)yoffset));
	}
}
