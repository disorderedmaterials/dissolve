// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/isotopedata.h"
#include "base/lineparser.h"
#include "base/messenger.h"
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
bool IsotopeData::serialise(LineParser &parser) const
{
    return parser.writeLineF("{} {} {} {}\n", isotope_->Z(), isotope_->A(), population_, fraction_);
}

// Read data through specified LineParser
bool IsotopeData::deserialise(LineParser &parser)
{
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    isotope_ = Isotopes::isotope(Elements::element(parser.argi(0)), parser.argi(1));
    population_ = parser.argi(2);
    fraction_ = parser.argd(3);
    return true;
}
