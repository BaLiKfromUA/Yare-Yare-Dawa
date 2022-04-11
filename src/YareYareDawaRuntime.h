//
// Created by balik on 4/8/2022.
//

#ifndef YARE_YARE_DAWA_YAREYAREDAWARUNTIME_H
#define YARE_YARE_DAWA_YAREYAREDAWARUNTIME_H


#include <string_view>
#include <iostream>

class YareYareDawaRuntime {
public:
    /*=== error handling ===*/
    static bool isError;

    static void report(int line, std::string_view where,
                       std::string_view message) {
        std::cerr << "[line " << line << "] Error" << where << ": " << message << "\n";
        isError = true;
    }

    static void error(int line, std::string_view message) {
        report(line, "", message);
    }

    /*=== entry points ===*/
    void runFile(std::string_view filePath);

    void runPrompt();

private:
    void run(std::string_view source);
};

// todo: fix inline
inline bool YareYareDawaRuntime::isError = false;

#endif //YARE_YARE_DAWA_YAREYAREDAWARUNTIME_H
