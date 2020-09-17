#include "ThreadPool.h"


ThreadPool::ThreadPool(PoolSize poolSize, bool autoStart)
{
	ReserveThreads(poolSize);

	if (autoStart)
	{
		Start();
	}
}


ThreadPool::~ThreadPool()
{
	Stop();
}


void ThreadPool::EnqueueTask(Task task)
{
	std::lock_guard lock(m_eventMutex);
	m_taskQueue.emplace(std::move(task));
	m_condVar.notify_one();
}


bool ThreadPool::HasNoTasks() const
{
	std::lock_guard lock(m_eventMutex);
	return (m_taskQueue.empty());
}


void ThreadPool::Start()
{
	m_stopped = false; // ok to do it without mutex here since threads are not started

	// if threads have not been created - create them
	if (m_threads.empty())
	{
		for (auto iThread = 0u; iThread < m_threads.capacity(); iThread++)
		{
			m_threads.emplace_back(&ThreadPool::WaitForTask, this);
		}
	}

}


void ThreadPool::Stop() noexcept
{
	{
		std::lock_guard lock(m_eventMutex);
		m_stopped = true;
	}

	m_condVar.notify_all();

	for (auto& thread : m_threads)
	{
		thread.join();
	}
}

bool ThreadPool::IsStopped() const
{
	// we don't know if threads are started or not so lock just in case
	std::lock_guard lock(m_eventMutex);
	return m_stopped;
}


void ThreadPool::ReserveThreads(PoolSize poolSize)
{
	int usedThreadsNbr;

	switch (poolSize)
	{
	case PoolSize::Hardware:
		{
			const int hwConcurrency = (int)std::thread::hardware_concurrency();
			usedThreadsNbr = hwConcurrency;
		}
		break;
	case PoolSize::HardwareMinusOne:
	{
		const int hwConcurrencyMinOne = (int)std::thread::hardware_concurrency() - 1;
		usedThreadsNbr = hwConcurrencyMinOne;
	}
		break;
	default:
		usedThreadsNbr = (int)poolSize;
		break;
	}

	usedThreadsNbr = std::max(usedThreadsNbr, 1);

	m_threads.reserve(usedThreadsNbr);
}


#include <iostream>

std::mutex s_coutMutex;

template <typename Function>
void	Talk(Function talkFunc)
{
	std::lock_guard lock(s_coutMutex);
	talkFunc();
}

void ThreadPool::WaitForTask()
{
	Talk([] { 	std::cout << "Thread " << std::this_thread::get_id() << " is created!\n";  });

	while (!m_stopped)
	{
		std::unique_lock lock(m_eventMutex);

		m_condVar.wait(lock, [this] { return !m_taskQueue.empty() || m_stopped; });

		if (!m_stopped)
		{
			Task myTask = std::move(m_taskQueue.front());
			m_taskQueue.pop();

			lock.unlock(); // don't keep the lock for the whole duration of the task !

			myTask();
		}
	}



	Talk([] { std::cout << "Thread " << std::this_thread::get_id() << " is dead!\n"; });

}

