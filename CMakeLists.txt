cmake_minimum_required(VERSION 3.16)

# 定义工程的名称为 leomax-tokenizer
# 定义版本号，由Major, Minor和Patch组成
project(leomax-tokenizer LANGUAGES CXX C VERSION 1.0.0)

message(STATUS "Project Version: ${PROJECT_VERSION}")
message(STATUS "Major Version: ${PROJECT_VERSION_MAJOR}")
message(STATUS "Minor Version: ${PROJECT_VERSION_MINOR}")
message(STATUS "Patch Version: ${PROJECT_VERSION_PATCH}")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

# 设置两个选项
option(WITH_TESTING  "compile leomax-tokenizer with unit testing"               OFF)
option(WITH_PYTHON   "compile leomax-tokenizer with python interpreter"         ON)
option(USE_ABI0       "set -D_GLIBCXX_USE_CXX11_ABI to 0"                       OFF)

# 定义一个LEOMAX_TOKENIZER_LIB宏
add_definitions(-DLEOMAX_TOKENIZER_LIB)    # 添加了宏

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")     # 设置依赖包中各种cmake配置文件路径
set (PUBLIC_DEPEND_LIBS "")                                                         # 设置公共库变量，用来保存公共依赖库
message(STATUS "cmake_module_path: ${CMAKE_MODULE_PATH}")

set(TOKENIZERS_INSTALL_INCLUDE_DIR ${PROJECT_SOURCE_DIR})
message(STATUS "tokenizers install include dir: ${TOKENIZERS_INSTALL_INCLUDE_DIR}")

if(NOT CMAKE_BUILD_TYPE) 
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING 
        "Choose the type of build, option are: Debug Release RelWithDebInfo MinSizeRel.")
endif(NOT CMAKE_BUILD_TYPE)
message(STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")

# 对不同的操作系统设置是否用c++11的库
if (NOT WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11")       # 目前支持c++20
    if (CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "aarch64")          # 对arm64芯片的特殊配置
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-no-as-needed")
    endif ()

    if(USE_ABI0)        # 不使用C++11版本的std::string和std::list
        message(STATUS "-D_GLIBCXX_USE_CXX11_ABI will be set to 0.")
        add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_USE_CXX11_ABI=0")
    else ()
        message(STATUS "-D_GLIBCXX_USE_CXX11_ABI will be set to 1.")
        add_definitions(-D_GLIBCXX_USE_CXX11_ABI=1)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_USE_CXX11_ABI=1")
    endif ()
else ()
    set(CMAKE_CXX_STANDARD 11)
endif ()

# 对编译参数进行配置
if(WIN32)

else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fPIC") # 后续添加-O3 的优化
    if (NOT APPLE)
        # 在非APPLE系统生效
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ldl")
        if(NOT ANDROID) # 非安卓系统
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpthread")
        else ()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os")
        endif ()
    else ()
        message(STATUS "This an apple system")
    endif ()

    # 公共设置
    set(PUBLIC_DEPEND_LIBS ${CMAKE_DL_LIBS})
    message(STATUS "The public_depend_libs is ${PUBLIC_DEPEND_LIBS}")
endif()

# 设置安装的前缀与顶层的头文件依赖目录
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})
set(TOKENIZER_INSTALL_INCLUDE_DIR ${PROJECT_SOURCE_DIR})
include_directories(${TOKENIZERS_INSTALL_INCLUDE_DIR})
message(STATUS "cmake install prefix is ${CMAKE_INSTALL_PREFIX}")
message(STATUS "cmake_cxx_flags is: ${CMAKE_CXX_FLAGS}")
message(STATUS "cmake_exe_linker_flags is: ${CMAKE_EXE_LINKER_FLAGS}")
message(STATUS "tokenizer install include dir ${TOKENIZER_INSTALL_INCLUDE_DIR}")

# 添加预定义的函数以及第三方库需要的一些配置
# config GIT_URL with github mirrors to speed up dependent repos clone
option(GIT_URL "Git URL to clone dependent repos" ${GIT_URL})
if(NOT GIT_URL)
    set(GIT_URL "https://github.com")
endif()

# 引入预先定义好的cmake文件夹中的内容
include(generic)
include(third_party)

# cmake_binary_dir 是指build_py39中保存cpp库的路径
set(CPP_PACKAGE_DIR ${CMAKE_BINARY_DIR}/cpp/leomax_tokenizer)
add_custom_target(build_cpp_package_dir ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CPP_PACKAGE_DIR}/lib
                ${CPP_PACKAGE_DIR}/include ${CPP_PACKAGE_DIR}/third_party/include
                ${CPP_PACKAGE_DIR}/third_party/lib
        DEPENDS core_tokenizers)

message(STATUS "cpp package dir: ${CPP_PACKAGE_DIR}")
set(TOKENIZER_CORE_NAME "core_tokenizers")                          # 设置c++库的名称
set(TOKENIZER_CORE_PATH ${CMAKE_BINARY_DIR}/leomax_tokenizer)       # 设置 c++ 动态库的路径
message(STATUS "tokenizer core path: ${TOKENIZER_CORE_PATH}")

######################################################################
#########################添加 c++ 版本的 tokenizer #####################
######################################################################
add_subdirectory(leomax_tokenizer)

if(WITH_PYTHON)
    # 编译 python 的库
    add_subdirectory(python)
    # 编译对应的版本包
#    if (NOT APPLE AND NOT WIN32) # Linux
#        add_custom_target(build_tokenizers_bdist_wheel ALL
#                COMMAND ${PYTHON_EXECUTABLE} setup.py bdist_wheel --plat-name=manylinux1_x86_64
#                COMMENT "Packing whl packages------>>>"
#                DEPENDS copy_python_tokenizers)
#    else()
#        add_custom_target(build_tokenizers_bdist_wheel ALL
#                COMMAND ${CMAKE_COMMAND} -E env ${py_env} ${PYTHON_EXECUTABLE} setup.py bdist_wheel
#                COMMENT "Packing whl packages------>>>"
#                DEPENDS copy_python_tokenizers)
#    endif()
endif(WITH_PYTHON)


