/*
	*** Messaging Routines
	*** src/lib/base/messenger.h
	Copyright T. Youngs 2012-2014

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

#include "base/lineparser.h"

/*!
 * \brief Messaging Output Handler
 */
class OutputHandler
{
	public:
	// Master print routine
	virtual void printText(char* text);
};
 
/*!
 * \brief Global messaging handler
 */
class Messenger
{
	public:
	// Constructor
	Messenger();


	/*
	// Messaging
	*/
	private:
	// Quiet mode
	bool quiet_;
	// Verbose mode
	bool verbose_;
	// Master-only mode
	bool masterOnly_;
	// Basic OutputHandler
	OutputHandler basicOutputHandler_;
	// Target Output handler
	OutputHandler* targetOutputHandler_;

	private:
	// Storage for text to print
	char text_[8096];
	// Master text creation / formatting routine
	void createAndPrintText(const char* indentText, const char* format, va_list arguments);

	public:
	// Set status of quiet mode
	void setQuiet(bool b);
	// Set status of verbose mode
	void setVerbose(bool b);
	// Set status of master-only mode
	void setMasterOnly(bool b);
	// Set OutputHandler
	void setTargetOutputHandler(OutputHandler* handler);
	// Print normal message
	void print(const char*, ...);
	// Print verbose message
	void printVerbose(const char*, ...);
	// Print error message
	void error(const char*, ...);
	// Print warn message
	void warn(const char*, ...);


	/*
	// File Redirection
	*/
	private:
	// Whether file redirection is enabled
	bool redirect_;
	// Local LineParser for file redirection
	LineParser parser_;
	
	public:
	// Enable redirection of all messaging to specified file
	bool enableRedirect(const char* fileName);
};

extern Messenger msg;

#endif
