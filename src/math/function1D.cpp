// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#define _USE_MATH_DEFINES
#include "math/function1D.h"
#include "templates/algorithms.h"
#include <math.h>

#include <map>

/*
 * One-Dimensional Function Definition
 */

Function1DDefinition::Function1DDefinition(const std::vector<std::string> &parameterNames, Function1DXOmega valueFunction)
    : parameterNames_(parameterNames), y_(std::move(valueFunction))
{
}

// Return number of parameters the function requires
int Function1DDefinition::nParameters() const { return parameterNames_.size(); }

// Return parameter names
const std::vector<std::string> &Function1DDefinition::parameterNames() const { return parameterNames_; }

// Return properties of the function
const Flags<FunctionProperties::FunctionProperty> &Function1DDefinition::properties() const { return properties_; }

// Set setup function
void Function1DDefinition::setSetupFunction(Function1DSetup func) { setup_ = std::move(func); }

// Return function for setup
Function1DSetup Function1DDefinition::setup() const { return setup_; }

// Return function for y value at specified x, omega
Function1DXOmega Function1DDefinition::y() const { return y_; }

// Set derivative function
void Function1DDefinition::setDerivativeFunction(Function1DXOmega func)
{
    dYdX_ = std::move(func);
    properties_.setFlag(FunctionProperties::FirstDerivative);
}

// Return function for first derivative of y with respect to x (at specified omega)
Function1DXOmega Function1DDefinition::dYdX() const { return dYdX_; }

// Set FT function
void Function1DDefinition::setFTFunction(Function1DXOmega func)
{
    yFT_ = std::move(func);
    properties_.setFlag(FunctionProperties::FourierTransform);
}

// Return function for FT of y value at the specified x, omega
Function1DXOmega Function1DDefinition::yFT() const { return yFT_; }

// Set normalisation function
void Function1DDefinition::setNormalisationFunction(Function1DOmega func)
{
    normaliser_ = std::move(func);
    properties_.setFlag(FunctionProperties::Normalisation);
}

// Return normalisation function
Function1DOmega Function1DDefinition::normalisation() const { return normaliser_; }

