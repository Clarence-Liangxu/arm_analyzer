cmake_minimum_required(VERSION 3.13)
project(arm_intrin_tool)

# 必须在build.sh中指定LLVM的安装路径
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

# 添加源文件
add_executable(arm_intrin_tool
    src/main.cpp
    src/LoopAnalyzer.cpp
    src/FactEmitter.cpp
    src/Suggest.cpp
)

target_link_libraries(arm_intrin_tool
    PRIVATE
    clangFrontend
    clangTooling
    clangBasic
    clangParse
    clangDriver
    clangSerialization
    clangSema
    clangEdit
    clangAnalysis
    clangAST
    clangASTMatchers
    clangSupport
    clangFormat
    clangToolingInclusions
    clangToolingCore
    clangRewrite
    clangLex
    LLVM
)