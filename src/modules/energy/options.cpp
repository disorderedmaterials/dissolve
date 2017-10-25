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
#include "modules/modulekeywords.h"

// Set up keywords for Module
void EnergyModule::setupKeywords()
{
	keywords_.add(new BoolModuleKeyword(true), "Save", "Save calculate energy points to the file '<name>.energy.txt'");
	keywords_.add(new IntegerModuleKeyword(10), "StabilityWindow", "Number of points over which to assess the stability of the energy (per Configuration)");
	keywords_.add(new DoubleModuleKeyword(0.01), "StabilityThreshold", "Threshold value at which energy is deemed stable over the defined windowing period");
	keywords_.add(new BoolModuleKeyword(false), "Test", "Test parallel energy routines against simplified, serial ones");
	keywords_.add(new BoolModuleKeyword(false), "TestAnalytic", "Compare parallel energy routines against exact (analytic) energy rather than tabulated values");
	keywords_.add(new DoubleModuleKeyword(0.0), "TestReferenceInter", "Reference value for interatomic energy against which to test calculated value");
	keywords_.add(new DoubleModuleKeyword(0.0), "TestReferenceIntra", "Reference value for intramolecular energy against which to test calculated value");
	keywords_.add(new DoubleModuleKeyword(0.1), "TestThreshold", "Threshold of energy at which test comparison will fail");
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int EnergyModule::parseComplexKeyword(ModuleKeywordBase* keyword, LineParser& parser, DUQ* duq, GenericList& targetList, const char* prefix)
{
	return -1;
}
