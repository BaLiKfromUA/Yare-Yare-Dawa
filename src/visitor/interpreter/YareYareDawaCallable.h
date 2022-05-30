#ifndef YARE_YARE_DAWA_YAREYAREDAWACALLABLE_H
#define YARE_YARE_DAWA_YAREYAREDAWACALLABLE_H

#pragma once

#include <any>
#include <string>
#include <vector>

namespace visitor {

    class Interpreter;

    class YareYareDawaCallable {
    public:
        virtual int arity() = 0;

        virtual std::any call(Interpreter &interpreter,
                              std::vector<std::any> arguments) = 0;

        virtual std::string toString() = 0;

        virtual ~YareYareDawaCallable() = default;
    };

}

#endif //YARE_YARE_DAWA_YAREYAREDAWACALLABLE_H
