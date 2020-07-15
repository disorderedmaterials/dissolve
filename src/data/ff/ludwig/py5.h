/*
    *** Py5 Forcefield
    *** src/data/ff/ludwig/py5.h
    Copyright T. Youngs 2019-2020

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

#include "data/ff.h"

// Forward Declarations
class CoreData;
class SpeciesAtom;

// Py4OH Forcefield
class Forcefield_Ludwig_Py5 : public Forcefield
{
    public:
    Forcefield_Ludwig_Py5();
    ~Forcefield_Ludwig_Py5();

    /*
     * Set Up
     */
    protected:
    // Set up / create all forcefield terms
    bool setUp();

    /*
     * Definition
     */
    public:
    // Return name of Forcefield
    const char *name() const;
    // Return description for Forcefield
    const char *description() const;
    // Return short-range interaction style for AtomTypes
    Forcefield::ShortRangeType shortRangeType() const;
};
