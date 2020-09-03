/*
    *** PairBroadening Function
    *** src/math/pairbroadeningfunction.cpp
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

#include "math/pairbroadeningfunction.h"
#include "base/lineparser.h"
#include "base/processpool.h"
#include "base/sysfunc.h"
#include "classes/atomtype.h"
#include "classes/speciesintra.h"
#include "data/atomicmass.h"
#include "genericitems/array2ddouble.h"
#include "templates/enumhelpers.h"

PairBroadeningFunction::PairBroadeningFunction(PairBroadeningFunction::FunctionType function)
{
    function_ = function;

    gaussianFWHM_ = 0.18;

    // Create element broadening array
    elementPairGaussianFWHM_.initialise(Elements::nElements(), Elements::nElements(), true);
    elementPairGaussianFlags_.initialise(Elements::nElements(), Elements::nElements(), true);
    elementPairGaussianFWHM_ = 0.13;
    elementPairGaussianFlags_ = false;

    frequencyBondConstant_ = 1.0e-3;
    frequencyAngleConstant_ = 1.0e-3;
}

PairBroadeningFunction::~PairBroadeningFunction() {}

PairBroadeningFunction::PairBroadeningFunction(const PairBroadeningFunction &source) { (*this) = source; }

void PairBroadeningFunction::operator=(const PairBroadeningFunction &source)
{
    function_ = source.function_;

    gaussianFWHM_ = source.gaussianFWHM_;

    elementPairGaussianFWHM_ = source.elementPairGaussianFWHM_;
    elementPairGaussianFlags_ = source.elementPairGaussianFlags_;

    frequencyBondConstant_ = source.frequencyBondConstant_;
    frequencyAngleConstant_ = source.frequencyAngleConstant_;
}

std::string_view PairBroadeningFunctionKeywords[] = {"None", "Gaussian", "GaussianElements", "BROKEN_Frequency"};
int PairBroadeningFunctionNParameters[] = {0, 1, 0, 2};

// Return FunctionType from supplied string
PairBroadeningFunction::FunctionType PairBroadeningFunction::functionType(std::string_view s)
{
    for (int n = 0; n < nFunctionTypes; ++n)
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
bool PairBroadeningFunction::readAsKeyword(LineParser &parser, int startArg, CoreData &coreData)
{
    // First argument is the form of the function, or a '&' to indicate that a full block-style definition of the data
    if (DissolveSys::sameString("&", parser.argsv(startArg)))
        return read(parser, coreData);

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
        case (PairBroadeningFunction::FrequencyFunction):
            frequencyBondConstant_ = parser.argd(startArg + 1);
            frequencyAngleConstant_ = parser.argd(startArg + 2);
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
bool PairBroadeningFunction::writeAsKeyword(LineParser &parser, std::string_view prefix, bool writeBlockMarker)
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
            for (int n = 0; n < elementPairGaussianFlags_.linearArraySize(); ++n)
                if (elementPairGaussianFlags_.constLinearValue(n))
                    ++count;
            if (!parser.writeLineF("{}{}\n", prefix, count))
                return false;

            // Loop again and write the data proper
            for (int i = 0; i < elementPairGaussianFlags_.nRows(); ++i)
            {
                for (int j = i; j < elementPairGaussianFlags_.nColumns(); ++j)
                {
                    if (elementPairGaussianFlags_.constAt(i, j))
                    {
                        if (!parser.writeLineF("{}{}  {}  {}\n", prefix, Elements::element(i).symbol(),
                                               Elements::element(j).symbol(), elementPairGaussianFWHM_.constAt(i, j)))
                            return false;
                    }
                }
            }
            break;
        case (PairBroadeningFunction::FrequencyFunction):
            if (!parser.writeLineF("{}{}  {:e}  {:e}\n", prefix, functionType(function_), frequencyBondConstant_,
                                   frequencyAngleConstant_))
                return false;
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

// Set frequency bond constant
void PairBroadeningFunction::setFrequencyBondConstant(double k) { frequencyBondConstant_ = k; }

// Return frequency bond constant
double PairBroadeningFunction::frequencyBondConstant() const { return frequencyBondConstant_; }

// Set frequency angle constant
void PairBroadeningFunction::setFrequencyAngleConstant(double k) { frequencyAngleConstant_ = k; }

// Return frequency angle constant
double PairBroadeningFunction::frequencyAngleConstant() const { return frequencyAngleConstant_; }

// Return array of pointers to all adjustable parameters
Array<double *> PairBroadeningFunction::parameters()
{
    Array<double *> params;

    switch (function_)
    {
        case (PairBroadeningFunction::NoFunction):
            break;
        case (PairBroadeningFunction::GaussianFunction):
            params.add(&gaussianFWHM_);
            break;
        case (PairBroadeningFunction::GaussianElementPairFunction):
            for (int n = 0; n < elementPairGaussianFlags_.linearArraySize(); ++n)
            {
                if (elementPairGaussianFlags_.constLinearValue(n))
                    params.add(&elementPairGaussianFWHM_.linearValue(n));
            }
            break;
        case (PairBroadeningFunction::FrequencyFunction):
            params.add(&frequencyBondConstant_);
            params.add(&frequencyAngleConstant_);
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
        case (PairBroadeningFunction::FrequencyFunction):
            return "Frequency";
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
            if (!elementPairGaussianFlags_.at(at1->element()->Z(), at2->element()->Z()))
                elementPairGaussianFlags_.at(at1->element()->Z(), at2->element()->Z()) = true;
            result.set(BroadeningFunction::GaussianFunction,
                       elementPairGaussianFWHM_.at(at1->element()->Z(), at2->element()->Z()));
            break;
        case (PairBroadeningFunction::FrequencyFunction):
            // Broadening based on fundamental frequency of interaction - requires SpeciesIntra
            if (!intra)
                Messenger::error("Broadening type is '{}', but a valid SpeciesIntra reference has not been provided.\n",
                                 PairBroadeningFunction::functionType(function_));
            else
            {
                // If this interaction is a torsion, treat it as a special case
                if ((intra->type() == SpeciesIntra::TorsionInteraction) || (intra->type() == SpeciesIntra::ImproperInteraction))
                {
                    // TODO This will kill all torsion and improper interactions!
                    result.set(BroadeningFunction::NoFunction);
                }
                else
                {
                    // Bond or an angle, so calculate the fundamental frequency
                    double v = intra->fundamentalFrequency(AtomicMass::reducedMass(at1->element(), at2->element()));

                    // Convert to cm-1?
                    double wno = v / (SPEEDOFLIGHT * 100.0);

                    if (intra->type() == SpeciesIntra::BondInteraction)
                        result.set(BroadeningFunction::GaussianFunction, frequencyBondConstant_ * wno);
                    else if (intra->type() == SpeciesIntra::AngleInteraction)
                        result.set(BroadeningFunction::GaussianFunction, frequencyAngleConstant_ * wno);
                }
            }
            break;
            // POSSIBLE USE AS FUNCTION FOR ELEMENT/ATOMTYPE-DEPENDENT BROADENING?
            // 		case (PairBroadeningFunction::GaussianElementFunction):
            // 			// Calculate reduced mass (store in parameters_[1])
            // 			parameters_[1] = sqrt((AtomicMass::mass(at1->element()) *
            // AtomicMass::mass(at2->element())) / (AtomicMass::mass(at1->element()) +
            // AtomicMass::mass(at2->element())));
            //
            // 			// Calculate final broadening
            // 			parameters_[0] = 1.0 / (2.0 * sqrt(2.0) * parameters_[1]);
            //
            // 			result.set(BroadeningFunction::GaussianFunction, parameters_[0]);
            // 			break;
        default:
            Messenger::error("Function form '{}' not accounted for in setUpDependentParameters().\n",
                             PairBroadeningFunction::functionType(function_));
    }

    return result;
}

/*
 * GenericItemBase Implementations
 */

