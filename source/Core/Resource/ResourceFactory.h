#pragma once

#include "Monocle_Core_Export.h"

#include "Core/Containers/AssetRegistry/ObjectRegistry.h"

namespace moe
{
	class Monocle_Core_API IResourceFactory
	{
	public:
		virtual ~IResourceFactory() {}

		virtual RegistryID	IncrementReference(RegistryID id) = 0;

		virtual void		DecrementReference(RegistryID id) = 0;



	};

}


