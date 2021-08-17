#pragma once
#ifdef MOE_VULKAN

#include "Graphics/Vulkan/VulkanMacros.h"
#include "Math/Vec4.h"
#include "Graphics/Color/Color.h"
#include "Core/Containers/SparseArray/SparseArray.h"

#include "Graphics/Vulkan/Buffer/VulkanBuffer.h"
#include "Graphics/Vulkan/DescriptorSet/VulkanDescriptorSetAllocator.h"

#include <variant>

namespace moe
{

	// Properties common to all light types.
	struct BaseLight
	{};

	struct DirectionalLight : BaseLight
	{
		ColorRGBAf	Ambient{ ColorRGBAf::Black() };
		ColorRGBAf	Diffuse{ ColorRGBAf::Black() };
		ColorRGBAf	Specular{ ColorRGBAf::Black() };
		Vec4		Direction{ Vec4::ZeroVector() };
	};


	// Lights with a position in the 3D world.
	struct PointLight : BaseLight
	{
		ColorRGBf	Ambient{ ColorRGBf::Black() };
		float		ConstantAttenuation{ 1 };
		ColorRGBf	Diffuse{ ColorRGBf::Black() };
		float		LinearAttenuation{ 0 };
		ColorRGBf	Specular{ ColorRGBf::Black() };
		float		QuadraticAttenuation{ 0 };
		Vec4		Position{ Vec4::ZeroVector() }; // The last byte is useless but there for padding purposes
	};


	struct SpotLight : BaseLight
	{
		ColorRGBf	Ambient{ ColorRGBf::Black() };
		float		ConstantAttenuation{ 1 };
		ColorRGBf	Diffuse{ ColorRGBf::Black() };
		float		LinearAttenuation{ 0 };
		ColorRGBf	Specular{ ColorRGBf::Black() };
		float		QuadraticAttenuation{ 0 };
		Vec3		Position{ Vec3::ZeroVector() };
		float		SpotInnerCutoff{ 0 };
		Vec3		Direction{ Vec3::ZeroVector() };
		float		SpotOuterCutoff{ 0 };
	};

	using LightID = uint32_t;
	using LightVariant = std::variant<PointLight, DirectionalLight, SpotLight>;
	static const LightID INVALID_LIGHT = ~0u;

	enum class LightType : uint8_t
	{
		Directional,
		Point,
		Spot,
		_COUNT_,
		_INVALID_ = _COUNT_,
	};

	class VulkanLightSystem;

	class Light
	{
	public:

		Light(VulkanLightSystem& system, LightType type, LightID id) :
			m_system(&system), m_type(type), m_ID(id)
		{}


	private:
		VulkanLightSystem*	m_system{ nullptr };
		LightType			m_type{LightType::_INVALID_};
		LightID				m_ID{ INVALID_LIGHT };
	};


	struct LightData
	{
		LightID			ID{ INVALID_LIGHT };
		LightVariant	Data{};
	};


	template <typename TLight>
	struct HostToDeviceDataMapping
	{
		SparseArray<TLight, LightID>	HostMemory{};				// The CPU-side, "ground truth" data.
		VulkanBuffer					DeviceBuffer{};				// The source buffer for the device data
		TLight*							DeviceMemoryMap{ nullptr };	// The pointer to the mapped device memory.
		uint32_t						DirtyUploadCounter{ 0 };	// Number of frames we should consider the GPU data 'dirty'.
	};

	class VulkanLightSystem
	{
	public:

		void	Initialize(MyVkDevice& device, uint32_t frameCount);

		template <typename TLight, typename... Args>
		TLight&	New(Args&&... args)
		{
			static_assert(std::is_base_of_v<BaseLight, TLight>, "This function only creates types deriving from BaseLight");

			if constexpr (std::is_same_v<TLight, DirectionalLight>)
			{
				m_directionalLights.HostMemory.Emplace(std::forward<Args>(args)...);
			}
			else if constexpr (std::is_same_v<TLight, PointLight>)
			{
				m_pointLights.HostMemory.Emplace(std::forward<Args>(args)...);
			}
			else if (std::is_same_v<TLight, SpotLight>)
			{
				m_spotLights.HostMemory.Emplace(std::forward<Args>(args)...);
			}
		}


	protected:

	private:

		void	InitializeGPUResources();

		MOE_VK_DEVICE_GETTER()

		MyVkDevice*									m_device {nullptr};

		HostToDeviceDataMapping<DirectionalLight>	m_directionalLights;
		HostToDeviceDataMapping<PointLight>			m_pointLights;
		HostToDeviceDataMapping<SpotLight>			m_spotLights;

		uint32_t		m_frameCount{ 0 }; // Maximum number of frames in flight

		VulkanDescriptorSetAllocator	m_setAllocator{};

		std::vector<vk::DescriptorSet>	m_lightDescriptors{};

		static const auto sz = sizeof(DirectionalLight);
	};
}


#endif // MOE_VULKAN
