### 一、预处理

project ：设置好工程名

add_subdirectory ：添加子目录

CMAKE_C_COMPILER ：设置C编译器

CMAKE_CXX_COMPILER ：设置C++编译器

message(WARNING  "message to display") ： 打印变量

-DCMAKE_INSTALL_PREFIX --- 设置生成的Makefile的安装目录

### 二、模块编译



aux_source_directory --- 在目录中查找所有源文件

add_executable  --- 将源文件编译成二进制可执行文件

add_library  -- 将源文件编译成动态库

target_link_libraries 添加动态链接库

install  --- 安装二进制

cmake ：生成MakeFile

make ：编译MakeFile生成二进制

### 三、Cmake默认目录

CMAKE_CURRENT_SOURCE_DIR  -- 当前CMakeLists.txt所在目录

PROJECT_SOURCE_DIR  -- 当前工程根目录







