#!/bin/bash
set -e

PROJECT_DIR=$(pwd)
TEST_FILE="test/sample.c"

rm -rf build souffle/facts/*.facts souffle/output/* compile_commands.json

mkdir -p build
mkdir -p souffle/facts souffle/output
cd build

cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
make -j$(sysctl -n hw.ncpu)
cp compile_commands.json ../

cd ..

# 生成针对 sample.c 的 compile_commands.json
cat <<EOF > compile_commands.json
[
  {
    "directory": "$PROJECT_DIR",
    "command": "/opt/homebrew/opt/llvm@17/bin/clang++ -x c -std=c99 -I/opt/homebrew/opt/llvm@17/include $TEST_FILE",
    "file": "$TEST_FILE"
  }
]
EOF

echo "[1/3] Running static analyzer..."
./build/arm_intrin_tool "$TEST_FILE"

# 保证 Soufflé 所需 .facts 存在
for f in addition.facts sub.facts mul.facts div.facts fma.facts fabs.facts neg.facts; do
  [ -f "souffle/facts/$f" ] || touch "souffle/facts/$f"
done

echo "[2/3] Running Soufflé..."
souffle -F souffle/facts -D souffle/output souffle/patterns.dl

echo "[3/3] Optimization suggestions:"
cat souffle/output/optimize.csv
