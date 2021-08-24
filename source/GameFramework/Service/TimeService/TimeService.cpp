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
		m_gameTime.Restart();
		m_gameStartTime = m_gameTime.GetStartTime();
	}


	void TimeService::PauseGameTime()
	{
		m_gameTime.Pause();
	}

	void TimeService::FreezeGameTime()
	{
		m_gameTime.Freeze();
	}


	void TimeService::Update()
	{
		m_frameDeltaTime = m_gameTime.Elapsed();
	}


}
