add_executable(ThreadPool main.cpp)

# pthread is needed on POSIX systems for std::thread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(ThreadPool PRIVATE Threads::Threads)

if(MSVC)
    target_compile_definitions(ThreadPool PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
