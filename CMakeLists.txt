# Xbox360 USB Gamepad Userspace Driver
# Copyright (C) 2015 Ingo Ruhnke <grumbel@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.0)
project(argparser)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/mk/cmake)

include(GNUInstallDirs)
include(MaximumWarnings)
include(ClangTidy)

option(BUILD_TESTS "Build tests" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(argparser
  src/argparser.cpp
  src/prettyprinter.cpp)
target_include_directories(argparser PUBLIC include/ src/)
target_compile_options(argparser PRIVATE ${WARNINGS_CXX_FLAGS})
set_target_properties(argparser PROPERTIES PUBLIC_HEADER "include/argparser.hpp")

if(BUILD_TESTS)
  enable_testing()

  file(GLOB TEST_ARGPARSER_SOURCES test/*_test.cpp)

  foreach(SRC ${TEST_ARGPARSER_SOURCES})
    get_filename_component(SRC_BASENAME ${SRC} NAME_WE)
    add_executable(${SRC_BASENAME} ${SRC})
    target_link_libraries(${SRC_BASENAME} argparser)
    target_compile_options(${SRC_BASENAME} PRIVATE ${WARNINGS_CXX_FLAGS})

    add_test(NAME ${SRC_BASENAME}
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
      COMMAND "${SRC_BASENAME}")
  endforeach()
endif()

install(TARGETS argparser
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}")

# EOF #