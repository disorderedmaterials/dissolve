// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"

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
