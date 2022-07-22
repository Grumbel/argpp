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
project(argparser VERSION 2.0.0)

include(mk/cmake/TinyCMMC.cmake)

function(build_dependencies)
  find_package(fmt REQUIRED)
  tinycmmc_find_dependency(strut)
endfunction()
build_dependencies()

file(GLOB ARGPARSER_SOURCES src/*.cpp)
file(GLOB ARGPARSER_HEADERS include/argparser/*.hpp)
add_library(argparser STATIC
  ${ARGPARSER_SOURCES})
set_target_properties(argparser PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF)
target_include_directories(argparser PUBLIC include/ include/argparser/ src/)
target_compile_options(argparser PRIVATE ${WARNINGS_CXX_FLAGS})
target_link_libraries(argparser PUBLIC fmt::fmt strut)
set_target_properties(argparser PROPERTIES PUBLIC_HEADER
  "${ARGPARSER_HEADERS}")

if(BUILD_TESTS)
  enable_testing()

  file(GLOB EXAMPLE_ARGPARSER_SOURCES test/*_example.cpp)
  foreach(SRC ${EXAMPLE_ARGPARSER_SOURCES})
    get_filename_component(SRC_BASENAME ${SRC} NAME_WE)
    add_executable(${SRC_BASENAME} ${SRC})
    set_target_properties(${SRC_BASENAME} PROPERTIES
      CXX_STANDARD 20
      CXX_STANDARD_REQUIRED ON
      CXX_EXTENSIONS OFF)
    target_link_libraries(${SRC_BASENAME} argparser)
    target_compile_options(${SRC_BASENAME} PRIVATE ${WARNINGS_CXX_FLAGS})

    add_test(NAME ${SRC_BASENAME}
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
      COMMAND "${SRC_BASENAME}")
  endforeach()

  find_package(GTest REQUIRED)

  file(GLOB TEST_ARGPARSER_SOURCES test/*_test.cpp)
  add_executable(test_argparser ${TEST_ARGPARSER_SOURCES})
  set_target_properties(test_argparser PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF)
  target_compile_options(test_argparser PRIVATE ${WARNINGS_CXX_FLAGS})
  target_link_libraries(test_argparser
    argparser
    GTest::GTest
    GTest::Main)

    add_test(NAME test_argparser
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
      COMMAND test_argparser)
endif()

install(TARGETS argparser
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}")

# EOF #
