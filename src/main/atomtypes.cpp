/*
    *** Dissolve - AtomTypes
    *** src/main/atomtypes.cpp
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

#include "base/sysfunc.h"
#include "classes/atomtype.h"
#include "main/dissolve.h"

// Add AtomType with specified Element
std::shared_ptr<AtomType> Dissolve::addAtomType(Element *el)
{
    std::shared_ptr<AtomType> newAtomType = coreData_.addAtomType(el);

    return newAtomType;
}

// Return number of AtomTypes in list
int Dissolve::nAtomTypes() const { return coreData_.nAtomTypes(); }

// Return core AtomTypes list
std::vector<std::shared_ptr<AtomType>> &Dissolve::atomTypes() { return coreData_.atomTypes(); }

// Return nth AtomType in list
std::shared_ptr<AtomType> Dissolve::atomType(int n) { return coreData_.atomType(n); }

// Search for AtomType by name
std::shared_ptr<AtomType> Dissolve::findAtomType(std::string_view name) const { return coreData_.findAtomType(name); }

// Clear all AtomTypes
void Dissolve::clearAtomTypes() { coreData_.atomTypes().clear(); }
