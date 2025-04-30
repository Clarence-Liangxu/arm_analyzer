#!/bin/bash
set -e

PROJECT_DIR=$(pwd)
TEST_FILE="test/sample.c"

echo "[0/3] Cleaning previous builds..."
# rm -rf build souffle/facts souffle/output compile_commands.json
mkdir -p build souffle/facts souffle/output

echo "[1/3] Building static analyzer..."
cd build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
make -j$(sysctl -n hw.ncpu)
cp compile_commands.json ../
cd ..

echo "[2/3] Running static analyzer..."
./build/arm_intrin_tool "$TEST_FILE"
cp ./build/souffle/facts/* souffle/facts
ls -alh souffle/facts
echo "[3/3] Running Soufflé analysis..."
souffle -F souffle/facts -D souffle/output souffle/patterns.dl

echo "[✔] Optimization suggestions:"
if [ -s souffle/output/optimize.csv ]; then
  column -t -s $'\t' souffle/output/optimize.csv
else
  echo "⚠️  No optimization opportunities detected."
fi
