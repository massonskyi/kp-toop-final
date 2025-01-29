#ifndef MATRIXPARSER_HPP
#define MATRIXPARSER_HPP

#include "vectorparser.hxx"
#include "../types/matrix.tpp"
#include "../types/vector.tpp"
#include "../types/variable.tpp"


class MatrixParser final{
public:
    using value_type = Matrix;

    /// @brief This static method is parsing the expression and returning the parsed object class
    /// @param expr The expression to be parsed
    /// @param index The index of the expression to be parsed in the expression
    /// @return The parsed object class
    static value_type parse(const std::string& expr, size_t& index);

    /// @brief This static method is parsing the expression and searching Class in this expression
    /// @param expr The expression to be parsed
    /// @return True if Class is found in the expression, false otherwise
    static bool search(const std::string& expr);
};


#endif //MATRIXPARSER_HPP
