// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#define _USE_MATH_DEFINES
#include "math/function1D.h"
#include "templates/algorithms.h"
#include <math.h>

#include <map>

namespace Functions
{
/*
 * One-Dimensional Function Definition
 */

FunctionDefinition1D::FunctionDefinition1D(std::vector<std::string> parameterNames, int properties, FunctionSetup setup,
                                           Function1DXOmega y, Function1DXOmega yFT, Function1DOmega norm)
    : parameterNames_(parameterNames), properties_(properties), setup_(std::move(setup)), y_(std::move(y)),
      yFT_(std::move(yFT)), normaliser_(std::move(norm))
{
}

// Return number of parameters the function requires
int FunctionDefinition1D::nParameters() const { return parameterNames_.size(); }

// Return parameter names
const std::vector<std::string> &FunctionDefinition1D::parameterNames() const { return parameterNames_; }

// Return properties of the function
int FunctionDefinition1D::properties() const { return properties_; }

// Return function for setup
FunctionSetup FunctionDefinition1D::setup() const { return setup_; }

// Return function for y value at specified x, omega
Function1DXOmega FunctionDefinition1D::y() const { return y_; }

// Return function for FT of y value at the specified x, omega
Function1DXOmega FunctionDefinition1D::yFT() const { return yFT_; }

// Return normalisation function
Function1DOmega FunctionDefinition1D::normalisation() const { return normaliser_; }

// One-Dimensional Function Definitions
static std::map<Function1D, FunctionDefinition1D> functions1D_ = {
    // No Function - always returns 1.0
    {Function1D::None,
     {{},
      FunctionProperties::FourierTransform | FunctionProperties::Normalisation,
      [](std::vector<double> p) { return p; },
      [](double x, double omega, const std::vector<double> &p) { return 1.0; },
      [](double x, double omega, const std::vector<double> &p) { return 1.0; }}},
    /*
     * Gaussian
     *
     * Parameters:
     *  INPUT  0 = fwhm
     *  CALC   1 = c = fwhm / (2 * sqrt(2 ln 2))
     *  CALC   2 = 1.0 / c
     */
    {Function1D::Gaussian,
     {{"fwhm"},
      FunctionProperties::FourierTransform | FunctionProperties::Normalisation,
      [](std::vector<double> p)
      {
          p.push_back(p[0] / (2.0 * sqrt(2.0 * log(2.0))));
          p.push_back(1.0 / p[1]);
          return p;
      },
      /*
       *            (     x * x   )
       * f(x) = exp ( - --------- )
       * 	        (   2 * c * c )
       */
      [](double x, double omega, const std::vector<double> &p) { return exp(-(0.5 * x * x * p[2] * p[2])); },
      /*
       *             (   x * x * c * c )
       * FT(x) = exp ( - ------------- )
       *             (         2       )
       */
      [](double x, double omega, const std::vector<double> &p) { return exp(-(0.5 * x * x * p[1] * p[1])); },
      /*
       *             1
       * Norm = ------------
       *        c sqrt(2 pi)
       */
      [](double omega, const std::vector<double> &p) { return p[2] / sqrt(2.0 * M_PI); }}},
    /*
     * Gaussian with prefactor
     *
     * Parameters:
     *  INPUT  0 = A
     *  INPUT  1 = fwhm
     *  CALC   2 = c = fwhm / (2 * sqrt(2 ln 2))
     *  CALC   3 = 1.0 / c
     */
    {Function1D::ScaledGaussian,
     {{"A", "fwhm"},
      FunctionProperties::FourierTransform | FunctionProperties::Normalisation,
      [](std::vector<double> p)
      {
          p.push_back(p[1] / (2.0 * sqrt(2.0 * log(2.0))));
          p.push_back(1.0 / p[2]);
          return p;
      },
      /*
       *              (     x * x   )
       * f(x) = A exp ( - --------- )
       *              (   2 * c * c )
       */
      [](double x, double omega, const std::vector<double> &p) { return p[0] * exp(-(0.5 * x * x * p[3] * p[3])); },
      /*
       *               (   x * x * c * c )
       * FT(x) = A exp ( - ------------- )
       *               (         2       )
       */
      [](double x, double omega, const std::vector<double> &p) { return p[0] * exp(-(0.5 * x * x * p[2] * p[2])); },
      /*
       *              1
       * Norm = --------------
       *        A c sqrt(2 pi)
       */
      [](double omega, const std::vector<double> &p) { return p[3] / (p[0] * sqrt(2.0 * M_PI)); }}},
    /*
     * Gaussian with omega-dependent fwhm
     *
     * Parameters:
     * INPUT  0 = fwhm
     * CALC   1 = c = fwhm / (2 * sqrt(2 ln 2))
     * CALC   2 = 1.0 / c
     */
    {Function1D::OmegaDependentGaussian,
     {{"fwhm(x)"},
      FunctionProperties::FourierTransform | FunctionProperties::Normalisation,
      [](std::vector<double> p)
      {
          p.push_back(p[0] / (2.0 * sqrt(2.0 * log(2.0))));
          p.push_back(1.0 / p[1]);
          return p;
      },
      /*
       *            (         x * x      )
       * f(x) = exp ( - ---------------- )
       *            (   2 * (c*omega)**2 )
       */
      [](double x, double omega, const std::vector<double> &p)
      { return exp(-(x * x) / (2.0 * (p[1] * omega) * (p[1] * omega))); },
      /*
       *             (   x*x * (c*omega)**2 )
       * FT(x) = exp ( - ------------------ )
       *             (            2         )
       */
      [](double x, double omega, const std::vector<double> &p)
      { return exp(-(0.5 * x * x * (p[1] * omega) * (p[1] * omega))); },
      /*
       *                1
       * Norm = ------------------
       *        c omega sqrt(2 pi)
       */
      [](double omega, const std::vector<double> &p) { return 1.0 / (p[1] * omega * sqrt(2.0 * M_PI)); }}},
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
    {Function1D::GaussianC2,
     {{"fwhm", "fwhm(x)"},
      FunctionProperties::FourierTransform,
      [](std::vector<double> p)
      {
          p.push_back(p[0] / (2.0 * sqrt(2.0 * log(2.0))));
          p.push_back(p[1] / (2.0 * sqrt(2.0 * log(2.0))));
          p.push_back(1.0 / p[2]);
          p.push_back(1.0 / p[3]);
          return p;
      },
      /*
       *            (            x * x         )
       * f(x) = exp ( - ---------------------- )
       *            (   2 * (c1 + c2*omega)**2 )
       */
      [](double x, double omega, const std::vector<double> &p)
      { return exp(-(x * x) / (2.0 * (p[2] + p[3] * omega) * (p[2] + p[3] * omega))); },
      /*
       *             (   x * x * (c1 + c2*omega)**2 )
       * FT(x) = exp ( - -------------------------- )
       *             (                2             )
       */
      [](double x, double omega, const std::vector<double> &p)
      { return exp(-(0.5 * x * x * (p[2] + p[3] * omega) * (p[2] + p[3] * omega))); },
      /*
       *                    1
       * Norm = --------------------------
       *        (c1 + c2 omega) sqrt(2 pi)
       */
      [](double omega, const std::vector<double> &p) { return 1.0 / ((p[2] + p[3] * omega) * sqrt(2.0 * M_PI)); }}}};

// Return enum option info for Function1D
EnumOptions<Function1D> function1D()
{
    return EnumOptions<Function1D>("Function1D", {{Function1D::None, "None"},
                                                  {Function1D::Gaussian, "Gaussian"},
                                                  {Function1D::ScaledGaussian, "ScaledGaussian"},
                                                  {Function1D::OmegaDependentGaussian, "OmegaDependentGaussian"},
                                                  {Function1D::GaussianC2, "GaussianC2"}});
}

// Return base function requested
FunctionDefinition1D functionDefinition1D(Functions::Function1D func) { return functions1D_.at(func); }

// Check function properties against those supplied, returning truth if the function meets all requirements
bool validFunction1DProperties(Function1D func, int properties)
{
    return properties == FunctionProperties::None || (functions1D_.at(func).properties() & properties) == properties;
}

// Return all available functions with properties matching those provided
std::vector<Function1D> matchingFunction1D(int properties)
{
    std::vector<Function1D> matches;
    for (auto n = 0; n < function1D().nOptions(); ++n)
        if (validFunction1DProperties(function1D().enumerationByIndex(n), properties))
            matches.push_back(function1D().enumerationByIndex(n));
    return matches;
}

/*
 * One-Dimensional Function Wrapper
 */

Function1DWrapper::Function1DWrapper(Function1D func, std::vector<double> params)
    : type_(func), function_(functions1D_.at(func)), parameters_(params)
{
    calculateInternalParameters();
}

// Initialise internal function parameters from current base parameters
void Function1DWrapper::calculateInternalParameters() { internalParameters_ = function_.setup()(parameters_); }

// Set function type and parameters
bool Function1DWrapper::setFunctionAndParameters(Function1D func, std::vector<double> params)
{
    type_ = func;
    function_ = functions1D_.at(type_);

    if (params.size() != function_.nParameters())
        return Messenger::error("1D function '{}' requires {} parameters, but {} were given.\n", function1D().keyword(type_),
                                function_.nParameters(), params.size());

    parameters_ = std::move(params);
    calculateInternalParameters();

    return true;
}

// Set current function type
void Function1DWrapper::setFunction(Function1D func)
{
    type_ = func;
    function_ = functions1D_.at(type_);
    calculateInternalParameters();
}

// Return function type
Function1D Function1DWrapper::type() const { return type_; }

// Set current function parameters
bool Function1DWrapper::setParameters(std::vector<double> params)
{
    if (params.size() != function_.nParameters())
        return Messenger::error("1D function '{}' requires {} parameters, but {} were given.\n", function1D().keyword(type_),
                                function_.nParameters(), params.size());

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
} // namespace Functions
