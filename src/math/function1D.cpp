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

Function1DDefinition::Function1DDefinition(const std::vector<std::string> &parameterNames,
                                           const Flags<FunctionProperties::FunctionProperty> &properties, Function1DSetup setup,
                                           Function1DXOmega y, Function1DXOmega yFT, Function1DOmega norm)
    : parameterNames_(parameterNames), properties_(properties), setup_(std::move(setup)), y_(std::move(y)),
      yFT_(std::move(yFT)), normaliser_(std::move(norm))
{
}

// Return number of parameters the function requires
int Function1DDefinition::nParameters() const { return parameterNames_.size(); }

// Return parameter names
const std::vector<std::string> &Function1DDefinition::parameterNames() const { return parameterNames_; }

// Return properties of the function
const Flags<FunctionProperties::FunctionProperty> &Function1DDefinition::properties() const { return properties_; }

// Return function for setup
Function1DSetup Function1DDefinition::setup() const { return setup_; }

// Return function for y value at specified x, omega
Function1DXOmega Function1DDefinition::y() const { return y_; }

// Return function for FT of y value at the specified x, omega
Function1DXOmega Function1DDefinition::yFT() const { return yFT_; }

// Return normalisation function
Function1DOmega Function1DDefinition::normalisation() const { return normaliser_; }

// One-Dimensional Function Definitions
static std::map<Functions1D::Form, Function1DDefinition> functions1D_ = {
    // No Function - returns zero
    {Functions1D::Form::None,
     {{},
      {FunctionProperties::FourierTransform, FunctionProperties::Normalisation},
      [](std::vector<double> params) { return params; },
      [](double x, double omega, const std::vector<double> &params) { return 0.0; }}},
    /*
     * Gaussian
     *
     * Parameters:
     *  INPUT  0 = fwhm
     *  CALC   1 = c = fwhm / (2 * sqrt(2 ln 2))
     *  CALC   2 = 1.0 / c
     */
    {Functions1D::Form::Gaussian,
     {{"fwhm"},
      {FunctionProperties::FourierTransform, FunctionProperties::Normalisation},
      [](std::vector<double> params)
      {
          params.push_back(params[0] / (2.0 * sqrt(2.0 * log(2.0))));
          params.push_back(1.0 / params[1]);
          return params;
      },
      /*
       *            (     x * x   )
       * f(x) = exp ( - --------- )
       * 	        (   2 * c * c )
       */
      [](double x, double omega, const std::vector<double> &params) { return exp(-(0.5 * x * x * params[2] * params[2])); },
      /*
       *             (   x * x * c * c )
       * FT(x) = exp ( - ------------- )
       *             (         2       )
       */
      [](double x, double omega, const std::vector<double> &params) { return exp(-(0.5 * x * x * params[1] * params[1])); },
      /*
       *             1
       * Norm = ------------
       *        c sqrt(2 pi)
       */
      [](double omega, const std::vector<double> &params) { return params[2] / sqrt(2.0 * M_PI); }}},
    /*
     * Gaussian with prefactor
     *
     * Parameters:
     *  INPUT  0 = A
     *  INPUT  1 = fwhm
     *  CALC   2 = c = fwhm / (2 * sqrt(2 ln 2))
     *  CALC   3 = 1.0 / c
     */
    {Functions1D::Form::ScaledGaussian,
     {{"A", "fwhm"},
      {FunctionProperties::FourierTransform, FunctionProperties::Normalisation},
      [](std::vector<double> params)
      {
          params.push_back(params[1] / (2.0 * sqrt(2.0 * log(2.0))));
          params.push_back(1.0 / params[2]);
          return params;
      },
      /*
       *              (     x * x   )
       * f(x) = A exp ( - --------- )
       *              (   2 * c * c )
       */
      [](double x, double omega, const std::vector<double> &params)
      { return params[0] * exp(-(0.5 * x * x * params[3] * params[3])); },
      /*
       *               (   x * x * c * c )
       * FT(x) = A exp ( - ------------- )
       *               (         2       )
       */
      [](double x, double omega, const std::vector<double> &params)
      { return params[0] * exp(-(0.5 * x * x * params[2] * params[2])); },
      /*
       *              1
       * Norm = --------------
       *        A c sqrt(2 pi)
       */
      [](double omega, const std::vector<double> &params) { return params[3] / (params[0] * sqrt(2.0 * M_PI)); }}},
    /*
     * Gaussian with omega-dependent fwhm
     *
     * Parameters:
     * INPUT  0 = fwhm
     * CALC   1 = c = fwhm / (2 * sqrt(2 ln 2))
     * CALC   2 = 1.0 / c
     */
    {Functions1D::Form::OmegaDependentGaussian,
     {{"fwhm(x)"},
      {FunctionProperties::FourierTransform, FunctionProperties::Normalisation},
      [](std::vector<double> params)
      {
          params.push_back(params[0] / (2.0 * sqrt(2.0 * log(2.0))));
          params.push_back(1.0 / params[1]);
          return params;
      },
      /*
       *            (         x * x      )
       * f(x) = exp ( - ---------------- )
       *            (   2 * (c*omega)**2 )
       */
      [](double x, double omega, const std::vector<double> &params)
      { return exp(-(x * x) / (2.0 * (params[1] * omega) * (params[1] * omega))); },
      /*
       *             (   x*x * (c*omega)**2 )
       * FT(x) = exp ( - ------------------ )
       *             (            2         )
       */
      [](double x, double omega, const std::vector<double> &params)
      { return exp(-(0.5 * x * x * (params[1] * omega) * (params[1] * omega))); },
      /*
       *                1
       * Norm = ------------------
       *        c omega sqrt(2 pi)
       */
      [](double omega, const std::vector<double> &params) { return 1.0 / (params[1] * omega * sqrt(2.0 * M_PI)); }}},
    /*
     * Gaussian with omega-independent and omega-dependent fwhm
     *
     * Parameters:
     *  INPUT  0 = fwhm1
     *  INPUT  1 = fwhm2  (omega-dependent)
     *  CALC   2 = c1 = fwhm1 / (2 * sqrt(2 ln 2))
     *  CALC   3 = c2 = fwhm2 / (2 * sqrt(2 ln 2))
     *  CALC   4 = 1.0 / c1
     *  CALC   5 = 1.0 / c2
     */
    {Functions1D::Form::GaussianC2,
     {{"fwhm", "fwhm(x)"},
      {FunctionProperties::FourierTransform},
      [](std::vector<double> params)
      {
          params.push_back(params[0] / (2.0 * sqrt(2.0 * log(2.0))));
          params.push_back(params[1] / (2.0 * sqrt(2.0 * log(2.0))));
          params.push_back(1.0 / params[2]);
          params.push_back(1.0 / params[3]);
          return params;
      },
      /*
       *            (            x * x         )
       * f(x) = exp ( - ---------------------- )
       *            (   2 * (c1 + c2*omega)**2 )
       */
      [](double x, double omega, const std::vector<double> &params)
      { return exp(-(x * x) / (2.0 * (params[2] + params[3] * omega) * (params[2] + params[3] * omega))); },
      /*
       *             (   x * x * (c1 + c2*omega)**2 )
       * FT(x) = exp ( - -------------------------- )
       *             (                2             )
       */
      [](double x, double omega, const std::vector<double> &params)
      { return exp(-(0.5 * x * x * (params[2] + params[3] * omega) * (params[2] + params[3] * omega))); },
      /*
       *                    1
       * Norm = --------------------------
       *        (c1 + c2 omega) sqrt(2 pi)
       */
      [](double omega, const std::vector<double> &params)
      { return 1.0 / ((params[2] + params[3] * omega) * sqrt(2.0 * M_PI)); }}}};

