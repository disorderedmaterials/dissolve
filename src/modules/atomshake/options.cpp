/*
	*** AtomShake Module - Options
	*** src/modules/atomshake/options.cpp
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
#include "module/keywordtypes.h"

// Set up options for Module
void AtomShakeModule::setUpKeywords()
{
	keywords_.add(new DoubleModuleKeyword(-1.0, -1.0), "CutoffDistance", "Interatomic cutoff distance to employ");
	keywords_.add(new IntegerModuleKeyword(1, 1, 1000), "ShakesPerAtom", "Number of shakes per Atom to attempt");
	keywords_.add(new DoubleModuleKeyword(0.33, 0.01, 1.0), "TargetAcceptanceRate", "Target acceptance rate for Monte Carlo moves");
	keywords_.add(new DoubleModuleKeyword(0.05), "StepSize", "Maximal step size for single Monte Carlo move", GenericItem::InRestartFileFlag);
}

// Parse complex keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int AtomShakeModule::parseComplexKeyword(ModuleKeywordBase* keyword, LineParser& parser, Dissolve* dissolve, GenericList& targetList, const char* prefix)
{
	return -1;
}