// One-Dimensional Function Definitions
const std::map<Functions1D::Form, Function1DDefinition> &functions1D()
{
    static std::map<Functions1D::Form, Function1DDefinition> functions;
    if (functions.empty())
    {
        /*
         * No function - returns zero
         */
        functions[Functions1D::Form::None] =
            Function1DDefinition({}, [](double x, double omega, const std::vector<double> &params) { return 0.0; });
        functions[Functions1D::Form::None].setDerivativeFunction([](double x, double omega, const std::vector<double> &params)
                                                                 { return 0.0; });
        functions[Functions1D::Form::None].setFTFunction([](double x, double omega, const std::vector<double> &params)
                                                         { return 0.0; });
        functions[Functions1D::Form::None].setNormalisationFunction([](double omega, const std::vector<double> &params)
                                                                    { return 0.0; });

        /*
         * Gaussian
         *
         * Parameters:
         * INPUT  0 = fwhm
         * CALC   1 = c = fwhm / (2 * sqrt(2 ln 2))
         * CALC   2 = 1.0 / c
         *
         *            (     x * x   )               (   x * x * c * c )
         * f(x) = exp ( - --------- )   FT(x) = exp ( - ------------- )
         * 	          (   2 * c * c )               (         2       )
         */
        functions[Functions1D::Form::Gaussian] =
            Function1DDefinition({"fwhm"}, [](double x, double omega, const std::vector<double> &params)
                                 { return exp(-(0.5 * x * x * params[2] * params[2])); });
        functions[Functions1D::Form::Gaussian].setSetupFunction(
            [](std::vector<double> params)
            {
                params.push_back(params[0] / (2.0 * sqrt(2.0 * log(2.0))));
                params.push_back(1.0 / params[1]);
                return params;
            });
        functions[Functions1D::Form::Gaussian].setFTFunction([](double x, double omega, const std::vector<double> &params)
                                                             { return exp(-(0.5 * x * x * params[1] * params[1])); });
        functions[Functions1D::Form::Gaussian].setNormalisationFunction([](double omega, const std::vector<double> &params)
                                                                        { return params[2] / sqrt(2.0 * M_PI); });

        /*
         * Gaussian with prefactor
         *
         * Parameters:
         * INPUT  0 = A
         * INPUT  1 = fwhm
         * CALC   2 = c = fwhm / (2 * sqrt(2 ln 2))
         * CALC   3 = 1.0 / c
         *
         *              (     x * x   )                 (   x * x * c * c )                 1
         * f(x) = A exp ( - --------- )   FT(x) = A exp ( - ------------- )   Norm = --------------
         *              (   2 * c * c )                 (         2       )          A c sqrt(2 pi)
         */
        functions[Functions1D::Form::ScaledGaussian] =
            Function1DDefinition({"A", "fwhm"}, [](double x, double omega, const std::vector<double> &params)
                                 { return params[0] * exp(-(0.5 * x * x * params[3] * params[3])); });
        functions[Functions1D::Form::ScaledGaussian].setSetupFunction(
            [](std::vector<double> params)
            {
                params.push_back(params[1] / (2.0 * sqrt(2.0 * log(2.0))));
                params.push_back(1.0 / params[2]);
                return params;
            });
        functions[Functions1D::Form::ScaledGaussian].setFTFunction(
            [](double x, double omega, const std::vector<double> &params)
            { return params[0] * exp(-(0.5 * x * x * params[2] * params[2])); });
        functions[Functions1D::Form::ScaledGaussian].setNormalisationFunction(
            [](double omega, const std::vector<double> &params) { return params[3] / (params[0] * sqrt(2.0 * M_PI)); });

        /*
         * Gaussian with omega-dependent fwhm
         *
         * Parameters:
         * INPUT  0 = fwhm
         * CALC   1 = c = fwhm / (2 * sqrt(2 ln 2))
         * CALC   2 = 1.0 / c
         *
         *            (         x * x      )               (   x*x * (c*omega)**2 )                  1
         * f(x) = exp ( - ---------------- )   FT(x) = exp ( - ------------------ )   Norm = ------------------
         *            (   2 * (c*omega)**2 )               (            2         )          c omega sqrt(2 pi)
         */
        functions[Functions1D::Form::OmegaDependentGaussian] =
            Function1DDefinition({"fwhm(x)"}, [](double x, double omega, const std::vector<double> &params)
                                 { return exp(-(x * x) / (2.0 * (params[1] * omega) * (params[1] * omega))); });
        functions[Functions1D::Form::OmegaDependentGaussian].setSetupFunction(
            [](std::vector<double> params)
            {
                params.push_back(params[0] / (2.0 * sqrt(2.0 * log(2.0))));
                params.push_back(1.0 / params[1]);
                return params;
            });
        /*
         *             (   x*x * (c*omega)**2 )                  1
         * FT(x) = exp ( - ------------------ )   Norm = ------------------
         *             (            2         )          c omega sqrt(2 pi)
         */
        functions[Functions1D::Form::OmegaDependentGaussian].setFTFunction(
            [](double x, double omega, const std::vector<double> &params)
            { return exp(-(0.5 * x * x * (params[1] * omega) * (params[1] * omega))); });
        /*
         *               1
         * Norm = ------------------
         *        c omega sqrt(2 pi)
         */
        functions[Functions1D::Form::OmegaDependentGaussian].setNormalisationFunction(
            [](double omega, const std::vector<double> &params) { return 1.0 / (params[1] * omega * sqrt(2.0 * M_PI)); });

        /*
         * Gaussian with omega-independent and omega-dependent fwhm
         *
         * Parameters:
         * INPUT  0 = fwhm1
         * INPUT  1 = fwhm2  (omega-dependent)
         * CALC   2 = c1 = fwhm1 / (2 * sqrt(2 ln 2))
         * CALC   3 = c2 = fwhm2 / (2 * sqrt(2 ln 2))
         * CALC   4 = 1.0 / c1
         * CALC   5 = 1.0 / c2
         *
         *            (            x * x         )
         * f(x) = exp ( - ---------------------- )
         *            (   2 * (c1 + c2*omega)**2 )
         */
        functions[Functions1D::Form::GaussianC2] = Function1DDefinition(
            {"fwhm", "fwhm(x)"}, [](double x, double omega, const std::vector<double> &params)
            { return exp(-(x * x) / (2.0 * (params[2] + params[3] * omega) * (params[2] + params[3] * omega))); });
        functions[Functions1D::Form::GaussianC2].setSetupFunction(
            [](std::vector<double> params)
            {
                params.push_back(params[0] / (2.0 * sqrt(2.0 * log(2.0))));
                params.push_back(params[1] / (2.0 * sqrt(2.0 * log(2.0))));
                params.push_back(1.0 / params[2]);
                params.push_back(1.0 / params[3]);
                return params;
            });
        /*
         *             (   x * x * (c1 + c2*omega)**2 )
         * FT(x) = exp ( - -------------------------- )
         *             (                2             )
         */
        functions[Functions1D::Form::GaussianC2].setFTFunction(
            [](double x, double omega, const std::vector<double> &params)
            { return exp(-(0.5 * x * x * (params[2] + params[3] * omega) * (params[2] + params[3] * omega))); });
        /*
         *                    1
         * Norm = --------------------------
         *        (c1 + c2 omega) sqrt(2 pi)
         */
        functions[Functions1D::Form::GaussianC2].setNormalisationFunction(
            [](double omega, const std::vector<double> &params)
            { return 1.0 / ((params[2] + params[3] * omega) * sqrt(2.0 * M_PI)); });

        /*
         * Lennard-Jones 12-6 Potential
         *
         * Parameters:
         * INPUT  0 = epsilon
         * INPUT  1 = sigma
         *
         *                      [ ( sigma )**12   ( sigma )**6 ]
         * F(x) = 4 * epsilon * [ ( ----- )     - ( ----- )    ]
         *                      [ (   x   )       (   x   )    ]
         */
        functions[Functions1D::Form::LennardJones126] =
            Function1DDefinition({"epsilon", "sigma"},
                                 [](double x, double omega, const std::vector<double> &params)
                                 {
                                     auto sigmar = params[1] / x;
                                     auto sigmar6 = pow(sigmar, 6.0);
                                     auto sigmar12 = sigmar6 * sigmar6;
                                     return 4.0 * params[0] * (sigmar12 - sigmar6);
                                 });
        /*
         *                           [ ( sigma**12 )         ( sigma**6 ) ]
         * dYdX(x) = -48 * epsilon * [ ( --------- ) - 0.5 * ( -------- ) ]
         *                           [ (   x**13   )         (   x**7   ) ]
         */
        functions[Functions1D::Form::LennardJones126].setDerivativeFunction(
            [](double x, double omega, const std::vector<double> &params)
            {
                auto sigmar = params[1] / x;
                auto sigmar6 = pow(sigmar, 6.0);
                return 48.0 * params[0] * sigmar6 * (-sigmar6 + 0.5) / x;
            });

        /*
         * Buckingham Potential
         *
         * Parameters:
         * INPUT  0 = A
         * INPUT  1 = B
         * INPUT  2 = C
         *
         *                       C
         * F(x)=A exp(-B * x) - ----
         *                      x**6
         */
        functions[Functions1D::Form::Buckingham] =
            Function1DDefinition({"A", "B", "C"},
                                 [](double x, double omega, const std::vector<double> &params)
                                 {
                                     auto B = exp(-params[1] * x);
                                     auto C = params[2] / pow(x, 6.0);
                                     return params[0] * B - C;
                                 });
        /*
         * dYdX(x) = -B * A exp(-B * x) + 6 * C * x**-7
         */
        functions[Functions1D::Form::Buckingham].setDerivativeFunction(
            [](double x, double omega, const std::vector<double> &params)
            {
                auto expo = exp(-params[1] * x);
                auto C = 6 * params[2] * pow(x, -7.0);
                return -params[1] * params[0] * expo + C;
            });

        /*
         * GaussianPotential with prefactor, located at specific x.
         * Intended for use as a potential override.
         *
         * Parameters:
         * INPUT  0 = A
         * INPUT  1 = fwhm
         * INPUT  2 = x0
         * CALC   3 = c = fwhm / (2 * sqrt(2 ln 2))
         * CALC   4 = 1.0 / c
         *
         *            (     x * x   )
         * f(x) = exp ( - --------- )
         *            (   2 * c * c )
         */

        functions[Functions1D::Form::GaussianPotential] = Function1DDefinition(
            {"A", "fwhm", "x0"}, [](double x, double omega, const std::vector<double> &params)
            { return params[0] * exp(-(0.5 * (x - params[2]) * (x - params[2]) * params[4] * params[4])); });
        functions[Functions1D::Form::GaussianPotential].setSetupFunction(
            [](std::vector<double> params)
            {
                params.push_back(params[1] / (2.0 * sqrt(2.0 * log(2.0))));
                params.push_back(1.0 / params[3]);
                return params;
            });
        /*
         *              1             (     x * x   )
         * dYdX(x) = - ---- * x * exp ( - --------- )
         *             c**2           (   2 * c * c )
         */
        functions[Functions1D::Form::GaussianPotential].setDerivativeFunction(
            [](double x, double omega, const std::vector<double> &params)
            {
                auto dx = x - params[2];
                auto c1 = (1 / (params[3] * params[3])) * dx;
                return -params[0] * c1 * exp(-(0.5 * dx * dx * params[4] * params[4]));
            });

        /*
         * Harmonic Well Potential
         *
         * Parameters:
         * INPUT  0 = k
         */
        functions[Functions1D::Form::Harmonic] = Function1DDefinition(
            {"k"}, [](double x, double omega, const std::vector<double> &params) { return 0.5 * params[0] * x * x; });
        /*
         * dYdX(x) = k * x
         */
        functions[Functions1D::Form::Harmonic].setDerivativeFunction(
            [](double x, double omega, const std::vector<double> &params) { return params[0] * x; });

        /*
         * Coulombic Potential
         *
         * Parameters:
         * INPUT  0 = k
         * INPUT  1 = q1
         * INPUT  2 = q2
         *
         *          q1 * q2
         * F(x)=k - -------
         *           x * x
         */
        functions[Functions1D::Form::Coulombic] =
            Function1DDefinition({"k", "q1", "q2"}, [](double x, double omega, const std::vector<double> &params)
                                 { return (params[0] * params[1] * params[2]) / pow(x, 2.0); });
        /*
         * dYdX(x) = -2 * k * q1 * q2 * r**-3
         */
        functions[Functions1D::Form::Coulombic].setDerivativeFunction(
            [](double x, double omega, const std::vector<double> &params)
            { return (-2 * params[0] * params[1] * params[2]) / pow(x, 3.0); });
    }

    return functions;
};

