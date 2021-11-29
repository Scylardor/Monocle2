// Monocle source files - Alexandre Baron

#pragma once

#include "Durations.h"
#include "GameFramework/Service/Service.h"
#include "Stopwatch/Stopwatch.h"

namespace moe
{

	class TimeService : public Service
	{
	public:
		using ServiceLocatorType = TimeService;


		TimeService(Engine& ownerEngine);

		void	Start();
		void	PauseGameTime();
		void	FreezeGameTime();

		void	Update();


		template <typename TUnit = sec>
		[[nodiscard]] auto	GetFrameDeltaTime() const
		{
			return m_frameDeltaTime;
		}


		template <typename TUnit = sec>
		[[nodiscard]] auto	GetTimeSinceGameStart() const
		{
			auto elapsed = Stopwatch::ElapsedSince<TUnit>(m_gameStartTime);
			return elapsed;
		}

		[[nodiscard]] float	GetSecondsSinceGameStart() const
		{
			return GetTimeSinceGameStart<sec>();
		}


	private:
		Stopwatch::TimePoint	m_gameStartTime{};

		Stopwatch	m_gameWatch;

		float	m_frameDeltaTime{ 0 };
	};
}

