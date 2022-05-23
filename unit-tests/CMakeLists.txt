set(TESTS ${PROJECT_NAME}_test)

file(GLOB_RECURSE TESTS_SOURCES *.cpp)

add_executable(${TESTS} ${TESTS_SOURCES})

target_link_libraries(
        ${TESTS}
        gtest_main
        ${PROJECT_NAME}_lib
)

# todo: refactor
target_compile_features(${TESTS} PRIVATE cxx_std_17)
target_include_directories(${TESTS}  PRIVATE ${LLVM_INCLUDE_DIRS})
target_link_libraries(${TESTS} ${llvm_libs})

include(GoogleTest)
gtest_discover_tests(${TESTS})

add_test(${TESTS} ${TESTS})