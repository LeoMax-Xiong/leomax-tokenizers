# include(python_module)

# ## 查找 Python 3 解释器和库
# find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
# #
# ## 获取 Python 3 的头文件和库
# #include_directories(${Python3_INCLUDE_DIRS})
# #link_directories(${Python3_LIBRARY_DIRS})
# #
# message(STATUS "Python3_LIBRARIES: ${Python3_LIBRARIES}")
# message(STATUS "Python3_INCLUDE_DIRS: ${Python3_INCLUDE_DIRS}")
# message(STATUS "Python3_EXECUTABLE: ${Python3_EXECUTABLE}")
# #
# add_library(python SHARED IMPORTED GLOBAL)
# set_property(TARGET python PROPERTY IMPORTED_LOCATION ${Python3_LIBRARIES})
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

INCLUDE(python_module)

FIND_PACKAGE(PythonInterp ${PY_VERSION} REQUIRED)
FIND_PACKAGE(PythonLibs ${PY_VERSION} REQUIRED)

# Fixme: Maybe find a static library. Get SHARED/STATIC by FIND_PACKAGE.
ADD_LIBRARY(python SHARED IMPORTED GLOBAL)
#SET_PROPERTY(TARGET python PROPERTY IMPORTED_LOCATION ${PYTHON_LIBRARIES})
set_property(TARGET python PROPERTY IMPORTED_LOCATION ${Python3_LIBRARIES})

INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${PYTHON_NUMPY_INCLUDE_DIR})
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

# include(python_module)

# ## 查找 Python 3 解释器和库
# find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# message(STATUS "Python3_LIBRARIES: ${Python3_LIBRARIES}")
# message(STATUS "Python3_INCLUDE_DIRS: ${Python3_INCLUDE_DIRS}")
# message(STATUS "Python3_EXECUTABLE: ${Python3_EXECUTABLE}")

# include_directories(${Python3_INCLUDE_DIRS})        # 添加 python3 的头文件路径
# add_library(python SHARED IMPORTED GLOBAL)
# set_property(TARGET python PROPERTY IMPORTED_LOCATION ${Python3_LIBRARIES}) # 将python3 的库可以赋值给 python 变量

# # Fixme: Maybe find a static library. Get SHARED/STATIC by FIND_PACKAGE.

# SET(py_env "")
# IF(PYTHONINTERP_FOUND)
#     find_python_module(pip REQUIRED)
#     find_python_module(numpy REQUIRED)
#     find_python_module(wheel REQUIRED)
#     FIND_PACKAGE(NumPy REQUIRED)
#     INCLUDE_DIRECTORIES(${PYTHON_NUMPY_INCLUDE_DIR})
# ENDIF(PYTHONINTERP_FOUND)
