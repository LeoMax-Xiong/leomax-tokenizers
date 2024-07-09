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

include(ExternalProject)

set(OPTIONAL_PREFIX_DIR     ${THIRD_PARTY_PATH}/optional)
SET(OPTIONAL_REPOSITORY     ${GIT_URL}/TartanLlama/optional.git)
SET(OPTIONAL_TAG            v1.1.0)

set(OPTIONAL_INCLUDE_DIR ${THIRD_PARTY_PATH}/optional/src/extern_optional/include/)
include_directories(${OPTIONAL_INCLUDE_DIR})

ExternalProject_Add(
  extern_optional
        ${EXTERNAL_PROJECT_LOG_ARGS}
        ${SHALLOW_CLONE}
        GIT_REPOSITORY    ${OPTIONAL_REPOSITORY}
        GIT_TAG           ${OPTIONAL_TAG}
        GIT_PROGRESS      1
        PREFIX            ${OPTIONAL_PREFIX_DIR}
        # If we explicitly leave the `UPDATE_COMMAND` of the ExternalProject_Add
        # function in CMakeLists blank, it will cause another parameter GIT_TAG
        # to be modified without triggering incremental compilation, and the
        # third-party library version changes cannot be incorporated.
        # reference: https://cmake.org/cmake/help/latest/module/ExternalProject.html
        UPDATE_COMMAND    ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND     ""
        INSTALL_COMMAND   ""
        TEST_COMMAND      ""
)

add_library(optioanl INTERFACE)

add_dependencies(optioanl extern_optioanl)