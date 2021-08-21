#pragma once

#include <vector>
#include <thread>
#include <condition_variable>
#include <queue>
#include <future>


class ThreadPool
{
public:

	enum class PoolSize
	{
		Hardware = 0,
		HardwareMinusOne,
		Custom
	};

	template <typename T>
	using PackagedTask = std::packaged_task<T>;

	using Task = PackagedTask<void()>;


	explicit ThreadPool(PoolSize poolSize, bool autoStart = true);

	~ThreadPool();

	void	EnqueueTask(Task task);


	template <typename Callable>
	void	EnqueueCallable(Callable&& callable);

	template <typename Callable, typename... Args>
	void	EnqueueCallable(Callable&& callable, Args&&... args);

	template <typename Callable, typename... Args>
	auto	EnqueueAsyncCallable(Callable&& callable, Args&&... args);

	template<class F>
	auto EnqueueAsync(F&& func);



	bool	HasNoTasks() const;
	bool	HasTasks() const { return !HasNoTasks(); }

	size_t	GetSize() const
	{
		// Returning capacity in case we call the function before starting the threads.
		return (m_threads.empty() ? m_threads.capacity() : m_threads.size());
	}



private:

	void	Start();
	void	Stop() noexcept;

	bool	IsStopped() const;

	void	ReserveThreads(PoolSize poolSize);

	void	WaitForTask();


	std::vector<std::thread>	m_threads;

	mutable std::mutex			m_eventMutex;

	std::condition_variable		m_condVar;

	std::queue<Task>			m_taskQueue;

	std::unordered_map < size_t, std::function<void()> >	m_runnableThreadFunctions;

	bool	m_stopped = true;
};


#include "ThreadPool.inl"