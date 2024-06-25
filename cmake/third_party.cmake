include(ExternalProject)
set(THIRD_PARTY_PATH  "${CMAKE_BINARY_DIR}/third_party" CACHE STRING
        "A path setting third party libraries download & build directories.")

if (WITH_PYTHON)
    include(cmake/external/python.cmake)
    include(cmake/external/pybind11.cmake)
endif()

include(cmake/external/re2.cmake)
include(cmake/external/gflags.cmake)    # gflags 一定要放在glog之前
include(cmake/external/glog.cmake)
include(cmake/external/icu.cmake)
include(cmake/external/nlohmann_json.cmake)
ADD_LIBRARY(python SHARED IMPORTED GLOBAL)
SET_PROPERTY(TARGET python PROPERTY IMPORTED_LOCATION ${PYTHON_LIBRARIES})

SET(py_env "")
if(PYTHONINTERP_FOUND)
    find_python_module(pip REQUIRED)
    find_python_module(numpy REQUIRED)
    find_python_module(wheel REQUIRED)
    FIND_PACKAGE(NumPy REQUIRED)
endif(PYTHONINTERP_FOUND)
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${PYTHON_NUMPY_INCLUDE_DIR})