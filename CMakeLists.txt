# 指定了当前工程支持的cmake版本范围, 为了保障在不同的cmake版本间的兼容性
cmake_minimum_required(VERSION 3.5)

# 工程名字
project(TIMEOUT)

# 编译目标
set(TARGET timeout)

# 头文件目录
include_directories(inc)

# 源文件目录
aux_source_directory(src DIR_SRC)

# 编译的源文件
set(SRC_LIST main.c ${DIR_SRC})

# 生成文件的目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 生成文件名字以及所需的源文件
add_executable(${TARGET} ${SRC_LIST})

# 链接线程库, 必须在add_executable后面
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(${TARGET} PUBLIC Threads::Threads)
