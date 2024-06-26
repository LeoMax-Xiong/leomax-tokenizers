cmake_minimum_required(VERSION 3.10)

project(leomax-tokenizer LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

option(WITH_TESTING  "compile leomax-tokenizer with unit testing"               OFF)
option(WITH_PYTHON   "compile leomax-tokenizer with python interpreter"         ON)

add_definitions(-DLEXOMAX_TOKENIZER_LIB)    # 添加了宏

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


set(CPP_PACKAGE_DIR ${CMAKE_BINARY_DIR}/cpp/leomax_tokenizer)

set(TOKENIZER_CORE_NAME "core_tokenizers")
set(TOKENIZER_CORE_PATH ${CMAKE_BINARY_DIR}/leomax_tokenizer)
message(STATUS "tokenizer core path: ${TOKENIZER_CORE_PATH}")

if (APPLE)
    set(TOKENIZER_CORE_LIBS_PATH "${TOKENIZER_CORE_PATH}/lib${TOKENIZER_CORE_NAME}.dylib")
else ()
    set(TOKENIZER_CORE_LIBS_PATH "${TOKENIZER_CORE_PATH}/lib${TOKENIZER_CORE_NAME}.so")
endif ()

# add_custom_target(copy_shared_library ALL
#     COMMAND ${CMAKE_COMMAND} -E copy ${TOKENIZER_CORE_LIBS_PATH} ${CPP_PACKAGE_DIR}/lib
#     DEPENDS build_cpp_package_dir core_tokenizers)

set(TOKENIZER_INSTALL_INCLUDE_DIR ${PROJECT_SOURCE_DIR})
include_directories(${TOKENIZERS_INSTALL_INCLUDE_DIR})
message(STATUS "tokenizer install include dir ${TOKENIZER_INSTALL_INCLUDE_DIR}")

# if(WIN32) 

# else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC") # 后续添加-O3 的优化
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ldl")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpthread")
# endif()

# 添加预定义的函数以及第三方库需要的一些配置
# config GIT_URL with github mirrors to speed up dependent repos clone
option(GIT_URL "Git URL to clone dependent repos" ${GIT_URL})
if(NOT GIT_URL)
    set(GIT_URL "https://github.com")
endif()

include(cmake/generic.cmake)
include(cmake/third_party.cmake)

######################################################################
#########################添加 c++ 版本的 tokenizer #####################
######################################################################
add_subdirectory(leomax_tokenizer)

if(WITH_PYTHON)
    # 编译 python 的库
    add_subdirectory(python)
endif(WITH_PYTHON)

if (APPLE)
    SET(CMAKE_INSTALL_RPATH "@loader_path/core_tokenizers.so")
endif()



