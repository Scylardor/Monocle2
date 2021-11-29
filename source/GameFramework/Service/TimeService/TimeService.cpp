#include "TimeService.h"


namespace moe
{
	TimeService::TimeService(Engine& ownerEngine) :
		Service(ownerEngine)
	{
	}


	void TimeService::Start()
	{
		// Reset game timer
		m_gameWatch.Restart();
		m_gameStartTime = m_gameWatch.GetStartTime();
	}


	void TimeService::PauseGameTime()
	{
		m_gameWatch.Pause();
	}

	void TimeService::FreezeGameTime()
	{
		m_gameWatch.Freeze();
	}


	void TimeService::Update()
	{
		m_frameDeltaTime = m_gameWatch.Elapsed();
		m_gameWatch.Restart();
	}


}
