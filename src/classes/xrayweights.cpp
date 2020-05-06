/*
    *** XRay Weights Container
    *** src/classes/xrayweights.cpp
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

#include "classes/xrayweights.h"
#include "base/lineparser.h"
#include "base/processpool.h"
#include "classes/atomtype.h"
#include "classes/species.h"
#include "classes/speciesinfo.h"
#include "genericitems/array2ddouble.h"
#include "templates/enumhelpers.h"

XRayWeights::XRayWeights()
    : formFactors_(XRayFormFactors::NoFormFactorData), valid_(false), boundCoherentSquareOfAverage_(0.0),
      boundCoherentAverageOfSquares_(0.0)
{
}

XRayWeights::XRayWeights(const XRayWeights &source) { (*this) = source; }

void XRayWeights::operator=(const XRayWeights &source)
{
    formFactors_ = source.formFactors_;
    boundCoherentSquareOfAverage_ = source.boundCoherentSquareOfAverage_;
    boundCoherentAverageOfSquares_ = source.boundCoherentAverageOfSquares_;
    atomTypes_ = source.atomTypes_;
    concentrations_ = source.concentrations_;
    concentrationProducts_ = source.concentrationProducts_;
    preFactors_ = source.preFactors_;
    valid_ = source.valid_;
}

/*
 * Source AtomTypes
 */

// Initialise form factor data for the current atom types
bool XRayWeights::initialiseFormFactors()
{
    formFactorData_.clear();

    for (AtomTypeData *atd = atomTypes_.first(); atd != NULL; atd = atd->next())
    {
        auto *at = atd->atomType();

        // Try to retrieve form factor data for this atom type (element, formal charge [TODO])
        auto data = XRayFormFactors::formFactorData(formFactors_, at->element());
        if (std::get<1>(data))
            return Messenger::error("No form factor data present for element %s (formal charge %i) in x-ray data set '%s'.\n",
                                    at->element()->symbol(), 0, XRayFormFactors::xRayFormFactorData().keyword(formFactors_));

        formFactorData_.push_back(std::reference_wrapper<const FormFactorData>(std::get<0>(data)));
    }

    return true;
}

// Clear contents
void XRayWeights::clear()
{
    atomTypes_.clear();
    concentrations_.clear();
    concentrationProducts_.clear();
    preFactors_.clear();
    valid_ = false;
}

// Set-up from supplied SpeciesInfo list
bool XRayWeights::setUp(List<SpeciesInfo> &speciesInfoList, XRayFormFactors::XRayFormFactorData formFactors)
{
    valid_ = false;

    // Fill atomTypes_ list with AtomType populations, based on Isotopologues relative populations and associated Species
    // populations
    atomTypes_.clear();
    for (SpeciesInfo *spInfo = speciesInfoList.first(); spInfo != NULL; spInfo = spInfo->next())
    {
        const Species *sp = spInfo->species();

        // Loop over Atoms in the Species
        for (SpeciesAtom *i = sp->firstAtom(); i != NULL; i = i->next())
            atomTypes_.add(i->atomType(), spInfo->population());
    }

    // Perform final setup based on now-completed atomtypes list
    return finalise(formFactors);
}

// Add Species to weights in the specified population
void XRayWeights::addSpecies(const Species *sp, int population)
{
    for (SpeciesAtom *i = sp->firstAtom(); i != NULL; i = i->next())
        atomTypes_.add(i->atomType(), population);

    valid_ = false;
}

// Finalise weights after addition of all individual Species
bool XRayWeights::finalise(XRayFormFactors::XRayFormFactorData formFactors)
{
    valid_ = false;

    atomTypes_.finalise();

    // Retrieve form factor data for the current atom types
    formFactors_ = formFactors;
    if (!initialiseFormFactors())
        return false;

    setUpMatrices();

    valid_ = true;

    return true;
}

// Return AtomTypeList
AtomTypeList &XRayWeights::atomTypes() { return atomTypes_; }

// Return number of used AtomTypes
int XRayWeights::nUsedTypes() const { return atomTypes_.nItems(); }

// Print atomtype information
void XRayWeights::print() const
{
    // Print atomtypes table
    Messenger::print("\n");
    atomTypes_.print();
}

/*
 * Data
 */

// Set up matrices based on current AtomType information
void XRayWeights::setUpMatrices()
{
    concentrations_.initialise(atomTypes_.nItems());
    concentrationProducts_.initialise(atomTypes_.nItems(), atomTypes_.nItems(), true);
    preFactors_.initialise(atomTypes_.nItems(), atomTypes_.nItems(), true);

    double ci, cj;

    // Determine atomic concentration products and full pre-factor
    AtomTypeData *atd1 = atomTypes_.first(), *atd2;
    for (int typeI = 0; typeI < atomTypes_.nItems(); ++typeI, atd1 = atd1->next())
    {
        ci = atd1->fraction();
        concentrations_.at(typeI) = ci;

        atd2 = atd1;
        for (int typeJ = typeI; typeJ < atomTypes_.nItems(); ++typeJ, atd2 = atd2->next())
        {
            cj = atd2->fraction();

            concentrationProducts_.at(typeI, typeJ) = ci * cj;
            preFactors_.at(typeI, typeJ) = ci * cj * (typeI == typeJ ? 1 : 2);
        }
    }
}

// Return concentration product for type i
double XRayWeights::concentration(int typeIndexI) const { return concentrations_.constAt(typeIndexI); }

// Return concentration product for types i and j
double XRayWeights::concentrationProduct(int typeIndexI, int typeIndexJ) const
{
    return concentrationProducts_.constAt(typeIndexI, typeIndexJ);
}

