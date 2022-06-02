#!/usr/bin/env bash

# USE IT ONLY FOR DEBUG!

INPUT_FILE="./examples/functions_example.yyd"

cmake -Bbuild
cmake --build build

./build/src/Yare_Yare_Dawa_run -c "$INPUT_FILE" >./llvm_ir.ll
clang++-13 ./lib/yyd_lib.cpp -S -emit-llvm
llvm-link-13 ./llvm_ir.ll ./yyd_lib.ll -o test_linked.ll
llc-13 -filetype=obj test_linked.ll
clang++-13 test_linked.o
./a.out
