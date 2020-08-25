/*
    *** Refine Module - Definition
    *** src/modules/refine/definition.cpp
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

#include "modules/refine/refine.h"

// Return type of module
std::string_view RefineModule::type() const { return "Refine"; }

// Return category for module
std::string_view RefineModule::category() const { return "Forcefield"; }

// Return brief description of module
std::string_view RefineModule::brief() const
{
    return "Refine interatomic potentials based on differences between calculated and reference data";
}

// Return the number of Configuration targets this Module requires
int RefineModule::nRequiredTargets() const { return Module::ZeroTargets; }
