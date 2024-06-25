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

function(cc_library TARGET_NAME)
  set(options STATIC static SHARED shared INTERFACE interface)
  set(oneValueArgs "")
  set(multiValueArgs SRCS DEPS)
  cmake_parse_arguments(cc_library "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  if(WIN32)
      # add libxxx.lib prefix in windows
      set(${TARGET_NAME}_LIB_NAME "${CMAKE_STATIC_LIBRARY_PREFIX}${TARGET_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}" CACHE STRING "output library name for target ${TARGET_NAME}")
  endif(WIN32)
  if(cc_library_SRCS)
      if(cc_library_SHARED OR cc_library_shared) # build *.so
        add_library(${TARGET_NAME} SHARED ${cc_library_SRCS})
      elseif(cc_library_INTERFACE OR cc_library_interface)
        generate_dummy_static_lib(LIB_NAME ${TARGET_NAME} FILE_PATH ${target_SRCS} GENERATOR "generic.cmake:cc_library")
      else()
        add_library(${TARGET_NAME} STATIC ${cc_library_SRCS})
      endif()
    if(cc_library_DEPS)
      # remove link to python, see notes at:
      # https://github.com/pybind/pybind11/blob/master/docs/compiling.rst#building-manually
      if("${cc_library_DEPS};" MATCHES "python;")
        list(REMOVE_ITEM cc_library_DEPS python)
        add_dependencies(${TARGET_NAME} python)
        if(WIN32)
          target_link_libraries(${TARGET_NAME} ${PYTHON_LIBRARIES})
        else()
          target_link_libraries(${TARGET_NAME} "-Wl,-undefined,dynamic_lookup")
        endif(WIN32)
      endif()
      target_link_libraries(${TARGET_NAME} ${cc_library_DEPS} ${PUBLIC_DEPEND_LIBS})
    endif()
    # For C++ 17 filesystem
    # target_link_libraries(${TARGET_NAME} stdc++fs)

    # cpplint code style
    foreach(source_file ${cc_library_SRCS})
      string(REGEX REPLACE "\\.[^.]*$" "" source ${source_file})
      if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${source}.h)
        list(APPEND cc_library_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/${source}.h)
      endif()
    endforeach()

  else(cc_library_SRCS)
    if(cc_library_DEPS)
      list(REMOVE_DUPLICATES cc_library_DEPS)
      set(dummy_FILE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}_dummy.c")
      configure_file(${PROJECT_SOURCE_DIR}/cmake/dummy.c.in ${dummy_FILE_PATH} @ONLY)
      if(cc_library_SHARED OR cc_library_shared) # build *.so
        add_library(${TARGET_NAME} SHARED ${dummy_FILE_PATH})
      elseif(cc_library_INTERFACE OR cc_library_interface)
        generate_dummy_static_lib(LIB_NAME ${TARGET_NAME} FILE_PATH ${dummy_FILE_PATH} GENERATOR "generic.cmake:cc_library")
      else()
        add_library(${TARGET_NAME} STATIC ${dummy_FILE_PATH})
      endif()
      target_link_libraries(${TARGET_NAME} ${cc_library_DEPS})
    else()
      message(FATAL_ERROR "Please specify source files or libraries in cc_library(${TARGET_NAME} ...).")
    endif()
  endif(cc_library_SRCS)
endfunction(cc_library)
