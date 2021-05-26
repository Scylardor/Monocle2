#pragma once

#include "Monocle_Core_Export.h"

#include "Core/Containers/AssetRegistry/ObjectRegistry.h"

namespace moe
{
	// Needed for Core not to depend on graphics, Matches the Vulkan values for now (TODO: Clean up this)
	enum VertexIndexType
	{
		eUint16 = 0,
		eUint32
	};

	class Monocle_Core_API IResourceFactory
	{
	public:
		virtual ~IResourceFactory() {}

		virtual RegistryID	IncrementReference(RegistryID id) = 0;

		virtual bool		DecrementReference(RegistryID id) = 0;



	};

}


