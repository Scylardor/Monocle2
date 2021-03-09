#pragma once
#ifdef MOE_VULKAN

#include <array>

namespace moe
{
	struct VulkanValidationLayers
	{
	private:
#ifdef MOE_DEBUG
		static const bool	S_Enabled = true;
#else
		static const bool	S_Enabled = false;
#endif

		static inline const std::array<const char*, 1>	S_UsedValidationLayers =
		{ "VK_LAYER_KHRONOS_validation" };

	public:

		static constexpr bool	AreEnabled()
		{
			return S_Enabled;
		}

		static constexpr const auto& Names()
		{
			return S_UsedValidationLayers;
		}
	};


}


#endif // MOE_VULKAN
