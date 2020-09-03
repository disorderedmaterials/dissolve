/*
    *** Expression Functions
    *** src/expression/functions.h
    Copyright T. Youngs 2015-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "expression/value.h"
#include <string_view>

// Forward declarations
class ExpressionFunction;

// Function pointer typedef and call #define
typedef bool (*FunctionFunction)(ExpressionFunction *c, ExpressionValue &result);
#define CALL_COMMAND(object, ptrToMember) ((object).*(ptrToMember))

// Function Data
class ExpressionFunctionData
{
    public:
    // Function keyword
    std::string_view keyword;
    // Number of arguments required by the function
    int nArguments;
    // Function argument names
    std::string_view argText;
    // Function description
    std::string_view description;
};

// Expression Functions
class ExpressionFunctions
{
    public:
    ExpressionFunctions();
    ~ExpressionFunctions();

    /*
     * Settings
     */
    private:
    // Whether to assume values used in trig functions are degrees, and need to be converted to radians
    static bool assumeDegrees_;

    public:
    // Set whether to assume values used in trig functions are degrees
    static void setAssumeDegrees(bool b);
    // Return whether to assume values used in trig functions are degrees
    static bool assumeDegrees();

    /*
     * Function list
     */
    enum Function
    {
        // Operators
        OperatorAdd,
        OperatorAnd,
        OperatorDivide,
        OperatorEqualTo,
        OperatorGreaterThan,
        OperatorGreaterThanEqualTo,
        OperatorLessThan,
        OperatorLessThanEqualTo,
        OperatorModulus,
        OperatorMultiply,
        OperatorNegate,
        OperatorNot,
        OperatorNotEqualTo,
        OperatorOr,
        OperatorPower,
        OperatorSubtract,

        // Flow control
        NoFunction,
        Joiner,
        If,

        // Math Functions
        Abs,
        ACos,
        ASin,
        ATan,
        Cos,
        Exp,
        Ln,
        Log,
        Nint,
        Sin,
        Sqrt,
        Tan,

        nFunctions
    };
    // Return enumerated function id from string
    static Function function(std::string_view s);

    /*
     * Function declarations
     */
    private:
    // AST-specific functions
    static bool function_NoFunction(ExpressionFunction *c, ExpressionValue &result);
    static bool function_Joiner(ExpressionFunction *c, ExpressionValue &result);
    // Flow control
    static bool function_If(ExpressionFunction *c, ExpressionValue &result);
    // Math Functions
    static bool function_Abs(ExpressionFunction *c, ExpressionValue &result);
    static bool function_ACos(ExpressionFunction *c, ExpressionValue &result);
    static bool function_ASin(ExpressionFunction *c, ExpressionValue &result);
    static bool function_ATan(ExpressionFunction *c, ExpressionValue &result);
    static bool function_Cos(ExpressionFunction *c, ExpressionValue &result);
    static bool function_Exp(ExpressionFunction *c, ExpressionValue &result);
    static bool function_Ln(ExpressionFunction *c, ExpressionValue &result);
    static bool function_Log(ExpressionFunction *c, ExpressionValue &result);
    static bool function_Nint(ExpressionFunction *c, ExpressionValue &result);
    static bool function_Sin(ExpressionFunction *c, ExpressionValue &result);
    static bool function_Sqrt(ExpressionFunction *c, ExpressionValue &result);
    static bool function_Tan(ExpressionFunction *c, ExpressionValue &result);
    // Variable Operators
    static bool function_OperatorAdd(ExpressionFunction *c, ExpressionValue &result);
    static bool function_OperatorAnd(ExpressionFunction *c, ExpressionValue &result);
    static bool function_OperatorDivide(ExpressionFunction *c, ExpressionValue &result);
    static bool function_OperatorEqualTo(ExpressionFunction *c, ExpressionValue &result);
    static bool function_OperatorGreaterThan(ExpressionFunction *c, ExpressionValue &result);
    static bool function_OperatorGreaterThanEqualTo(ExpressionFunction *c, ExpressionValue &result);
    static bool function_OperatorLessThan(ExpressionFunction *c, ExpressionValue &result);
    static bool function_OperatorLessThanEqualTo(ExpressionFunction *c, ExpressionValue &result);
    static bool function_OperatorModulus(ExpressionFunction *c, ExpressionValue &result);
    static bool function_OperatorMultiply(ExpressionFunction *c, ExpressionValue &result);
    static bool function_OperatorNegate(ExpressionFunction *c, ExpressionValue &result);
    static bool function_OperatorNot(ExpressionFunction *c, ExpressionValue &result);
    static bool function_OperatorNotEqualTo(ExpressionFunction *c, ExpressionValue &result);
    static bool function_OperatorOr(ExpressionFunction *c, ExpressionValue &result);
    static bool function_OperatorPower(ExpressionFunction *c, ExpressionValue &result);
    static bool function_OperatorSubtract(ExpressionFunction *c, ExpressionValue &result);
    // Other Functions
    static bool function_PrintF(ExpressionFunction *c, ExpressionValue &result);

    /*
     * Function descriptions / syntax etc.
     */
    private:
    // Function pointers
    FunctionFunction pointers_[ExpressionFunctions::nFunctions];

    public:
    // Function data
    static ExpressionFunctionData data[ExpressionFunctions::nFunctions];
    // Initialise function pointers
    void initPointers();
    // Execute specified function
    bool call(ExpressionFunctions::Function cf, ExpressionFunction *node, ExpressionValue &result);
};

// External Singleton
extern ExpressionFunctions expressionFunctions;
