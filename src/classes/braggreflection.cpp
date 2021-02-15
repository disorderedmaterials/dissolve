// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/braggreflection.h"
#include "base/lineparser.h"
#include "base/processpool.h"
#include "genericitems/array2ddouble.h"

BraggReflection::BraggReflection()
{
    q_ = 0.0;
    index_ = -1;
    nKVectors_ = 0;
}

BraggReflection::~BraggReflection() {}

BraggReflection::BraggReflection(const BraggReflection &source) { (*this) = source; }

// Operator=
void BraggReflection::operator=(const BraggReflection &source)
{
    q_ = source.q_;
    index_ = source.index_;
    intensities_ = source.intensities_;
    nKVectors_ = source.nKVectors_;
}

// Operator+= (intensity addition)
void BraggReflection::operator+=(const BraggReflection &source)
{
    // If we have no intensities array, just copy the source's array
    if (intensities_.nRows() == 0)
        intensities_ = source.intensities_;
    else
        intensities_ += source.intensities_;

    // Copy other data to ensure completeness
    q_ = source.q_;
    index_ = source.index_;
    nKVectors_ = source.nKVectors_;
}

// Operator*= (intensity scaling)
void BraggReflection::operator*=(double scale) { intensities_ *= scale; }

/*
 * Data
 */

// Initialise arrays
void BraggReflection::initialise(double q, int index, int nTypes)
{
    q_ = q;
    index_ = index;
    intensities_.initialise(nTypes, nTypes, true);
    nKVectors_ = 0;
}

// Return Q value
double BraggReflection::q() const { return q_; }

// Set index
void BraggReflection::setIndex(int index) { index_ = index; }

// Return index
int BraggReflection::index() const { return index_; }

// Reset stored intensities
void BraggReflection::reset()
{
    intensities_ = 0.0;
    nKVectors_ = 0;
}

// Add intensity between specified atomtypes
void BraggReflection::addIntensity(int typeI, int typeJ, double intensity) { intensities_[{typeI, typeJ}] += intensity; }

// Scale intensities between all atom types by factor provided
void BraggReflection::scaleIntensities(double factor) { intensities_ *= factor; }

// Scale intensity between all specific atom types by factor provided
void BraggReflection::scaleIntensity(int typeI, int typeJ, double factor)
{
    assert(typeI >= 0 && typeI < intensities_.nRows());
    assert(typeJ >= 0 && typeJ < intensities_.nColumns());

    intensities_[{typeI, typeJ}] *= factor;
}

// Return intensity between specified atom types for this reflection
double BraggReflection::intensity(int typeI, int typeJ) const { return intensities_[{typeI, typeJ}]; }

// Increment number of contributing k-vectors
void BraggReflection::addKVectors(int count) { nKVectors_ += count; }

// Return number of k-vectors contributing to this reflection
int BraggReflection::nKVectors() const { return nKVectors_; }

/*
 * GenericItemBase Implementations
 */

// Return class name
std::string_view BraggReflection::itemClassName() { return "BraggReflection"; }

// Read data through specified parser
bool BraggReflection::read(LineParser &parser, CoreData &coreData)
{
    // Read index, Q centre, and number of contributing K-vectors
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    index_ = parser.argi(0);
    q_ = parser.argd(1);
    nKVectors_ = parser.argi(2);

    // Read intensities array
    if (!GenericItemContainer<Array2D<double>>::read(intensities_, parser))
        return false;

    return true;
}

// Write data through specified parser
bool BraggReflection::write(LineParser &parser)
{
    // Write index, Q centre, and number of contributing K-vectors
    if (!parser.writeLineF("{}  {}  {}\n", index_, q_, nKVectors_))
        return false;

    // Write intensities array
    if (!GenericItemContainer<Array2D<double>>::write(intensities_, parser))
        return false;

    return true;
}

/*
 * Parallel Comms
 */

// Broadcast data from Master to all Slaves
bool BraggReflection::broadcast(ProcessPool &procPool, const int root, const CoreData &coreData)
{
#ifdef PARALLEL
    if (!procPool.broadcast(q_, root))
        return false;
    if (!procPool.broadcast(index_, root))
        return false;
    if (!procPool.broadcast(nKVectors_, root))
        return false;
    if (!procPool.broadcast(intensities_, root))
        return false;
#endif
    return true;
}

// Check item equality
bool BraggReflection::equality(ProcessPool &procPool)
{
#ifdef PARALLEL
    if (!procPool.equality(q_))
        return Messenger::error("BraggReflection Q value is not equivalent (process {} has {:e}).\n", procPool.poolRank(), q_);
    if (!procPool.equality(index_))
        return Messenger::error("BraggReflection index is not equivalent (process {} has {}).\n", procPool.poolRank(), index_);
    if (!procPool.equality(nKVectors_))
        return Messenger::error("BraggReflection nKVectors is not equivalent (process {} has {}).\n", procPool.poolRank(),
                                nKVectors_);
    if (!procPool.equality(intensities_))
        return Messenger::error("BraggReflection intensities are not equivalent.\n");
#endif
    return true;
}
