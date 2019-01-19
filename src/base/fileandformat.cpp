/*
	*** File And Format
	*** src/base/fileandformat.cpp
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

#include "base/fileandformat.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Constructor
FileAndFormat::FileAndFormat()
{
	format_ = -1;
}

// Conversion operators
FileAndFormat::operator const char*() const
{
	return filename_.get();
}

/*
 * Formats
 */

// Convert text string to format index
int FileAndFormat::format(const char* s) const
{
	for (int n=0; n<nFormats(); ++n) if (DissolveSys::sameString(s, formats()[n])) return n;

	return nFormats();
}

// Convert format index to text string
const char* FileAndFormat::format(int id) const
{
	if ((id < 0) || (id >= nFormats())) return "???";

	return formats()[id];
}

// Return format
int FileAndFormat::format() const
{
	return format_;
}

// Return format string
const char* FileAndFormat::formatString() const
{
	if ((format_ < 0) || (format_ >= nFormats())) return "???";
	else return format(format_);
}

/*
 * Filename
 */

// Return filename
const char* FileAndFormat::filename() const
{
	return filename_.get();
}

/*
 * Check
 */

// Return whether a valid filename and format have been set
bool FileAndFormat::hasValidFileAndFormat() const
{
	if (filename_.isEmpty()) return false;
	if ((format_ < 0) || (format_ >= nFormats())) return false;

	return true;
}

/*
 * Read / Write
 */

// Read format / filename from specified parser
bool FileAndFormat::read(LineParser& parser, int startArg, bool checkFileExists)
{
	// Convert first argument to format type
	format_ = format(parser.argc(startArg));
	if (format_ == nFormats()) return Messenger::error("Unrecognised format '%s' given for file.\n", parser.argc(startArg));

	// Set filename if present
	if (parser.hasArg(startArg+1))
	{
		filename_ = parser.argc(startArg+1);

		// Check that the file exists?
		if (checkFileExists && (!DissolveSys::fileExists(filename_))) return Messenger::error("Specified file '%s' does not exist.\n", filename_.get());
	}

	return true;
}

// Return formatted string for writing
const char* FileAndFormat::asString() const
{
}
