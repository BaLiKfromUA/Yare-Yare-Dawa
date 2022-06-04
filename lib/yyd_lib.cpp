#include <iostream>
#include <chrono>
#include <vector>
#include <cstring>
#include <cmath>

struct helper {
    std::vector<char *> string_cache{};

    ~helper() {
        for (char *str: string_cache) {
            operator delete(str);
        }
    }

} helper_instance;

// helper
extern "C" void __yyd_start();

// input/output
extern "C" void __yyd_print_double(double n) { std::cout << n << std::endl; }

extern "C" void __yyd_print_bool(bool n) { std::cout << (n ? "true" : "false") << std::endl; }

extern "C" void __yyd_print_string(char *n) { std::cout << n << std::endl; }

// todo: scan

// strings
extern "C" char *__yyd_string_concat(char *left, char *right) {
    char *tmp = new char[strlen(left) + 1];
    strcpy(tmp, left);
    strcat(tmp, right);
    helper_instance.string_cache.push_back(tmp); // in order to be free later
    return tmp;
}

extern "C" char *__yyd_string_multiply(char *str, double n_double) {
    int n = floor(n_double);
    int size = strlen(str);

    if (n <= 0 || size == 0) {
        char *empty = new char[1];
        empty[0] = '\0';
        helper_instance.string_cache.push_back(empty);
        return empty;
    }

    char *result = new char[(strlen(str) * n) + 1];
    int offset = 0;
    for (int i = 0; i < n; ++i, offset += size) {
        strcpy(result + offset, str);
    }

    helper_instance.string_cache.push_back(result);

    return result;
}

extern "C" double len(char *str) {
    return strlen(str);
}

// standard library
extern "C" double now() {
    auto ticks = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration<double>{ticks}.count() / 1000.0;
}

int main() {
    __yyd_start();
}