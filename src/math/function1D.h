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
    FourierTransform,
    Normalisation,
    FirstDerivative
};
};

// Function Types
// -- Setup function parameters, returning vector augmented with any precalculated additional parameters
using Function1DSetup = std::function<std::vector<double>(std::vector<double> parameters)>;
// -- Function taking x and omega arguments and returning a value
using Function1DXOmega = std::function<double(double x, double omega, const std::vector<double> &parameters)>;
// -- Function taking omega argument and returning a value
using Function1DOmega = std::function<double(double omega, const std::vector<double> &parameters)>;

// One-Dimensional Function Definition
class Function1DDefinition
{
    public:
    Function1DDefinition() = default;
    Function1DDefinition(const std::vector<std::string> &parameterNames, Function1DXOmega valueFunction);

    private:
    // Names of parameters defining the function
    std::vector<std::string> parameterNames_;
    // Properties of the function
    Flags<FunctionProperties::FunctionProperty> properties_;
    // Functions
    Function1DSetup setup_;
    Function1DXOmega y_, dYdX_, yFT_;
    Function1DOmega normaliser_;

    public:
    // Return number of parameters the function requires
    int nParameters() const;
    // Return parameter names
    const std::vector<std::string> &parameterNames() const;
    // Return properties of the function
    const Flags<FunctionProperties::FunctionProperty> &properties() const;
    // Set setup function
    void setSetupFunction(Function1DSetup func);
    // Return function for setup
    Function1DSetup setup() const;
    // Return function for y value
    Function1DXOmega y() const;
    // Set derivative function
    void setDerivativeFunction(Function1DXOmega func);
    // Return function for first derivative
    Function1DXOmega dYdX() const;
    // Set FT function
    void setFTFunction(Function1DXOmega func);
    // Return function for FT of y value
    Function1DXOmega yFT() const;
    // Set normalisation function
    void setNormalisationFunction(Function1DOmega func);
    // Return normalisation function
    Function1DOmega normalisation() const;
};

// One-Dimensional Functional Forms
class Functions1D
{
    public:
    // Functional Forms
    enum Form
    {
        None,
        Gaussian,
        ScaledGaussian,
        OmegaDependentGaussian,
        GaussianC2,
        LennardJones126,
        Buckingham,
        GaussianPotential,
        Harmonic,
        Coulombic,
        ShiftedCoulomb
    };
    // Return enum options for form
    static EnumOptions<Form> forms();
    // Return parameters for specified form
    static const std::vector<std::string> &parameters(Form form);
    // Return nth parameter for the given form
    static std::string parameter(Form form, int n);
    // Return index of parameter in the given form
    static std::optional<int> parameterIndex(Form form, std::string_view name);
    // Return base function requested
    static Function1DDefinition functionDefinition1D(Form func);
    // Check function properties against those supplied, returning truth if the function meets all requirements
    static bool validFunction1DProperties(Form func, const Flags<FunctionProperties::FunctionProperty> &properties);
    // Return all available functions with properties matching those provided
    static std::vector<Form> matchingFunction1D(const Flags<FunctionProperties::FunctionProperty> &properties);
};

// Function 1D Wrapper
class Function1DWrapper
{
    public:
    Function1DWrapper(Functions1D::Form form = Functions1D::Form::None, const std::vector<double> &params = {});

    /*
     * Function
     */
    private:
    // Functional form
    Functions1D::Form form_;
    // Function definition
    Function1DDefinition function_;
    // Input arguments to function
    std::vector<double> parameters_;
    // Internal function parameters (input params followed by any calculated parameters)
    std::vector<double> internalParameters_;

    private:
    // Initialise internal function parameters from current base parameters
    void calculateInternalParameters();

    public:
    // Set function type and parameters
    bool setFormAndParameters(Functions1D::Form form, const std::vector<double> &params);
    // Set current functional form
    void setForm(Functions1D::Form form);
    // Return functional form
    Functions1D::Form form() const;
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
    // Return first derivative of y at specified x, omega
    double dYdX(double x, double omega = 0.0) const;
    // Return Fourier transformed y value at specified x, omega
    double yFT(double x, double omega = 0.0) const;
    // Return normalisation factor at specified omega
    double normalisation(double omega = 0.0) const;
};