// Return enum option info for forms
EnumOptions<Functions1D::Form> Functions1D::forms()
{
    return EnumOptions<Functions1D::Form>("Function1D",
                                          {{Functions1D::Form::None, "None"},
                                           {Functions1D::Form::Gaussian, "Gaussian", 1},
                                           {Functions1D::Form::ScaledGaussian, "ScaledGaussian", 2},
                                           {Functions1D::Form::OmegaDependentGaussian, "OmegaDependentGaussian", 1},
                                           {Functions1D::Form::GaussianC2, "GaussianC2", 2},
                                           {Functions1D::Form::LennardJones126, "LennardJones126", 2},
                                           {Functions1D::Form::Buckingham, "Buckingham", 3},
                                           {Functions1D::Form::GaussianPotential, "GaussianPotential", 3},
                                           {Functions1D::Form::Harmonic, "Harmonic", 1},
                                           {Functions1D::Form::Coulombic, "Coulombic", 3}});
}

// Return parameters for specified form
const std::vector<std::string> &Functions1D::parameters(Form form) { return functions1D().at(form).parameterNames(); }

// Return nth parameter for the given form
std::string Functions1D::parameter(Form form, int n) { return functions1D().at(form).parameterNames()[n]; }

// Return index of parameter in the given form
std::optional<int> Functions1D::parameterIndex(Form form, std::string_view name)
{
    auto it = std::find(functions1D().at(form).parameterNames().begin(), functions1D().at(form).parameterNames().end(), name);
    if (it == functions1D().at(form).parameterNames().end())
        return {};
    return it - functions1D().at(form).parameterNames().begin();
}

