#!/usr/bin/env bash

set -eoux pipefail

function run_test() {
  INPUT_FILE=$1
  NAME=$(basename "$INPUT_FILE" .yyd)
  EXPECTED_OUTPUT_FILE="$PWD/integration-tests/expected-output/$NAME.output"
  REAL_OUTPUT_FILE="$PWD/test.output"

  RUN_MODE=$2

  if [ "$RUN_MODE" == "compiler" ]; then
    ./build/src/Yare_Yare_Dawa_run -c "$INPUT_FILE" >./llvm_ir.ll
    clang++-13 ./lib/yyd_lib.cpp -S -emit-llvm
    llvm-link-13 ./llvm_ir.ll ./yyd_lib.ll -o test_linked.ll
    llc-13 -filetype=obj test_linked.ll
    clang++-13 test_linked.o
    ./a.out >"$REAL_OUTPUT_FILE"
  else
    ./build/src/Yare_Yare_Dawa_run -i "$INPUT_FILE" >"$REAL_OUTPUT_FILE"
  fi

  TEST_RESULT=$(diff "$EXPECTED_OUTPUT_FILE" "$REAL_OUTPUT_FILE")

  if [ -n "$TEST_RESULT" ]; then
    return 1
  fi
}

EXAMPLES="$PWD/examples"
for example in "$EXAMPLES"/*.yyd; do
  run_test "$example" "interpreter"
done

for example in "$EXAMPLES"/*.yyd; do
  run_test "$example" "compiler"
done
