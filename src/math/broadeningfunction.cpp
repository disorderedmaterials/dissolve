// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "math/broadeningfunction.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

BroadeningFunction::BroadeningFunction(BroadeningFunction::FunctionType function, double p1, double p2, double p3, double p4,
                                       double p5, double p6)
{
    set(function, p1, p2, p3, p4, p5, p6);

    inverted_ = false;
}

BroadeningFunction::~BroadeningFunction() {}

BroadeningFunction::BroadeningFunction(const BroadeningFunction &source) { (*this) = source; }

void BroadeningFunction::operator=(const BroadeningFunction &source)
{
    function_ = source.function_;
    for (auto n = 0; n < MAXBROADENINGFUNCTIONPARAMS; ++n)
        parameters_[n] = source.parameters_[n];
    inverted_ = source.inverted_;
    staticOmega_ = source.staticOmega_;
}

std::string_view BroadeningFunctionKeywords[] = {"None", "Gaussian", "ScaledGaussian", "OmegaDependentGaussian", "GaussianC2"};
int BroadeningFunctionNParameters[] = {0, 1, 2, 1, 2};

std::string_view BroadeningFunctionParameters[][MAXBROADENINGFUNCTIONPARAMS] = {{
                                                                                    "",
                                                                                    "",
                                                                                    "",
                                                                                    "",
                                                                                    "",
                                                                                    "",
                                                                                },
                                                                                {
                                                                                    "FWHM",
                                                                                    "",
                                                                                    "",
                                                                                    "",
                                                                                    "",
                                                                                    "",
                                                                                },
                                                                                {
                                                                                    "A",
                                                                                    "FWHM",
                                                                                    "",
                                                                                    "",
                                                                                    "",
                                                                                    "",
                                                                                },
                                                                                {
                                                                                    "FWHM",
                                                                                    "",
                                                                                    "",
                                                                                    "",
                                                                                    "",
                                                                                    "",
                                                                                },
                                                                                {
                                                                                    "FWHM1 (independent)",
                                                                                    "FWHM2 (dependent)",
                                                                                    "",
                                                                                    "",
                                                                                    "",
                                                                                    "",
                                                                                }};

// Return FunctionType from supplied string
BroadeningFunction::FunctionType BroadeningFunction::functionType(std::string_view s)
{
    for (auto n = 0; n < nFunctionTypes; ++n)
        if (DissolveSys::sameString(s, BroadeningFunctionKeywords[n]))
            return (FunctionType)n;
    return BroadeningFunction::nFunctionTypes;
}

// Return FunctionType name
std::string_view BroadeningFunction::functionType(BroadeningFunction::FunctionType func)
{
    return BroadeningFunctionKeywords[func];
}

// Return number of parameters needed to define function
int BroadeningFunction::nFunctionParameters(FunctionType func) { return BroadeningFunctionNParameters[func]; }

// Return description for FunctionType
std::string_view BroadeningFunction::functionDescription(FunctionType func)
{
    switch (func)
    {
        case (BroadeningFunction::NoFunction):
            return "Function that always returns 1.0";
            break;
        case (BroadeningFunction::GaussianFunction):
            return "Gaussian (no prefactor, unnormalised)";
            break;
        case (BroadeningFunction::ScaledGaussianFunction):
            return "Gaussian with prefactor";
            break;
        case (BroadeningFunction::OmegaDependentGaussianFunction):
            return "Gaussian (no prefactor, unnormalised, omega dependent FWHM)";
            break;
        case (BroadeningFunction::GaussianC2Function):
            return "Gaussian (no prefactor, unnormalised, independent and omega-dependent FWHM, EPSR-style)";
            break;
        default:
            break;
    }

    return "NO BROADENING FUNCTION DESCRIPTION AVAILABLE.";
}

/*
 * Function Data
 */

void BroadeningFunction::set(BroadeningFunction::FunctionType function, double p1, double p2, double p3, double p4, double p5,
                             double p6)
{
    function_ = function;
    parameters_[0] = p1;
    parameters_[1] = p2;
    parameters_[2] = p3;
    parameters_[3] = p4;
    parameters_[4] = p5;
    parameters_[5] = p6;

    // Set up any necessary dependent parameters
    setUpDependentParameters();
}

