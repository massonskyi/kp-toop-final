#ifndef VARIABLE_H
#define VARIABLE_H

#include "core.h"
#include "abstractsimpletype.h"

#include <stdexcept>
#include <variant>
#include <sstream>
#include <cmath>
#include <limits>
#include <string>
#include <bitset>
#include <utility>
#include <iostream>

#ifdef __GNUG__
#include <cxxabi.h>
#endif

/// @brief  This class is used to create a variable with a value
/// @tparam T type of the value
class Variable final : public AbstractSimpleType
{
public:
    using AbstractSimpleType::AbstractSimpleType;
    using value_type = std::variant<int, float, double>;

    /// @brief Constructor without parameters
    Variable()
        : AbstractSimpleType(extractClassName()), value_(0), type_(Type::INT) {}

    /// @brief  Constructor with parameter
    /// @details This constructor is used to create a variable with a value
    /// @tparam T type of the value
    /// @param value
    explicit Variable(int value)
        : AbstractSimpleType(extractClassName()), value_(value), type_(Type::INT) {}

    /// @brief  Constructor with parameter
    /// @details This constructor is used to create a variable with a value
    /// @tparam T type of the value
    /// @param value
    explicit Variable(float value)
        : AbstractSimpleType(extractClassName()), value_(value), type_(Type::FLOAT) {}

    /// @brief  Constructor with parameter
    /// @details This constructor is used to create a variable with a value
    /// @tparam T type of the value
    /// @param value
    explicit Variable(double value)
        : AbstractSimpleType(extractClassName()), value_(value), type_(Type::DOUBLE) {}

    /// @brief Copy constructor
    /// @param other Variable to copy
    Variable(const Variable &other) noexcept
        : AbstractSimpleType(extractClassName()), value_(other.value_), type_(other.type_) {}

    /// @brief Move constructor
    /// @param other Variable to move
    Variable(Variable &&other) noexcept
        : AbstractSimpleType(extractClassName()),
          value_(std::move(other.value_)),
          type_(std::move(other.type_)) {}

    /// @brief Destructor
    /// @details Destructor of the class variable
    ~Variable() override {
        std::cout << "Destructor called for Variable\n";
    }

    /// @brief This method returns the value of the object as a string hex
    /// @return The value of the object as a string hex
    inline std::string hex() const override
    {
        try
        {
            std::ostringstream stream;
            switch (type_)
            {
            case Type::INT:
                stream << "0x" << std::uppercase << std::hex
                       << std::get<int>(value_);
                break;
            case Type::FLOAT:
                stream << std::hexfloat << std::get<float>(value_);
                break;
            case Type::DOUBLE:
                stream << std::hexfloat << std::get<double>(value_);
                break;
            default:
                throw std::invalid_argument("Unsupported numeric type");
            }
            return stream.str();
        }
        catch (const std::exception &e)
        {
            throw std::runtime_error("Error converting value to hexadecimal string: " +
                                     std::string(e.what()));
        }
    }

    /// @brief This method returns the value of the object as a string dec
    /// @return The value of the object as a string dec
    inline std::string dec() const override
    {
        try
        {
            std::ostringstream stream;
            switch (type_)
            {
            case Type::INT:
                return std::to_string(std::get<int>(value_));
            case Type::FLOAT:
                stream << std::fixed << std::get<float>(value_);
                break;
            case Type::DOUBLE:
                stream << std::fixed << std::get<double>(value_);
                break;
            default:
                throw std::invalid_argument("Unsupported numeric type");
            }
            return stream.str();
        }
        catch (const std::exception &e)
        {
            throw std::runtime_error("Error converting value to decimal string: " +
                                     std::string(e.what()));
        }
    }

    /// @brief This method returns the value of the object as a string bin
    /// @return The value of the object as a string bin
    inline std::string bin() const override
    {
        try
        {
            if (type_ != Type::INT)
            {
                throw std::invalid_argument("Binary representation supported only for integer types");
            }

            constexpr size_t BITS = sizeof(int) * 8;
            return std::bitset<BITS>(std::get<int>(value_)).to_string();
        }
        catch (const std::exception &e)
        {
            throw std::runtime_error("Error converting value to binary string: " +
                                     std::string(e.what()));
        }
    }

    /// @brief This method returns the value of the object as a string oct
    /// @return The value of the object as a string oct
    inline std::string oct() const override
    {
        try
        {
            if (type_ != Type::INT)
            {
                throw std::invalid_argument("Octal representation supported only for integer types");
            }

            std::ostringstream stream;
            stream << '0' << std::oct << std::get<int>(value_);
            return stream.str();
        }
        catch (const std::exception &e)
        {
            throw std::runtime_error("Error converting value to octal string: " +
                                     std::string(e.what()));
        }
    }

