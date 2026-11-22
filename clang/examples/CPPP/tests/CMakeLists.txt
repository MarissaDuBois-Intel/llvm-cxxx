get_filename_component(gtest_root
  "${CMAKE_CURRENT_SOURCE_DIR}/../../../../third-party/unittest/googletest"
  ABSOLUTE)

add_library(cxxx-gtest STATIC "${gtest_root}/src/gtest-all.cc")
target_include_directories(cxxx-gtest
  PUBLIC "${gtest_root}/include"
  PRIVATE "${gtest_root}")
target_compile_definitions(cxxx-gtest PUBLIC GTEST_NO_LLVM_SUPPORT=1)
target_compile_features(cxxx-gtest PUBLIC cxx_std_20)

set(cxxx_test_sources
  TrineTests.cpp
  ComparisonTests.cpp
  IfTests.cpp
  WhenTests.cpp)
set(cxxx_test_objects)
foreach(source IN LISTS cxxx_test_sources)
  get_filename_component(stem "${source}" NAME_WE)
  set(object "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/${stem}.obj")
  add_custom_command(
    OUTPUT "${object}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory
            "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>"
    COMMAND "${CXXX_CLANG}" ${cxxx_options} -std=c++20
            "$<IF:$<CONFIG:Debug>,-O0,-O2>" -g -gcodeview
            "-fms-runtime-lib=$<IF:$<CONFIG:Debug>,dll_dbg,dll>"
            "-DCXXX_TEST_OTHER_EQUAL=$<BOOL:${CXXX_TRINE_OTHER_EQUAL}>"
            "-DCXXX_TEST_OTHER_ORDERED=$<BOOL:${CXXX_TRINE_OTHER_ORDERED}>"
            -c "${CMAKE_CURRENT_SOURCE_DIR}/${source}" -o "${object}"
    DEPENDS "${source}" TestSupport.h "${CXXX_CLANG}"
            "${CMAKE_BINARY_DIR}/CMakeCache.txt"
    COMMENT "Compiling ${source} as C+++"
    VERBATIM)
  list(APPEND cxxx_test_objects "${object}")
endforeach()

set_source_files_properties(${cxxx_test_objects}
  PROPERTIES GENERATED TRUE EXTERNAL_OBJECT TRUE)
set_source_files_properties(${cxxx_test_sources} DiagnosticInputs.cpp
  PROPERTIES HEADER_FILE_ONLY TRUE)

add_executable(trine-tests
  TestMain.cpp DiagnosticTests.cpp DiagnosticInputs.cpp TestSupport.h
  ${cxxx_test_sources} ${cxxx_test_objects})
target_link_libraries(trine-tests PRIVATE cxxx-gtest)
target_compile_definitions(trine-tests PRIVATE
  "CXXX_COMPILER_PATH=\"${CXXX_CLANG}\""
  "CXXX_DIAGNOSTIC_SOURCE=\"${CMAKE_CURRENT_SOURCE_DIR}/DiagnosticInputs.cpp\"")
set_target_properties(cxxx-gtest trine-tests PROPERTIES
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
set_target_properties(trine-tests PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>"
  VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")

include(GoogleTest)
gtest_discover_tests(trine-tests
  TEST_PREFIX "CXXX."
  DISCOVERY_TIMEOUT 30
  PROPERTIES LABELS "cxxx")
