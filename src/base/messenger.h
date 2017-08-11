/*
	*** Messaging Routines
	*** src/base/messenger.h
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

#ifndef DUQ_MESSENGER_H
#define DUQ_MESSENGER_H

#include <stdarg.h>

// Forward Declarations
class LineParser;

// Global messaging handler
class Messenger
{
	public:
	// Constructor
	Messenger();


	/*
	 * Print Routines
	 */
	private:
	// Quiet mode
	static bool quiet_;
	// Verbose mode
	static bool verbose_;
	// Master-only mode
	static bool masterOnly_;

	private:
	// Storage for text to print
	static char text_[8096];
	// Working storage for text to print
	static char workingText_[8096];
	// Master text creation / formatting routine
	static void createText(const char* indentText, const char* format, va_list arguments);
	// Print text
	static void printText(const char* text);
	// Create and print text
	static void createAndPrintText(const char* indentText, const char* format, va_list arguments);

	public:
	// Set status of quiet mode
	static void setQuiet(bool b);
	// Set status of verbose mode
	static void setVerbose(bool b);
	// Set status of master-only mode
	static void setMasterOnly(bool b);
	// Print normal message
	static void print(const char* fmt, ...);
	// Print verbose message
	static void printVerbose(const char* fmt, ...);
	// Print error message
	static void error(const char* fmt, ...);
	// Print warn message
	static void warn(const char* fmt, ...);
	// Print banner message of specified width
	static void banner(const char* fmt, ...);


	/*
	 * File Redirection
	 */
	private:
	// Whether file redirection is enabled
	static bool redirect_;
	// Local LineParser for file redirection
	static LineParser* parser_;
	
	public:
	// Enable redirection of all messaging to specified file
	static bool enableRedirect(const char* filename);
};

#endif
