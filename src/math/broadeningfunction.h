/*
    *** Broadening Function
    *** src/math/broadeningfunction.h
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

#pragma once

#define MAXBROADENINGFUNCTIONPARAMS 6

#include "genericitems/base.h"

// Forward Declarations
class LineParser;
class ProcessPool;

// Broadening Function
class BroadeningFunction : public GenericItemBase
{
    public:
    // Function Types
    enum FunctionType
    {
        NoFunction,
        GaussianFunction,
        ScaledGaussianFunction,
        OmegaDependentGaussianFunction,
        GaussianC2Function,
        nFunctionTypes
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
    BroadeningFunction(FunctionType function = NoFunction, double p1 = 0.0, double p2 = 0.0, double p3 = 0.0, double p4 = 0.0,
                       double p5 = 0.0, double p6 = 0.0);
    ~BroadeningFunction();
    BroadeningFunction(const BroadeningFunction &source);
    void operator=(const BroadeningFunction &source);

    /*
     * Function Data
     */
    private:
    // Function Type
    FunctionType function_;
    // Parameters
    double parameters_[MAXBROADENINGFUNCTIONPARAMS];
    // Whether function is inverted - y returns yFT, and vice versa
    bool inverted_;
    // Static value of omega to use if required
    double staticOmega_;

    public:
    // Set function data
    void set(FunctionType function, double p1 = 0.0, double p2 = 0.0, double p3 = 0.0, double p4 = 0.0, double p5 = 0.0,
             double p6 = 0.0);
    // Set function data from LineParser source
    bool set(LineParser &parser, int startArg);
    // Return function type
    FunctionType function() const;
    // Return number of parameters required
    int nParameters() const;
    // Return specified parameter
    double parameter(int index) const;
    // Return parameters array
    double *parameters();
    // Return specified parameter name
    std::string_view parameterName(int index) const;
    // Return short summary of function parameters
    std::string parameterSummary() const;
    // Set up any dependent parameters based on the current parameters
    void setUpDependentParameters();
    // Set inversion state
    void setInverted(bool state);
    // Set static omega value
    void setOmega(double omega);
    // Return value of function given parameters x and omega
    double y(double x, double omega) const;
    // Return value of Fourier transform of function, given parameters x and omega
    double yFT(double x, double omega) const;
    // Return value of function given parameters x and omega, regardless of inversion state
    double yActual(double x, double omega) const;
    // Return value of Fourier transform of function, given parameters x and omega, regardless of inversion state
    double yFTActual(double x, double omega) const;
    // Return value of function given parameter x, and using static omega if necessary
    double y(double x) const;
    // Return value of Fourier transform of function, given parameter x, and using static omega if necessary
    double yFT(double x) const;
    // Return value of function given parameter x, and using static omega if necessary, regardless of inversion state
    double yActual(double x) const;
    // Return value of Fourier transform of function, given parameter x, and using static omega if necessary, regardless of
    // inversion state
    double yFTActual(double x) const;
    // Return the discrete kernel normalisation factor for the current function, given the underlying data binwidth, and
    // using static omega if necessary
    double discreteKernelNormalisation(double deltaX) const;
    // Return the discrete kernel normalisation factor for the current function, given the underlying data binwidth and
    // omega value
    double discreteKernelNormalisation(double deltaX, double omega) const;

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
