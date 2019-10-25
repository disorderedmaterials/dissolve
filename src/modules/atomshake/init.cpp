/*
	*** AtomShake Module - Initialisation
	*** src/modules/atomshake/init.cpp
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

#include "modules/atomshake/atomshake.h"
#include "keywords/types.h"

// Perform any necessary initialisation for the Module
void AtomShakeModule::initialise()
{
	// Calculation
	keywords_.add("Calculation", new DoubleKeyword(-1.0, -1.0), "CutoffDistance", "Interatomic cutoff distance to employ");
	keywords_.add("Calculation", new IntegerKeyword(1, 1, 1000), "ShakesPerAtom", "Number of shakes per Atom to attempt");
	keywords_.add("Calculation", new DoubleKeyword(0.33, 0.01, 1.0), "TargetAcceptanceRate", "Target acceptance rate for Monte Carlo moves");

	// Translations
	keywords_.add("Translations", new DoubleKeyword(0.05), "StepSize", "Step size for translational component of Monte Carlo move (Angstroms)", "<stepsize>", KeywordBase::InRestartFileOption);
	keywords_.add("Translations", new DoubleKeyword(1.0), "StepSizeMax", "Maximum step size for translations (Angstroms)", "<stepsize>");
	keywords_.add("Translations", new DoubleKeyword(0.001), "StepSizeMin", "Minimum step size for translations (Angstroms)", "<stepsize>");
}
