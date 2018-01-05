/*
	*** Refine Module - Options
	*** src/modules/refine/options.cpp
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

#include "modules/refine/refine.h"
#include "module/list.h"
#include "module/keywordtypes.h"
#include "base/lineparser.h"

// Potential Generation Type
const char* PotentialGenerationTypeKeywords[] = { "Direct", "PY", "HNC" };

// Convert text string to PotentialGenerationType
RefineModule::PotentialGenerationType RefineModule::potentialGenerationType(const char* s)
{
	for (int n=0; n<RefineModule::nPotentialGenerationTypes; ++n) if (DUQSys::sameString(s, PotentialGenerationTypeKeywords[n])) return (RefineModule::PotentialGenerationType) n;
	return RefineModule::nPotentialGenerationTypes;
}

// Convert PotentialGenerationType to text string
const char* RefineModule::potentialGenerationType(RefineModule::PotentialGenerationType pgt)
{
	return PotentialGenerationTypeKeywords[pgt];
}

// Set up keywords for Module
void RefineModule::setUpKeywords()
{
	keywords_.add(new BoolModuleKeyword(true), "OnlyWhenStable", "Assesses the energy of the Configurations contributing to the Partials, refining the potential only when all related Configuration energies are stable");
	keywords_.add(new CharStringModuleKeyword("PY", RefineModule::nPotentialGenerationTypes, PotentialGenerationTypeKeywords), "PotentialGeneration", "Pair potential generation method to employ");
	keywords_.add(new DoubleModuleKeyword(0.25, 0.01, 1.0), "Weighting", "Fractional (maximal) amounts of generated perturbations to apply to pair potentials");
	keywords_.add(new CharStringModuleKeyword("None", XYData::nWindowFunctions, XYData::windowFunctionKeywords()), "WindowFunction", "Window function to apply when Fourier-transforming delta S(Q) to delta g(r)");

}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int RefineModule::parseComplexKeyword(ModuleKeywordBase* keyword, LineParser& parser, DUQ* duq, GenericList& targetList, const char* prefix)
{
	if (DUQSys::sameString("__", parser.argc(0)))
	{
	}
	else return -1;

	return true;
}
