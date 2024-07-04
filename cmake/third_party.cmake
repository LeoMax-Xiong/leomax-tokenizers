include(ExternalProject)
set(THIRD_PARTY_PATH  "${CMAKE_BINARY_DIR}/third_party" CACHE STRING
        "A path setting third party libraries download & build directories.")

if (WITH_PYTHON)
    include(external/python)
    include(external/pybind11)
endif()

# 添加额外的第三方库
include(external/re2)
include(external/gflags)    # gflags 一定要放在glog之前
include(external/glog)
#include(external/icu)      # icu 可以后续自建仓库，官方仓库太大
include(external/nlohmann_json)

add_library(python SHARED IMPORTED GLOBAL)
SET_PROPERTY(TARGET python PROPERTY IMPORTED_LOCATION ${PYTHON_LIBRARIES})
#
SET(py_env "")
if(PYTHONINTERP_FOUND)
    find_python_module(pip REQUIRED)
    find_python_module(numpy REQUIRED)
    find_python_module(wheel REQUIRED)
    FIND_PACKAGE(NumPy REQUIRED)
endif(PYTHONINTERP_FOUND)
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${PYTHON_NUMPY_INCLUDE_DIR})