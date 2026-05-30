# Copyright 2026 JarryGoon
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

cmake_minimum_required(VERSION 3.25)
project(Plot LANGUAGES C CXX VERSION 0.2.0)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(ImPlot)
add_subdirectory(plotting_app)

file(GLOB_RECURSE SRC
     src/*.c
     src/*.cpp
)

file(GLOB_RECURSE INCLUDE
     include/*.h
     include/*.hpp
)

add_library(plot SHARED ${SRC} ${INCLUDE})
target_link_libraries(plot PUBLIC zmq)
target_include_directories(plot PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                           $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

include(GNUInstallDirs)

install(TARGETS Plotting_App
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(TARGETS plot
        EXPORT plotTargets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(FILES include/Plot.hpp include/PlotHeader.hpp
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT plotTargets
        FILE plotTargets.cmake
        NAMESPACE libplot::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/plot
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/plotConfigVersion.cmake"
        VERSION "${PROJECT_VERSION}"
        COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/plotConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake/plotConfig.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/plot
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/cmake/plotConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/plotConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/plot
)
