// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "math/pairbroadeningfunction.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/atomtype.h"
#include "classes/speciesintra.h"
#include "data/atomicmasses.h"
#include "templates/algorithms.h"

PairBroadeningFunction::PairBroadeningFunction(PairBroadeningFunction::FunctionType function)
{
    function_ = function;

    gaussianFWHM_ = 0.18;

    // Create element broadening array
    elementPairGaussianFWHM_.initialise(Elements::nElements, Elements::nElements, true);
    elementPairGaussianFlags_.initialise(Elements::nElements, Elements::nElements, true);
    elementPairGaussianFWHM_ = 0.13;
    elementPairGaussianFlags_ = false;
}

PairBroadeningFunction::~PairBroadeningFunction() {}

PairBroadeningFunction::PairBroadeningFunction(const PairBroadeningFunction &source) { (*this) = source; }

void PairBroadeningFunction::operator=(const PairBroadeningFunction &source)
{
    function_ = source.function_;

    gaussianFWHM_ = source.gaussianFWHM_;

    elementPairGaussianFWHM_ = source.elementPairGaussianFWHM_;
    elementPairGaussianFlags_ = source.elementPairGaussianFlags_;
}

std::string_view PairBroadeningFunctionKeywords[] = {"None", "Gaussian", "GaussianElements", "BROKEN_Frequency"};
int PairBroadeningFunctionNParameters[] = {0, 1, 0, 2};

// Return FunctionType from supplied string
PairBroadeningFunction::FunctionType PairBroadeningFunction::functionType(std::string_view s)
{
    for (auto n = 0; n < nFunctionTypes; ++n)
        if (DissolveSys::sameString(s, PairBroadeningFunctionKeywords[n]))
            return (FunctionType)n;
    return PairBroadeningFunction::nFunctionTypes;
}

// Return FunctionType name
std::string_view PairBroadeningFunction::functionType(PairBroadeningFunction::FunctionType func)
{
    return PairBroadeningFunctionKeywords[func];
}

// Return number of parameters needed to define function
int PairBroadeningFunction::nFunctionParameters(FunctionType func) { return PairBroadeningFunctionNParameters[func]; }

/*
 * Function Data
 */

// Read function data from LineParser source
bool PairBroadeningFunction::readAsKeyword(LineParser &parser, int startArg, const CoreData &coreData)
{
    // First argument is the form of the function, or a '&' to indicate that a full block-style definition of the data
    if (DissolveSys::sameString("&", parser.argsv(startArg)))
        return deserialise(parser);

    PairBroadeningFunction::FunctionType funcType = PairBroadeningFunction::functionType(parser.argsv(startArg));
    if (funcType == PairBroadeningFunction::nFunctionTypes)
    {
        Messenger::error("Unrecognised Function type '{}'.\n", parser.argsv(startArg));
        return false;
    }

    // Do we have the right number of arguments for the function specified?
    if ((parser.nArgs() - startArg) < PairBroadeningFunction::nFunctionParameters(funcType))
    {
        Messenger::error("Too few parameters supplied for Function '{}' (expected {}, found {}).\n",
                         PairBroadeningFunction::functionType(funcType), PairBroadeningFunction::nFunctionParameters(funcType),
                         parser.nArgs() - startArg);
        return false;
    }

    // Set up function and basic parameters
    function_ = funcType;
    switch (function_)
    {
        case (PairBroadeningFunction::NoFunction):
            break;
        case (PairBroadeningFunction::GaussianFunction):
            gaussianFWHM_ = parser.argd(startArg + 1);
            break;
        case (PairBroadeningFunction::GaussianElementPairFunction):
            Messenger::print("Gaussian element-pair broadening requested - default starting values will be used.\n");
            break;
        default:
            Messenger::error("Function form '{}' not accounted for in PairBroadeningFunction::set(LineParser&,int).\n",
                             PairBroadeningFunction::functionType(funcType));
            return false;
    }

    // Note: Setting up dependent parameters cannot be done here, as we do not know the AtomTypes involved in the pair

    return true;
}

// Write function data to LineParser source
bool PairBroadeningFunction::writeAsKeyword(LineParser &parser, std::string_view prefix, bool writeBlockMarker) const
{
    // If the functional form requires a block rather than a single line, write an '&' and start a new line
    if (writeBlockMarker && (function_ == PairBroadeningFunction::GaussianElementPairFunction))
    {
        if (!parser.writeLineF(" &\n"))
            return false;
    }

    auto count = 0;
    switch (function_)
    {
        case (PairBroadeningFunction::NoFunction):
            if (!parser.writeLineF("{}{}\n", prefix, functionType(function_)))
                return false;
            break;
        case (PairBroadeningFunction::GaussianFunction):
            if (!parser.writeLineF("{}{}  {:e}\n", prefix, functionType(function_), gaussianFWHM_))
                return false;
            break;
        case (PairBroadeningFunction::GaussianElementPairFunction):
            // Must be written as a block
            if (!parser.writeLineF("{}{}", prefix, functionType(function_)))
                return false;

            // Count number of pairs/values to expect and write to file
            count += std::count(elementPairGaussianFlags_.begin(), elementPairGaussianFlags_.end(), true);
            if (!parser.writeLineF("{}{}\n", prefix, count))
                return false;

            // Loop again and write the data proper
            for (auto i = 0; i < elementPairGaussianFlags_.nRows(); ++i)
            {
                for (auto j = i; j < elementPairGaussianFlags_.nColumns(); ++j)
                {
                    if (elementPairGaussianFlags_[{i, j}])
                    {
                        if (!parser.writeLineF("{}{}  {}  {}\n", prefix, Elements::symbol(Elements::element(i)),
                                               Elements::symbol(Elements::element(j)), elementPairGaussianFWHM_[{i, j}]))
                            return false;
                    }
                }
            }
            break;
        default:
            break;
    }

    return true;
}