// Return base function requested
Function1DDefinition Functions1D::functionDefinition1D(Functions1D::Form form) { return functions1D().at(form); }

// Check function properties against those supplied, returning truth if the function meets all requirements
bool Functions1D::validFunction1DProperties(Functions1D::Form form,
                                            const Flags<FunctionProperties::FunctionProperty> &properties)
{
    return (functions1D().at(form).properties() & properties) == properties;
}

// Return all available functions with properties matching those provided
std::vector<Functions1D::Form> Functions1D::matchingFunction1D(const Flags<FunctionProperties::FunctionProperty> &properties)
{
    std::vector<Functions1D::Form> matches;
    for (auto n = 0; n < forms().nOptions(); ++n)
        if (validFunction1DProperties(forms().enumerationByIndex(n), properties))
            matches.push_back(forms().enumerationByIndex(n));
    return matches;
}

/*
 * One-Dimensional Function Wrapper
 */

Function1DWrapper::Function1DWrapper(Functions1D::Form form, const std::vector<double> &params)
    : form_(form), function_(functions1D().at(form)), parameters_(params)
{
    calculateInternalParameters();
}

// Initialise internal function parameters from current base parameters
void Function1DWrapper::calculateInternalParameters()
{
    internalParameters_ = function_.setup() ? function_.setup()(parameters_) : parameters_;
}

