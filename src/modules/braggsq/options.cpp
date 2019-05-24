/*
	*** BraggSQ Module - Options
	*** src/modules/braggsq/options.cpp
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

#include "modules/braggsq/braggsq.h"
#include "module/keywordtypes.h"
#include "main/dissolve.h"
#include "math/averaging.h"
#include "classes/species.h"
#include "base/lineparser.h"
#include "templates/enumhelpers.h"
#include "templates/genericlisthelper.h"

// Set up keywords for Module
void BraggSQModule::setUpKeywords()
{
	frequency_ = 5;

	ModuleKeywordGroup* group = addKeywordGroup("Calculation");
	group->add(new IntegerModuleKeyword(5, 1), "Averaging", "Number of historical data sets to combine into final reflection data", "<5>");
	group->add(new EnumStringModuleKeyword(Averaging::averagingSchemes() = Averaging::LinearAveraging), "AveragingScheme", "Weighting scheme to use when averaging reflection data", "<Linear>");
	group->add(new DoubleModuleKeyword(0.001), "QDelta", "Resolution (binwidth) in Q space to use when calculating Bragg reflections", "<0.001>");
	group->add(new DoubleModuleKeyword(1.0), "QMax", "Maximum Q value for Bragg calculation", "<1.0>");
	group->add(new DoubleModuleKeyword(0.01), "QMin", "Minimum Q value for Bragg calculation", "<0.01>");
	group->add(new Vec3IntegerModuleKeyword(Vec3<int>(1,1,1), Vec3<int>(1,1,1)), "Multiplicity", "Bragg intensity scaling factor accounting for number of repeat units in Configuration", "<1 1 1>");

	group = addKeywordGroup("Export");
	group->add(new BoolModuleKeyword(false), "SavePartials", "Whether to save Bragg partials to disk after calculation", "<True|False>");
	group->add(new BoolModuleKeyword(false), "SaveReflections", "Whether to save Bragg reflection data to disk", "<True|False>");
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int BraggSQModule::parseComplexKeyword(ModuleKeywordBase* keyword, LineParser& parser, Dissolve* dissolve, GenericList& targetList, const char* prefix)
{
	return -1;
}

