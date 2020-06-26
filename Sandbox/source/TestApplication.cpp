// Monocle Game Engine source files - Alexandre Baron

#include "TestApplication.h"


#include <glfw3/include/GLFW/glfw3.h>

#include "Math/Vec2.h"
#include <Math/Vec3.h>
#include <Math/Vec4.h>

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

struct VertexPositionTexture
{
	Vec3	m_position;
	Vec2	m_texture;
};


struct VertexPositionNormalTexture
{
	Vec3	m_position;
	Vec3	m_normal;
	Vec2	m_texcoords;
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
//
//
//struct LightCastersData
//{
//	Vec4	m_ambientColor{ColorRGBAf::White().ToVec()};
//	Vec4	m_diffuseColor{ ColorRGBAf::White().ToVec() };
//	Vec4	m_specularColor{ ColorRGBAf::White().ToVec() };
//	Vec4	m_lightPos{ 0.f };
//};


struct ProjectionPlanes
{
	float m_near{0};
	float m_far{0};
};


void TestApplication::Run()
{
	TestHDR();

}

	void TestApplication::CameraMoveForward()
	{
		if (m_currentCamera != nullptr)
		{
			Vec3 camPos = m_currentCamera->GetTransform().Matrix().GetTranslation();
			camPos += (m_deltaTime * m_cameraSpeed) * m_currentCamera->GetFrontVector();

			m_currentCamera->LookAt(camPos, camPos + m_currentCamera->GetFrontVector(), m_currentCamera->GetUpVector());

			if (m_flashLight)
			{
				m_flashLight->SetPosition(Vec4(camPos, 1));
			}
		}
	}


	void TestApplication::CameraMoveBackwards()
	{
		if (m_currentCamera != nullptr)
		{
			Vec3 camPos = m_currentCamera->GetTransform().Matrix().GetTranslation();
			camPos -= (m_deltaTime * m_cameraSpeed) * m_currentCamera->GetFrontVector();

			m_currentCamera->LookAt(camPos, camPos + m_currentCamera->GetFrontVector(), m_currentCamera->GetUpVector());

			if (m_flashLight)
			{
				m_flashLight->SetPosition(Vec4(camPos, 1));
			}
		}
	}


	void TestApplication::CameraMoveStrafeLeft()
	{
		if (m_currentCamera != nullptr)
		{
			Vec3 camPos = m_currentCamera->GetTransform().Matrix().GetTranslation();
			camPos -= (m_deltaTime * m_cameraSpeed) * m_currentCamera->GetRightVector();

			m_currentCamera->LookAt(camPos, camPos + m_currentCamera->GetFrontVector(), m_currentCamera->GetUpVector());

			if (m_flashLight)
			{
				m_flashLight->SetPosition(Vec4(camPos, 1));
			}
		}
	}


	void TestApplication::CameraMoveStrafeRight()
	{
		if (m_currentCamera != nullptr)
		{
			Vec3 camPos = m_currentCamera->GetTransform().Matrix().GetTranslation();
			camPos += (m_deltaTime * m_cameraSpeed) * m_currentCamera->GetRightVector();

			m_currentCamera->LookAt(camPos, camPos + m_currentCamera->GetFrontVector(), m_currentCamera->GetUpVector());

			if (m_flashLight)
			{
				m_flashLight->SetPosition(Vec4(camPos, 1));
			}
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

		if (m_flashLight)
		{
			m_flashLight->SetSpotlightDirection(m_currentCamera->GetFrontVector());
		}
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
