// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "base/enumoptions.h"
#include <fmt/format.h>

// Forward Declarations
class LineParser;
class Data1D;

// Window Function
class WindowFunction
{
    public:
    // Window Function Forms
    enum class Form
    {
        None,     /* No window */
        Bartlett, /* Bartlett (triangular) window */
        Hann,     /* von Hann (Hanning) window */
        Lanczos,  /* Lanczos window */
        Nuttall,  /* Nuttall window (continuous first derivatives over range) */
        Sine,     /* Sine Window */
        Lorch0    /* Original Lorch function */
    };
    // Return EnumOptions for FunctionType
    static EnumOptions<WindowFunction::Form> forms();
    WindowFunction(WindowFunction::Form function = Form::None);
    ~WindowFunction() = default;

    /*
     * Function Data
     */
    private:
    // Functional Form
    Form form_;
    // Maximal x value for current data target
    double xMax_{0.0};

    public:
    // Set functional form
    void set(WindowFunction::Form form);
    // Set function data from LineParser source
    bool set(LineParser &parser, int startArg);
    // Return function type
    WindowFunction::Form form() const;
    // Set-up function for specified data
    bool setUp(const Data1D &data);
    // Return value of function given parameters x (current abscissa value) and omega (target abscissa value)
    double y(double x, double omega) const;
};

template <> struct fmt::formatter<WindowFunction::Form> : formatter<std::string>
{
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext> auto format(WindowFunction::Form c, FormatContext &ctx)
    {
        switch (c)
        {
            case WindowFunction::Form::None:
                return formatter<std::string>::format("None", ctx);
            case WindowFunction::Form::Bartlett:
                return formatter<std::string>::format("Bartlett", ctx);
            case WindowFunction::Form::Hann:
                return formatter<std::string>::format("Hann", ctx);
            case WindowFunction::Form::Lanczos:
                return formatter<std::string>::format("Lanczos", ctx);
            case WindowFunction::Form::Nuttall:
                return formatter<std::string>::format("Nuttall", ctx);
            case WindowFunction::Form::Sine:
                return formatter<std::string>::format("Sine", ctx);
            case WindowFunction::Form::Lorch0:
                return formatter<std::string>::format("Lorch0", ctx);
        }
        throw(std::runtime_error("Unknown WindowFunction::Form"));
    }
};
