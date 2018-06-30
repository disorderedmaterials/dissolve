/*
	*** Messaging Routines
	*** src/base/messenger.cpp
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

#include "base/messenger.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "base/processpool.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

// Static Members
bool Messenger::quiet_ = false;
bool Messenger::muted_ = false;
bool Messenger::verbose_ = false;
bool Messenger::redirect_ = false;
bool Messenger::masterOnly_ = false;
LineParser* Messenger::parser_ = new LineParser;
char Messenger::text_[8096];
char Messenger::workingText_[8096];
OutputHandler* Messenger::outputHandler_ = NULL;

// Constructor
Messenger::Messenger()
{
}

/*
 * General Print Routines (Private)
 */

// Master text creation / formatting routine
void Messenger::createText(const char* indentText, const char* format, va_list arguments)
{
	// Reset main text string storage
	text_[0] = '\0';

	// First, vsprintf the supplied format/arguments into workingText_
	vsprintf(workingText_, format, arguments);
	
	// Now, use strtok to split the workingText_ up into lines, prepending the indentText and/or process id to each
	CharString prependedLine;
	char* startChar = workingText_;
	char* endChar = workingText_;
	bool lastPart = (*endChar == '\0');
	while (!lastPart)
	{
		// Find end position of next segment, which is either a newline or NULL (if it is the latter, note that it is the true end of the string)
		// We will set this end position to be NULL regardless, since a newline will be appended in the next part
		while ((*endChar != '\n') && (*endChar != '\0')) endChar++;
		if (*endChar == '\0') lastPart = true;
		*endChar = '\0';

		// If we were given some 'indentText', print copies of it before the message
		if (indentText != NULL)
		{
			if (redirect_ || (ProcessPool::nWorldProcesses() == 1)) prependedLine.sprintf("%s %s\n", indentText, startChar);
			else prependedLine.sprintf("[%i] %s %s\n", ProcessPool::worldRank(), indentText, startChar);
		}
		else
		{
			if (redirect_ || (ProcessPool::nWorldProcesses() == 1)) prependedLine.sprintf("%s\n", startChar);
			else prependedLine.sprintf("[%i] %s\n", ProcessPool::worldRank(), startChar);
		}
		strcat(text_, prependedLine.get());

		// Move our endChar position to the next character, and set the startChar to this position as well
		++endChar;
		startChar = endChar;

		// Make an additional check for this being the actual string terminator
		if (*endChar == '\0') break;
	}
}

// Master text creation / formatting routine
void Messenger::createAndPrintText(const char* indentText, const char* format, va_list arguments)
{
	if (masterOnly_ && (!ProcessPool::isWorldMaster())) return;

	createText(indentText, format, arguments);

	outputText(text_);
}

// Create and print text (simple)
void Messenger::createAndPrintText(const char* text)
{
	if (masterOnly_ && (!ProcessPool::isWorldMaster())) return;

	// Reset main text string storage
	text_[0] = '\0';

	if (redirect_ || (ProcessPool::nWorldProcesses() == 1)) sprintf(text_, "%s\n", text);
	else sprintf(text_, "[%i] %s\n", ProcessPool::worldRank(), text);

	outputText(text_);
}

/*
 * General Print Routines
 */

// Set status of quiet mode
void Messenger::setQuiet(bool b)
{
	quiet_ = b;
}

// Return status of quiet mode
bool Messenger::isQuiet()
{
	return quiet_;
}

// Temporarily mute output
void Messenger::mute()
{
	muted_ = true;
}

// Unmute output
void Messenger::unMute()
{
	muted_ = false;
}

// Set status of verbose mode
void Messenger::setVerbose(bool b)
{
	verbose_ = b;
}

// Return status of verbose mode
bool Messenger::isVerbose()
{
	return verbose_;
}

// Set status of master-only mode
void Messenger::setMasterOnly(bool b)
{
	masterOnly_ = b;
}

// Print standard message
void Messenger::print(const char* fmt, ...)
{
	if (quiet_ || muted_) return;

	va_list arguments;
	va_start(arguments,fmt);
	createAndPrintText(NULL, fmt, arguments);
	va_end(arguments);
}

