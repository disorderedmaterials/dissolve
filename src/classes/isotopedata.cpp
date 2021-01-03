// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/isotopedata.h"
#include "base/lineparser.h"
#include "base/messenger.h"
#include "base/processpool.h"
#include "data/elements.h"
#include "data/isotopes.h"

IsotopeData::IsotopeData() : ListItem<IsotopeData>()
{
    isotope_ = nullptr;
    population_ = 0.0;
    fraction_ = 0.0;
}

IsotopeData::IsotopeData(const IsotopeData &source) { (*this) = source; }

void IsotopeData::operator=(const IsotopeData &source)
{
    isotope_ = source.isotope_;
    population_ = source.population_;
    fraction_ = source.fraction_;
}

/*
 * Properties
 */

// Initialise
bool IsotopeData::initialise(Isotope *isotope)
{
    isotope_ = isotope;
    if (isotope_ == nullptr)
    {
        Messenger::error("NULL_POINTER - NULL Isotope pointer passed to IsotopeData::initialise().\n");
        return false;
    }

    population_ = 0.0;
    fraction_ = 0.0;

    return true;
}

// Add to population of Isotope
void IsotopeData::add(double nAdd) { population_ += nAdd; }

// Finalise, calculating local fractional population (e.g. within an IsotopeData)
void IsotopeData::finalise(double totalAtoms) { fraction_ = population_ / totalAtoms; }

// Zero populations
void IsotopeData::zeroPopulation()
{
    population_ = 0.0;
    fraction_ = 0.0;
}

// Return reference Isotope
Isotope *IsotopeData::isotope() const { return isotope_; }

// Return total population over all isotopes
double IsotopeData::population() const { return population_; }

// Return fractional population
double IsotopeData::fraction() const { return fraction_; }

/*
 * I/O
 */

// Write data through specified LineParser
bool IsotopeData::write(LineParser &parser)
{
    return parser.writeLineF("{} {} {} {}\n", isotope_->Z(), isotope_->A(), population_, fraction_);
}

// Read data through specified LineParser
bool IsotopeData::read(LineParser &parser, CoreData &coreData)
{
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    isotope_ = Isotopes::isotope(parser.argi(0), parser.argi(1));
    population_ = parser.argi(2);
    fraction_ = parser.argd(3);
    return true;
}

/*
 * Parallel Comms
 */

// Broadcast data from Master to all Slaves
bool IsotopeData::broadcast(ProcessPool &procPool, const int root, const CoreData &coreData)
{
#ifdef PARALLEL
    // For isotope_, need to broadcast element Z and isotope A
    int Z, A;
    if (procPool.poolRank() == root)
    {
        Z = isotope_->element().Z();
        A = isotope_->A();
    }
    procPool.broadcast(Z, root);
    procPool.broadcast(A, root);
    isotope_ = Isotopes::isotope(Z, A);

    procPool.broadcast(population_, root);
    procPool.broadcast(fraction_, root);
#endif
    return true;
}

// Check item equality
bool IsotopeData::equality(ProcessPool &procPool)
{
#ifdef PARALLEL
    if (!procPool.equality(isotope_->element().Z()))
        return Messenger::error("IsotopeData element z is not equivalent (process {} has '{}').\n", procPool.poolRank(),
                                isotope_->element().Z());
    if (!procPool.equality(isotope_->A()))
        return Messenger::error("IsotopeData isotope A is not equivalent (process {} has {}).\n", procPool.poolRank(),
                                isotope_->A());
    if (!procPool.equality(population_))
        return Messenger::error("IsotopeData population is not equivalent (process {} has {}).\n", procPool.poolRank(),
                                population_);
    if (!procPool.equality(fraction_))
        return Messenger::error("IsotopeData fraction is not equivalent (process {} has {:e}).\n", procPool.poolRank(),
                                fraction_);
#endif
    return true;
}