// Set function type and parameters
bool Function1DWrapper::setFormAndParameters(Functions1D::Form form, const std::vector<double> &params)
{
    form_ = form;
    function_ = functions1D().at(form_);

    if (params.size() != function_.nParameters())
        return Messenger::error("1D function '{}' requires {} parameters, but {} were given.\n",
                                Functions1D::forms().keyword(form_), function_.nParameters(), params.size());

    parameters_ = params;
    calculateInternalParameters();

    return true;
}

// Set current functional form
void Function1DWrapper::setForm(Functions1D::Form form)
{
    form_ = form;
    function_ = functions1D().at(form_);
    calculateInternalParameters();
}

// Return functional form
Functions1D::Form Function1DWrapper::form() const { return form_; }

// Set current function parameters
bool Function1DWrapper::setParameters(const std::vector<double> &params)
{
    if (params.size() != function_.nParameters())
        return Messenger::error("1D function '{}' requires {} parameters, but {} were given.\n",
                                Functions1D::forms().keyword(form_), function_.nParameters(), params.size());

    parameters_ = params;
    calculateInternalParameters();

    return true;
}

// Return number of parameters for current function
int Function1DWrapper::nParameters() const { return function_.nParameters(); }

// Return current parameters
const std::vector<double> &Function1DWrapper::parameters() const { return parameters_; }

// Return name of nth parameter
std::string Function1DWrapper::parameterName(int i) const { return function_.parameterNames()[i]; }

// Return parameter summary ("name = value, ...")
std::string Function1DWrapper::parameterSummary() const
{
    std::string summary;
    for (const auto &[name, p] : zip(function_.parameterNames(), parameters_))
        summary += fmt::format("{}{} = {}", summary.empty() ? "" : ", ", name, p);
    return summary;
}

// Return y value at specified x, omega
double Function1DWrapper::y(double x, double omega) const { return function_.y()(x, omega, internalParameters_); }

// Return first derivative of y at specified x, omega
double Function1DWrapper::dYdX(double x, double omega) const
{
    return function_.dYdX() ? function_.dYdX()(x, omega, internalParameters_) : 0.0;
}

// Return Fourier transformed y value at specified x, omega
double Function1DWrapper::yFT(double x, double omega) const
{
    return function_.yFT() ? function_.yFT()(x, omega, internalParameters_) : 0.0;
}

// Return normalisation factor at specified omega
double Function1DWrapper::normalisation(double omega) const
{
    return function_.normalisation() ? function_.normalisation()(omega, internalParameters_) : 1.0;
}
