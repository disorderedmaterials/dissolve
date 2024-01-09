// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/isotopeData.h"
#include "base/lineParser.h"
#include "base/messenger.h"
#include "data/elements.h"
#include "data/isotopes.h"

IsotopeData::IsotopeData(Sears91::Isotope isotope, double population, double fraction)
    : isotope_(isotope), population_(population), fraction_(fraction)
{
}

/*
 * Properties
 */

// Initialise
void IsotopeData::initialise(Sears91::Isotope isotope)
{
    isotope_ = isotope;
    population_ = 0.0;
    fraction_ = 0.0;
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
Sears91::Isotope IsotopeData::isotope() const { return isotope_; }

// Return total population over all isotopes
double IsotopeData::population() const { return population_; }

// Return fractional population
double IsotopeData::fraction() const { return fraction_; }