    /// @brief  This method is used to print the object
    /// @return The string representation of the object
    inline std::string toString() const override
    {
        {
            try
            {
                if (value_.valueless_by_exception())
                {
                    return "Error: variant is empty";
                }

                std::ostringstream ss;

                // Определяем тип и возвращаем строковое представление значения
                switch (type_)
                {
                case Type::INT:
                    ss << std::get<int>(value_);
                    break;
                case Type::FLOAT:
                    ss << std::get<float>(value_);
                    break;
                case Type::DOUBLE:
                    ss << std::get<double>(value_);
                    break;
                default:
                    return "Error: unsupported type";
                }

                return ss.str();
            }
            catch (const std::exception &e)
            {
                return "Error: " + std::string(e.what());
            }
        }
    }

    /// @brief  This method is used to get the type of the object
    /// @return The type of the object
    inline std::string getTypeName() const override
    {
        return std::visit([](const auto &arg) -> std::string
                          {
        using U = std::remove_cvref_t<decltype(arg)>;
            std::string type_name = typeid(U).name();
#ifdef __GNUG__
            int status;
            char* demangled = abi::__cxa_demangle(type_name.c_str(), nullptr, nullptr, &status);
            if (demangled) {
                type_name = demangled;
                free(demangled);
            }
#else
            // Получаем имя типа
            std::string type_name = typeid(U).name();
            
            // Удаляем "class " и "struct " из начала имени (специфично для MSVC)
            if (type_name.substr(0, 6) == "class ") {
                type_name = type_name.substr(6);
            }
            if (type_name.substr(0, 7) == "struct ") {
                type_name = type_name.substr(7);
            }
#endif
            return type_name; }, value_);
    }

    /// @brief This method is used to get the type of the object
    /// @return The type of the object
    inline Type getType() const
    {
        return type_;
    }

    /// @brief This method returns the value of the object as a boolean
    /// @return The value of the object as a boolean
    inline bool equals(const AbstractSimpleType &other) const override
    {
        const Variable *otherVariable = dynamic_cast<const Variable *>(&other);
        if (otherVariable)
        {
            return value_ == otherVariable->value_;
        }
        return false;
    }

    /// @brief This method returns the value of the object as a boolean
    /// @return The value of the object as a boolean
    inline bool equals(const Variable &other) const
    {
        try
        {
            return value_ == other.value_;
        }
        catch (const std::exception &e)
        {
            throw std::runtime_error("Error comparing values: " +
                                     std::string(e.what()));
        }
    }

    /// @brief This method returns the value of the object as a boolean
    template <AllowedTypes _Tp>
    inline bool equals(const _Tp &other) const
    {
        try
        {
            return std::get<_Tp>(value_) == other;
        }
        catch (const std::exception &e)
        {
            throw std::runtime_error("Error comparing values: " + std::string(e.what()));
        }
    }

    /// @brief  This method clones the object value of the object
    /// @return The cloned object
    inline std::unique_ptr<AbstractSimpleType> clone() const override
    {
        return std::make_unique<Variable>(*this);
    }

