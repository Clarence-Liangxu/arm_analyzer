#!/bin/bash
set -e

# 自动判断 macOS/Linux 核心数
if [[ "$OSTYPE" == "darwin"* ]]; then
    CORES=$(sysctl -n hw.ncpu)
else
    CORES=$(nproc)
fi

PROJECT_DIR=$(pwd)
TEST_FILE="test/sample.c"

echo "🔍 Checking dependencies..."

# 检查 souffle 是否安装
if ! command -v souffle &> /dev/null
then
    echo "❌ Soufflé not found. Please install Soufflé Datalog engine first."
    exit 1
fi

echo "✅ Dependencies OK."

echo "🧹 Cleaning old build and facts..."
rm -rf build souffle/facts/*.facts souffle/output/* compile_commands.json

echo "🔨 [1/3] Building project with $CORES cores..."

mkdir build
cd build

# 关键：让 cmake 自动输出 compile_commands.json
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
make -j"$CORES"

# 拷贝 compile_commands.json（备用，虽然后面自己覆盖）
cp compile_commands.json ../

cd ..

echo "✅ Build completed."

echo "🧩 [1.5/3] Generating compile_commands.json for $TEST_FILE..."

# 根据文件后缀智能判断语言
EXT="${TEST_FILE##*.}"

# 默认C++
LANG_OPTION=""
STD_OPTION="-std=c++17"

if [[ "$EXT" == "c" ]]; then
    LANG_OPTION="-x c"
    STD_OPTION="-std=c99"
fi

# 生成 compile_commands.json
cat <<EOF > compile_commands.json
[
  {
    "directory": "$PROJECT_DIR",
    "command": "/opt/homebrew/opt/llvm@17/bin/clang++ $LANG_OPTION $STD_OPTION -I/opt/homebrew/opt/llvm@17/include $TEST_FILE",
    "file": "$TEST_FILE"
  }
]
EOF

echo "✅ compile_commands.json generated."

echo "🧪 [2/3] Running static analyzer..."

./build/arm_intrin_tool "$TEST_FILE"

echo "✅ Static analysis completed."

echo "📊 [3/3] Running Soufflé analysis..."

souffle -F souffle/facts -D souffle/output souffle/patterns.dl

echo "✅ Soufflé analysis completed."

echo -e "\033[1;32m🎉 All green! Your ARM intrinsic suggestions are ready! 🚀\033[0m"