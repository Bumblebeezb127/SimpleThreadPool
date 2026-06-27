# SimpleThreadPool

一个使用 C++11 标准库实现的小型线程池，演示 **生产者-消费者** 模型与 `std::thread`、`std::mutex`、`std::condition_variable` 的基本用法。

## 项目简介

本项目实现了一个通用的线程池 `ThreadPool`，其核心思想是：

- 预先创建一组工作线程，这些线程在后台循环等待任务。
- 主线程（或任意业务线程）通过 `enqueue()` 接口向任务队列提交可调用对象（函数、lambda、bind 表达式等）。
- 工作线程通过互斥锁与条件变量安全地从任务队列中取出一个任务并执行。

线程池常用于：
- 避免频繁创建/销毁线程带来的开销。
- 控制并发线程数量，防止系统资源被耗尽。
- 把短小的并发任务统一调度执行。

## 目录结构

```
ThreadPool/
├── CMakeLists.txt            # 顶层 CMake 配置
├── ThreadPool.sln            # Visual Studio 解决方案
├── CMake/                    # CMake 工具脚本
└── ThreadPool/
    ├── CMakeLists.txt        # 子项目 CMake 配置
    ├── main.cpp              # 线程池实现 + 示例
    ├── ThreadPool.vcxproj    # VS 工程文件
    └── ...
```

## 环境要求

- C++ 编译器，支持 **C++11** 或更高（MSVC 2017+、MinGW、Clang、GCC 均可）
- Windows / Linux / macOS 均可

## 编译与运行

### 方式一：Visual Studio

直接用 Visual Studio 打开 `ThreadPool.sln`，选择 `Debug | x64`（或 `Release`）配置，生成并运行即可。

### 方式二：CMake（命令行）

```bash
# 在项目根目录
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

或使用 Ninja / Unix Makefiles：

```bash
cmake -S . -B build -G "Ninja"
cmake --build build
```

构建完成后，可执行文件位于 `build/` 或 `x64/Debug/ThreadPool.exe`（Windows）。

## 使用方法

线程池的使用非常简洁，只需三步：

```cpp
#include "ThreadPool.h"  // 实际项目可把 ThreadPool 类抽到独立头文件

int main() {
    // 1. 创建线程池，指定工作线程数量
    ThreadPool pool(4);

    // 2. 提交任务
    for (int i = 0; i < 10; ++i) {
        pool.enqueue([i] {
            // 业务逻辑
        });
    }

    // 3. 离开作用域后，pool 析构时会等待所有任务执行完毕
    return 0;
}
```

`enqueue` 是一个可变参数模板，支持任意可调用对象与参数：

```cpp
int add(int a, int b) { return a + b; }

pool.enqueue(add, 1, 2);                      // 普通函数
pool.enqueue([](int x){ return x * x; }, 10); // lambda
```

## 实现要点

- **互斥锁 + 条件变量** 保证任务队列的线程安全与高效的等待/唤醒。
- **`std::function<void()>` + `std::bind`** 把任意签名的可调用对象包装为无参无返回值类型，统一存入任务队列。
- **析构时设置 `stop` 标志并 `notify_all`**，再 `join` 所有工作线程，做到优雅退出。
- 取出任务后立即释放锁，再执行任务，最大化并发度。

## 注意事项

- 当前示例未对任务队列长度做上限限制，生产速度远大于消费速度时会占用较多内存。
- 示例中的 `main.cpp` 简单演示 10 个任务并发执行，每个任务休眠 1 秒模拟 I/O。

## License

仅作学习演示使用。