// Return class name
std::string_view PairBroadeningFunction::itemClassName() { return "PairBroadeningFunction"; }

// Read data through specified LineParser
bool PairBroadeningFunction::read(LineParser &parser, CoreData &coreData)
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
                for (int n = 0; n < nPairs; ++n)
                {
                    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                        return false;

                    // Line format is:  Element1  Element2  FWHM
                    Element &el1 = Elements::element(parser.argsv(0));
                    if (el1.isUnknown())
                        return Messenger::error("Unrecognised element '{}' found in pair broadening parameters.\n",
                                                parser.argsv(0));
                    Element &el2 = Elements::element(parser.argsv(1));
                    if (el2.isUnknown())
                        return Messenger::error("Unrecognised element '{}' found in pair broadening parameters.\n",
                                                parser.argsv(1));

                    // Set the value
                    elementPairGaussianFlags_.at(el1.Z(), el2.Z()) = parser.argd(2);
                    elementPairGaussianFlags_.at(el1.Z(), el2.Z()) = true;
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

/*
 * Parallel Comms
 */

// Broadcast data from Master to all Slaves
bool PairBroadeningFunction::broadcast(ProcessPool &procPool, const int root, const CoreData &coreData)
{
#ifdef PARALLEL
    if (!procPool.broadcast(EnumCast<PairBroadeningFunction::FunctionType>(function_), root))
        return false;
    if (!procPool.broadcast(gaussianFWHM_, root))
        return false;
    if (!procPool.broadcast(elementPairGaussianFWHM_, root))
        return false;
    if (!procPool.broadcast(elementPairGaussianFlags_, root))
        return false;
#endif
    return true;
}

// Check item equality
bool PairBroadeningFunction::equality(ProcessPool &procPool)
{
#ifdef PARALLEL
    if (!procPool.equality(EnumCast<PairBroadeningFunction::FunctionType>(function_)))
        return Messenger::error("PairBroadeningFunction function type is not equivalent (process {} has {}).\n",
                                procPool.poolRank(), function_);
    if (!procPool.equality(gaussianFWHM_))
        return Messenger::error("PairBroadeningFunction Gaussian parameters are not equivalent.\n");
    if (!procPool.equality(elementPairGaussianFWHM_))
        return Messenger::error("PairBroadeningFunction element pair Gaussian parameters are not equivalent.\n");
    if (!procPool.equality(elementPairGaussianFlags_))
        return Messenger::error("PairBroadeningFunction element pair Gaussian parameters are not equivalent.\n");
#endif
    return true;
}
