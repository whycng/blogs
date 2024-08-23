windows下cmake默认使用的msvc编译器，因此在运行cmake指令时需要加上参数：
cmake .. -G "MinGW Makefiles"

---
 
即使在 CMakeLists.txt 中显式指定了 CMAKE_C_COMPILER 和 CMAKE_CXX_COMPILER，CMake 仍然可能优先使用 Visual Studio (MSVC) 作为默认生成器。这是因为 CMake 的生成器选择是独立于编译器设置的。CMake 生成器控制了如何构建项目，而编译器设置仅控制使用哪个编译器来编译代码。

可能的问题和解决方案
1. 生成器的默认行为
即使你在 CMakeLists.txt 中指定了编译器，CMake 仍然可能选择 MSVC 作为生成器，因为它是默认的生成器之一。为了确保使用 MinGW 编译器，你需要指定生成器为 MinGW Makefiles，这会强制 CMake 使用 MinGW 的 gcc 和 g++。

2. 解决方法：显式指定生成器
你需要在运行 cmake 命令时显式指定 MinGW Makefiles 作为生成器：
```bash
cmake -G "MinGW Makefiles" ..
```
这种方式会明确告诉 CMake 使用 MinGW 生成器，而不是默认的 Visual Studio 生成器。

3. 检查配置结果
你可以通过以下命令来确认 CMake 使用的生成器和编译器：

cmake --system-information | findstr /C:"CMAKE_GENERATOR"
cmake --system-information | findstr /C:"CMAKE_C_COMPILER"
cmake --system-information | findstr /C:"CMAKE_CXX_COMPILER"
这会显示当前的生成器和编译器设置。

4. 删除缓存并重新配置
CMake 使用缓存文件 (CMakeCache.txt) 来存储配置选项。如果你在配置项目时先使用了 MSVC 生成器，那么这些选项可能已经被缓存。要切换到 MinGW 编译器，你需要删除 CMakeCache.txt 文件并重新运行 CMake 配置：

rm CMakeCache.txt
cmake -G "MinGW Makefiles" ..
这样可以确保 CMake 不会使用之前的配置缓存。

总结
在 CMakeLists.txt 中指定 CMAKE_C_COMPILER 和 CMAKE_CXX_COMPILER 只是告诉 CMake 使用哪个编译器，但并不能控制生成器的选择。
为了确保使用 MinGW，你需要在调用 CMake 时显式指定生成器为 MinGW Makefiles

---

使用这三个参数打印信息发现确实如此

