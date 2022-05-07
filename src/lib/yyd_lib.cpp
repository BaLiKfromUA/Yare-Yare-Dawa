#include <iostream>

extern "C" void __yyd_start();

extern "C" void __yyd_print(double n) { std::cout << n << std::endl; }

int main() { __yyd_start(); }

// todo: scan