/*
	*** CalculateCoordinationNumber Module - Definition
	*** src/modules/calculate_cn/definition.cpp
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

#include "modules/calculate_cn/cn.h"

// Return type of module
const char* CalculateCoordinationNumberModule::type() const
{
	return "CalculateCoordinationNumber";
}

// Return category for module
const char* CalculateCoordinationNumberModule::category() const
{
	return "Analysis";
}

// Return brief description of module
const char* CalculateCoordinationNumberModule::brief() const
{
	return "Calculate coordination numbers from an existing radial distribution function";
}

// Return the maximum number of Configurations the Module can target (or -1 for any number)
int CalculateCoordinationNumberModule::nTargetableConfigurations() const
{
	return 0;
}
