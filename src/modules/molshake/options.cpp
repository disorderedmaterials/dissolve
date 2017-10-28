/*
	*** MolShake Module - Options
	*** src/modules/molshake/options.cpp
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

#include "modules/molshake/molshake.h"
#include "module/keywordtypes.h"

// Setup options for Module
void MolShakeModule::setupKeywords()
{
	keywords_.add(new DoubleModuleKeyword(-1.0), "CutoffDistance", "Interatomic cutoff distance to employ");
	keywords_.add(new IntegerModuleKeyword(1), "ShakesPerMolecule", "Number of shakes per Molecule");
	keywords_.add(new DoubleModuleKeyword(0.33), "TargetAcceptanceRate", "Target acceptance rate for Monte Carlo moves");
	keywords_.add(new DoubleModuleKeyword(0.05), "StepSize", "Step size for single Monte Carlo move", GenericItem::InRestartFileFlag);
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int MolShakeModule::parseComplexKeyword(ModuleKeywordBase* keyword, LineParser& parser, DUQ* duq, GenericList& targetList, const char* prefix)
{
	return -1;
}