// Set function data from LineParser source
bool BroadeningFunction::set(LineParser &parser, int startArg)
{
    // Zero all parameters before we start
    for (auto n = 0; n < MAXBROADENINGFUNCTIONPARAMS; ++n)
        parameters_[n] = 0.0;

    // First argument is the form of the function
    BroadeningFunction::FunctionType funcType = BroadeningFunction::functionType(parser.argsv(startArg));
    if (funcType == BroadeningFunction::nFunctionTypes)
    {
        Messenger::error("Unrecognised Function type '{}'.\n", parser.argsv(startArg));
        return false;
    }

    // Do we have the right number of arguments for the function specified?
    if ((parser.nArgs() - startArg) < BroadeningFunction::nFunctionParameters(funcType))
    {
        Messenger::error("Too few parameters supplied for Function '{}' (expected {}, found {}).\n",
                         BroadeningFunction::functionType(funcType), BroadeningFunction::nFunctionParameters(funcType),
                         parser.nArgs() - startArg);
        return false;
    }

    // Set up function and basic parameters
    function_ = funcType;
    switch (function_)
    {
        case (BroadeningFunction::NoFunction):
            break;
        case (BroadeningFunction::GaussianFunction):
        case (BroadeningFunction::OmegaDependentGaussianFunction):
            // FWHM
            parameters_[0] = parser.argd(startArg + 1);
            break;
        case (BroadeningFunction::ScaledGaussianFunction):
            // Prefactor A
            parameters_[0] = parser.argd(startArg + 1);
            // FWHM
            parameters_[1] = parser.argd(startArg + 2);
            break;
        case (BroadeningFunction::GaussianC2Function):
            // FWHM1
            parameters_[0] = parser.argd(startArg + 1);
            // FWHM2
            parameters_[1] = parser.argd(startArg + 2);
            break;
        default:
            Messenger::error("Function form '{}' not accounted for in BroadeningFunction::set(LineParser&,int).\n",
                             BroadeningFunction::functionType(funcType));
            return false;
    }

    // Set up any necessary dependent parameters
    setUpDependentParameters();

    return true;
}

// Return function type
BroadeningFunction::FunctionType BroadeningFunction::function() const { return function_; }

// Return number of parameters required
int BroadeningFunction::nParameters() const { return nFunctionParameters(function_); }

// Return specified parameter
double BroadeningFunction::parameter(int index) const { return parameters_[index]; }

// Return parameters array
double *BroadeningFunction::parameters() { return parameters_; }

// Return specified parameter name
std::string_view BroadeningFunction::parameterName(int index) const { return BroadeningFunctionParameters[function_][index]; }

// Return short summary of function parameters
std::string BroadeningFunction::parameterSummary() const
{
    if (BroadeningFunctionNParameters[function_] == 0)
        return "<No Parameters>";

    std::string result;
    for (auto n = 0; n < BroadeningFunctionNParameters[function_]; ++n)
        result += fmt::format("{}{}={}", n == 0 ? "" : ", ", BroadeningFunctionParameters[function_][n], parameters_[n]);

    return result;
}

// Set up any dependent parameters based on the input set of parameters
void BroadeningFunction::setUpDependentParameters()
{
    switch (function_)
    {
        case (BroadeningFunction::NoFunction):
            break;
        case (BroadeningFunction::GaussianFunction):
        case (BroadeningFunction::OmegaDependentGaussianFunction):
            // parameters_[0] = FWHM
            // c (calculated from FWHM)
            parameters_[1] = parameters_[0] / (2.0 * sqrt(2.0 * log(2.0)));
            // 1/c
            parameters_[2] = 1.0 / parameters_[1];
            break;
        case (BroadeningFunction::ScaledGaussianFunction):
            // parameters_[0] = A
            // parameters_[1] = FWHM
            // c (calculated from FWHM)
            parameters_[2] = parameters_[1] / (2.0 * sqrt(2.0 * log(2.0)));
            // 1/c
            parameters_[3] = 1.0 / parameters_[2];
            break;
        case (BroadeningFunction::GaussianC2Function):
            // parameters_[0] = FWHM1
            // parameters_[1] = FWHM2
            // c1 (calculated from FWHM1)
            parameters_[2] = parameters_[0] / (2.0 * sqrt(2.0 * log(2.0)));
            // c2 (calculated from FWHM2)
            parameters_[3] = parameters_[1] / (2.0 * sqrt(2.0 * log(2.0)));
            // 1/c1
            parameters_[4] = 1.0 / parameters_[2];
            // 1/c2
            parameters_[5] = 1.0 / parameters_[3];
            break;
        default:
            Messenger::error("Function form '{}' not accounted for in BroadeningFunction::setUpDependentParameters().\n",
                             BroadeningFunction::functionType(function_));
    }
}

// Set inversion state
void BroadeningFunction::setInverted(bool state) { inverted_ = state; }

