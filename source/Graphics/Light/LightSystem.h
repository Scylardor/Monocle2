// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Containers/FreeList/Freelist.h"
#include "Core/Containers/Vector/Vector.h"
#include "Graphics/Device/GraphicsDevice.h"
#include "Graphics/DeviceBuffer/DeviceBufferHandle.h"
#include "Math/Vec4.h"

#include "LightObject.h"

#include "Graphics/Material/MaterialBindings.h"

#include "Graphics/OpenGL/Std140.h"

namespace moe
{
	enum
	{
		MAX_LIGHTS = 16
	};

	 struct LightData
	{
		alignas(16) Vec4	m_position{0, 0, 0, 1};
		Vec4	m_direction{ 0 };
		Vec4	m_ambientColor{1};
		Vec4	m_diffuseColor{1};
		Vec4	m_specularColor{1};
		float	m_constantAttenuation{1};
		float	m_linearAttenuation{0};
		float	m_quadraticAttenuation{0};
		float	m_spotLightInnerCutoff{0};
		float	m_spotLightOuterCutoff{ 0 };
	};

	// Disable padding warning because padding here is exactly what we want to satisfy Std140 layout.
	#pragma warning (push)
	#pragma warning (disable : 4324)
	struct LightCastersData
	{
		using AlignedLightData = LightData;

		uint32_t	m_numLights{0};
		LightData	m_lightDataBuffer[MAX_LIGHTS];
	};
	#pragma warning (pop)

	class LightSystem
	{

	public:
		Monocle_Graphics_API LightSystem(IGraphicsDevice& device);
		Monocle_Graphics_API ~LightSystem();


		Monocle_Graphics_API void	UpdateLights();

		Monocle_Graphics_API void	BindLightBuffer();

		Monocle_Graphics_API LightObject*	AddNewLight(LightData newLightData);

		Monocle_Graphics_API void			RemoveLight(LightObject* light);

		bool			NeedsUpdate() const { return m_needBufferUpdateFlag; }

		uint32_t		LightsNumber() const { return m_lightCastersData.m_numLights; }

		size_t			GetLightsBufferSizeBytes() const { return sizeof(m_lightCastersData); }

		size_t			GetLightDataSizeBytes() const { return sizeof(LightCastersData::AlignedLightData); }


		void	SetLightPosition(uint32_t lightIdx, const Vec4& pos);

		void	SetLightDirection(uint32_t lightIdx, const Vec3& dir);

		void	SetLightAmbientColor(uint32_t lightIdx, const ColorRGBAf& ambient);

		void	SetLightDiffuseColor(uint32_t lightIdx, const ColorRGBAf& diffuse);

		void	SetLightSpecularColor(uint32_t lightIdx, const ColorRGBAf& specular);

		void	MakeDirectionalLight(uint32_t lightIdx, const Vec3& direction);


		void	SetConstantAttenuation(uint32_t lightIdx, float cstAtten);

		void	SetLinearAttenuation(uint32_t lightIdx, float linAtten);

		void	SetQuadraticAttenuation(uint32_t lightIdx, float quadAtten);

		void	SetAttenuationFactors(uint32_t lightIdx, float constant, float linear, float quadratic);

		void	SetLightSpotInnerCutoff(uint32_t lightIdx, float cutoff);

		void	SetLightSpotOuterCutoff(uint32_t lightIdx, float cutoff);


		DeviceBufferHandle	GetLightsBufferHandle() const { return m_lightsUniformBlock; }

	private:

		IGraphicsDevice&	m_device;

		DeviceBufferHandle	m_lightsUniformBlock;

		ResourceLayoutHandle	m_lightsResourceLayout;

		ResourceSetHandle		m_lightsResourceSet;

		//Vector<LightData>	m_lightDataBuffer;
		LightCastersData	m_lightCastersData;

		Vector<LightObject>		m_lightObjects;

		Vector<LightObject*>	m_lightsToUpdate;

		//uint32_t	m_nextLightIdx = 0;

		bool		m_needBufferUpdateFlag = false;

	};

}
