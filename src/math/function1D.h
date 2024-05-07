// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include "templates/flags.h"
#include <functional>
#include <string_view>
#include <vector>

// Function Properties
namespace FunctionProperties
{
enum FunctionProperty
{
    None,
    FourierTransform,
    Normalisation
};
};

namespace Functions
{
// Function Types
// -- Setup function parameters, returning vector augmented with any precalculated additional parameters
using FunctionSetup = std::function<std::vector<double>(std::vector<double> parameters)>;
// -- Function taking x and omega arguments and returning a value
using Function1DXOmega = std::function<double(double x, double omega, const std::vector<double> &parameters)>;
// -- Function taking omega argument and returning a value
using Function1DOmega = std::function<double(double omega, const std::vector<double> &parameters)>;

// One-Dimensional Function Definition
class FunctionDefinition1D
{
    public:
    FunctionDefinition1D(const std::vector<std::string> &parameterNames,
                         const Flags<FunctionProperties::FunctionProperty> &properties, FunctionSetup setup, Function1DXOmega y,
                         Function1DXOmega yFT = {}, Function1DOmega norm = {});

    private:
    // Names of parameters defining the function
    std::vector<std::string> parameterNames_;
    // Properties of the function
    Flags<FunctionProperties::FunctionProperty> properties_;
    // Functions
    FunctionSetup setup_;
    Function1DXOmega y_, yFT_;
    Function1DOmega normaliser_;

    public:
    // Return number of parameters the function requires
    int nParameters() const;
    // Return parameter names
    const std::vector<std::string> &parameterNames() const;
    // Return properties of the function
    const Flags<FunctionProperties::FunctionProperty> &properties() const;
    // Return function for setup
    FunctionSetup setup() const;
    // Return function for y value
    Function1DXOmega y() const;
    // Return function for FT of y value
    Function1DXOmega yFT() const;
    // Return normalisation function
    Function1DOmega normalisation() const;
};

// Function Types
enum class Function1D
{
    None,
    Gaussian,
    ScaledGaussian,
    OmegaDependentGaussian,
    GaussianC2
};
// Return enum options for Function1D
EnumOptions<Functions::Function1D> function1D();
// Return base function requested
FunctionDefinition1D functionDefinition1D(Functions::Function1D func);
// Check function properties against those supplied, returning truth if the function meets all requirements
bool validFunction1DProperties(Function1D func, int properties);
// Return all available functions with properties matching those provided
std::vector<Function1D> matchingFunction1D(int properties);

// Function 1D Wrapper
class Function1DWrapper
{
    public:
    Function1DWrapper(Function1D func = Function1D::None, const std::vector<double> &params = {});

    /*
     * Function
     */
    private:
    // Function type
    Function1D type_;
    // Function definition
    FunctionDefinition1D function_;
    // Input arguments to function
    std::vector<double> parameters_;
    // Internal function parameters (input params followed by any calculated parameters)
    std::vector<double> internalParameters_;

    private:
    // Initialise internal function parameters from current base parameters
    void calculateInternalParameters();

    public:
    // Set function type and parameters
    bool setFunctionAndParameters(Function1D func, const std::vector<double> &params);
    // Set current function type
    void setFunction(Function1D func);
    // Return function type
    Function1D type() const;
    // Return number of parameters for current function
    int nParameters() const;
    // Set current function parameters
    bool setParameters(const std::vector<double> &params);
    // Return current parameters
    const std::vector<double> &parameters() const;
    // Return name of nth parameter
    std::string parameterName(int i) const;
    // Return parameter summary ("name = value, ...")
    std::string parameterSummary() const;
    // Return y value at specified x, omega
    double y(double x, double omega = 0.0) const;
    // Return Fourier transformed y value at specified x, omega
    double yFT(double x, double omega = 0.0) const;
    // Return normalisation factor at specified omega
    double normalisation(double omega = 0.0) const;
};
} // namespace Functions
