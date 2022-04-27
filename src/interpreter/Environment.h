//
// Created by balik on 4/27/2022.
//

#ifndef YARE_YARE_DAWA_ENVIRONMENT_H
#define YARE_YARE_DAWA_ENVIRONMENT_H

#include <any>
#include <map>
#include <memory>
#include <string>
#include <utility>    // std::move
#include "Errors.h"
#include "scanning/Token.h"

namespace interpreter {
    class Environment : public std::enable_shared_from_this<Environment> {
        std::shared_ptr<Environment> enclosing;
        std::map<std::string, std::any> values;

    public:
        Environment() : enclosing{nullptr} {}

        explicit Environment(std::shared_ptr<Environment> enclosing)
                : enclosing{std::move(enclosing)} {}

        std::any get(const scanning::Token &name) {
            auto elem = values.find(name.lexeme);
            if (elem != values.end()) {
                return elem->second;
            }

            if (enclosing != nullptr) return enclosing->get(name);

            throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
        }

        void assign(const scanning::Token &name, std::any value) {
            auto elem = values.find(name.lexeme);
            if (elem != values.end()) {
                elem->second = std::move(value);
                return;
            }

            if (enclosing != nullptr) {
                enclosing->assign(name, std::move(value));
                return;
            }

            throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
        }

        void define(const std::string &name, std::any value) {
            values[name] = std::move(value);
        }
    };
}

#endif //YARE_YARE_DAWA_ENVIRONMENT_H
