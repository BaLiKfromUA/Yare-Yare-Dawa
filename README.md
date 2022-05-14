# Yare-Yare-Dawa
tiny programming language for learning purpose. 


```bash
clang++-13 ./lib/yyd_lib.cpp -S -emit-llvm
llvm-link-13 examples/test.ll ./yyd_lib.ll -o test_linked.ll
llc-13 -filetype=obj test_linked.ll
clang++-13 test_linked.o
./a.out
```