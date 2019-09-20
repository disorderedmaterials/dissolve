/*
	*** Energy Module - Initialisation
	*** src/modules/energy/init.cpp
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

#include "modules/energy/energy.h"
#include "keywords/types.h"

// Perform any necessary initialisation for the Module
void EnergyModule::initialise()
{
	// Calculation
	keywords_.add("Calculation", new DoubleKeyword(0.001), "StabilityThreshold", "Threshold value at which energy is deemed stable over the defined windowing period", "<value[0.0-1.0]>");
	keywords_.add("Calculation", new IntegerKeyword(10), "StabilityWindow", "Number of points over which to assess the stability of the energy (per Configuration)");

	// Test
	keywords_.add("Test", new BoolKeyword(false), "Test", "Test parallel energy routines against simplified, serial ones");
	keywords_.add("Test", new BoolKeyword(false), "TestAnalytic", "Compare parallel energy routines against exact (analytic) energy rather than tabulated values");
	keywords_.add("Test", new DoubleKeyword(0.0), "TestReferenceInter", "Reference value for interatomic energy against which to test calculated value");
	keywords_.add("Test", new DoubleKeyword(0.0), "TestReferenceIntra", "Reference value for intramolecular energy against which to test calculated value");
	keywords_.add("Test", new DoubleKeyword(0.1), "TestThreshold", "Threshold of energy at which test comparison will fail");

	// Export
	keywords_.add("Export", new BoolKeyword(false), "Save", "Save calculate energy points to the file '<name>.energy.txt'");
}


