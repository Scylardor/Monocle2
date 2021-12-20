#pragma once
#include <utility>

#include "ObjectHandle.h"
#include "Core/Misc/Types.h"

namespace moe
{

	using DeviceDynamicResourceSetHandle = uint32_t;

	struct DeviceMaterialHandle : RenderableObjectHandle<std::uint64_t>
	{
	private:
		static const Underlying ms_INVALID_ID = (Underlying)-1;

	public:

		DeviceMaterialHandle(Underlying handleID = ms_INVALID_ID) :
			RenderableObjectHandle(handleID)
		{}

		static DeviceMaterialHandle	Null() { return DeviceMaterialHandle(); }

		[[nodiscard]] std::pair<uint32_t, uint32_t>	DecomposeMaterialAndShaderIndices() const
		{
			return { (uint32_t)(Get() >> 32), (uint32_t)Get() };
		}

		static const uint32_t INVALID_ID = (uint32_t)-1;
		[[nodiscard]] static DeviceMaterialHandle	Encode(uint32_t matID, uint32_t dynamicResourceSetsID = INVALID_ID)
		{
			return DeviceMaterialHandle{ ((uint64_t)(matID) << 32) | dynamicResourceSetsID };
		}

		[[nodiscard]] std::pair<uint32_t, uint32_t>	Decode() const
		{
			return { (uint32_t)(Get() >> 32), (uint32_t)Get() };
		}

		[[nodiscard]] bool	HasDynamicResourceSets() const
		{
			return (uint32_t)Get() != INVALID_ID;
		}

	};

	struct DeviceMeshHandle : RenderableObjectHandle<uint32_t>
	{
	private:
		static const Underlying ms_INVALID_ID = (Underlying)-1;

	public:

		DeviceMeshHandle(Underlying handleID = ms_INVALID_ID) :
			RenderableObjectHandle(handleID)
		{}

		static DeviceMeshHandle	Null() { return DeviceMeshHandle(); }
	};



}
