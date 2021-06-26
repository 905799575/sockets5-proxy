project(socket5)
#指定编译器
#SET(CMAKE_C_COMPILER "/usr/bin/gcc")
set(CMAKE_C_FLAGS "-std=c99") # that would be bad
#include的搜索目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR} #CMAKE的当前目录
    ${CMAKE_CURRENT_BINARY_DIR} #Target的编译目录
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

#.c .cpp的目录
aux_source_directory(./src DIR_SRCS)

#增加可执行文件 他需要${DIR_SRCS}的.c文件
add_executable(socket5_exe ${DIR_SRCS})

#链接的静态库和动态库
target_link_libraries(socket5_exe pthread)