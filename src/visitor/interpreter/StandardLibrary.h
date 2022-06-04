//
// Created by balik on 05.06.22.
//

#ifndef YARE_YARE_DAWA_STANDARDLIBRARY_H
#define YARE_YARE_DAWA_STANDARDLIBRARY_H

#include <chrono>
#include "YareYareDawaCallable.h"

namespace visitor {
    class NativeClock final : public YareYareDawaCallable {
    public:
        int arity() override { return 0; }

        std::any call(Interpreter &interpreter,
                      std::vector<std::any> arguments) override {
            auto ticks = std::chrono::system_clock::now().time_since_epoch();
            return std::chrono::duration<double>{ticks}.count() / 1000.0;
        }

        std::string toString() override { return "<native fn>"; }
    };

    class StrLen final : public YareYareDawaCallable {
    public:
        int arity() override {
            return 1;
        }

        std::any call(Interpreter &interpreter, std::vector<std::any> arguments) override {
            auto arg = arguments[0];
            if (arg.type() != typeid(std::string)) {
                throw std::runtime_error("Invalid argument for len() function! Requires string");
            }

            return static_cast<double >(std::any_cast<std::string>(arg).size());
        }

        std::string toString() override {
            return "<native fn>";
        }
    };

}

#endif //YARE_YARE_DAWA_STANDARDLIBRARY_H
