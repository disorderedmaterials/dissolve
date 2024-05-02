// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/windowFunction.h"
#include "base/lineParser.h"
#include "math/data1D.h"

WindowFunction::WindowFunction(WindowFunction::Form function) : form_(function) {}

// Return EnumOptions for FunctionType
EnumOptions<WindowFunction::Form> WindowFunction::forms()
{
    return EnumOptions<WindowFunction::Form>("WindowFunction", {{Form::None, "None"},
                                                                {Form::Bartlett, "Bartlett"},
                                                                {Form::Hann, "Hann"},
                                                                {Form::Lanczos, "Lanczos"},
                                                                {Form::Nuttall, "Nuttall"},
                                                                {Form::Sine, "Sine"},
                                                                {Form::Lorch0, "Lorch0"}});
}

/*
 * Function Data
 */

void WindowFunction::set(WindowFunction::Form form) { form_ = form; }

// Set function data from LineParser source
bool WindowFunction::set(LineParser &parser, int startArg)
{
    // First argument is the form of the function
    if (forms().isValid(parser.argsv(startArg)))
        return forms().errorAndPrintValid(parser.argsv(startArg));

    form_ = forms().enumeration(parser.argsv(startArg));

    return true;
}

// Return functional form
WindowFunction::Form WindowFunction::form() const { return form_; }

// Set-up function for specified data
bool WindowFunction::setUp(const Data1D &data)
{
    // Store maximum x value of data
    xMax_ = data.xAxis().back();

    return true;
}

// Return value of function given parameters x (current abscissa value) and omega (target abscissa value)
double WindowFunction::y(double x, double omega) const
{
    // Determine current fractional x value (from our stored xMax_)
    const auto chi = x / xMax_;
    assert(chi >= 0.0 && chi <= 1.0);

    switch (form_)
    {
        case (Form::None):
            return 1.0;
        case (Form::Bartlett):
            return (1.0 - fabs((chi - 0.5) / 0.5));
        case (Form::Hann):
            return 0.5 * (1.0 - cos(2 * PI * chi));
        case (Form::Lanczos):
            return sin(PI * (2 * chi - 1.0)) / (PI * (2 * chi - 1.0));
        case (Form::Nuttall):
            return (0.355768 - 0.487396 * cos(2.0 * PI * chi) + 0.144232 * cos(4.0 * PI * chi) -
                    0.012604 * cos(6.0 * PI * chi));
        case (Form::Sine):
            return 1.0 - sin(PI * 0.5 * chi);
        case (Form::Lorch0):
            /*
             * Original Lorch function
             *
             * Parameters:
             *  0 = delta0  ( = PI/xmax )
             *
             *        sin(x * delta0)
             * f(x) = ---------------
             *          x * delta0
             */
            return sin(x * (PI / xMax_)) / (x * (PI / xMax_));
        default:
            throw(std::runtime_error(fmt::format("Window function enumeration {} not implemented.\n", (int)form_)));
    }

    return 0.0;
}
