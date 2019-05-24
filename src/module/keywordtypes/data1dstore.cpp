/*
	*** Module Keyword - Data Store
	*** src/modules/keywordtypes/datastore.cpp
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

#include "module/keywordtypes/data1dstore.h"
#include "base/lineparser.h"

// Constructor
Data1DStoreModuleKeyword::Data1DStoreModuleKeyword(Data1DStore& dataStore) : ModuleKeywordBase(ModuleKeywordBase::DataStoreData), ModuleKeywordData<Data1DStore&>(dataStore)
{
}

// Destructor
Data1DStoreModuleKeyword::~Data1DStoreModuleKeyword()
{
}

/*
 * Data
 */

// Return whether the current data value has ever been set
bool Data1DStoreModuleKeyword::isSet()
{
	return set_;
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int Data1DStoreModuleKeyword::minArguments()
{
	// Must have reference data name and format as a minimum
	return 2;
}

// Return maximum number of arguments accepted
int Data1DStoreModuleKeyword::maxArguments()
{
	// Filename, name of data, and other args
	return 99;
}

// Parse arguments from supplied LineParser, starting at given argument offset, utilising specified ProcessPool if required
bool Data1DStoreModuleKeyword::read(LineParser& parser, int startArg, const CoreData& coreData, ProcessPool& procPool)
{
	Messenger::print("Reading test data '%s' from file '%s'...\n", parser.argc(startArg), parser.argc(startArg+1));

	// Target data is named as the first argument - a FileAndFormat specifies the rest...
	Data1DImportFileFormat ff;
	if (!ff.read(parser, startArg+1)) return false;

	if (!data_.addData(procPool, ff, parser.argc(startArg))) return Messenger::error("Failed to add data.\n");

	set_ = true;

	return true;
}

// Write keyword data to specified LineParser
bool Data1DStoreModuleKeyword::write(LineParser& parser, const char* prefix)
{
	// Loop over list of one-dimensional data
	RefListIterator<Data1D,Data1DImportFileFormat> dataIterator(data_.dataReferences());
	while (Data1D* data = dataIterator.iterate())
	{
		Data1DImportFileFormat ff = dataIterator.currentData();
		if (!parser.writeLineF("%s%s  '%s'  %s\n", prefix, keyword(), data->name(), ff.asString())) return false;
	}

	return true;
}
