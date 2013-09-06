/*
	*** Messaging Routines
	*** src/lib/base/messenger.cpp
	Copyright T. Youngs 2012-2013

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
#include "math/constants.h"
#include "base/comms.h"
#include <stdarg.h>
#include <stdio.h>

// Singleton
Messenger msg;

/*
 * Messaging Output Handler
 */

/*!
 * \brief Master print routine
 * \details Basic text output handler
 */
void OutputHandler::printText(char* text)
{
	printf("%s", text);
}

/*
 * Messenger
 */

// Constructor
Messenger::Messenger()
{
	// Private variables
	quiet_ = false;
	verbose_ = false;
	redirect_ = false;
	masterOnly_ = false;

	// Set basicOutputHandler_ as default target
	targetOutputHandler_ = &basicOutputHandler_;
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

// Set target OutputHandler
void Messenger::setTargetOutputHandler(OutputHandler* handler)
{
	targetOutputHandler_ = handler;
}

// Master text creation / formatting routine
void Messenger::createAndPrintText(const char* indentText, const char* format, va_list arguments)
{
	text_[0] = '\0';
	
	// If we were given some 'indentText', print copies of it before the message
	Dnchar newFormat;
	if (indentText != NULL)
	{
		if (redirect_ || (Comm.nProcesses() == 1)) newFormat.sprintf("%s\n%s %s%s\n", indentText, indentText, format, indentText);
		else newFormat.sprintf("[%i] %s\n[%i]%s %s[%i]%s\n", Comm.rank(), indentText, Comm.rank(), indentText, format, Comm.rank(), indentText);
	}
	else
	{
		if (redirect_ || (Comm.nProcesses() == 1)) newFormat = format;
		else newFormat.sprintf("[%i] %s", Comm.rank(), format);
	}

	// Parse the argument list (...) and internally write the output string into text[]
	vsprintf(text_, newFormat.get(), arguments);

	// Print the text
	if (masterOnly_ && Comm.slave()) return;
	if (redirect_) parser_.writeLineF("%s", text_);
	else targetOutputHandler_->printText(text_);
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
// File Redirection
*/

/*!
 * \brief Enable redirection of all messaging to specified file
 * \details If called with a valid fileName, all calls to Messenger::print() and Messenger::printVerbose() will be redirected
 * to the file rather than printed to stdout. This is useful for separating the output of parallel processes.
 */
bool Messenger::enableRedirect(const char* fileName)
{
	parser_.openOutput(fileName, true);
	if (!parser_.isFileGoodForWriting())
	{
		msg.print("Couldn't open output file '%s' for writing.\n", fileName);
		return false;
	}
	
	redirect_ = true;
	return true;
}
