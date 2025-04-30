#!/bin/bash
set -euo pipefail

BUILD_DIR="build"
EXECUTABLE="arm_intrin_tool"

echo "=== [1/3] 创建构建目录 (${BUILD_DIR}) ==="
rm -rf ${BUILD_DIR}
mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

echo "=== [2/3] 运行 CMake 配置（生成 Ninja 构建文件） ==="
cmake -G Ninja -DLLVM_DIR=/opt/homebrew/opt/llvm@17/lib/cmake/llvm -DClang_DIR=/opt/homebrew/opt/llvm@17/lib/cmake/clang ..

echo "=== [3/3] 使用 Ninja 编译项目 ==="
ninja

echo "✅ 编译成功！可执行文件在 ${BUILD_DIR}/${EXECUTABLE}"

# 回到项目根目录
cd ..

echo "=== [4/4] 运行测试用例 ==="
# 你可以根据需要传测试文件
if [[ -f "${BUILD_DIR}/${EXECUTABLE}" ]]; then
    ./scripts/analyze.sh test/sample.c
else
    echo "❌ 错误：找不到可执行文件 ${BUILD_DIR}/${EXECUTABLE}"
    exit 1
fi

echo "✅ 测试完成！"
echo -e "\033[1;32m🎉 All green! Ready to optimize your code with ARM NEON! 🚀\\
033[0m"
