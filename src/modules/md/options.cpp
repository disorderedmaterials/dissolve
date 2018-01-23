/*
	*** MD Module - Options
	*** src/modules/md/options.cpp
	Copyright T. Youngs 2012-2018

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

#include "modules/md/md.h"
#include "module/keywordtypes.h"

// Set up keywords for Module
void MDModule::setUpKeywords()
{
	keywords_.add(new BoolModuleKeyword(false), "CapForces", "Control whether atomic forces are capped every step");
	keywords_.add(new DoubleModuleKeyword(1.0e7), "CapForcesAt", "Set cap on allowable force (kJ/mol/Angstrom**2) per atom");
	keywords_.add(new DoubleModuleKeyword(-1.0), "CutoffDistance", "Interatomic cutoff distance to employ");
	keywords_.add(new DoubleModuleKeyword(1.0e-4), "DeltaT", "Timestep (ps) to use in MD simulation");
	keywords_.add(new IntegerModuleKeyword(10), "EnergyFrequency", "Frequency at which to calculate total system energy (or 0 to inhibit)");
	keywords_.add(new IntegerModuleKeyword(100), "NSteps", "Number of MD steps to perform");
	keywords_.add(new IntegerModuleKeyword(5), "OutputFrequency", "Frequency at which to output step information (or 0 to inhibit)");
	keywords_.add(new BoolModuleKeyword(false), "RandomVelocities", "Whether random velocities should always be assigned before beginning MD simulation");
	keywords_.add(new IntegerModuleKeyword(0), "TrajectoryFrequency", "Write frequency for trajectory file (or 0 to inhibit)");
	keywords_.add(new BoolModuleKeyword(true), "VariableTimestep", "Whether a variable timestep should be used, determined from the maximal force vector");
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int MDModule::parseComplexKeyword(ModuleKeywordBase* keyword, LineParser& parser, DUQ* duq, GenericList& targetList, const char* prefix)
{
	return -1;
}
