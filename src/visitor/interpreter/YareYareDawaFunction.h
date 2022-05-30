//
// Created by valen on 5/30/2022.
//

#ifndef YARE_YARE_DAWA_YAREYAREDAWAFUNCTION_H
#define YARE_YARE_DAWA_YAREYAREDAWAFUNCTION_H

#include <any>
#include <memory>
#include "YareYareDawaCallable.h"

namespace ast {
    class Function;
}

namespace visitor {
    template<typename T>
    class Environment;

    class YareYareDawaFunction : public YareYareDawaCallable {
        std::shared_ptr<ast::Function> declaration;
        std::shared_ptr<Environment<std::any>> closure;

    public:

        YareYareDawaFunction(std::shared_ptr<ast::Function> declaration,
                             std::shared_ptr<Environment<std::any>> closure) : closure{std::move(closure)},
                                                                               declaration{std::move(declaration)} {}

        std::string toString() override;

        int arity() override;

        std::any call(Interpreter &interpreter,
                      std::vector<std::any> arguments) override;
    };
}


#endif //YARE_YARE_DAWA_YAREYAREDAWAFUNCTION_H
