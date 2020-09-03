/*
    *** Energy Module - Definition
    *** src/modules/energy/definition.cpp
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

#include "modules/energy/energy.h"

// Return type of module
std::string_view EnergyModule::type() const { return "Energy"; }

// Return category for module
std::string_view EnergyModule::category() const { return "Forcefield"; }

// Return brief description of module
std::string_view EnergyModule::brief() const { return "Calculate the total energy of a Configuration"; }

// Return the number of Configuration targets this Module requires
int EnergyModule::nRequiredTargets() const { return Module::OneOrMoreTargets; }
