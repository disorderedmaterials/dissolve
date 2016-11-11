/*
	*** Messaging Routines
	*** src/base/messenger.cpp
	Copyright T. Youngs 2012-2016

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

#include "base/messenger.h"
#include "base/sysfunc.h"
#include "base/processpool.h"
#include <stdarg.h>
#include <stdio.h>

// Static Members
bool Messenger::quiet_ = false;
bool Messenger::verbose_ = false;
bool Messenger::redirect_ = false;
bool Messenger::masterOnly_ = false;
LineParser Messenger::parser_;
char Messenger::text_[8096];

// Constructor
Messenger::Messenger()
{
}

// Set status of quiet mode
void Messenger::setQuiet(bool b)
{
	quiet_ = b;
}

// Set status of verbose mode
void Messenger::setVerbose(bool b)
{
	verbose_ = b;
}

// Set status of master-only mode
void Messenger::setMasterOnly(bool b)
{
	masterOnly_ = b;
}

// Master text creation / formatting routine
void Messenger::createAndPrintText(const char* indentText, const char* format, va_list arguments)
{
	text_[0] = '\0';
	
	// If we were given some 'indentText', print copies of it before the message
	Dnchar newFormat;
	if (indentText != NULL)
	{
		if (redirect_ || (ProcessPool::nWorldProcesses() == 1)) newFormat.sprintf("%s\n%s %s%s\n", indentText, indentText, format, indentText);
		else newFormat.sprintf("[%i] %s\n[%i]%s %s[%i]%s\n", ProcessPool::worldRank(), indentText, ProcessPool::worldRank(), indentText, format, ProcessPool::worldRank(), indentText);
	}
	else
	{
		if (redirect_ || (ProcessPool::nWorldProcesses() == 1)) newFormat = format;
		else newFormat.sprintf("[%i] %s", ProcessPool::worldRank(), format);
	}

	// Parse the argument list (...) and internally write the output string into text[]
	vsprintf(text_, newFormat.get(), arguments);

	// Print the text
	if (masterOnly_ && (!ProcessPool::isWorldMaster())) return;
	if (redirect_) parser_.writeLineF("%s", text_);
	else printf("%s", text_);
}

// Print standard message
void Messenger::print(const char* fmt, ...)
{
	if (quiet_) return;

	va_list arguments;
	va_start(arguments,fmt);
	createAndPrintText(NULL, fmt, arguments);
	va_end(arguments);
}

// Print verbose message
void Messenger::printVerbose(const char* fmt, ...)
{
	if (quiet_ || (!verbose_)) return;

	va_list arguments;
	va_start(arguments,fmt);
	createAndPrintText(NULL, fmt, arguments);
	va_end(arguments);
}

// Print error message
void Messenger::error(const char* fmt, ...)
{
	va_list arguments;
	va_start(arguments,fmt);
	createAndPrintText("[[[ ERROR ]]]", fmt, arguments);
	va_end(arguments);
}

// Print warning message
void Messenger::warn(const char* fmt, ...)
{
	va_list arguments;
	va_start(arguments, fmt);
	createAndPrintText("[[[ WARNING ]]]", fmt, arguments);
	va_end(arguments);
}

/*
 * File Redirection
 */

// Enable redirection of all messaging to specified file
bool Messenger::enableRedirect(const char* filename)
{
	parser_.openOutput(filename, true);
	if (!parser_.isFileGoodForWriting())
	{
		Messenger::print("Couldn't open output file '%s' for writing.\n", filename);
		return false;
	}
	
	redirect_ = true;
	return true;
}
