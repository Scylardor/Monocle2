#pragma once
#include "Core/Containers/AssetRegistry/ObjectRegistry.h"

namespace moe
{

	class IResourceManager
	{
	public:
		virtual ~IResourceManager() = default;

		virtual uint32_t	IncrementReference(RegistryID rscID) = 0;

		virtual void		DecrementReference(RegistryID rscID) = 0;
	};



}
