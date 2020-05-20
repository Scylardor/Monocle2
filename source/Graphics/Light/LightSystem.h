// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Containers/FreeList/Freelist.h"
#include "Core/Containers/Vector/Vector.h"
#include "Graphics/Device/GraphicsDevice.h"
#include "Graphics/DeviceBuffer/DeviceBufferHandle.h"
#include "Math/Vec4.h"

#include "LightObject.h"

#include "Graphics/Material/MaterialBindings.h"

namespace moe
{
	struct LightData
	{
		Vec4	m_position{0};
		Vec4	m_ambientColor{1};
		Vec4	m_diffuseColor{1};
		Vec4	m_specularColor{1};
	};

	class LightSystem
	{

	public:
		Monocle_Graphics_API LightSystem(IGraphicsDevice& device);
		Monocle_Graphics_API ~LightSystem();


		Monocle_Graphics_API void	UpdateLights();

		Monocle_Graphics_API void	BindLightBuffer();

		Monocle_Graphics_API LightObject*	AddNewLight(const LightData& newLightData);

		Monocle_Graphics_API void			RemoveLight(LightObject* light);

		bool			NeedsUpdate() const { return m_needBufferUpdateFlag; }

		uint32_t		LightsNumber() const { return m_nextLightIdx; }

		size_t			GetLightsBufferSizeBytes() const { return GetLightDataSizeBytes() * m_lightDataBuffer.Capacity(); }

		size_t			GetLightDataSizeBytes() const { return sizeof(decltype(m_lightDataBuffer)::ValueType); }


		void	SetLightPosition(uint32_t lightIdx, const Vec4& pos);

		void	SetLightAmbientColor(uint32_t lightIdx, const ColorRGBAf& ambient);

		void	SetLightDiffuseColor(uint32_t lightIdx, const ColorRGBAf& diffuse);

		void	SetLightSpecularColor(uint32_t lightIdx, const ColorRGBAf& specular);


		DeviceBufferHandle	GetLightsBufferHandle() const { return m_lightsUniformBlock; }

	private:

		IGraphicsDevice&	m_device;

		DeviceBufferHandle	m_lightsUniformBlock;

		ResourceLayoutHandle	m_lightsResourceLayout;

		ResourceSetHandle		m_lightsResourceSet;

		Vector<LightData>	m_lightDataBuffer;

		Vector<LightObject>		m_lightObjects;

		Vector<LightObject*>	m_lightsToUpdate;

		uint32_t	m_nextLightIdx = 0;

		bool		m_needBufferUpdateFlag = false;

		static const int	ms_MAX_LIGHTS = 16;
	};

}
