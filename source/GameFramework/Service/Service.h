// Monocle Game Engine source files - Alexandre Baron

#pragma once
#include "GameFramework/Engine/EnginePart.h"


namespace moe
{
	class Engine;

	// So far just an empty class to be able to store services
	class Service : public EnginePart
	{

	public:
		Service(Engine& ownerEngine) :
			EnginePart(ownerEngine)
		{}

		virtual ~Service() = default;

	};


}
