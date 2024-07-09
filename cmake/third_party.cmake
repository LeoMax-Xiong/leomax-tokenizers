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
include(cmake/external/optional.cmake)