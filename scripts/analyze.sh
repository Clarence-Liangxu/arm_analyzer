#!/bin/bash
set -e

SRC=$1

if [ -z "$SRC" ]; then
  echo "Usage: ./scripts/analyze.sh path/to/source.c"
  exit 1
fi

echo "[1/3] Running static analyzer..."
./build/arm_intrin_tool "$SRC"

echo "[2/3] Running Soufflé Datalog inference..."
souffle -F souffle/facts -D souffle/output souffle/patterns.dl

echo "[3/3] Suggested optimizations:"

# 读取 optimize.csv，自动根据 op 和 type 输出对应 NEON intrinsic
cat souffle/output/optimize.csv | while IFS=$'\t' read -r file line op type; do
    echo "Analyzing: $file Line $line (operation: $op, type: $type)"
    suggestion=""

    # 匹配 op+type，给出对应 NEON 指令
    if [[ "$op" == "add" && "$type" == "f32" ]]; then suggestion="vaddq_f32"; fi
    if [[ "$op" == "add" && "$type" == "f64" ]]; then suggestion="vaddq_f64"; fi
    if [[ "$op" == "add" && "$type" == "s32" ]]; then suggestion="vaddq_s32"; fi
    if [[ "$op" == "add" && "$type" == "s16" ]]; then suggestion="vaddq_s16"; fi
    if [[ "$op" == "add" && "$type" == "u8" ]];  then suggestion="vaddq_u8";  fi

    if [[ "$op" == "sub" && "$type" == "f32" ]]; then suggestion="vsubq_f32"; fi
    if [[ "$op" == "sub" && "$type" == "s32" ]]; then suggestion="vsubq_s32"; fi

    if [[ "$op" == "mul" && "$type" == "f32" ]]; then suggestion="vmulq_f32"; fi
    if [[ "$op" == "mul" && "$type" == "f64" ]]; then suggestion="vmulq_f64"; fi
    if [[ "$op" == "mul" && "$type" == "s32" ]]; then suggestion="vmulq_s32"; fi

    if [[ "$op" == "div" && "$type" == "f32" ]]; then suggestion="vmulq_f32(a, vrecpeq_f32(b)) // approximate division"; fi

    if [[ "$op" == "fma" && "$type" == "f32" ]]; then suggestion="vfmaq_f32(c, a, b)"; fi
    if [[ "$op" == "fabs" && "$type" == "f32" ]]; then suggestion="vabsq_f32"; fi
    if [[ "$op" == "neg" && "$type" == "f32" ]]; then suggestion="vnegq_f32"; fi

    if [[ -z "$suggestion" ]]; then
        echo "⚠️  File: $file Line: $line => No known optimization for ($op, $type)"
    else
        echo "✅  File: $file Line: $line => Suggested NEON intrinsic: $suggestion"
    fi
done
