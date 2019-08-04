/*
	*** MolShake Module - Options
	*** src/modules/molshake/options.cpp
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
#include "keywords/types.h"

// Set up options for Module
void MolShakeModule::setUpKeywords()
{
	// Calculation
	ModuleKeywordGroup* group = addKeywordGroup("Calculation");
	group->add(new DoubleModuleKeyword(-1.0), "CutoffDistance", "Interatomic cutoff distance to employ", "<rcut>");
	group->add(new IntegerModuleKeyword(1), "ShakesPerMolecule", "Number of shakes per Molecule", "<n>");
	group->add(new DoubleModuleKeyword(0.33), "TargetAcceptanceRate", "Target acceptance rate for Monte Carlo moves", "<rate (0.0 - 1.0)>");

	// Rotations
	group = addKeywordGroup("Rotations");
	group->add(new DoubleModuleKeyword(1.0), "RotationStepSize", "Step size for rotational component of Monte Carlo move (degrees)", "<stepsize>", GenericItem::InRestartFileFlag);
	group->add(new DoubleModuleKeyword(0.01), "RotationStepSizeMin", "Minimum step size for rotations (degrees)", "<stepsize>");
	group->add(new DoubleModuleKeyword(90.0), "RotationStepSizeMax", "Maximum step size for rotations (degrees)", "<stepsize>");


	// Translations
	group = addKeywordGroup("Translations");
	group->add(new DoubleModuleKeyword(0.05), "TranslationStepSize", "Step size for translational component of Monte Carlo move (Angstroms)", "<stepsize>", GenericItem::InRestartFileFlag);
	group->add(new DoubleModuleKeyword(0.001), "TranslationStepSizeMin", "Minimum step size for translations (Angstroms)", "<stepsize>");
	group->add(new DoubleModuleKeyword(3.0), "TranslationStepSizeMax", "Maximum step size for translations (Angstroms)", "<stepsize>");
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int MolShakeModule::parseComplexKeyword(ModuleKeywordBase* keyword, LineParser& parser, Dissolve* dissolve, GenericList& targetList, const char* prefix)
{
	return -1;
}
