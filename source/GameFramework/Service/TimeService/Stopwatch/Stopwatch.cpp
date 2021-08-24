#include "Stopwatch.h"



namespace moe
{
	void Stopwatch::Restart()
	{
		m_startTime = Now();
		m_endTime = Now();
		m_status = TimeStatus::Normal;
	}
}