// Set static omega value
void BroadeningFunction::setOmega(double omega) { staticOmega_ = omega; }

// Return value of function given parameters x and omega
double BroadeningFunction::y(double x, double omega) const { return (inverted_ ? yFTActual(x, omega) : yActual(x, omega)); }

// Return value of Fourier transform of function, given parameters x and omega
double BroadeningFunction::yFT(double x, double omega) const { return (inverted_ ? yActual(x, omega) : yFTActual(x, omega)); }

// Return value of function given parameters x and omega, regardless of inversion state
double BroadeningFunction::yActual(double x, double omega) const
{
    switch (function_)
    {
        case (BroadeningFunction::NoFunction):
            return 1.0;
            break;
        case (BroadeningFunction::GaussianFunction):
            /*
             * Unnormalised Gaussian with no prefactor, centred at zero
             *
             * Parameters:
             *  0 = FWHM
             *  1 = c       (precalculated from FWHM)
             *  2 = 1.0 / c
             *
             *            (     x * x   )                    FWHM
             * f(x) = exp ( - --------- )      where c = --------------
             * 	          (   2 * c * c )                2 sqrt(2 ln 2)
             */
            return exp(-(0.5 * x * x * parameters_[2] * parameters_[2]));
            break;
        case (BroadeningFunction::ScaledGaussianFunction):
            /*
             * Gaussian with prefactor, centred at zero
             *
             * Parameters:
             *  0 = A, prefactor
             *  1 = FWHM
             *  2 = c       (precalculated from FWHM)
             *  3 = 1.0 / c
             *
             *              (     x * x   )                    FWHM
             * f(x) = A exp ( - --------- )      where c = --------------
             *              (   2 * c * c )                2 sqrt(2 ln 2)
             */
            return parameters_[0] * exp(-(0.5 * x * x * parameters_[3] * parameters_[3]));
            break;
        case (BroadeningFunction::OmegaDependentGaussianFunction):
            /*
             * Unnormalised Gaussian with no prefactor, centred at zero, with variable FWHM
             *
             * Parameters:
             *  0 = FWHM
             *  1 = c       (precalculated from FWHM)
             *  2 = 1.0 / c
             *
             *            (         x * x      )                    FWHM
             * f(x) = exp ( - ---------------- )      where c = --------------
             *            (   2 * (c*omega)**2 )                2 sqrt(2 ln 2)
             */
            return exp(-(x * x) / (2.0 * (parameters_[1] * omega) * (parameters_[1] * omega)));
            break;
        case (BroadeningFunction::GaussianC2Function):
            /*
             * Unnormalised Gaussian with no prefactor, centred at zero, with constant and variable FWHM
             *
             * Parameters:
             *  0 = FWHM1
             *  1 = FWHM2
             *  2 = c1      (precalculated from FWHM1)
             *  3 = c2      (precalculated from FWHM2)
             *  4 = 1.0 / c1
             *  5 = 1.0 / c2
             *
             *            (            x * x         )                     FWHMn
             * f(x) = exp ( - ---------------------- )      where cn = --------------
             *            (   2 * (c1 + c2*omega)**2 )                 2 sqrt(2 ln 2)
             */
            return exp(-(x * x) /
                       (2.0 * (parameters_[2] + parameters_[3] * omega) * (parameters_[2] + parameters_[3] * omega)));
            break;
        default:
            Messenger::warn("BroadeningFunction::value() - Function id {} not accounted for.\n", function_);
            break;
    }

    return 0.0;
}

