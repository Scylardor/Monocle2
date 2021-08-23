#pragma once

namespace moe
{
	class Engine;


	class EnginePart
	{
	public:

		EnginePart(Engine& myEngine) :
			m_ownerEngine(&myEngine)
		{}

		Engine const* GetEngine() const
		{
			return m_ownerEngine;
		}

		Engine* EditEngine()
		{
			return m_ownerEngine;
		}


	private:

		Engine* m_ownerEngine = nullptr;

	};

}