/*
	*** DataTest Module - Options
	*** src/modules/datatest/options.cpp
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

#include "modules/datatest/datatest.h"
#include "module/keywordtypes.h"

// Set up keywords for Module
void DataTestModule::setUpKeywords()
{
	// Test
	ModuleKeywordGroup* group = addKeywordGroup("Test");
	group->add(new Data1DStoreModuleKeyword(testData_), "Data1D", "Specify one-dimensional test reference data", "<target> <fileformat> <filename> [x=1] [y=2]");
	group->add(new ModuleReferenceListModuleKeyword(targetModule_, 1), "Target", "Module containing target data", "<Module>");
	group->add(new DoubleModuleKeyword(0.1, 1.0e-5), "Threshold", "Test threshold (%%error) above which test fails", "<threshold[0.1]>");
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int DataTestModule::parseComplexKeyword(ModuleKeywordBase* keyword, LineParser& parser, Dissolve* dissolve, GenericList& targetList, const char* prefix)
{
	return -1;
}
