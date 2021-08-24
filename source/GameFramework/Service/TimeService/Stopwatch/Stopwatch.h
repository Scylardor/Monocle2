// Monocle source files - Alexandre Baron

#pragma once


#include "GameFramework/Service/TimeService/Durations.h"

namespace moe
{
	/* Basic stopwatch class
	 * Can be in three states:
	 * - Normal : running OK (Elapsed will return time actually spent since last time)
	 * - Paused : The timer is temporarily disabled (Elapsed always returns zero)
	 * - Frozen : Elapsed will always return the same amount of time (end timepoint is "frozen in time")
	 */
	class Stopwatch
	{
		enum class TimeStatus : uint8_t
		{
			Normal,
			Paused,	// Elapsed will always return 0 (start timepoint = end timepoint = no delta)
			Frozen	// Elapsed will always return the same amount of time (end timepoint is "frozen in time")
		};

	public:
		using Clock = std::chrono::high_resolution_clock;

		typedef std::chrono::time_point<Clock> TimePoint;


		Stopwatch() = default;

		void	Restart();

		void	Pause()
		{
			m_status = TimeStatus::Paused;
		}

		void	Unpause()
		{
			m_status = TimeStatus::Normal;
		}


		[[nodiscard]] bool	IsPaused() const
		{
			return (m_status == TimeStatus::Paused);
		}

		void	Freeze()
		{
			m_endTime = Now();
			m_status = TimeStatus::Frozen;
		}

		void	Unfreeze()
		{
			m_endTime = Now();
			m_status = TimeStatus::Normal;
		}

		[[nodiscard]] bool	IsFrozen() const
		{
			return (m_status == TimeStatus::Frozen);
		}

		TimePoint	GetStartTime() const
		{
			return m_startTime;
		}

		template <typename Unit = sec>
		typename Unit::rep Elapsed() const;


		template <typename Unit>
		static typename Unit::rep ElapsedSince(TimePoint start, TimePoint end = Now());

	private:
		TimePoint			m_startTime = Now();
		mutable TimePoint	m_endTime{}; // Mutable so it can be changed by Elapsed() if necessary

		TimeStatus			m_status{ TimeStatus::Normal };


	/*
	 * In case of tests : provide a way for the testing framework to "hack" the stopwatch Now() method
	 * So that we don't have to _actually_ wait for any amount of real time inside the tests...
	 */
	#ifndef MOE_UNIT_TESTS
		public:

		static TimePoint Now()
		{
			return Clock::now();
		}

	#else
	private:

		inline static TimePoint	s_TestNow = Clock::now();

	public:

		static void	SetNow(TimePoint now)
		{
			s_TestNow = now;
		}

		static TimePoint Now()
		{
			return s_TestNow;
		}

	#endif
	};

}

#include "Stopwatch.inl"