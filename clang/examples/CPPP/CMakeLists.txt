cmake_minimum_required(VERSION 3.20)
project(Trine LANGUAGES CXX)

get_filename_component(default_clang
  "${CMAKE_CURRENT_LIST_DIR}/../../../build-cppp/bin/clang++.exe" ABSOLUTE)
set(CXXX_CLANG "${default_clang}" CACHE FILEPATH "Clang with C+++ support")
option(CXXX_TRINE_OTHER_EQUAL "Make other compare equal to itself" ON)
option(CXXX_TRINE_OTHER_ORDERED "Order other before boolean and integer values" OFF)
option(CXXX_WARN_MISSING_OTHERWISE "Warn about omitted trine otherwise handlers" OFF)
if(CXXX_TRINE_OTHER_ORDERED AND NOT CXXX_TRINE_OTHER_EQUAL)
  message(FATAL_ERROR "Ordered trines require CXXX_TRINE_OTHER_EQUAL=ON.")
endif()
if(NOT EXISTS "${CXXX_CLANG}")
  message(FATAL_ERROR "Build the C+++ compiler first, or set CXXX_CLANG.")
endif()

set(cxxx_options
  -x c+++
  "$<IF:$<BOOL:${CXXX_TRINE_OTHER_EQUAL}>,-ftrine-other-equal,-fno-trine-other-equal>"
  "$<IF:$<BOOL:${CXXX_TRINE_OTHER_ORDERED}>,-ftrine-other-ordered,-fno-trine-other-ordered>"
  "$<IF:$<BOOL:${CXXX_WARN_MISSING_OTHERWISE}>,-Wtrine-missing-otherwise,-Wno-trine-missing-otherwise>")

set(trine_exe "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/trine-demo.exe")
add_custom_target(trine-demo ALL
  COMMAND "${CMAKE_COMMAND}" -E make_directory
          "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>"
  COMMAND "${CXXX_CLANG}" ${cxxx_options}
          "$<IF:$<CONFIG:Debug>,-O0,-O2>" -g -gcodeview -fuse-ld=lld
          "-Wl,/debug" "${CMAKE_CURRENT_LIST_DIR}/trine.cpp" -o "${trine_exe}"
  BYPRODUCTS "${trine_exe}"
  SOURCES trine.cpp
  VERBATIM)

set_target_properties(trine-demo PROPERTIES
  VS_DEBUGGER_COMMAND "${trine_exe}"
  VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>")
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT trine-demo)

include(CTest)
if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
