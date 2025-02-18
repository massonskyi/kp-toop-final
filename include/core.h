#ifndef CORE_H_
#define CORE_H_

#include <type_traits>
#include <concepts>
#include <cstdint>
#include <iostream>
#include <memory>

/* INFORMATIONS */

#define VERSION "Math interpreter. Version 0.0.1alpha"
#define ABOUT """\
Математический интерпретатор\n\
\n\
Этот математический интерпретатор предназначен для выполнения различных математических вычислений и операций. Он поддерживает основные арифметические операции, функции, и может работать с переменными и выражениями.\n\
\n\
Основные возможности:\n\
- Вычисление арифметических выражений\n\
- Поддержка математических функций (синус, косинус, логарифмы и т.д.)\n\
- Работа с переменными\n\
- Поддержка пользовательских функций\n\
\n\
Используйте команду --help для получения дополнительной информации о доступных командах и их использовании.\n\
\n\
Copyright (c) 2023 Ваше Имя или Организация. Все права защищены.\n\
"""



/// @brief This concept macro is used to check if a type is allowed.
template<typename T>
concept AllowedTypes =
    std::is_same_v<T, int>      ||
    std::is_same_v<T, float>    ||
    std::is_same_v<T, double>;

enum class Type {
    INT = 0x00,
    DOUBLE,
    FLOAT,
    OTHER,
};

enum class CustomType {
    Variable=0x10,
    RATIONAL,
    VECTOR,
    MATRIX,
    // ARRAY,
    // MAP
};



#endif /* CORE_H_ */