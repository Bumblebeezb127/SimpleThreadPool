# SimpleThreadPool

A small C++11 thread pool demonstrating the producer-consumer pattern with `std::thread`, `std::mutex` and `std::condition_variable`.

## Requirements

- CMake **3.16** or newer
- A C++11-capable compiler (GCC, Clang, MSVC, Apple Clang, …)

## Build

All commands are run from the project root. Builds are out-of-source under `build/`.

### Linux / macOS (single-config generator)

```bash
cmake -S . -B build
cmake --build build
```

The executable is produced at `build/bin/ThreadPool`.

Run it with:

```bash
./build/bin/ThreadPool
```

### Windows (Visual Studio, multi-config)

```bat
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

The executable is produced at `build\bin\Release\ThreadPool.exe`.

Run it with:

```bat
build\bin\Release\ThreadPool.exe
```

### Optional: choose a different generator

```bash
# Ninja (recommended if available)
cmake -S . -B build -G Ninja
cmake --build build

# Unix Makefiles
cmake -S . -B build -G "Unix Makefiles"
cmake --build build

# Xcode
cmake -S . -B build -G Xcode
cmake --build build
```

### Build type

Pass `-DCMAKE_BUILD_TYPE=Debug` (single-config generators only) for a debug build, or omit it / use `Release` for an optimized build.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## Clean

```bash
rm -rf build          # Linux / macOS
rmdir /s /q build     # Windows
```

## Layout

```
.
├── CMakeLists.txt
├── README.md
├── .gitignore
└── ThreadPool/
    ├── CMakeLists.txt
    └── main.cpp
```
