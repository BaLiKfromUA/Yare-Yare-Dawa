#include <iostream>
#include <vector>
#include <cstring>

struct helper {
    std::vector<char *> string_cache{};

    ~helper() {
        for (char *str: string_cache) {
            free(str);
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
    char *tmp = (char *) malloc(strlen(left) + 1);// todo: replace with new
    strcpy(tmp, left);
    helper_instance.string_cache.push_back(tmp); // in order to be free later
    return strcat(tmp, right);
}

// std
// todo:

int main() {
    __yyd_start();
}