/*
	*** Module Keyword - File and Format
	*** src/modules/keywordtypes/fileandformat.cpp
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

#include "module/keywordtypes/fileandformat.h"
#include "base/fileandformat.h"
#include "base/lineparser.h"

// Constructor
FileAndFormatModuleKeyword::FileAndFormatModuleKeyword(FileAndFormat& fileAndFormat) : ModuleKeywordBase(ModuleKeywordBase::FileAndFormatData), ModuleKeywordData<FileAndFormat&>(fileAndFormat)
{
}

// Destructor
FileAndFormatModuleKeyword::~FileAndFormatModuleKeyword()
{
}

/*
 * Data
 */

// Return whether the current data value has ever been set
bool FileAndFormatModuleKeyword::isSet()
{
	return set_;
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int FileAndFormatModuleKeyword::minArguments()
{
	// Must have data format as bare minimum
	return 1;
}

// Return maximum number of arguments accepted
int FileAndFormatModuleKeyword::maxArguments()
{
	// Data format and filename
	return 2;
}

// Parse arguments from supplied LineParser, starting at given argument offset, utilising specified ProcessPool if required
bool FileAndFormatModuleKeyword::read(LineParser& parser, int startArg, const CoreData& coreData, ProcessPool& procPool)
{
	if (!data_.read(parser, startArg)) return Messenger::error("Failed to read file/format.\n");

	set_ = true;

	return true;
}

// Write keyword data to specified LineParser
bool FileAndFormatModuleKeyword::write(LineParser& parser, const char* prefix)
{
	if (!parser.writeLineF("%s%s  %s\n", prefix, keyword(), data_.asString())) return false;

	return true;
}
