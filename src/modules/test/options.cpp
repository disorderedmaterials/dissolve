/*
	*** Test Module - Options
	*** src/modules/test/options.cpp
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

#include "modules/test/test.h"
#include "modules/modulelist.h"
#include "base/lineparser.h"

// Setup options for module
void TestModule::setupOptions()
{
	// Boolean options must be set as 'bool(false)' or 'bool(true)' rather than just 'false' or 'true' so that the correct overloaded add() function is called
	options_.add("Test", bool(false));
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int TestModule::parseKeyword(LineParser& parser, DUQ* duq, GenericList& targetList)
{
	if (DUQSys::sameString("AddTotalSQ", parser.argc(0)))
	{
		// Find the Module named by the first argument - it must be a Partials module in which the StructureFactor calculation is 'On'
		Module* partialsModule = ModuleList::findInstanceByUniqueName(parser.argc(1));
		if (!partialsModule)
		{
			Messenger::error("Specified Module target '%s' does not exist.", parser.argc(1));
			return false;
		}
		else if (!DUQSys::sameString(partialsModule->name(), "Partials"))
		{
			Messenger::error("Specified Module target '%s' is not a Partials module.", parser.argc(1));
			return false;
		}
		else if (!partialsModule->options().valueAsBool("StructureFactor"))
		{
			Messenger::error("Specified Module target '%s' does not calculate a structure factor.", parser.argc(1));
			return false;
		}

		// All fine, so add data to the list and load any reference data
		RefListItem<Module,XYData>* newSample = sampleData_.add(partialsModule);
		if (parser.hasArg(2) && !newSample->data.load(parser.argc(2))) return false;
	}
	else return -1;

	return true;
}
