#include "OperatorInterface.h"
#include "basicarith_global.h"
#include "Arithmetic/Addition.h"
#include "Arithmetic/Subtraction.h"
#include "Arithmetic/Multiplication.h"
#include "Arithmetic/Division.h"
#include "Arithmetic/NumberArith.h"
#include "Arithmetic/Exponentiation.h"
#include "Arithmetic/Matrix.h"
#include "Natural.h"
#include "ExpressionParser.h"

#include <string>
#include <iostream>
#include <QString>

extern "C" {

OperatorInterface BASICARITHSHARED_EXPORT Addition_jmodule()
{
    OperatorInterface oi;
    oi.parse = [](std::unique_ptr<CAS::AbstractArithmetic> first, std::unique_ptr<CAS::AbstractArithmetic> second) {
        return make_unique<CAS::Addition>(std::move(first), std::move(second));
    };
    return oi;
}

OperatorInterface BASICARITHSHARED_EXPORT Subtraction_jmodule()
{
    OperatorInterface oi;
    oi.parse = [](std::unique_ptr<CAS::AbstractArithmetic> first, std::unique_ptr<CAS::AbstractArithmetic> second) {
        return make_unique<CAS::Subtraction>(std::move(first), std::move(second));
    };
    return oi;
}

OperatorInterface BASICARITHSHARED_EXPORT Multiplication_jmodule()
{
    OperatorInterface oi;
    oi.parse = [](std::unique_ptr<CAS::AbstractArithmetic> first, std::unique_ptr<CAS::AbstractArithmetic> second) {
        return make_unique<CAS::Multiplication>(std::move(first), std::move(second));
    };
    return oi;
}

OperatorInterface BASICARITHSHARED_EXPORT Division_jmodule()
{
    OperatorInterface oi;
    oi.parse = [](std::unique_ptr<CAS::AbstractArithmetic> first, std::unique_ptr<CAS::AbstractArithmetic> second) {
        return make_unique<CAS::Division>(std::move(first), std::move(second));
    };
    return oi;
}


OperatorInterface BASICARITHSHARED_EXPORT Exponentiation_jmodule()
{
    OperatorInterface oi;
    oi.parse = [](std::unique_ptr<CAS::AbstractArithmetic> first, std::unique_ptr<CAS::AbstractArithmetic> second) {
        return make_unique<CAS::Exponentiation>(std::move(first), std::move(second));
    };
    return oi;
}

std::unique_ptr<CAS::AbstractArithmetic> BASICARITHSHARED_EXPORT Number_jmodule(const std::string &candidate, std::function<std::unique_ptr<CAS::AbstractArithmetic>(std::string)>)
{
    if (candidate.size() != 1 && candidate.front() == '0') return nullptr;
    else {
        if (candidate.find_first_not_of("0123456789") != std::string::npos) return nullptr;
        else return make_unique<CAS::NumberArith>(CAS::Natural(candidate));
    }
}

std::unique_ptr<CAS::AbstractArithmetic> BASICARITHSHARED_EXPORT Pi_jmodule(const std::string &candidate, std::function<std::unique_ptr<CAS::AbstractArithmetic>(std::string)>)
{
    if (candidate == "pi") return make_unique<CAS::NumberArith>(3);
    else return nullptr;
}

std::unique_ptr<CAS::AbstractArithmetic> BASICARITHSHARED_EXPORT Matrix_jmodule(const std::string &candidate, std::function<std::unique_ptr<CAS::AbstractArithmetic>(std::string)> parseFunc)
{
    if (candidate.front() != '[' || candidate.back() != ']') return nullptr;

    std::vector<std::unique_ptr<CAS::AbstractArithmetic>> result;
    if (candidate.at(1) == '[') {
        auto lastPos = candidate.cbegin();
        int level = 0;
        for (auto it = candidate.cbegin() + 1; it != candidate.cend() - 1; ++it) {
            if (*it == '(' || *it == '[' || *it == '{')  level--;
            else if (*it == ')' || *it == ']' || *it == '}') level++;
            if (level == 0) {
                result.emplace_back(parseFunc(std::string{lastPos + 1, it + 1}));
                lastPos = it;
            }
        }
    } else {
        std::vector<std::string> tokens = ExpressionParser::tokenize(std::string{candidate.cbegin() + 1, candidate.cend() - 1}, ",");
        for (const auto &token : tokens) result.emplace_back(parseFunc(token));
    }
    return make_unique<CAS::Matrix>(std::move(result));
}


}

