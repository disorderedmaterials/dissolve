/*
	*** Import Module - Formats
	*** src/modules/import/formats.h
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

#ifndef DISSOLVE_FILEANDFORMAT_H
#define DISSOLVE_FILEANDFORMAT_H

#include "base/charstring.h"

// Forward Declarations
class LineParser;

// File / Format Base
class FileAndFormat
{
	public:
	// Constructor
	FileAndFormat();
	// Conversion to const char*
	operator const char*() const;


	/*
	 * Available Formats
	 */
	protected:
	// Format of associated file
	int format_;

	public:
	// Return umber of available formats
	virtual int nFormats() const = 0;
	// Return formats array
	virtual const char** formats() const = 0;
	// Return nice formats array
	virtual const char** niceFormats() const = 0;
	// Convert text string to format index
	int format(const char* s) const;
	// Convert format index to text string
	const char* format(int id) const;
	// Return format index
	int formatIndex() const;
	// Return format string
	const char* format() const;
	// Return nice format string
	const char* niceFormat() const;


	/*
	 * Filename
	 */
	protected:
	// Name of associated file
	CharString filename_;

	public:
	// Return filename
	const char* filename() const;


	/*
	 * Check
	 */
	public:
	// Return whether a valid filename and format have been set
	bool hasValidFileAndFormat() const;


	/*
	 * Read / Write
	 */
	public:
	// Read format / filename from specified parser
	bool read(LineParser& parser, int startArg, bool checkFileExists = true);
	// Return formatted string for writing
	const char* asString() const;
};

#endif

