cmake_minimum_required(VERSION 3.13)
project(FindFunctionsTool)

# 指定LLVM的安装路径
set(LLVM_DIR "/opt/homebrew/opt/llvm@17/lib/cmake/llvm")
find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Found Clang ${Clang_VERSION}")
message(STATUS "LLVM include dirs: ${LLVM_INCLUDE_DIRS}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(find-functions find_functions.cpp)

target_link_libraries(find-functions
  PRIVATE
  clangTooling
  clangBasic
  clangASTMatchers
)
