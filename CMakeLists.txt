cmake_minimum_required(VERSION 3.21)

set(PROJECT_NAME Yare_Yare_Dawa)

project(${PROJECT_NAME} C CXX)

set(CMAKE_CXX_STANDARD 17)

#! Be default -- build release version if not specified otherwise.
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()


# install Google test
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# modules enabling
include_directories(src)

add_subdirectory(src)
add_subdirectory(test)
