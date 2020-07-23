/*
    *** Isotopologue Weight
    *** src/classes/isotopologueweight.h
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

#pragma once

#include "genericitems/base.h"
#include "templates/listitem.h"
#include "templates/refdatalist.h"

// Forward Declarations
class Species;
class Isotopologue;
class ProcessPool;
class LineParser;

// Isotopologue Weight
class IsotopologueWeight
{
    public:
    IsotopologueWeight(const Isotopologue *iso = nullptr, double weight = 1.0);
    ~IsotopologueWeight();

    /*
     * Isotopologue and Weight
     */
    private:
    // Associated Isotopologue
    const Isotopologue *isotopologue_;
    // Relative weight
    double weight_;

    public:
    // Set associated Isotopologue and weight
    void set(const Isotopologue *iso, double weight);
    // Set associated Isotopologue
    void setIsotopologue(const Isotopologue *iso);
    // Return associated Isotopologue
    const Isotopologue *isotopologue() const;
    // Set relative weight
    void setWeight(double weight);
    // Return relative weight
    double weight() const;
};
