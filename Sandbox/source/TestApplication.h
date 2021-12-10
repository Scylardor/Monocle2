#pragma once

#include "Application/GlfwApplication/OpenGL/OpenGLGlfwApplication.h"

#include "Math/Vec2.h"

namespace moe
{


class TestApplication final : public moe::OpenGLGlfwApplication
{
public:

	TestApplication(const moe::OpenGLGlfwAppDescriptor& appDesc);
	~TestApplication() = default;

	void	Run();

	// Test Basics
	void	TestSceneGraph();
	void	Test3DPlane();
	void	TestCubeWorld();
	void	TestMultiLights();

	// Test Advanced 1
	void	TestVisualizeDepthBuffer();
	void	TestStencilBuffer();
	void	TestBlending();
	void	TestFramebuffer();
	void	TestCubemap();
	void	TestGeometryShaderHouses();
	void	TestGeometryShaderExplode();
	void	TestInstancedAsteroids();

	// Test Advanced Lighting
	void	TestBlinnPhong();
	void	TestGammaCorrection();
	void	TestShadowMapping();
	void	TestOmnidirectionalShadowMapping();

	void	TestNormalMapping();
	void	TestParallaxMapping();
	void	TestHDR();
	void	TestBloom();
	void	TestDeferredRendering();
	void	TestSSAO();


	// Test PBR
	void	TestPBRLighting();
	void	TestPBRTextured();

	void	TestPBRIrradianceConversion();

	// Others
	void	CameraMoveForward();
	void	CameraMoveBackwards();
	void	CameraMoveStrafeLeft();
	void	CameraMoveStrafeRight();

	void	OrientCameraWithMouse(double xpos, double ypos);

	void	CameraZoomMouseScroll(double xpos, double ypos);


	struct VertexPositionTexture
	{
		Vec3	m_position;
		Vec2	m_texture;
	};


private:

	struct VertexPosition
	{
		Vec3	m_position;
	};


	struct VertexPositionNormalTexture
	{
		Vec3	m_position;
		Vec3	m_normal;
		Vec2	m_texcoords;
	};

	struct VertexPositionNormalTextureTangentBitangent
	{
		Vec3	m_position;
		Vec3	m_normal;
		Vec2	m_texcoords;
		Vec3	m_tangent;
		Vec3	m_bitangent;
	};

	struct PhongMaterial
	{
		Vec4	m_ambientColor{ 1.f };
		Vec4	m_diffuseColor{ 1.f };
		Vec4	m_specularColor{ 1.f };
		float	m_shininess{ 32 };
	};


	struct ToneMappingParams
	{
		uint32_t	m_enabled{ true };	// Use int instead of bool for std 140
		float		m_exposure{ 1.f };
		uint32_t	m_useReinhardToneMapping{ 0 };
		float padding;
	};


	#define TWO_PASS_GAUSSIAN_BLUR_MAX_WEIGHTS 32

	struct TwoPassGaussianBlurParams
	{
		int			m_horizontalBlur = true; // is this the horizontal pass or the vertical pass ? Use int instead of bool for std140 requirements
		// Dimensions of the texture to sample
		int			m_textureWidth = 0;
		int			m_textureHeight = 0;
		uint32_t	m_weightsSize = 0;	// number of weights to read in the buffer
		Std140Array<float, TWO_PASS_GAUSSIAN_BLUR_MAX_WEIGHTS>	m_weightsBuffer; // use Vec4 to align for std140
	};


	Array<VertexPosition, 36>	CreateCube(float halfExtent);
	Array<VertexPositionNormalTexture, 36>	CreateCubePositionNormalTexture(float halfExtent, bool invertNormals = false);
	Array<VertexPositionNormalTexture, 24>	CreateIndexedCubePositionNormalTexture(float halfExtent, bool invertNormals = false);

	Array<VertexPositionNormalTexture, 36>	CreateCubePositionNormalTexture_ReversedNormals(float halfExtent);

	std::pair<Vector<TestApplication::VertexPositionNormalTexture>, Vector<uint32_t>>	CreateSphereGeometry(unsigned int latitudeSegments, unsigned int longitudeSegments);



	Camera*	m_currentCamera = nullptr;
	float	m_deltaTime{0.f};
	float	m_lastFrame = 0.0f; // Time of last frame
	const float m_cameraSpeed{5.F};


	bool		m_moveForward = false;
	bool		m_moveBackward = false;
	bool		m_strafeLeft = false;
	bool		m_strafeRight = false;
	float		m_lastX = 0;
	float		m_lastY = 0;
	float		m_cameraYaw = -90.f;
	float		m_cameraPitch = 0.f;
	const float m_mouseSensitivity = 0.05f;

	class LightObject*	m_flashLight = nullptr;

	ToneMappingParams	m_toneMappingParams;
};

}
