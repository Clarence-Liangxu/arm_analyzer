cmake_minimum_required(VERSION 3.15)
project(arm_intrin_tool)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 指定 LLVM/Clang 安装路径（根据你的环境调整）
set(LLVM_DIR "/opt/homebrew/opt/llvm@17/lib/cmake/llvm")
set(Clang_DIR "/opt/homebrew/opt/llvm@17/lib/cmake/clang")

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

include_directories(${CMAKE_SOURCE_DIR}/src)

file(GLOB SOURCES
  src/*.cpp
)

add_executable(arm_intrin_tool ${SOURCES})

target_link_libraries(arm_intrin_tool
  PRIVATE
  clangTooling
  clangFrontend
  clangAST
  clangASTMatchers
  clangBasic
  clangParse
  clangDriver
  clangSerialization
  clangSema
  clangEdit
  clangAnalysis
  clangRewrite
  clangLex
  clangSupport
  LLVM
)
