cmake_minimum_required(VERSION 3.0)
project(main) #工程名字

#配置编译选项
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGES} -g)

#配置头文件搜索路径 include_directories()
#配置库文件搜索路径 link_directories()

#设置需要编译的源文件列表
set(SRC_LIST muduo_server.cpp)

#把.指定路径下的所有源文件放入 aux_source_direcotry(. SRC_LIST)
add_executable(server ${SRC_LIST})

#表示server这个目标程序需要链接 muduo_net muduo_base pthread这三个库文件
target_link_libraries(server muduo_net muduo_base pthread)