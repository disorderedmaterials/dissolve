/*
	*** Module Keyword - Data Store
	*** src/modules/keywordtypes/datastore.cpp
	Copyright T. Youngs 2012-2018

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

#include "module/keywordtypes/datastore.h"
#include "base/lineparser.h"

// Constructor
DataStoreModuleKeyword::DataStoreModuleKeyword(DataStore& dataStore) : ModuleKeywordBase(ModuleKeywordBase::DataStoreData), ModuleKeywordData<DataStore&>(dataStore)
{
}

// Destructor
DataStoreModuleKeyword::~DataStoreModuleKeyword()
{
}

/*
 * Data
 */

// Return whether the current data value has ever been set
bool DataStoreModuleKeyword::isSet()
{
	return set_;
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int DataStoreModuleKeyword::minArguments()
{
	// Must have filename and name of data
	return 2;
}

// Return maximum number of arguments accepted
int DataStoreModuleKeyword::maxArguments()
{
	// Filename, name of data, x column, y column
	return 4;
}

// Parse arguments from supplied LineParser, starting at given argument offset, utilising specified ProcessPool if required
bool DataStoreModuleKeyword::read(LineParser& parser, int startArg, ProcessPool& procPool)
{
	Messenger::print("Reading test data '%s' from file '%s'...\n", parser.argc(startArg), parser.argc(startArg+1));

	// N+2 and N+3 arguments are x and y columns respectively (defaulting to 0,1 if not given)
	int xcol = parser.hasArg(startArg+2) ? parser.argi(startArg+2)-1 : 0;
	int ycol = parser.hasArg(startArg+3) ? parser.argi(startArg+3)-1 : 1;

	if (!data_.addData1D(procPool, parser.argc(startArg), parser.argc(startArg+1), xcol, ycol))
	{
		Messenger::error("Failed to add data.\n");
		return false;
	}

	set_ = true;

	return true;
}

// Write keyword data to specified LineParser
bool DataStoreModuleKeyword::write(LineParser& parser, const char* prefix)
{
	// Loop over list of one-dimensional data
	RefListIterator<Data1D,FileReference> dataIterator(data_.data1DReferences());
	while (Data1D* data = dataIterator.iterate())
	{
		FileReference ref = dataIterator.currentData();
		if (!parser.writeLineF("%s%s  '%s'  '%s'  '%s'  %f\n", prefix, keyword(), ref.filename(), data->name(), ref.xColumn(), ref.yColumn())) return false;
	}

	return true;
}
