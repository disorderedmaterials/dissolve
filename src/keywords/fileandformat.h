/*
	*** Keyword - File Format
	*** src/keywords/fileformat.h
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

#ifndef DISSOLVE_KEYWORD_FILEANDFORMAT_H
#define DISSOLVE_KEYWORD_FILEANDFORMAT_H

#include "keywords/data.h"

// Forward Declarations
class FileAndFormat;

// Keyword with a file format 
class FileAndFormatKeyword : public KeywordData<FileAndFormat&>
{
	public:
	// Constructor
	FileAndFormatKeyword(FileAndFormat& fileAndFormat, const char* endKeyword);
	// Destructor
	~FileAndFormatKeyword();


	/*
	 * End Keyword
	 */
	private:
	// Keyword that signals the end of 'block' for the data
	CharString endKeyword_;


	/*
	 * Keyword Options
	 */
	public:
	// Return whether the underlying FileAndFormat has any options
	bool hasOptions() const;


	/*
	 * Arguments
	 */
	public:
	// Return minimum number of arguments accepted
	int minArguments() const;
	// Return maximum number of arguments accepted
	int maxArguments() const;
	// Parse arguments from supplied LineParser, starting at given argument offset
	bool read(LineParser& parser, int startArg, const CoreData& coreData);
	// Write keyword data to specified LineParser
	bool write(LineParser& parser, const char* keywordName, const char* prefix);
};

#endif
