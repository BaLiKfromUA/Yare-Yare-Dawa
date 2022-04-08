cmake_minimum_required(VERSION 3.21)

set(PROJECT_NAME Yare_Yare_Dawa)
set(TEST_NAME ${PROJECT_NAME}_test)

project(${PROJECT_NAME} C CXX)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE INCLUDES ${PROJECT_SOURCE_DIR}/src/*.h*)

add_executable(${PROJECT_NAME} ${INCLUDES} ${SOURCES})

##########################################################
# Google test
##########################################################
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
        ${TEST_NAME}
        ${PROJECT_SOURCE_DIR}/test/sample_test.cpp
)

target_include_directories(${TEST_NAME} PRIVATE src)

target_link_libraries(
        ${TEST_NAME}
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(${TEST_NAME})

add_test(${TEST_NAME} ${TEST_NAME})
