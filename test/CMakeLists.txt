set(TESTS ${PROJECT_NAME}_test)

enable_testing()

file(GLOB_RECURSE TESTS_SOURCES *.cpp)

add_executable(${TESTS} ${TESTS_SOURCES})

target_link_libraries(
        ${TESTS}
        gtest_main
        ${PROJECT_NAME}_lib
)

include(GoogleTest)
gtest_discover_tests(${TESTS})

add_test(${TESTS} ${TESTS})