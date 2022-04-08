//
// Created by balik on 4/8/2022.
//

#ifndef YARE_YARE_DAWA_YAREYAREDAWARUNTIME_H
#define YARE_YARE_DAWA_YAREYAREDAWARUNTIME_H


#include <string_view>
#include <iostream>

class YareYareDawaRuntime {
private:
    bool errorState = false;
public:
    /*=== error handling ===*/
    [[nodiscard]] bool isErrorState() const {
        return errorState;
    }

    void report(int line, std::string_view where,
                std::string_view message) {
        std::cerr << "[line " << line << "] Error" << where << ": " << message << "\n";
        errorState = true;
    }

    void error(int line, std::string_view message) {
        report(line, "", message);
    }

    void runFile(std::string_view filePath);

    void runPrompt();

private:
    bool run(std::string_view source);
};


#endif //YARE_YARE_DAWA_YAREYAREDAWARUNTIME_H