// Return value of Fourier transform of function, given parameters x and omega, regardless of inversion state
double BroadeningFunction::yFTActual(double x, double omega) const
{
    switch (function_)
    {
        case (BroadeningFunction::NoFunction):
            return 1.0;
            break;
        case (BroadeningFunction::GaussianFunction):
            /*
             * Unnormalised Gaussian with no prefactor, centred at zero
             *
             * Parameters:
             *  0 = FWHM
             *  1 = c       (precalculated from FWHM)
             *  2 = 1.0 / c
             *
             *            (   x * x * c * c )                    FWHM
             * f(x) = exp ( - ------------- )      where c = --------------
             *            (         2       )                2 sqrt(2 ln 2)
             */
            return exp(-(0.5 * x * x * parameters_[1] * parameters_[1]));
            break;
        case (BroadeningFunction::ScaledGaussianFunction):
            /*
             * Gaussian with prefactor, centred at zero
             *
             * Parameters:
             *  0 = A, prefactor
             *  1 = FWHM
             *  2 = c       (precalculated from FWHM)
             *  3 = 1.0 / c
             *
             *              (   x * x * c * c )                    FWHM
             * f(x) = A exp ( - ------------- )      where c = --------------
             *              (         2       )                2 sqrt(2 ln 2)
             */
            return parameters_[0] * exp(-(0.5 * x * x * parameters_[2] * parameters_[2]));
            break;
        case (BroadeningFunction::OmegaDependentGaussianFunction):
            /*
             * Unnormalised Gaussian with no prefactor, centred at zero, with variable FWHM
             *
             * Parameters:
             *  0 = FWHM
             *  1 = c       (precalculated from FWHM)
             *  2 = 1.0 / c
             *
             *            (   x*x * (c*omega)**2 )                     FWHM
             * f(x) = exp ( - ------------------ )      where c = --------------
             *            (            2         )                2 sqrt(2 ln 2)
             */
            return exp(-(0.5 * x * x * (parameters_[1] * omega) * (parameters_[1] * omega)));
            break;
        case (BroadeningFunction::GaussianC2Function):
            /*
             * Unnormalised Gaussian with no prefactor, centred at zero, with constant and variable FWHM
             *
             * Parameters:
             *  0 = FWHM1
             *  1 = FWHM2
             *  2 = c1      (precalculated from FWHM1)
             *  3 = c2      (precalculated from FWHM2)
             *  4 = 1.0 / c1
             *  5 = 1.0 / c2
             *
             *            (   x * x * (c1 + c2*omega)**2 )                     FWHMn
             * f(x) = exp ( - -------------------------- )      where cn = --------------
             *            (                2             )                 2 sqrt(2 ln 2)
             */
            return exp(-(0.5 * x * x * (parameters_[2] + parameters_[3] * omega) * (parameters_[2] + parameters_[3] * omega)));
            break;
        default:
            Messenger::warn("BroadeningFunction::ft() - Function id {} not accounted for.\n", function_);
            break;
    }

    return 0.0;
}

// Return value of function given parameter x, and using static omega if necessary
double BroadeningFunction::y(double x) const { return y(x, staticOmega_); }

// Return value of Fourier transform of function, given parameter x, and using static omega if necessary
double BroadeningFunction::yFT(double x) const { return yFT(x, staticOmega_); }

// Return value of function given parameter x, and using static omega if necessary, regardless of inversion state
double BroadeningFunction::yActual(double x) const { return yActual(x, staticOmega_); }

// Return value of Fourier transform of function, given parameter x, and using static omega if necessary, regardless of
// inversion state
double BroadeningFunction::yFTActual(double x) const { return yFTActual(x, staticOmega_); }

// Return the discrete kernel normalisation factor for the current function, given the underlying data binwidth, and using
// static omega if necessary
double BroadeningFunction::discreteKernelNormalisation(double deltaX) const
{
    // Return the multiplicative factor to normalise the current function against its discretised sum
    switch (function_)
    {
        case (BroadeningFunction::NoFunction):
            return 1.0;
            break;
        case (BroadeningFunction::GaussianFunction):
            /*
             * Gaussian with no prefactor, centred at zero
             *
             * Parameters:
             *  0 = FWHM
             *  1 = c       (precalculated from FWHM)
             *  2 = 1.0 / c
             *
             *          2 * deltaX
             * DKN = ----------------------
             *       sqrt(pi / ln 2) * FWHM
             */
            return (2.0 * deltaX) / (sqrt(PI / log(2.0)) * parameters_[0]);
            break;
        case (BroadeningFunction::ScaledGaussianFunction):
            /*
             * Gaussian with prefactor, centred at zero
             *
             * Parameters:
             *  0 = A, prefactor
             *  1 = FWHM
             *  2 = c       (precalculated from FWHM)
             *  3 = 1.0 / c
             *
             *           2 * deltaX
             * DKN = --------------------------
             *       sqrt(pi / ln 2) * FWHM * A
             */
            return (2.0 * deltaX) / (sqrt(PI / log(2.0)) * parameters_[0] * parameters_[1]);
            break;
        case (BroadeningFunction::OmegaDependentGaussianFunction):
            /*
             * Unnormalised Gaussian with no prefactor, centred at zero, with variable FWHM
             *
             * Parameters:
             *  0 = FWHM
             *  1 = c       (precalculated from FWHM)
             *  2 = 1.0 / c
             *
             *              2 * deltaX
             * DKN = ------------------------------
             *       sqrt(pi / ln 2) * FWHM * omega
             */
            return (2.0 * deltaX) / (sqrt(PI / log(2.0)) * parameters_[0] * staticOmega_);
            break;
        case (BroadeningFunction::GaussianC2Function):
            /*
             * Unnormalised Gaussian with no prefactor, centred at zero, with constant and variable FWHM
             *
             * Parameters:
             *  0 = FWHM1
             *  1 = FWHM2
             *  2 = c1      (precalculated from FWHM1)
             *  3 = c2      (precalculated from FWHM2)
             *  4 = 1.0 / c1
             *  5 = 1.0 / c2
             *
             *                  2 * deltaX
             * DKN = ----------------------------------------
             *       sqrt(pi / ln 2) * (FWHM1 + FWHM2 * omega)
             */
            return (2.0 * deltaX) / (sqrt(PI / log(2.0)) * (parameters_[0] + parameters_[1] * staticOmega_));
            break;
        default:
            Messenger::warn("BroadeningFunction::discreteKernelNormalisation(dx) - Function id {} not accounted for.\n",
                            function_);
            break;
    }

    return 1.0;
}

