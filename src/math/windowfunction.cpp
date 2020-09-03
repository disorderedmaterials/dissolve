/*
    *** Window Function
    *** src/math/windowfunction.cpp
    Copyright T. Youngs 2012-2020

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

#include "math/windowfunction.h"
#include "base/lineparser.h"
#include "base/processpool.h"
#include "base/sysfunc.h"
#include "math/data1d.h"
#include "templates/enumhelpers.h"

WindowFunction::WindowFunction(WindowFunction::FunctionType function, double p1, double p2, double p3, double p4, double p5,
                               double p6)
{
    set(function, p1, p2, p3, p4, p5, p6);

    xMax_ = -1.0;
}

void WindowFunction::operator=(const WindowFunction &source)
{
    function_ = source.function_;
    for (int n = 0; n < MAXWINDOWFUNCTIONPARAMS; ++n)
        parameters_[n] = source.parameters_[n];

    xMax_ = source.xMax_;
}

WindowFunction::~WindowFunction() {}

// Window Function keywords
std::string_view WindowFunctionKeywords[] = {"None", "Bartlett", "Hann", "Lanczos", "Nuttall", "Sine", "Lorch0"};

int WindowFunctionNParameters[] = {0, 0, 0, 0, 0, 0, 0};

// Return FunctionType from supplied string
WindowFunction::FunctionType WindowFunction::functionType(std::string_view s)
{
    for (int n = 0; n < nFunctionTypes; ++n)
        if (DissolveSys::sameString(s, WindowFunctionKeywords[n]))
            return (FunctionType)n;
    return WindowFunction::nFunctionTypes;
}

// Return FunctionType name
std::string_view WindowFunction::functionType(WindowFunction::FunctionType func) { return WindowFunctionKeywords[func]; }

// Return number of parameters needed to define function
int WindowFunction::nFunctionParameters(FunctionType func) { return WindowFunctionNParameters[func]; }

// Return description for FunctionType
std::string_view WindowFunction::functionDescription(FunctionType func)
{
    switch (func)
    {
        case (WindowFunction::NoWindow):
            return "No window";
            break;
        case (WindowFunction::BartlettWindow):
            return "Bartlett (triangular) window";
            break;
        case (WindowFunction::HannWindow):
            return "von Hann (Hanning) window";
            break;
        case (WindowFunction::LanczosWindow):
            return "Lanczos window";
            break;
        case (WindowFunction::NuttallWindow):
            return "Nuttall window (continuous first derivatives over range)";
            break;
        case (WindowFunction::SineWindow):
            return "Sine Window";
            break;
        case (WindowFunction::Lorch0Window):
            return "Original Lorch function";
            break;
        default:
            break;
    }

    return "NO WINDOW FUNCTION DESCRIPTION AVAILABLE.";
}

/*
 * Function Data
 */

void WindowFunction::set(WindowFunction::FunctionType function, double p1, double p2, double p3, double p4, double p5,
                         double p6)
{
    function_ = function;
    parameters_[0] = p1;
    parameters_[1] = p2;
    parameters_[2] = p3;
    parameters_[3] = p4;
    parameters_[4] = p5;
    parameters_[5] = p6;
}

// Set function data from LineParser source
bool WindowFunction::set(LineParser &parser, int startArg)
{
    // First argument is the form of the function
    WindowFunction::FunctionType funcType = WindowFunction::functionType(parser.argsv(startArg));
    if (funcType == WindowFunction::nFunctionTypes)
    {
        Messenger::error("Unrecognised Function type '{}'.\n", parser.argsv(startArg));
        return false;
    }

    // Do we have the right number of arguments for the function specified?
    if ((parser.nArgs() - startArg) < WindowFunction::nFunctionParameters(funcType))
    {
        Messenger::error("Too few parameters supplied for Function '{}' (expected {}, found {}).\n",
                         WindowFunction::functionType(funcType), WindowFunction::nFunctionParameters(funcType),
                         parser.nArgs() - startArg);
        return false;
    }

    // Set up function
    function_ = funcType;
    switch (function_)
    {
        case (WindowFunction::NoWindow):
        case (WindowFunction::BartlettWindow):
        case (WindowFunction::HannWindow):
        case (WindowFunction::LanczosWindow):
        case (WindowFunction::NuttallWindow):
        case (WindowFunction::SineWindow):
        case (WindowFunction::Lorch0Window):
            // No fixed parameters.
            break;
        default:
            Messenger::error("Function form '{}' not accounted for in set(LineParser&,int).\n",
                             WindowFunction::functionType(funcType));
            return false;
    }

    return true;
}

// Return function type
WindowFunction::FunctionType WindowFunction::function() const { return function_; }

// Return parameter specified
double WindowFunction::parameter(int n) const { return parameters_[n]; }

