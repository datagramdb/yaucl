//
// Created by giacomo on 03/01/2022.
//

#ifndef KNOBAB_OPTIONAL_BINARY_H
#define KNOBAB_OPTIONAL_BINARY_H

#include <variant>
#include <yaucl/structures/default_constructors.h>

namespace yaucl {
    namespace functional {
        template <typename T>
        class optional_binary {
            std::variant<T, bool> value;

            optional_binary(bool isValid) : value{isValid} {}
        public:
            optional_binary(T val) : value{val} {}
            optional_binary() : optional_binary(true) {}

            DEFAULT_COPY_ASSGN(optional_binary);
            static optional_binary<T> validButMissing() {
                return {true};
            }
            static optional_binary<T> invalid() {
                return {false};
            }
            T get() const {
                return std::get<T>(value);
            }
            bool isValid() {
                return std::holds_alternative<T>(value) || std::get<bool>(value);
            }
            bool isValidButMissing() {
                return (!std::holds_alternative<T>(value)) && std::get<bool>(value);
            }
            bool isValidButPresent() {
                return std::holds_alternative<T>(value);
            }
            bool isInvalid() {
                return std::holds_alternative<bool>(value) && (!std::get<bool>(value));
            }
        };
    }
}



#endif //KNOBAB_OPTIONAL_BINARY_H