// Return form factor for type i over supplied Q values
Array<double> XRayWeights::formFactor(int typeIndexI, const Array<double> &Q) const
{
#ifdef CHECKS
    if ((typeIndexI < 0) || (typeIndexI >= formFactorData_.size()))
    {
        Messenger::error("XRayWeights::formFactorProduct() - Type i index %i is out of range.\n", typeIndexI);
        return 0.0;
    }
#endif

    // Initialise results array
    Array<double> fiq(Q.nItems());

    auto &fi = formFactorData_[typeIndexI].get();

    for (int n = 0; n < Q.nItems(); ++n)
        fiq[n] = fi.magnitude(Q.constAt(n));

    return fiq;
}

// Return form factor product for types i and j at specified Q value
double XRayWeights::formFactorProduct(int typeIndexI, int typeIndexJ, double Q) const
{
#ifdef CHECKS
    if ((typeIndexI < 0) || (typeIndexI >= formFactorData_.size()))
    {
        Messenger::error("XRayWeights::formFactorProduct() - Type i index %i is out of range.\n", typeIndexI);
        return 0.0;
    }
    if ((typeIndexJ < 0) || (typeIndexJ >= formFactorData_.size()))
    {
        Messenger::error("XRayWeights::formFactorProduct() - Type j index %i is out of range.\n", typeIndexJ);
        return 0.0;
    }
#endif
    return formFactorData_[typeIndexI].get().magnitude(Q) * formFactorData_[typeIndexJ].get().magnitude(Q);
}

// Return full weighting for types i and j (ci * cj * f(i,Q) * F(j,Q) * [2-dij]) at specified Q value
double XRayWeights::weight(int typeIndexI, int typeIndexJ, double Q) const
{
    return preFactors_.constAt(typeIndexI, typeIndexJ) * formFactorProduct(typeIndexI, typeIndexJ, Q);
}

// Return full weighting for types i and j (ci * cj * f(i,Q) * F(j,Q) * [2-dij]) over supplied Q values
Array<double> XRayWeights::weight(int typeIndexI, int typeIndexJ, const Array<double> &Q) const
{
    // Get form factor data for involved types
#ifdef CHECKS
    if ((typeIndexI < 0) || (typeIndexI >= formFactorData_.size()))
    {
        Messenger::error("XRayWeights::weight() - Type i index %i is out of range.\n", typeIndexI);
        return 0.0;
    }
    if ((typeIndexJ < 0) || (typeIndexJ >= formFactorData_.size()))
    {
        Messenger::error("XRayWeights::weight() - Type j index %i is out of range.\n", typeIndexJ);
        return 0.0;
    }
#endif

    // Initialise results array
    Array<double> fijq(Q.nItems());

    auto &fi = formFactorData_[typeIndexI].get();
    auto &fj = formFactorData_[typeIndexJ].get();
    auto preFactor = preFactors_.constAt(typeIndexI, typeIndexJ);

    for (int n = 0; n < Q.nItems(); ++n)
        fijq[n] = fi.magnitude(Q.constAt(n)) * fj.magnitude(Q.constAt(n)) * preFactor;

    return fijq;
}

// Return whether the structure is valid (i.e. has been finalised)
bool XRayWeights::isValid() const { return valid_; }

/*
 * GenericItemBase Implementations
 */

// Return class name
const char *XRayWeights::itemClassName() { return "XRayWeights"; }

// Read data through specified LineParser
bool XRayWeights::read(LineParser &parser, const CoreData &coreData)
{
    clear();

    // Read form factor dataset to use
    if (!parser.getArgsDelim())
        return false;
    formFactors_ = XRayFormFactors::xRayFormFactorData().enumeration(parser.argc(0));

    // Read AtomTypeList
    if (!atomTypes_.read(parser, coreData))
        return false;

    return finalise(formFactors_);
}

// Write data through specified LineParser
bool XRayWeights::write(LineParser &parser)
{
    // Write x-ray form factor dataset
    if (!parser.writeLineF("%s\n", XRayFormFactors::xRayFormFactorData().keyword(formFactors_)))
        return false;

    // Write AtomTypeList
    if (!atomTypes_.write(parser))
        return false;

    return true;
}

/*
 * Parallel Comms
 */

// Broadcast item contents
bool XRayWeights::broadcast(ProcessPool &procPool, const int root, const CoreData &coreData)
{
#ifdef PARALLEL
    if (!procPool.broadcast(EnumCast<XRayFormFactors::XRayFormFactorData>(formFactors_), root))
        return false;
    if (!atomTypes_.broadcast(procPool, root, coreData))
        return false;
    if (!procPool.broadcast(concentrations_, root))
        return false;
    if (!procPool.broadcast(concentrationProducts_, root))
        return false;
    if (!procPool.broadcast(preFactors_, root))
        return false;
    if (!procPool.broadcast(valid_, root))
        return false;
#endif
    return true;
}

// Check item equality
bool XRayWeights::equality(ProcessPool &procPool)
{
#ifdef PARALLEL
    if (!procPool.equality(EnumCast<XRayFormFactors::XRayFormFactorData>(formFactors_)))
        return Messenger::error("XRayWeights form factor datasets are not equivalent.\n");
    if (!atomTypes_.equality(procPool))
        return Messenger::error("XRayWeights AtomTypes are not equivalent.\n");
    if (!procPool.equality(concentrations_))
        return Messenger::error("XRayWeights concentrations array is not equivalent.\n");
    if (!procPool.equality(concentrationProducts_))
        return Messenger::error("XRayWeights concentration matrix is not equivalent.\n");
    if (!procPool.equality(preFactors_))
        return Messenger::error("XRayWeights bound coherent matrix is not equivalent.\n");
    if (!procPool.equality(valid_))
        return Messenger::error("XRayWeights validity is not equivalent (process %i has %i).\n", procPool.poolRank(), valid_);
#endif
    return true;
}