// Return short summary of function parameters
std::string WindowFunction::parameterSummary() const
{
    switch (function_)
    {
        case (WindowFunction::NoWindow):
        case (WindowFunction::BartlettWindow):
        case (WindowFunction::HannWindow):
        case (WindowFunction::LanczosWindow):
        case (WindowFunction::NuttallWindow):
        case (WindowFunction::SineWindow):
            return "No Parameters";
            break;
        case (WindowFunction::Lorch0Window):
            return "Delta0=PI/xMax";
            break;
        default:
            Messenger::warn("WindowFunction::value(x) - Function id {} not accounted for.\n", function_);
            break;
    }

    return "NULL";
}

// Set-up function for specified data
bool WindowFunction::setUp(const Data1D &data)
{
    // Store maximum x value of data
    xMax_ = data.constXAxis().lastValue();

    switch (function_)
    {
        case (WindowFunction::NoWindow):
        case (WindowFunction::BartlettWindow):
        case (WindowFunction::HannWindow):
        case (WindowFunction::LanczosWindow):
        case (WindowFunction::NuttallWindow):
        case (WindowFunction::SineWindow):
            break;
        case (WindowFunction::Lorch0Window):
            // Set Delta0 from the high x limit of the data
            parameters_[0] = PI / xMax_;
            break;
        default:
            Messenger::warn("WindowFunction::value(x) - Function id {} not accounted for.\n", function_);
            break;
    }

    return true;
}

// Return value of function given parameters x (current abscissa value) and omega (target abscissa value)
double WindowFunction::y(double x, double omega) const
{
    // Determine current fractional x value (from our stored xMax_)
    const auto chi = x / xMax_;

#ifdef CHECKS
    if ((chi < 0.0) || (chi > 1.0))
        Messenger::warn("Position for window function is out of range ({}).\n", chi);
#endif

    switch (function_)
    {
        case (WindowFunction::NoWindow):
            return 1.0;
            break;
        case (WindowFunction::BartlettWindow):
            return (1.0 - fabs((chi - 0.5) / 0.5));
            break;
        case (WindowFunction::HannWindow):
            return 0.5 * (1.0 - cos(2 * PI * chi));
            break;
        case (WindowFunction::LanczosWindow):
            return sin(PI * (2 * chi - 1.0)) / (PI * (2 * chi - 1.0));
            break;
        case (WindowFunction::NuttallWindow):
            return (0.355768 - 0.487396 * cos(2.0 * PI * chi) + 0.144232 * cos(4.0 * PI * chi) -
                    0.012604 * cos(6.0 * PI * chi));
            break;
        case (WindowFunction::SineWindow):
            return 1.0 - sin(PI * 0.5 * chi);
            break;
        case (WindowFunction::Lorch0Window):
            /*
             * Original Lorch function
             *
             * Parameters:  0 = delta0	( = PI/xmax )
             *
             * 	  sin(x * delta0)
             * f(x) = ---------------
             * 	    x * delta0
             */
            return sin(x * parameters_[0]) / (x * parameters_[0]);
            break;
        default:
            Messenger::warn("WindowFunction::value() - Function id {} not accounted for.\n", function_);
            break;
    }

    return 0.0;
}

/*
 * GenericItemBase Implementations
 */

// Return class name
std::string_view WindowFunction::itemClassName() { return "WindowFunction"; }

// Read data through specified LineParser
bool WindowFunction::read(LineParser &parser, CoreData &coreData)
{
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    function_ = functionType(parser.argsv(0));
    for (int n = 0; n < nFunctionParameters(function_); ++n)
        parameters_[n] = parser.argd(n + 1);
    return true;
}

// Write data through specified LineParser
bool WindowFunction::write(LineParser &parser)
{
    std::string line{functionType(function_)};
    for (int n = 0; n < nFunctionParameters(function_); ++n)
        line += fmt::format(" {:16.9e}", parameters_[n]);
    return parser.writeLine(line);
}

/*
 * Parallel Comms
 */

// Broadcast data from Master to all Slaves
bool WindowFunction::broadcast(ProcessPool &procPool, const int root, const CoreData &coreData)
{
#ifdef PARALLEL
    if (!procPool.broadcast(EnumCast<WindowFunction::FunctionType>(function_), root))
        return false;
    if (!procPool.broadcast(parameters_, MAXWINDOWFUNCTIONPARAMS, root))
        return false;
    if (!procPool.broadcast(xMax_, root))
        return false;
#endif
    return true;
}

// Check item equality
bool WindowFunction::equality(ProcessPool &procPool)
{
#ifdef PARALLEL
    if (!procPool.equality(EnumCast<WindowFunction::FunctionType>(function_)))
        return Messenger::error("WindowFunction function type is not equivalent (process {} has {}).\n", procPool.poolRank(),
                                function_);
    if (!procPool.equality(parameters_, MAXWINDOWFUNCTIONPARAMS))
        return Messenger::error("WindowFunction parameters are not equivalent.\n");
#endif
    return true;
}
