#pragma once
namespace moe
{

	template <typename Unit>
	typename Unit::rep Stopwatch::Elapsed() const
	{
		if (IsPaused())
			return { 0 };

		if (!IsFrozen())
			m_endTime = Now();

		return ElapsedSince<Unit>(m_startTime, m_endTime);
	}

	template <typename Unit>
	typename Unit::rep Stopwatch::ElapsedSince(TimePoint start, TimePoint end)
	{
		auto const duration = end - start;

		auto elapsed = std::chrono::duration_cast<Unit>(duration);

		return elapsed.count();
	}

}