    /// @brief This method serializes the object value of the object
    /// @return The serialized object
    [[nodiscard]]
    inline std::string serialize() const override
    {
        return std::visit([](auto &&arg) -> std::string
                          {
        using U = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<U, std::string>) {
            return arg;
        } else {
            return std::to_string(arg);
        } }, value_);
    }

    /// @brief  This method deserializes the object value of the object
    /// @param value The serialized object to deserialize
    inline void deserialize(const std::string &data) override
    {
        try
        {
            switch (type_)
            {
            case Type::INT:
                value_ = std::stoi(data);
                break;
            case Type::FLOAT:
                value_ = std::stof(data);
                break;
            case Type::DOUBLE:
                value_ = std::stod(data);
                break;
            default:
                throw std::invalid_argument("Unsupported type for deserialization");
            }
        }
        catch (const std::exception &e)
        {
            throw std::runtime_error("Error during deserialization: " + std::string(e.what()));
        }
    }
    /// @brief This method sets the value of the object
    /// @tparam _Tp The type of the object
    /// @param other The value of the object
    template <AllowedTypes _Tp>
    void setValue(const _Tp &other)
    {
        // Проверяем, поддерживается ли тип
        if constexpr (std::is_same_v<_Tp, int> || std::is_same_v<_Tp, float> || std::is_same_v<_Tp, double>)
        {
            value_ = std::move(other); // Устанавливаем значение в std::variant
        }
        else
        {
            throw std::invalid_argument("Unsupported type assignment");
        }
    }
    /// @brief This method sets the value of the object
    /// @param value The value of the object
    inline void set(const std::string &value) override
    {
        {
            try
            {
                switch (type_)
                {
                case Type::INT:
                    value_ = std::stoi(value);
                    break;
                case Type::FLOAT:
                    value_ = std::stof(value);
                    break;
                case Type::DOUBLE:
                    value_ = std::stod(value);
                    break;
                default:
                    throw std::invalid_argument("Unsupported type for setting value");
                }
            }
            catch (const std::exception &e)
            {
                throw std::runtime_error("Error setting value: " + std::string(e.what()));
            }
        }
    }

    /// @brief This method sets the value of the object
    /// @param value The value of the object
    template <AllowedTypes _Tp>
    inline void set(const _Tp &value)
    {
        try
        {
            // Проверяем, является ли тип допустимым
            if constexpr (std::is_arithmetic<_Tp>::value)
            {
                value_ = value;
            }
            else
            {
                throw std::invalid_argument("Unsupported type");
            }
        }
        catch (const std::exception &e)
        {
            throw std::runtime_error("Error setting value: " + std::string(e.what()));
        }
    }

    /// @brief This method returns the value of the object
    /// @return The value of the object
    template <AllowedTypes _Tp>
    inline _Tp getValue() const
    {
        const _Tp *valuePtr = std::get_if<_Tp>(&value_);
        if (!valuePtr)
        {
            throw std::runtime_error("Type mismatch: Unable to get value");
        }
        return *valuePtr;
    }

    /// @brief This method returns the value of the object
    /// @return The value of the object
    inline AbstractSimpleType get() const override
    {
        try
        {
            return Variable(*this);
        }
        catch (const std::exception &e)
        {
            throw std::runtime_error("Error getting value: " + std::string(e.what()));
        }
    }

    /// @brief This method returns the value of the object
    /// @param other Variable to copy compare
    /// @return The value of the object
    Variable &operator=(const Variable &other)
    {
        try
        {
            value_ = other.value_;
            type_ = other.type_;
            return *this;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error assigning value: " + std::string(e.what()) << std::endl;
        }
    }

    /// @brief This method returns the value of the object
    /// @param other Variable to move compare
    /// @return The value of the object
    Variable &operator=(Variable &&other) noexcept
    {
        {
            try
            {
                value_ = std::move(other.value_);
                type_ = other.type_;
                return *this;
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error assigning value: " + std::string(e.what()) << std::endl;
            }
        }
    }

    /// @brief  This method returns the value of the object
    /// @param value int to copy compare
    /// @return The value of the object
    template <AllowedTypes _Tp>
    inline Variable &operator=(const _Tp &other)
    {
        try
        {
            // setValue(other); // Метод для установки значения
            value_ = other;
            return *this;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error assigning value: " + std::string(e.what()) << std::endl;
            throw;
        }
    }

    /// @brief This method returns the value of the object as a boolean
    /// @param other Variable<T> to compare
    /// @return The value of the object as a boolean
    bool operator==(const Variable &other) const
    {
        try
        {
            return value_ == other.value_ && type_ == other.type_;
        }
        catch (const std::exception &e)
        {
            throw std::runtime_error("Error comparing values: " + std::string(e.what()));
        }
    }
    bool operator==(Variable &&other) noexcept
    {
        try
        {
            return value_ == other.value_ && type_ == other.type_;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error comparing values: " + std::string(e.what());
        }
    }
    /// @brief This method returns the value of the object as a boolean
    /// @param value Variable<T> to compare
    /// @return The value of the object as a boolean
    template <AllowedTypes _Tp>
    bool operator==(const _Tp &other) const
    {
        static_assert(std::is_same_v<_Tp, int> || std::is_same_v<_Tp, float> || std::is_same_v<_Tp, double>,
                      "Type must be int, float, or double");

        try
        {
            // Сравнение типа и значения
            return type_ == getTypeFromValue(other) && value_ == other;
        }
        catch (const std::exception &e)
        {
            throw std::runtime_error("Error comparing values: " + std::string(e.what()));
        }
    }

    /// @brief This method returns the value of the object as a boolean
    /// @param other Variable<T> to compare
    /// @return The value of the object as a boolean
    bool operator!=(const Variable &other) const
    {
        try
        {
            return value_ != other.value_ || type_ != other.type_;
        }
        catch (const std::exception &e)
        {
            throw std::runtime_error("Error comparing values: " + std::string(e.what()));
        }
    }

    /// @brief This method returns the value of the object as a boolean
    /// @param value Variable<T> to compare
    /// @return The value of the object as a boolean
    template <AllowedTypes _Tp>
    bool operator!=(const _Tp &value) const
    {
        static_assert(std::is_same_v<_Tp, int> || std::is_same_v<_Tp, float> || std::is_same_v<_Tp, double>,
                      "Type must be int, float, or double");
        try
        {
            return !(value == this);
        }
        catch (const std::exception &e)
        {
            throw std::runtime_error("Error comparing values: " + std::string(e.what()));
        }
    }

    /// @brief This method returns result of the add operation
    /// @param other Variable<T> to add
    /// @return Result of the add operation
    Variable operator+(const Variable &other)
    {
        return binaryOperation(other, std::plus<>());
    }

    /// @brief This method returns result of the add operation
    /// @param other Variable<T> to add
    /// @return Result of the add operation
    Variable operator+(const Variable &other) const
    {
        return binaryOperation(other, std::plus<>());
    }
    /// @brief This method returns result of the sub operation
    /// @param other Variable<T> to substract
    /// @return Result of the sub operation
    Variable operator-(const Variable &other)
    {
        return binaryOperation(other, std::minus<>());
    }

    /// @brief This method returns result of the mul operation
    /// @param other Variable<T> to multiply
    /// @return Result of the mul operation
    Variable operator*(const Variable &other)
    {
        return binaryOperation(other, std::multiplies<>());
    }

    /// @brief This method returns result of the div operation
    /// @param other Variable<T> to divide
    /// @return Result of the div operation
    Variable operator/(const Variable &other)
    {
        if (isZero(other.value_))
        {
            throw std::runtime_error("Division by zero");
        }
        return binaryOperation(other, std::divides<>());
    }

    /// @brief This method returns result of the mod operation
    /// @param other Variable<T> to divide
    /// @return Result of the mod operation
    Variable operator%(const Variable &other)
    {
        // Проверка, что оба операнда имеют целочисленные типы
        if constexpr (std::is_integral_v<decltype(value_)> && std::is_integral_v<decltype(other.value_)>)
        {
            if (isZero(other.value_))
            {
                throw std::runtime_error("Modulo by zero");
            }
            return binaryOperation(other, std::modulus<>());
        }
        else
        {
            throw std::runtime_error("Modulo operation is only supported for integral types");
        }
    }

    /// @brief This method returns result of the add operation
    /// @param other Variable<T> to add
    /// @return Result of the add operation
    template <AllowedTypes _Tp>
    Variable operator+(const _Tp &other)
    {
        return binaryOperation(Variable(other), std::plus<>());
    }

    /// @brief This method returns result of the sub operation
    /// @param other Variable<T> to substract
    /// @return Result of the sub operation
    template <AllowedTypes _Tp>
    Variable operator-(const _Tp &other)
    {
        return binaryOperation(Variable(other), std::minus<>());
    }

    /// @brief This method returns result of the mul operation
    /// @param other Variable<T> to multiply
    /// @return Result of the mul operation
    template <AllowedTypes _Tp>
    Variable operator*(const _Tp &other)
    {
        return binaryOperation(Variable(other), std::multiplies<>());
    }

    /// @brief This method returns result of the div operation
    /// @param other Variable<T> to divide
    /// @return Result of the div operation
    template <AllowedTypes _Tp>
    Variable operator/(const _Tp &other)
    {
        if (isZero(other))
        {
            throw std::runtime_error("Division by zero");
        }
        return binaryOperation(Variable(other), std::divides<>());
    }

    /// @brief This method returns result of the mod operation
    /// @param other Variable<T> to divide
    /// @return Result of the mod operation
    template <AllowedTypes _Tp>
    Variable operator%(const _Tp &other)
    {
        // Проверка, что оба типа являются целочисленными
        if constexpr (std::is_integral_v<_Tp> && std::is_integral_v<decltype(this->value_)>)
        {
            if (isZero(other))
            {
                throw std::runtime_error("Modulo by zero");
            }
            return binaryOperation(Variable(other), std::modulus<>());
        }
        else
        {
            throw std::runtime_error("Modulo operation is only supported for integral types");
        }
    }

    /// @brief This method returns the value of the object
    /// @param other Variable<T> to add
    /// @return The value of the object
    Variable &operator+=(const Variable &other)
    {
        try
        {
            *this = binaryOperation(other, std::plus<>());
        }
        catch (std::runtime_error &e)
        {
            std::cerr
                << "Addition is only supported for arithmetic types" << std::endl;
        }
        return *this;
    }

    /// @brief This method returns the value of the object
    /// @param other Variable<T> to substract
    /// @return The value of the object
    Variable &
    operator-=(const Variable &other)
    {
        if constexpr (std::is_arithmetic_v<decltype(this->value_)> && std::is_arithmetic_v<decltype(other.value_)>)
        {
            *this = binaryOperation(other, std::minus<>());
        }
        else
        {
            throw std::runtime_error("Subtraction is only supported for arithmetic types");
        }
        return *this;
    }

    /// @brief This method returns the value of the object
    /// @param other Variable<T> to multiply
    /// @return The value of the object
    Variable &operator*=(const Variable &other)
    {
        if constexpr (std::is_arithmetic_v<decltype(this->value_)> && std::is_arithmetic_v<decltype(other.value_)>)
        {
            *this = binaryOperation(other, std::multiplies<>());
        }
        else
        {
            throw std::runtime_error("Multiplication is only supported for arithmetic types");
        }
        return *this;
    }

    /// @brief This method returns the value of the object
    /// @param other Variable<T> to divide
    /// @return The value of the object
    Variable &operator/=(const Variable &other)
    {
        if constexpr (std::is_arithmetic_v<decltype(this->value_)> && std::is_arithmetic_v<decltype(other.value_)>)
        {
            if (isZero(other.value_))
            {
                throw std::runtime_error("Division by zero");
            }
            *this = binaryOperation(other, std::divides<>());
        }
        else
        {
            throw std::runtime_error("Division is only supported for arithmetic types");
        }
        return *this;
    }

    /// @brief This method returns the value of the object
    /// @param other Variable<T> to divide
    /// @return The value of the object
    Variable &operator%=(const Variable &other)
    {
        if constexpr (std::is_integral_v<decltype(this->value_)> && std::is_integral_v<decltype(other.value_)>)
        {
            if (isZero(other.value_))
            {
                throw std::runtime_error("Modulo by zero");
            }
            *this = binaryOperation(other, std::modulus<>());
        }
        else
        {
            throw std::runtime_error("Modulo operation is only supported for integral types");
        }
        return *this;
    }

    /// @brief This method returns the value of the object
    /// @param other Variable<T> to add
    /// @return The value of the object
    template <AllowedTypes _Tp>
    Variable &operator+=(const _Tp &other)
    {
        if constexpr (std::is_arithmetic_v<_Tp>)
        {
            *(this) = std::get<_Tp>(value_) + other; // Сложение, если _Tp поддерживает арифметику
        }
        else
        {
            throw std::runtime_error("Addition is only supported for arithmetic types");
        }
        return *this;
    }

    /// @brief This method returns the value of the object
    /// @param other Variable<T> to substract
    /// @return The value of the object
    template <AllowedTypes _Tp>
    Variable &operator-=(const _Tp &other)
    {
        if constexpr (std::is_arithmetic_v<_Tp>)
        {
            *(this) = std::get<_Tp>(value_) - other; // Прямое вычитание, если _Tp поддерживает арифметику
        }
        else
        {
            throw std::runtime_error("Subtraction is only supported for arithmetic types");
        }
        return *this;
    }

    /// @brief This method returns the value of the object
    /// @param other Variable<T> to multiply
    /// @return The value of the object
    template <AllowedTypes _Tp>
    Variable &operator*=(const _Tp &other)
    {
        if constexpr (std::is_arithmetic_v<_Tp>)
        {
            *(this) = std::get<_Tp>(value_) * other; // Прямое умножение, если _Tp поддерживает арифметику
        }
        else
        {
            throw std::runtime_error("Multiplication is only supported for arithmetic types");
        }
        return *this;
    }

    /// @brief This method returns the value of the object
    /// @param other Variable<T> to divide
    /// @return The value of the object
    template <AllowedTypes _Tp>
    Variable &operator/=(const _Tp &other)
    {
        if constexpr (std::is_arithmetic_v<_Tp>)
        {
            if (isZero(other))
            {
                throw std::runtime_error("Division by zero");
            }
            *(this) = std::get<_Tp>(value_) / other; // Прямое деление, если _Tp поддерживает арифметику
        }
        else
        {
            throw std::runtime_error("Division is only supported for arithmetic types");
        }
        return *this;
    }

    /// @brief This method returns the value of the object
    /// @param other Variable<T> to divide
    /// @return The value of the object
    template <AllowedTypes _Tp>
    Variable &operator%=(const _Tp &other)
    {
        if constexpr (std::is_integral_v<_Tp>)
        {
            if (isZero(other))
            {
                throw std::runtime_error("Modulo by zero");
            }
            *(this) = std::get<_Tp>(value_) % other; // Прямое взятие остатка, если _Tp поддерживает целочисленные типы
        }
        else
        {
            throw std::runtime_error("Modulo operation is only supported for integral types");
        }
        return *this;
    }

    /// @brief This method returns the value of the object
    /// @return  The value of the object
    Variable &operator++()
    {
        std::visit([](auto &&arg)
                   {
                       ++arg; // Инкрементируем значение типа, с которым работаем
                   },
                   value_);
        return *this;
    }

    /// @brief This method returns the value of the object
    /// @return The value of the object
    Variable &operator--()
    {
        std::visit([](auto &&arg)
                   {
                       --arg; // Декрементируем значение типа, с которым работаем
                   },
                   value_);
        return *this;
    }

    /// @brief This method returns the value of the object
    /// @param  this the value of the object
    /// @return The value of the object
    Variable operator++(int)
    {
        Variable temp = *this; // Сохраняем текущую копию объекта
        ++(*this);             // Выполняем префиксный инкремент
        return temp;           // Возвращаем старое значение
    }

    /// @brief This method returns the value of the object
    /// @param  this the value of the object
    /// @return The value of the object
    Variable operator--(int)
    {
        Variable temp = *this; // Сохраняем текущую копию объекта
        --(*this);             // Выполняем префиксный декремент
        return temp;           // Возвращаем старое значение
    }

    /// @brief This friend method of operator << overloading
    /// @param out output stream
    /// @param Variable<T> the value of the object
    /// @return output stream
    friend std::ostream &operator<<(std::ostream &out, const Variable &var)
    {
        try
        {
            if (!out)
            {
                return out; // Return the bad stream instead of throwing
            }

            out << var.toString();

            return out;
        }
        catch (const std::ios_base::failure &e)
        {
            throw std::runtime_error("I/O error in operator<<: " + std::string(e.what()));
        }
        catch (const std::exception &e)
        {
            out.setstate(std::ios::failbit);
            throw std::runtime_error("Unexpected error in operator<<: " + std::string(e.what()));
        }
        catch (...)
        {
            out.setstate(std::ios::failbit);
            throw std::runtime_error("Unknown error in operator<<");
        }
    }

    /// @brief This friend method of operator >> overloading
    /// @param in input stream
    /// @param Variable<T> the value of the object
    /// @return input stream
    friend std::istream &operator>>(std::istream &in, Variable &var)
    {
        try
        {
            // Проверка состояния потока перед операцией
            if (!in)
            {
                throw std::ios_base::failure("Input stream is in bad state");
            }

            // Очищаем возможные ошибки потока
            in.clear();

            // Пропускаем начальные пробелы
            in >> std::ws;

            // Создаем временную переменную для безопасного чтения
            Variable temp;
            if (!(in >> temp))
            {
                throw std::ios_base::failure("Failed to read value");
            }

            // Присваиваем значение только если чтение прошло успешно
            var = std::move(temp);

            return in;
        }
        catch (const std::ios_base::failure &e)
        {
            throw std::runtime_error("I/O error in operator>>: " + std::string(e.what()));
        }
        catch (const std::exception &e)
        {
            throw std::runtime_error("Unexpected error in operator>>: " + std::string(e.what()));
        }
    };

    /// @brief This friend method of operator == overloading
    /// @param value the value of the object
    /// @param var the value of the object
    /// @return true if the values are equal, false otherwise
    template <AllowedTypes _Tp>
    friend bool operator==(const _Tp value, const Variable &var)
    {
        try
        {
            return value ==
                   std::get<_Tp>(var.value_);
        }
        catch (const std::exception &e)
        {
            throw std::runtime_error("Error in operator==: " + std::string(e.what()));
        }
    };

    /// @brief This friend method of operator != overloading
    /// @param value the value of the object
    /// @param var the value of the object
    /// @return true if the values are not equal, false otherwise
    template <AllowedTypes _Tp>
    friend bool operator!=(const _Tp value, const Variable &var)
    {
        try
        {
            return value !=
                   std::get<_Tp>(var.value_);
        }
        catch (const std::exception &e)
        {
            throw std::runtime_error("Error in operator!=: " + std::string(e.what()));
        }
    }

    /// @brief This friend method of operator + overloading
    /// @param value the value of the object
    /// @param var the value of the object
    /// @return the value of the object
    template <AllowedTypes _Tp>
    friend Variable operator+(const _Tp value, const Variable &var)
    {
        try
        {
            return Variable(value + std::get<_Tp>(var.value_));
        }
        catch (const std::exception &e)
        {
            throw std::runtime_error("Error in operator+: " + std::string(e.what()));
        }
    };

    /// @brief This friend method of operator - overloading
    /// @param value the value of the object
    /// @param var the value of the object
    /// @return the value of the object
    template <AllowedTypes _Tp>
    friend Variable operator-(const _Tp value, const Variable &var)
    {
        try
        {
            return Variable(value - std::get<_Tp>(var.value_));
        }
        catch (const std::exception &e)
        {
            throw std::runtime_error("Error in operator-: " + std::string(e.what()));
        }
    };

    /// @brief This friend method of operator * overloading
    /// @param value the value of the object
    /// @param var the value of the object
    /// @return the value of the object
    template <AllowedTypes _Tp>
    friend Variable operator*(const _Tp value, const Variable &var)
    {
        try
        {
            return Variable(value * std::get<_Tp>(var.value_));
        }
        catch (const std::exception &e)
        {
            throw std::runtime_error("Error in operator*: " + std::string(e.what()));
        }
    };

    /// @brief This friend method of operator / overloading
    /// @param value the value of the object
    /// @param var the value of the object
    /// @return the value of the object
    template <AllowedTypes _Tp>
    friend Variable operator/(const _Tp value, const Variable &var)
    {
        try
        {
            return Variable(value / std::get<_Tp>(var.value_));
        }
        catch (const std::exception &e)
        {
            throw std::runtime_error("Error in operator/: " + std::string(e.what()));
        }
    }

    /// @brief This friend method of operator % overloading
    /// @param value the value of the object
    /// @param var the value of the object
    /// @return the value of the object
    template <AllowedTypes _Tp>
    friend Variable operator%(const _Tp value, const Variable &var)
    {
        try
        {
            return Variable(value % std::get<_Tp>(var.value_));
        }
        catch (const std::exception &e)
        {
            throw std::runtime_error("Error in operator%: " + std::string(e.what()));
        }
    }
    friend Variable operator*(const Variable &lhs, const Variable &rhs)
    {
        // Убедимся, что типы значений совпадают
        if (lhs.value_.index() != rhs.value_.index())
        {
            throw std::runtime_error("Type mismatch: Cannot multiply variables of different types.");
        }

        // Используем std::visit для обработки значения внутри std::variant
        return std::visit(
            [](const auto &a, const auto &b) -> Variable
            {
                using T = std::decay_t<decltype(a)>;
                if constexpr (std::is_arithmetic_v<T>) // Проверяем, является ли тип арифметическим
                {
                    return Variable(a * b);
                }
                else
                {
                    throw std::runtime_error("Unsupported type for multiplication.");
                }
            },
            lhs.value_, rhs.value_);
    }

    friend Variable operator+(const Variable &lhs, const Variable &rhs)
    {
        if (lhs.value_.index() != rhs.value_.index())
        {
            throw std::runtime_error("Type mismatch: Cannot add variables of different types.");
        }

        return std::visit(
            [](const auto &a, const auto &b) -> Variable
            {
                using T = std::decay_t<decltype(a)>;
                if constexpr (std::is_arithmetic_v<T>)
                {
                    return Variable(a + b);
                }
                else
                {
                    throw std::runtime_error("Unsupported type for addition.");
                }
            },
            lhs.value_, rhs.value_);
    }

    friend Variable operator-(const Variable &lhs, const Variable &rhs)
    {
        if (lhs.value_.index() != rhs.value_.index())
        {
            throw std::runtime_error("Type mismatch: Cannot subtract variables of different types.");
        }

        return std::visit(
            [](const auto &a, const auto &b) -> Variable
            {
                using T = std::decay_t<decltype(a)>;
                if constexpr (std::is_arithmetic_v<T>)
                {
                    return Variable(a - b);
                }
                else
                {
                    throw std::runtime_error("Unsupported type for subtraction.");
                }
            },
            lhs.value_, rhs.value_);
    }

    friend Variable operator/(const Variable &lhs, const Variable &rhs)
    {
        if (lhs.value_.index() != rhs.value_.index())
        {
            throw std::runtime_error("Type mismatch: Cannot divide variables of different types.");
        }

        return std::visit(
            [](const auto &a, const auto &b) -> Variable
            {
                using T = std::decay_t<decltype(a)>;
                if constexpr (std::is_arithmetic_v<T>)
                {
                    if (b == 0)
                    {
                        throw std::runtime_error("Division by zero.");
                    }
                    return Variable(a / b);
                }
                else
                {
                    throw std::runtime_error("Unsupported type for division.");
                }
            },
            lhs.value_, rhs.value_);
    }

    friend Variable operator%(const Variable &lhs, const Variable &rhs)
    {
        if (lhs.value_.index() != rhs.value_.index())
        {
            throw std::runtime_error("Type mismatch: Cannot compute modulo for variables of different types.");
        }
        if constexpr (std::is_integral_v<std::decay_t<decltype(lhs)>> && std::is_integral_v<std::decay_t<decltype(rhs)>>)

            return std::visit(
                [](const auto &a, const auto &b) -> Variable
                {
                    using T = std::decay_t<decltype(a)>;
                    if constexpr (std::is_integral_v<T>)
                    {
                        if (b == 0)
                        {
                            throw std::runtime_error("Modulo by zero.");
                        }
                        return Variable(a % b);
                    }
                    else
                    {
                        throw std::runtime_error("Unsupported type for modulo. Only integers are supported.");
                    }
                },
                lhs.value_, rhs.value_);
        else
        {
            throw std::runtime_error("Division is only supported for arithmetic types");
        }
    }

