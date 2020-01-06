/*
	*** MolShake Module - Definition
	*** src/modules/molshake/definition.cpp
	Copyright T. Youngs 2012-2019

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

#include "modules/molshake/molshake.h"

// Return type of module
const char* MolShakeModule::type() const
{
	return "MolShake";
}

// Return category for module
const char* MolShakeModule::category() const
{
	return "Evolution";
}

// Return brief description of module
const char* MolShakeModule::brief() const
{
	return "Perform molecular Monte Carlo moves";
}

// Return the number of Configuration targets this Module requires
int MolShakeModule::nRequiredTargets() const
{
	return Module::ExactlyOneTarget;
}
