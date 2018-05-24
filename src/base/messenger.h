/*
	*** Messaging Routines
	*** src/base/messenger.h
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

#ifndef DISSOLVE_MESSENGER_H
#define DISSOLVE_MESSENGER_H

#include <stdarg.h>
#include "base/outputhandler.h"

// Forward Declarations
class LineParser;

// Global messaging handler
class Messenger
{
	public:
	// Constructor
	Messenger();


	/*
	 * General Print Routines
	 */
	private:
	// Quiet mode
	static bool quiet_;
	// Mute mode (temporary quietness if not already)
	static bool muted_;
	// Verbose mode
	static bool verbose_;
	// Master-only mode
	static bool masterOnly_;
	// Storage for text to print
	static char text_[8096];
	// Working storage for text to print
	static char workingText_[8096];

	private:
	// Master text creation / formatting routine
	static void createText(const char* indentText, const char* format, va_list arguments);
	// Master text creation / formatting routine (indent text only, for banners etc.)
	static void createText(const char* indentText);
	// Create and print text
	static void createAndPrintText(const char* indentText, const char* format, va_list arguments);
	// Create and print text (simple)
	static void createAndPrintText(const char* text);

	public:
	// Set status of quiet mode
	static void setQuiet(bool b);
	// Temporarily mute output
	static void mute();
	// Unmute output
	static void unMute();
	// Set status of verbose mode
	static void setVerbose(bool b);
	// Set status of master-only mode
	static void setMasterOnly(bool b);
	// Print normal message
	static void print(const char* fmt, ...);
	// Print verbose message
	static void printVerbose(const char* fmt, ...);
	// Print error message
	static bool error(const char* fmt, ...);
	// Print warn message
	static void warn(const char* fmt, ...);
	// Print banner message
	static void banner(const char* fmt, ...);
	// Print heading message
	static void heading(const char* fmt, ...);


	/*
	 * Text Output Routine
	 */
	private:
	// Output handler (if any)
	static OutputHandler* outputHandler_;

	public:
	// Set output handler
	static void setOutputHandler(OutputHandler* outputHandler);
	// Print text
	static void outputText(const char* text);


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
	// Cease redirection of messaging to file
	static void ceaseRedirect();
};

#endif