// Return enum option info for forms
EnumOptions<Functions1D::Form> Functions1D::forms()
{
    return EnumOptions<Functions1D::Form>("Function1D", {{Functions1D::Form::None, "None"},
                                                         {Functions1D::Form::Gaussian, "Gaussian"},
                                                         {Functions1D::Form::ScaledGaussian, "ScaledGaussian"},
                                                         {Functions1D::Form::OmegaDependentGaussian, "OmegaDependentGaussian"},
                                                         {Functions1D::Form::GaussianC2, "GaussianC2"}});
}

// Return parameters for specified form
const std::vector<std::string> &Functions1D::parameters(Form form) { return functions1D_.at(form).parameterNames(); }

// Return nth parameter for the given form
std::string Functions1D::parameter(Form form, int n) { return functions1D_.at(form).parameterNames()[n]; }

// Return index of parameter in the given form
std::optional<int> Functions1D::parameterIndex(Form form, std::string_view name)
{
    auto it = std::find(functions1D_.at(form).parameterNames().begin(), functions1D_.at(form).parameterNames().end(), name);
    if (it == functions1D_.at(form).parameterNames().end())
        return {};
    return it - functions1D_.at(form).parameterNames().begin();
}

// Return base function requested
Function1DDefinition Functions1D::functionDefinition1D(Functions1D::Form form) { return functions1D_.at(form); }

// Check function properties against those supplied, returning truth if the function meets all requirements
bool Functions1D::validFunction1DProperties(Functions1D::Form form,
                                            const Flags<FunctionProperties::FunctionProperty> &properties)
{
    return (functions1D_.at(form).properties() & properties) == properties;
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
    : form_(form), function_(functions1D_.at(form)), parameters_(params)
{
    calculateInternalParameters();
}

// Initialise internal function parameters from current base parameters
void Function1DWrapper::calculateInternalParameters() { internalParameters_ = function_.setup()(parameters_); }

// Set function type and parameters
bool Function1DWrapper::setFormAndParameters(Functions1D::Form form, const std::vector<double> &params)
{
    form_ = form;
    function_ = functions1D_.at(form_);

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
    function_ = functions1D_.at(form_);
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
