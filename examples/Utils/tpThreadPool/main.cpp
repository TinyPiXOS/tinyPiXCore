#include "tpThreadPool.h"
#include <iostream>
#include <chrono>
#include <thread>

/// @brief 打印简单任务
void simpleTask(int id)
{
	std::cout << "Task " << id << " started by thread "
			  << std::this_thread::get_id() << std::endl;
			  
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	std::cout << "Task " << id << " finished" << std::endl;
}

/// @brief 计算斐波那契数的任务
void fibonacciTask(int n)
{
	auto calculate = [](int num)
	{
		if (num <= 1)
			return num;
		int a = 0, b = 1;
		for (int i = 2; i <= num; i++)
		{
			int c = a + b;
			a = b;
			b = c;
		}
		return b;
	};

	int result = calculate(n);
	std::cout << "Fibonacci(" << n << ") = " << result
			  << " by thread " << std::this_thread::get_id() << std::endl;
}

int main()
{
	tpThreadPool pool;

	// 阶段1：提交8个简单任务
	std::cout << "===== 阶段1: 提交8个任务 =====" << std::endl;
	for (int i = 0; i < 8; ++i)
	{
		pool.enqueue([i]
					 { simpleTask(i); });
	}

	// 等待第一阶段任务完成
	std::this_thread::sleep_for(std::chrono::seconds(3));

	// 阶段2：提交15个CPU密集型任务
	std::cout << "\n===== 阶段2: 提交15个CPU任务 =====" << std::endl;
	for (int i = 0; i < 15; ++i)
	{
		pool.enqueue([i]
					 { fibonacciTask(35 + i); });
	}

	// 阶段3：再次提交5个简单任务
	std::this_thread::sleep_for(std::chrono::seconds(2));
	std::cout << "\n===== 阶段3: 提交5个简单任务 =====" << std::endl;
	for (int i = 100; i < 105; ++i)
	{
		pool.enqueue([i]
					 { simpleTask(i); });
	}

	// 让所有任务完成
	std::this_thread::sleep_for(std::chrono::seconds(10));

	// 显式停止线程池
	pool.stop();

	return 0;
}