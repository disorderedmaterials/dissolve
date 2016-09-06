/*
	*** Command Definition
	*** src/lib/classes/command.h
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

#ifndef DUQ_COMMAND_H
#define DUQ_COMMAND_H

#include "classes/configuration.h"
#include "main/keywords.h"
#include "base/dnchar.h"
#include "templates/list.h"
#include "templates/pair.h"

// Forward Declarations
class DUQ;

/// Command Return Value
enum CommandReturnValue
{
	CommandFail,		/**> Command execution failed */
	CommandCommFail,	/**> Inter-process communication failed */
	CommandSuccess,		/**> Command execution succeeded */
	CommandCanceled,	/**> Command execution intterupted / canceled by user */
	CommandQuit		/**> 'Quit' called */
};

// Command pointer typedef
typedef CommandReturnValue (DUQ::*CommandPointer)(Configuration&);

/*!
 * \brief Command Argument
 * \details TODO
 */
class Argument : public ListItem<Argument>
{
	public:
	// Constructor
	Argument();


	/*!
	 * \name Argument Definition
	 */
	///@{
	private:
	// Name
	Dnchar name_;
	// Description
	Dnchar description_;
	// Default value
	Dnchar defaultValue_;
	// Current value
	Dnchar value_;

	public:
	// Set argument data
	void set(const char* name, const char* description, const char* defaultValue);
	// Return name
	const char* name() const;
	// Return description
	const char* description() const;
	// Return default value
	const char* defaultValue() const;
	// Set current value to default value
	void setDefaultValue();
	// Set current value (from const char*)
	void setValue(const char* value);
	// Set current value (from double)
	void setValue(double value);
	// Set current value (from int)
	void setValue(int value);
	// Return value as const char*
	const char* asConstChar();
	// Return value as integer
	int asInteger();
	// Return value as double
	double asDouble();
	///@}
};

/*!
 * \brief Command
 * \details TODO
 */
class Command : public ListItem<Command>
{
	public:
	// Constructor
	Command();


	/*!
	 * \name Definition
	 */
	///@{
	private:
	// Command name
	Dnchar name_;
	// Command short names
	Dnchar shortNames_;
	// Command arguments
	List<Argument> arguments_;
	// Command parameters
	List<Argument> parameters_;
	// Command address
	CommandPointer ptr_;
	
	public:
	// Set name
	void setName(const char* name);
	// Return name
	const char* name() const;
	// Set short names
	void setShortNames(const char* shortNames);
	// Return short names
	const char* shortNames() const;
	// Add Argument definition
	Argument* addArgument(const char* argumentName, const char* description, const char* defaultValue);
	// Return Argument list
	Argument* arguments();
	// Return named Argument
	Argument* argument(const char* name);
	// Add parameter definition
	Argument* addParameter(const char* parameterName, const char* description, const char* startingValue);
	// Return parameter list
	Argument* parameters();
	// Return named parameter
	Argument* parameter(const char* name);
	// Set method pointer
	void setPointer(CommandPointer ptr);
	// Return method pointer
	CommandPointer commandPointer() const;
	// Set default argument values
	void setDefaultArguments();
	// Print formatted Command info
	void print();
	///@}
};

/*!
 * \brief Command Step
 * \details The Step class (2, 3, 4...) contains a reference to either a Command defined in the CommandLibrary or a known local language
 * command, and a set of zero or more arguments to be given to that Command when it is executed.
 */
class Step : public ListItem<Step>
{
	public:
	// Constructor
	Step();


	/*!
	 * \name Step Information
	 */
	///@{
	private:
	// Command to be performed (if type_ == CommandStep)
	Command* command_;
	// Arguments to command
	RefList<Argument,Dnchar> argumentValues_;
	// Number of times to run Command
	int iterations_;

	public:
	// Clear step data
	void clear();
	// Set Command to perform
	void setCommand(Command* func);
	// Return function to perform
	Command* command();
	// Set argument value
	bool setArgumentValue(const char* argumentName, const char* value);
	// Return stored argument values
	RefListItem<Argument,Dnchar>* argumentValues();
	// Prepare for execution 
	bool prepare();
	// Return pointer to Command method
	CommandPointer commandPointer();
	// Set number of times to run Command
	void setIterations(int n);
	// Return number of times to run Command
	int iterations();
	///@}


	/*!
	 * \name Parallel Comms
	 */
	///@{
	public:
	// Broadcast data from Master to all Slaves
	bool broadcast(const List<Command>& commands);
	///@}
};

#endif
