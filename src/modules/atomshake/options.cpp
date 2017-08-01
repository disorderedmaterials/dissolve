/*
	*** AtomShake Module - Options
	*** src/modules/atomshake/options.cpp
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

#include "modules/atomshake/atomshake.h"

// Setup options for Module
void AtomShakeModule::setupOptions()
{
	options_.add("CutoffDistance", -1.0, "Interatomic cutoff distance to employ");
	options_.add("ShakesPerAtom", 1, "Number of shakes per atom");
	options_.add("TargetAcceptanceRate", 0.33, "Target acceptance rate for Monte Carlo moves");
	options_.add("StepSize", 0.05, "Maximal step size for single Monte Carlo move", GenericItem::InRestartFileFlag);
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int AtomShakeModule::parseKeyword(LineParser& parser, DUQ* duq, GenericList& targetList)
{
	return -1;
}
