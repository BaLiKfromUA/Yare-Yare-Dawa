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


    void pop_last_str() {
        auto tmp = string_cache.back();
        operator delete(tmp);
        string_cache.pop_back();
    }

} helper_instance;

// helper
extern "C" void __yyd_start();

// strings
extern "C" char *__yyd_string_concat(char *left, char *right) {
    char *tmp = new char[strlen(left) + strlen(right) + 1];
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

extern "C" char *__yyd_num_to_string(double num) {
    // copy-pasted code from Interpreter just to be sure that behaviour the same
    std::string text = std::to_string(num);

    int ind = text.length() - 1;

    while (text[ind] == '0') {
        --ind;
    }

    if (text[ind] == '.') {
        text = text.substr(0, ind);
    } else {
        text = text.substr(0, ind + 1);
    }

    char *tmp = new char[text.size() + 1];
    strcpy(tmp, text.data());

    helper_instance.string_cache.push_back(tmp);

    return tmp;
}

extern "C" char *__yyd_bool_to_string(bool n) {
    auto n_str = n ? "true" : "false";
    char *tmp = new char[strlen(n_str) + 1];
    strcpy(tmp, n_str);

    helper_instance.string_cache.push_back(tmp);

    return tmp;
}

extern "C" char *__yyd_string_to_string(char *n) {
    return n;
}

extern "C" double len(char *str) {
    return strlen(str);
}

// output
extern "C" void __yyd_print_double(double n) {
    std::cout << __yyd_num_to_string(n) << std::endl;
    // in order to not accumulate printed strings in cache
    helper_instance.pop_last_str();
}

extern "C" void __yyd_print_bool(bool n) {
    std::cout << __yyd_bool_to_string(n) << std::endl;
    // in order to not accumulate printed strings in cache
    helper_instance.pop_last_str();
}

extern "C" void __yyd_print_string(char *n) { std::cout << n << std::endl; }


// input
extern "C" double __yyd_scan_double() {
    double n;
    std::cin >> n;
    if (!std::cin) {
        std::cerr << "Problem reading stdin\n";
        exit(1);
    }
    return n;
}

extern "C" bool __yyd_scan_bool() {
    bool n;
    std::cin >> n;
    if (!std::cin) {
        std::cerr << "Problem reading stdin\n";
        exit(1);
    }
    return n;
}

extern "C" char *__yyd_scan_string() {
    std::string n;
    std::cin >> n;
    if (!std::cin) {
        std::cerr << "Problem reading stdin\n";
        exit(1);
    }

    char *tmp = new char[n.size() + 1];
    strcpy(tmp, n.data());

    helper_instance.string_cache.push_back(tmp);

    return tmp;
}


// standard library
extern "C" double now() {
    auto ticks = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration<double>{ticks}.count() / 1000.0;
}

int main() {
    __yyd_start();
}