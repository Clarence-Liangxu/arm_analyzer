#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
LLVM_INSTALL_PATH="{SCRIPT_DIR}/third_party/llvm-install"

# LLVMConfig.cmake is found int LLVM_INSTALL_PATH
LLVM_DIR=${LLVM_INSTALL_PATH}/lib/cmake/llvm

function build_llvm()
{
    echo "build_llvm"
    cmake -G Ninja ../llvm \
	  -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" \
	  -DCMAKE_BUILD_TYPE=Release \
	  -DCMAKE_INSTALL_PREFIX=$LLVM_INSTALL \
	  -DCMAKE_C_COMPILER=clang  \
	  -DCMAKE_CXX_COMPILER=clang++ \
	  -DLLVM_TARGETS_TO_BUILD="AArch64"
    ninja
    ninja install
}

function build_proj()
{
    

}

function main()
{
    # build_llvm

}

main "$@"
