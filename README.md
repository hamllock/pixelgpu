# PixelGPU

A software 2D graphics engine, rasterizer, and virtual GPU emulator built from scratch in modern **C++20**.

PixelGPU operates with zero external drawing libraries and zero GPU APIs (none of that opengl directx vulkan whatevers). All rendering is performed directly on a raw heap-allocated virtual framebuffer in memory using pointer arithmetic, row-major stride indexing, and pure integer arithmetic.

Based on the GPU driver learning curriculum and display HAL framework created by **[AlphaPixel](https://github.com/AlphaPixel/gpu-driver-learning)**.

---

## Build & Run

### Prerequisites
- **C++20 Compiler:** GCC 12+, Clang 15+, or MSVC 2022+
- **Build System:** CMake 3.25+ & Ninja
- **Libraries:** SDL3 or GLFW3 (installed via system package manager or vcpkg)

### Building on Linux / macOS
```bash
# Configure the build with the gcc-debug preset
cmake --preset gcc-debug

# Compile the project
cmake --build --preset gcc-debug

# Run the showcase application
./build/gcc-debug/apps/demos/pixelgpu_demo
```

---

## License
MIT License
