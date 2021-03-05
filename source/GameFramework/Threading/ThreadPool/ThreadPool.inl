#pragma once

template <typename Callable>
void	ThreadPool::EnqueueCallable(Callable&& callable)
{
	Task workerPackage{ callable };
	EnqueueTask(std::move(workerPackage));
}


template <typename Callable, typename... Args>
void	ThreadPool::EnqueueCallable(Callable&& callable, Args&&... args)
{
	Task workerPackage{
		[	taskCallable = std::forward<Callable>(callable),
			taskArgs = std::make_tuple(std::forward<Args>(args)...)]
		{ std::apply(taskCallable, taskArgs); } };
	EnqueueTask(std::move(workerPackage));
}


template <typename Callable, typename... Args>
auto	ThreadPool::EnqueueAsyncCallable(Callable&& callable, Args&&... args)
{
	auto fn =
	[taskCallable = std::forward<Callable>(callable),
		taskArgs = std::make_tuple(std::forward<Args>(args)...)]
	{ return std::apply(taskCallable, taskArgs); };

	using ResultType = decltype(fn());

	auto wrappedFunction = std::make_shared<PackagedTask<ResultType()>>(std::move(fn));
	std::future<ResultType> futureResult = wrappedFunction->get_future();

	Task workerPackage{ [wrappedFunction] { (*wrappedFunction)(); } };
	EnqueueTask(std::move(workerPackage));

	// Give the user a future for retrieving the result.
	return futureResult;
}



// TODO : I'd like to implement the double-packaged task technique from Mastering C++17 STL (page 207),
// but unfortunately it's broken on MSVC until an unannounced future major release :
// https://github.com/microsoft/STL/issues/321
// So instead I'm using the shared_ptr technique like can be seen at https://www.youtube.com/watch?v=eWTGtp3HXiw for example.
template<class F>
auto ThreadPool::EnqueueAsync(F&& func)
{
	using ResultType = decltype(func());

	auto wrappedFunction = std::make_shared<PackagedTask<ResultType()>>(std::forward<F>(func));
	std::future<ResultType> futureResult = wrappedFunction->get_future();

	Task workerPackage{ [wrappedFunction] { (*wrappedFunction)(); } };
	EnqueueTask(std::move(workerPackage));

	// Give the user a future for retrieving the result.
	return futureResult;
}