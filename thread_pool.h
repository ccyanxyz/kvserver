#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <vector>
#include <queue>
#include <thread>
#include <condition_variable>
#include <atomic>
#include <future>
#include <functional>
#include <stdexcept>

class ThreadPool {
	using Task = std::function<void()>;

	// threads
	std::vector<std::thread> pool;
	// task queue
	std::queue<Task> tasks;
	// lock
	std::mutex m_lock;
	// condition variable
	std::condition_variable cv;
	// stop commit
	std::atomic<bool> stop;
	// idle thread number
	std::atomic<int> idle_num;

public:
	ThreadPool(const unsigned int pool_size = 10);
	~ThreadPool();

	int get_idle_num() const
	{
		return idle_num;
	}

	template <class F, class ...Args>
	auto commit(F &&f, Args &&...args) -> std::future<decltype(f(args...))>
	{
		if(stop.load()) {
			throw std::runtime_error("commit stopped");
		}
		
		using RetType = decltype(f(args...));
		auto task = std::make_shared<std::packaged_task<RetType()> >(
				std::bind(std::forward<F>(f), std::forward<Args>(args)...)
		);

		std::future<RetType> future = task->get_future();
		// code block for lock_guard
		{
			std::lock_guard<std::mutex> lock(m_lock);
			tasks.emplace(
					[task] ()
					{
						// function call
						(*task)();
					}
			);
			cv.notify_one();
			return future;
		}
	}
};

#endif
