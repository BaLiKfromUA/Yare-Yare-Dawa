set(BINARY ${PROJECT_NAME})

file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.cpp)

set(SOURCES ${SOURCES})

add_executable(${BINARY}_run ${SOURCES})

add_library(${BINARY}_lib STATIC ${SOURCES})

target_compile_features(${BINARY}_run  PRIVATE cxx_std_17)
target_include_directories(${BINARY}_run  PRIVATE ${LLVM_INCLUDE_DIRS})
target_link_libraries(${BINARY}_run ${llvm_libs})