// Return the discrete kernel normalisation factor for the current function, given the underlying data binwidth and omega value
double BroadeningFunction::discreteKernelNormalisation(double deltaX, double omega) const
{
    // Return the multiplicative factor to normalise the current function against its discretised sum
    switch (function_)
    {
        case (BroadeningFunction::NoFunction):
            return 1.0;
            break;
        case (BroadeningFunction::GaussianFunction):
            /*
             * Gaussian with no prefactor, centred at zero
             *
             * Parameters:
             *  0 = FWHM
             *  1 = c       (precalculated from FWHM)
             *  2 = 1.0 / c
             *
             *            2 * deltaX
             * DKN = ----------------------
             *       sqrt(pi / ln 2) * FWHM
             */
            return (2.0 * deltaX) / (sqrt(PI / log(2.0)) * parameters_[0]);
            break;
        case (BroadeningFunction::ScaledGaussianFunction):
            /*
             * Gaussian with prefactor, centred at zero
             *
             * Parameters:
             *  0 = A, prefactor
             *  1 = FWHM
             *  2 = c       (precalculated from FWHM)
             *  3 = 1.0 / c
             *
             *             2 * deltaX
             * DKN = --------------------------
             *       sqrt(pi / ln 2) * FWHM * A
             */
            return (2.0 * deltaX) / (sqrt(PI / log(2.0)) * parameters_[0] * parameters_[1]);
            break;
        case (BroadeningFunction::OmegaDependentGaussianFunction):
            /*
             * Unnormalised Gaussian with no prefactor, centred at zero, with variable FWHM
             *
             * Parameters:
             *  0 = FWHM
             *  1 = c       (precalculated from FWHM)
             *  2 = 1.0 / c
             *
             *                2 * deltaX
             * DKN = ------------------------------
             *       sqrt(pi / ln 2) * FWHM * omega
             */
            return (2.0 * deltaX) / (sqrt(PI / log(2.0)) * parameters_[0] * omega);
            break;
        case (BroadeningFunction::GaussianC2Function):
            /*
             * Unnormalised Gaussian with no prefactor, centred at zero, with constant and variable FWHM
             *
             * Parameters:
             *  0 = FWHM1
             *  1 = FWHM2
             *  2 = c1      (precalculated from FWHM1)
             *  3 = c2      (precalculated from FWHM2)
             *  4 = 1.0 / c1
             *  5 = 1.0 / c2
             *
             *                  2 * deltaX
             * DKN = ----------------------------------------
             *       sqrt(pi / ln 2) * (FWHM1 + FWHM2 * omega)
             */
            return (2.0 * deltaX) / (sqrt(PI / log(2.0)) * (parameters_[0] + parameters_[1] * omega));
            break;
        default:
            Messenger::warn("BroadeningFunction::discreteKernelNormalisation(dx,omega) - Function id {} not "
                            "accounted for.\n",
                            function_);
            break;
    }

    return 1.0;
}

/*
 * Serialisation
 */

// Read data through specified LineParser
bool BroadeningFunction::deserialise(LineParser &parser)
{
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    function_ = functionType(parser.argsv(0));
    for (auto n = 0; n < nFunctionParameters(function_); ++n)
        parameters_[n] = parser.argd(n + 1);
    return true;
}

// Write data through specified LineParser
bool BroadeningFunction::write(LineParser &parser)
{
    std::string line{functionType(function_)};
    for (auto n = 0; n < nFunctionParameters(function_); ++n)
        line += fmt::format(" {:16.9e}", parameters_[n]);
    return parser.writeLine(line);
}