private:
    value_type value_;
    Type type_;

    /// @brief This method returns type of the value
    /// @return type of the value
    static constexpr Type determineType(const value_type &val)
    {
        if (std::holds_alternative<int>(val))
            return Type::INT;
        if (std::holds_alternative<float>(val))
            return Type::FLOAT;
        if (std::holds_alternative<double>(val))
            return Type::DOUBLE;
        throw std::runtime_error("Unknown type");
    }

    /// @brief This method returns class name
    /// @return class name
    static std::string extractClassName()
    {
#ifdef _MSC_VER
        const char *funcSig = __FUNCSIG__;
#else
        const char *funcSig = __PRETTY_FUNCTION__;
#endif
        // Extract the class name from the function signature string
        std::string funcSigStr(funcSig);
        size_t start = funcSigStr.find("Variabl");
        size_t end = funcSigStr.find("e", start);
        if (start != std::string::npos && end != std::string::npos)
        {
            return funcSigStr.substr(start, end - (start - 1));
        }
        return "UnknownClass";
    }

    /// @brief This method execute binary operation
    /// @tparam Op binary operation
    /// @param other the value of the object
    /// @param operation binary operation
    /// @return the value of the object
    template <typename Op>
    Variable binaryOperation(const Variable &other, Op operation) const
    {
        // Проверка совместимости типов перед операцией
        if (type_ != other.type_)
        {
            throw std::runtime_error("Type mismatch in binary operation");
        }

        try
        {
            return std::visit([&](const auto &a, const auto &b)
                              {
                // Определяем тип результата
                using A = std::decay_t<decltype(a)>;
                using B = std::decay_t<decltype(b)>;
                using ResultType = decltype(operation(a, b));

                // Проверка на переполнение для целых чисел
                if constexpr (std::is_integral_v<A> && std::is_integral_v<B>) {
                    if constexpr (std::is_same_v<Op, std::plus<>> || 
                                std::is_same_v<Op, std::multiplies<>>) {
                        // Для сложения
                        if constexpr (std::is_same_v<Op, std::plus<>>) {
                            if (a > 0 && b > std::numeric_limits<ResultType>::max() - a) {
                                throw std::overflow_error("Integer overflow in addition");
                            }
                            if (a < 0 && b < std::numeric_limits<ResultType>::min() - a) {
                                throw std::overflow_error("Integer underflow in addition");
                            }
                        }
                        // Для умножения
                        else if constexpr (std::is_same_v<Op, std::multiplies<>>) {
                            if (a != 0 && b > std::numeric_limits<ResultType>::max() / std::abs(a)) {
                                throw std::overflow_error("Integer overflow in multiplication");
                            }
                        }
                    }
                }

                // Проверка на NaN и Inf для чисел с плавающей точкой
                if constexpr (std::is_floating_point_v<A> && std::is_floating_point_v<B>) {
                    auto result = operation(a, b);
                    if (std::isnan(result)) {
                        throw std::domain_error("Operation resulted in NaN");
                    }
                    if (std::isinf(result)) {
                        throw std::overflow_error("Operation resulted in Infinity");
                    }
                    return Variable(result);
                }

                // Выполнение операции с проверкой исключений
                auto result = operation(a, b);

                // Дополнительные проверки результата
                if constexpr (std::is_arithmetic_v<ResultType>) {
                    if constexpr (std::is_floating_point_v<ResultType>) {
                        if (!std::isfinite(result)) {
                            throw std::overflow_error("Operation resulted in non-finite value");
                        }
                    }
                }

                return Variable(static_cast<ResultType>(result)); }, value_, other.value_);
        }
        catch (const std::exception &e)
        {
            throw std::runtime_error(std::string("Binary operation failed: ") + e.what());
        }
    }

    static bool isZero(const value_type &val)
    {
        return std::visit([](const auto &v)
                          { return v == 0; }, val);
    }



    template <typename T>
    static Type getTypeFromValue(const T &)
    {
        if constexpr (std::is_same_v<T, int>)
        {
            return Type::INT;
        }
        else if constexpr (std::is_same_v<T, float>)
        {
            return Type::FLOAT;
        }
        else if constexpr (std::is_same_v<T, double>)
        {
            return Type::DOUBLE;
        }
        else
        {
            throw std::invalid_argument("Unsupported type for Variable");
        }
    }
};
#endif // VARIABLE_H