// Print verbose message
void Messenger::printVerbose(const char* fmt, ...)
{
	if (quiet_ || muted_ || (!verbose_)) return;

	va_list arguments;
	va_start(arguments,fmt);
	createAndPrintText(NULL, fmt, arguments);
	va_end(arguments);
}

// Print error message
bool Messenger::error(const char* fmt, ...)
{
	va_list arguments;
	va_start(arguments,fmt);

	outputText("\n");
	createAndPrintText("***  ERROR");
	createAndPrintText("***  ERROR    ", fmt, arguments);
	createAndPrintText("***  ERROR");
	outputText("\n");

	va_end(arguments);

	return false;
}

// Print warning message
void Messenger::warn(const char* fmt, ...)
{
	va_list arguments;
	va_start(arguments, fmt);
	outputText("\n!!! WARNING\n");
	createAndPrintText("!!! WARNING   ", fmt, arguments);
	outputText("!!! WARNING\n\n");
	va_end(arguments);
}

// Print banner message of specified width
void Messenger::banner(const char* fmt, ...)
{
	static CharString bannerChars;
	const int width = 80;
	if (bannerChars.length() < width)
	{
		bannerChars.createEmpty(width+1);
		bannerChars.fill('=');
	}

	// First, create the text using vsprintf
	va_list arguments;
	va_start(arguments, fmt);
	vsprintf(workingText_, fmt, arguments);
	va_end(arguments);
	CharString bannerText = workingText_;

	// Now, get the length of the banner text and create a format for printing it into a line 80 chars wide
	int leftPad = (width - bannerText.length()) / 2 - 1;
	int rightPad = width - bannerText.length() - leftPad - 2;
	char bannerFormat[64];
	sprintf(bannerFormat, "%%s\n%%c%%%is%%s%%%is%%c\n%%s", leftPad, rightPad);

	// Finally, print the banner
	outputText("\n");
	print(bannerFormat, bannerChars.get(), '*', " ", bannerText.get(), " ", '*', bannerChars.get());
	outputText("\n");
}

// Print heading message
void Messenger::heading(const char* fmt, ...)
{
	static CharString bannerChars;
	const int width = 80;
	if (bannerChars.length() < width)
	{
		bannerChars.createEmpty(width+1);
		bannerChars.fill('-');
	}

	// First, create the text using vsprintf
	va_list arguments;
	va_start(arguments, fmt);
	vsprintf(workingText_, fmt, arguments);
	va_end(arguments);
	CharString bannerText = workingText_;

	// Now, get the length of the banner text and create a format for printing it into a line 80 chars wide
	int leftPad = (width - bannerText.length()) / 2 - 1;
	int rightPad = width - bannerText.length() - leftPad - 2;
	char bannerFormat[64];
	sprintf(bannerFormat, "%%%is%%s%%%is", leftPad, rightPad);

	outputText("\n");
	print(bannerFormat, " ", bannerText.get(), " ");
	print("%s\n", bannerChars.get());
	outputText("\n");
}

/*
 * Text Output Routine
 */

// Set output handler
void Messenger::setOutputHandler(OutputHandler* outputHandler)
{
	outputHandler_ = outputHandler;
}

// Print text
void Messenger::outputText(const char* text)
{
	// If we are redirecting to files, use the parser_
	if (redirect_) parser_->writeLineF("%s", text);
	else
	{
		// Not redirecting - has an OutputHandler been defined?
		if (outputHandler_) outputHandler_->outputText(text);
		else printf("%s", text);
	}
}

/*
 * File Redirection
 */

// Enable redirection of all messaging to specified file
bool Messenger::enableRedirect(const char* filename)
{
	parser_->openOutput(filename, true);
	if (!parser_->isFileGoodForWriting())
	{
		Messenger::print("Couldn't open output file '%s' for writing.\n", filename);
		return false;
	}
	
	redirect_ = true;
	return true;
}

// Cease redirection of messaging to file
void Messenger::ceaseRedirect()
{
	parser_->closeFiles();
	redirect_ = false;
}
