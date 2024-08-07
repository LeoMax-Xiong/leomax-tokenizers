# Copyright (c) 2022 PaddlePaddle Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

INCLUDE(ExternalProject)

SET(GLOG_PREFIX_DIR  ${THIRD_PARTY_PATH}/glog)
SET(GLOG_INSTALL_DIR ${THIRD_PARTY_PATH}/install/glog)
SET(GLOG_INCLUDE_DIR "${GLOG_INSTALL_DIR}/include" CACHE PATH "glog include directory." FORCE)
SET(GLOG_REPOSITORY https://gitee.com/leomax-mirrors/glog.git)
SET(GLOG_TAG        v0.4.0)

IF(WIN32)
  SET(GLOG_LIBRARIES "${GLOG_INSTALL_DIR}/lib/glog.lib" CACHE FILEPATH "glog library." FORCE)
  SET(GLOG_CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4267 /wd4530")
  add_definitions("/DGOOGLE_GLOG_DLL_DECL=")
ELSE(WIN32)
  SET(GLOG_LIBRARIES "${GLOG_INSTALL_DIR}/lib/libglog.a" CACHE FILEPATH "glog library." FORCE)
  SET(GLOG_CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
ENDIF(WIN32)

INCLUDE_DIRECTORIES(${GLOG_INCLUDE_DIR})

IF(ANDROID)    
set(CROSS_COMPILE_CMAKE_ARGS
    "-DCMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}"
    "-DCMAKE_SYSTEM_VERSION=${CMAKE_SYSTEM_VERSION}"
    "-DCMAKE_ANDROID_ARCH_ABI=${CMAKE_ANDROID_ARCH_ABI}"
    "-DCMAKE_ANDROID_NDK=${CMAKE_ANDROID_NDK}"
    "-DCMAKE_ANDROID_STL_TYPE=${CMAKE_ANDROID_STL_TYPE}"
    "-DANDROID_ABI=${CMAKE_ANDROID_ARCH_ABI}"
    "-DANDROID_TOOLCHAIN=${ANDROID_TOOLCHAIN}"
    "-DANDROID_STL=${CMAKE_ANDROID_STL_TYPE}"
    "-DCMAKE_SYSTEM_PROCESSOR=${CMAKE_SYSTEM_PROCESSOR}"
    "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_ANDROID_NDK}/build/cmake/android.toolchain.cmake"
    "-DCMAKE_ANDROID_NDK_TOOLCHAIN_VERSION=${CMAKE_ANDROID_NDK_TOOLCHAIN_VERSION}"
    "-DANDROID_PLATFORM=android-${ANDROID_NATIVE_API_LEVEL}"
    "-D__ANDROID_API__=${ANDROID_NATIVE_API_LEVEL}")

ExternalProject_Add(
    extern_glog
    ${EXTERNAL_PROJECT_LOG_ARGS}
    ${SHALLOW_CLONE}
    GIT_REPOSITORY  ${GLOG_REPOSITORY}
    GIT_TAG         ${GLOG_TAG}
    DEPENDS         gflags
    PREFIX          ${GLOG_PREFIX_DIR}
    UPDATE_COMMAND  ""
    CMAKE_ARGS      ${CROSS_COMPILE_CMAKE_ARGS}
                    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                    -DCMAKE_CXX_FLAGS=${GLOG_CMAKE_CXX_FLAGS}
                    -DCMAKE_CXX_FLAGS_RELEASE=${CMAKE_CXX_FLAGS_RELEASE}
                    -DCMAKE_CXX_FLAGS_DEBUG=${CMAKE_CXX_FLAGS_DEBUG}
                    -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
                    -DCMAKE_C_FLAGS_DEBUG=${CMAKE_C_FLAGS_DEBUG}
                    -DCMAKE_C_FLAGS_RELEASE=${CMAKE_C_FLAGS_RELEASE}
                    -DCMAKE_INSTALL_PREFIX=${GLOG_INSTALL_DIR}
                    -DCMAKE_INSTALL_LIBDIR=${GLOG_INSTALL_DIR}/lib
                    -DCMAKE_POSITION_INDEPENDENT_CODE=ON
                    -DWITH_GFLAGS=OFF
                    -DBUILD_TESTING=OFF
                    -DCMAKE_BUILD_TYPE=${THIRD_PARTY_BUILD_TYPE}
                    ${EXTERNAL_OPTIONAL_ARGS}
    CMAKE_CACHE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${GLOG_INSTALL_DIR}
                     -DCMAKE_INSTALL_LIBDIR:PATH=${GLOG_INSTALL_DIR}/lib
                     -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON
                     -DCMAKE_BUILD_TYPE:STRING=${THIRD_PARTY_BUILD_TYPE}
    BUILD_BYPRODUCTS ${GLOG_LIBRARIES}
)
ELSE()
ExternalProject_Add(
    extern_glog
    ${EXTERNAL_PROJECT_LOG_ARGS}
    ${SHALLOW_CLONE}
    GIT_REPOSITORY  ${GLOG_REPOSITORY}
    GIT_TAG         ${GLOG_TAG}
    DEPENDS         gflags
    PREFIX          ${GLOG_PREFIX_DIR}
    UPDATE_COMMAND  ""
    CMAKE_ARGS      -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                    -DCMAKE_CXX_FLAGS=${GLOG_CMAKE_CXX_FLAGS}
                    -DCMAKE_CXX_FLAGS_RELEASE=${CMAKE_CXX_FLAGS_RELEASE}
                    -DCMAKE_CXX_FLAGS_DEBUG=${CMAKE_CXX_FLAGS_DEBUG}
                    -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
                    -DCMAKE_C_FLAGS_DEBUG=${CMAKE_C_FLAGS_DEBUG}
                    -DCMAKE_C_FLAGS_RELEASE=${CMAKE_C_FLAGS_RELEASE}
                    -DCMAKE_INSTALL_PREFIX=${GLOG_INSTALL_DIR}
                    -DCMAKE_INSTALL_LIBDIR=${GLOG_INSTALL_DIR}/lib
                    -DCMAKE_POSITION_INDEPENDENT_CODE=ON
                    -DWITH_GFLAGS=OFF
                    -DBUILD_TESTING=OFF
                    -DCMAKE_BUILD_TYPE=${THIRD_PARTY_BUILD_TYPE}
                    ${EXTERNAL_OPTIONAL_ARGS}
    CMAKE_CACHE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${GLOG_INSTALL_DIR}
                     -DCMAKE_INSTALL_LIBDIR:PATH=${GLOG_INSTALL_DIR}/lib
                     -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON
                     -DCMAKE_BUILD_TYPE:STRING=${THIRD_PARTY_BUILD_TYPE}
    BUILD_BYPRODUCTS ${GLOG_LIBRARIES}
)
ENDIF()

ADD_LIBRARY(glog STATIC IMPORTED GLOBAL)
SET_PROPERTY(TARGET glog PROPERTY IMPORTED_LOCATION ${GLOG_LIBRARIES})
ADD_DEPENDENCIES(glog extern_glog gflags)
LINK_LIBRARIES(glog)