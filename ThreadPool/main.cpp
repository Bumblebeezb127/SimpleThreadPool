#include <iostream>
#include <thread>
#include <queue>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <functional>

/**
 * 简易线程池类
 *
 * 功能：维护一组工作线程，用于并发执行由用户提交的任务（函数对象）。
 * 核心思想：生产者-消费者模型 —— 主线程向任务队列添加任务，工作线程竞争获取并执行任务。
 */
class ThreadPool {
private:
    // 工作线程集合
    std::vector<std::thread> threads;

    // 任务队列：存储待执行的函数对象（无返回值、无参数）
    std::queue<std::function<void()>> tasks;

    // 互斥锁：保护任务队列的并发访问
    std::mutex mtx;
    // 条件变量：用于线程间的等待/通知，避免忙等待
    std::condition_variable cv;

    // 停止标志：通知所有线程退出循环
    bool stop;

public:
    /**
     * 构造函数：创建并启动 numThreads 个工作线程
     * @param numThreads 线程池中线程的数量
     */
    ThreadPool(int numThreads) : stop(false) {
        // 创建 numThreads 个线程，每个线程执行相同的 lambda 函数
        for (int i = 0; i < numThreads; ++i) {
            // emplace_back 直接在 vector 中构造 thread 对象
            // lambda 捕获 this 指针，以访问成员变量
            threads.emplace_back([this] {
                // 每个工作线程的主循环
                while (true) {
                    // 1. 获取互斥锁，并用 unique_lock 管理（支持条件变量的等待）
                    std::unique_lock<std::mutex> lock(mtx);

                    // 2. 条件变量等待：阻塞直到满足以下条件之一：
                    //    - 有任务可执行（!tasks.empty()）
                    //    - 线程池被要求停止（stop == true）
                    //    wait 的第二个参数是一个谓词（lambda），用于防止虚假唤醒
                    cv.wait(lock, [this] {
                        return stop || !tasks.empty();
                        });

                    // 3. 如果停止标志为 true 且任务队列为空，则退出线程循环
                    if (stop && tasks.empty()) {
                        return;   // 线程函数返回，线程结束
                    }

                    // 4. 取出队首任务（使用移动语义避免拷贝，提高性能）
                    std::function<void()> task(std::move(tasks.front()));
                    tasks.pop();

                    // 5. 提前解锁，允许其他线程访问任务队列，提高并发性
                    lock.unlock();

                    // 6. 执行任务（此时锁已释放，其他线程可以同时处理其他任务）
                    task();

                    // 注意：任务执行完毕后，循环继续，会重新获取锁并等待下一个任务
                    // 这种设计使得线程在执行任务期间不持有锁，能最大化并发效率
                }
                });
        }
    }

    /**
     * 析构函数：安全关闭线程池
     *
     * 步骤：设置停止标志 -> 唤醒所有工作线程 -> 等待它们结束
     */
    ~ThreadPool() {
        {
            // 加锁设置停止标志
            std::unique_lock<std::mutex> lock(mtx);
            stop = true;
        } // 锁在此处释放，以便线程能尽快获取锁并检查停止条件

        // 唤醒所有等待中的线程，使它们检查 stop 标志并退出
        cv.notify_all();

        // 等待所有工作线程完成（join 会阻塞主线程直到所有子线程结束）
        for (auto& thread : threads) {
            thread.join();
        }
    }

    /**
     * 向线程池提交一个任务（函数或可调用对象）
     *
     * 模板参数：F 是可调用对象的类型，Args 是参数包类型
     * 使用完美转发和 std::bind 将函数及其参数包装为无参的 std::function<void()> 对象
     *
     * @param f   可调用对象（函数、lambda、函数指针等）
     * @param args 可变参数，将作为 f 的参数传递
     */
    template<class F, class... Args>
    void enqueue(F&& f, Args&&... args) {
        // 使用 std::bind 将 f 与 args 绑定，生成一个无参的可调用对象
        // std::forward 保持参数的右值/左值属性，实现完美转发
        std::function<void()> task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);

        {
            // 加锁，向任务队列添加新任务
            std::unique_lock<std::mutex> lock(mtx);
            tasks.emplace(task);  // emplace 直接在队列中构造元素（拷贝 task）
        } // 锁自动释放

        // 通知一个等待中的工作线程，有新任务可执行
        // 使用 notify_one 避免“惊群”效应，只唤醒一个线程来处理任务
        cv.notify_one();
    }
};

/**
 * 主函数示例：演示线程池的使用
 */
int main() {
    // 创建一个包含 4 个工作线程的线程池
    ThreadPool pool(4);

    // 提交 10 个任务，每个任务打印开始和结束信息，并睡眠 1 秒模拟工作
    for (int i = 0; i < 10; ++i) {
        // lambda 通过值捕获 i，确保每个任务拥有独立的 i 副本
        pool.enqueue([i] {
            printf("Task %d is running\n", i);
            std::this_thread::sleep_for(std::chrono::seconds(1));
            printf("Task %d is finished\n", i);
            });
    }

    // 主线程将任务全部提交后，立即返回，此时会触发 pool 的析构函数，
    // 析构函数会等待所有任务执行完毕并回收线程资源。
    // 因此，输出中会看到所有任务执行完毕的信息。
    return 0;
}