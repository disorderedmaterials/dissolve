// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#define MAXWINDOWFUNCTIONPARAMS 6

#include "genericitems/base.h"

// Forward Declarations
class LineParser;
class ProcessPool;
class Data1D;

// Window Function
class WindowFunction : public GenericItemBase
{
    public:
    // Function Types
    enum FunctionType
    {
        NoWindow,       /* No window */
        BartlettWindow, /* Bartlett (triangular) window */
        HannWindow,     /* von Hann (Hanning) window */
        LanczosWindow,  /* Lanczos window */
        NuttallWindow,  /* Nuttall window (continuous first derivatives over range) */
        SineWindow,     /* Sine Window */
        Lorch0Window,   /* Original Lorch function */
        nFunctionTypes  /* Number of defined WindowFunctions */
    };
    // Return FunctionType from supplied string
    static FunctionType functionType(std::string_view s);
    // Return FunctionType name
    static std::string_view functionType(FunctionType func);
    // Return number of parameters needed to define FunctionType
    static int nFunctionParameters(FunctionType func);
    // Return description for FunctionType
    static std::string_view functionDescription(FunctionType func);

    public:
    WindowFunction(FunctionType function = NoWindow, double p1 = 0.0, double p2 = 0.0, double p3 = 0.0, double p4 = 0.0,
                   double p5 = 0.0, double p6 = 0.0);
    ~WindowFunction();
    void operator=(const WindowFunction &source);

    /*
     * Function Data
     */
    private:
    // Function Type
    FunctionType function_;
    // Parameters
    double parameters_[MAXWINDOWFUNCTIONPARAMS];
    // Maximal x value for current data target
    double xMax_;

    public:
    // Set function data
    void set(FunctionType function, double p1 = 0.0, double p2 = 0.0, double p3 = 0.0, double p4 = 0.0, double p5 = 0.0,
             double p6 = 0.0);
    // Set function data from LineParser source
    bool set(LineParser &parser, int startArg);
    // Return function type
    FunctionType function() const;
    // Return parameter specified
    double parameter(int n) const;
    // Return short summary of function parameters
    std::string parameterSummary() const;
    // Set-up function for specified data
    bool setUp(const Data1D &data);
    // Return value of function given parameters x (current abscissa value) and omega (target abscissa value)
    double y(double x, double omega) const;

    /*
     * GenericItemBase Implementations
     */
    public:
    // Return class name
    static std::string_view itemClassName();
    // Read data through specified LineParser
    bool read(LineParser &parser, CoreData &coreData);
    // Write data through specified LineParser
    bool write(LineParser &parser);

    /*
     * Parallel Comms
     */
    public:
    // Broadcast data from Master to all Slaves
    bool broadcast(ProcessPool &procPool, const int root, const CoreData &coreData);
    // Check item equality
    bool equality(ProcessPool &procPool);
};
