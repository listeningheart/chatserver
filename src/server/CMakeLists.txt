
#定义一个SRC_LIST，包括目录下所有源文件
aux_source_directory(. SRC_LIST)
aux_source_directory(./db DB_LIST)
#指定生成可执行文件
add_executable(ChatServer ${SRC_LIST} ${DB_LIST})

#以来的库文件
target_link_libraries(ChatServer muduo_net muduo_base mysqlclient pthread)