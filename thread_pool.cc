#include "thread_pool.h"

ThreadPool::ThreadPool(const unsigned int pool_size)
{
	idle_num = pool_size < 1 ? 1 : pool_size;
	for(int i = 0; i < idle_num; ++i) {
		pool.emplace_back(
				[this] ()
				{
					while(!this->stop) {
						std::function<void()> task;
						// code block for unique_lock
						{
							std::unique_lock<std::mutex> lock(this->m_lock);
							this->cv.wait(lock,
									[this] ()
									{
										return this->stop.load() || !this->tasks.empty();
									}
							);
							if(this->stop && this->tasks.empty()) {
								return;
							}
							task = std::move(this->tasks.front());
							this->tasks.pop();
						}
						--idle_num;
						task();
						++idle_num;
					}
				}
		);
	}
}

ThreadPool::~ThreadPool()
{
	stop.store(true);
	cv.notify_all();
	for(auto &thread: pool) {
		if(thread.joinable()) {
			thread.join();
		}
	}
}
