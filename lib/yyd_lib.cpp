#include <iostream>

extern "C" void __yyd_start();

extern "C" void __yyd_print_double(double n) { std::cout << n << std::endl; }

extern "C" void __yyd_print_bool(bool n) { std::cout << (n ? "true" : "false") << std::endl; }

extern "C" void __yyd_print_string(char *n) { std::cout << n << std::endl; }

int main() { __yyd_start(); }

// todo: scan