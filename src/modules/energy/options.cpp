/*
	*** Energy Module - Options
	*** src/modules/energy/options.cpp
	Copyright T. Youngs 2012-2017

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "modules/energy/energy.h"

// Setup options for module
void EnergyModule::setupOptions()
{
	// Boolean options must be set as 'bool(false)' or 'bool(true)' rather than just 'false' or 'true' so that the correct overloaded add() function is called
	options_.add("Save", bool(true), "Save calculate energy points to the file '<name>.energy.txt'");
	options_.add("StabilityWindow", 10, "Number of points over which to assess the stability of the energy (per Configuration)");
	options_.add("StabilityThreshold", 0.01, "Threshold value at which energy is deemed stable over the defined windowing period");
	options_.add("Test", bool(false), "Test parallel energy routines against simplified, serial ones");
	options_.add("TestAnalytic", bool(false), "Compare parallel energy routines against exact (analytic) energy rather than tabulated values");
	options_.add("TestReferenceInter", 0.0, "Reference value for interatomic energy against which to test calculated value");
	options_.add("TestReferenceIntra", 0.0, "Reference value for intramolecular energy against which to test calculated value");
	options_.add("TestThreshold", 1.0e-1, "Threshold of energy at which test comparison will fail");
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int EnergyModule::parseKeyword(LineParser& parser, DUQ* duq, GenericList& targetList)
{
	return -1;
}