void PairBroadeningFunction::setFunction(PairBroadeningFunction::FunctionType function) { function_ = function; }

// Return function type
PairBroadeningFunction::FunctionType PairBroadeningFunction::function() const { return function_; }

// Set Gaussian FWHM parameters
void PairBroadeningFunction::setGaussianFWHM(double fwhm) { gaussianFWHM_ = fwhm; }

// Return Gaussian FWHM parameter
double PairBroadeningFunction::gaussianFWHM() const { return gaussianFWHM_; }

// Return array of pointers to all adjustable parameters
std::vector<double *> PairBroadeningFunction::parameters()
{
    std::vector<double *> params;

    switch (function_)
    {
        case (PairBroadeningFunction::NoFunction):
            break;
        case (PairBroadeningFunction::GaussianFunction):
            params.push_back(&gaussianFWHM_);
            break;
        case (PairBroadeningFunction::GaussianElementPairFunction):
            for (auto &&[flags, fwhm] : zip(elementPairGaussianFlags_, elementPairGaussianFWHM_))
                if (flags)
                    params.push_back(&fwhm);
            break;
        default:
            break;
    }

    return params;
}

// Return short summary of function parameters
std::string PairBroadeningFunction::summary() const
{
    switch (function_)
    {
        case (PairBroadeningFunction::NoFunction):
            return "None";
            break;
        case (PairBroadeningFunction::GaussianFunction):
            return "Gaussian";
            break;
        case (PairBroadeningFunction::GaussianElementPairFunction):
            return "Gaussian [Z1-Z2]";
            break;
        default:
            break;
    }

    return "NOT RECOGNISED";
}

// Return a BroadeningFunction tailored to the specified AtomType pair, using intramolecular data if required
BroadeningFunction PairBroadeningFunction::broadeningFunction(std::shared_ptr<AtomType> at1, std::shared_ptr<AtomType> at2,
                                                              SpeciesIntra *intra)
{
    BroadeningFunction result;

    switch (function_)
    {
        case (PairBroadeningFunction::NoFunction):
            break;
        case (PairBroadeningFunction::GaussianFunction):
            // Simple broadening - same FWHM for any AtomType pair
            result.set(BroadeningFunction::GaussianFunction, gaussianFWHM_);
            break;
        case (PairBroadeningFunction::GaussianElementPairFunction):
            // If this matrix value has never been used/read, set the flag now
            if (!elementPairGaussianFlags_[{at1->Z(), at2->Z()}])
                elementPairGaussianFlags_[{at1->Z(), at2->Z()}] = true;
            result.set(BroadeningFunction::GaussianFunction, elementPairGaussianFWHM_[{at1->Z(), at2->Z()}]);
            break;
        default:
            Messenger::error("Function form '{}' not accounted for in setUpDependentParameters().\n",
                             PairBroadeningFunction::functionType(function_));
    }

    return result;
}

/*
 * Serialisation
 */

// Read data through specified LineParser
bool PairBroadeningFunction::deserialise(LineParser &parser)
{
    // First line is function name
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    function_ = functionType(parser.argsv(0));
    if (function_ == nFunctionTypes)
        return Messenger::error("Unrecognised pair broadening function '{}'.\n", parser.argsv(0));

    // Our next action depends on the function
    switch (function_)
    {
        case (PairBroadeningFunction::GaussianFunction):
            if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                return false;
            gaussianFWHM_ = parser.argd(0);
            break;
        case (PairBroadeningFunction::GaussianElementPairFunction):
            // First line is the number of values we need to read
            if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                return false;
            else
            {
                // Reset the parameters to boring default values
                elementPairGaussianFWHM_ = 0.12;
                elementPairGaussianFlags_ = false;

                auto nPairs = parser.argi(0);
                for (auto n = 0; n < nPairs; ++n)
                {
                    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                        return false;

                    // Line format is:  Element1  Element2  FWHM
                    auto Z1 = Elements::element(parser.argsv(0));
                    if (Z1 == Elements::Unknown)
                        return Messenger::error("Unrecognised element '{}' found in pair broadening parameters.\n",
                                                parser.argsv(0));
                    auto Z2 = Elements::element(parser.argsv(1));
                    if (Z2 == Elements::Unknown)
                        return Messenger::error("Unrecognised element '{}' found in pair broadening parameters.\n",
                                                parser.argsv(1));

                    // Set the value
                    elementPairGaussianFlags_[{Z1, Z2}] = parser.argd(2);
                    elementPairGaussianFlags_[{Z1, Z2}] = true;
                }
            }
            break;
        default:
            break;
    }

    return true;
}

// Write data through specified LineParser
bool PairBroadeningFunction::write(LineParser &parser) { return writeAsKeyword(parser, "", false); }
