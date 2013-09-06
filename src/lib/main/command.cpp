/*
	*** Simulation Control
	*** src/lib/main/control.cpp
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

#include "main/command.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "base/comms.h"
#include <string.h>

/*
// Function Argument
*/

/*!
 * \brief Constructor
 * \details Constructor for Argument
 */
Argument::Argument()
{

}

/*!
 * \brief Set argument data
 */
void Argument::set(const char* name, const char* description, const char* defaultValue)
{
	name_ = name;
	description_ = description;
	defaultValue_ = defaultValue;
}

/*!
 * \brief Return name
 */
const char* Argument::name() const
{
	return name_.get();
}

/*!
 * \brief Return description
 */
const char* Argument::description() const
{
	return description_.get();
}

/*!
 * \brief Return stored value
 */
const char* Argument::defaultValue() const
{
	return defaultValue_.get();
}

/*!
 * \brief Set current value to default
 */
void Argument::setDefaultValue()
{
	value_ = defaultValue_;
}

/*!
 * \brief Set current value (from const char*)
 */
void Argument::setValue(const char* value)
{
	value_ = value;
}

/*!
 * \brief Set current value (from double)
 */
void Argument::setValue(double value)
{
	value_ = ftoa(value);
}

/*!
 * \brief Set current value (from int)
 */
void Argument::setValue(int value)
{
	value_ = itoa(value);
}

/*!
 * \brief Return current value as const char*
 */
const char* Argument::asConstChar()
{
	return value_.get();
}

/*!
 * \brief Return current value as integer
 */
int Argument::asInteger()
{
	return value_.asInteger();
}

/*!
 * \brief Return current value as double
 */
double Argument::asDouble()
{
	return value_.asDouble();
}

/*
// Command
*/

/*!
 * \brief Constructor
 * \details Constructor for Function
 */
Command::Command()
{
	name_ = "UnnamedFunction";
	ptr_ = NULL;
}

/*!
 * \brief Set name
 */
void Command::setName(const char* name)
{
	name_ = name;
}

/*!
 * \brief Return name
 */
const char* Command::name() const
{
	return name_.get();
}

/*!
 * \brief Set short names
 */
void Command::setShortNames(const char* shortNames)
{
	shortNames_ = shortNames;
}

/*!
 * \brief Return short names
 */
const char* Command::shortNames() const
{
	return shortNames_.get();
}

/*!
 * \brief Add Argument definition
 */
Argument* Command::addArgument(const char* argumentName, const char* description, const char* defaultValue)
{
	// Check for a duplicate argument name
	Argument* arg = argument(argumentName);
	if (arg != NULL)
	{
		msg.error("Can't add Argument '%s' to Command '%s' since one already exists with this name.\n", argumentName, name_.get());
		return NULL;
	}

	// Add a new argument
	arg = arguments_.add();
	arg->set(argumentName, description, defaultValue);
	return arg;
}

/*!
 * \brief Return Argument list
 */
Argument* Command::arguments()
{
	return arguments_.first();
}

/*!
 * \brief Return named Argument
 */
Argument* Command::argument(const char* name)
{
	Dnchar lcaseName = lowerCase(name);
	for (Argument* arg = arguments_.first(); arg != NULL; arg = arg->next) if (lcaseName == lowerCase(arg->name())) return arg;
	return NULL;
}

/*!
 * \brief Add parameter definition
 */
Argument* Command::addParameter(const char* parameterName, const char* description, const char* startingValue)
{
	// Check for a duplicate parameter name
	Argument* arg = argument(parameterName);
	if (arg != NULL)
	{
		msg.error("Can't add parameter '%s' to Command '%s' since one already exists with this name.\n", parameterName, name_.get());
		return NULL;
	}

	// Add a new parameter
	arg = parameters_.add();
	arg->set(parameterName, description, startingValue);
	arg->setValue(startingValue);
	return arg;
}

/*!
 * \brief Return parameter list
 */
Argument* Command::parameters()
{
	return parameters_.first();
}

/*!
 * \brief Return named parameter
 */
Argument* Command::parameter(const char* name)
{
	Dnchar lcaseName = lowerCase(name);
	for (Argument* arg = parameters_.first(); arg != NULL; arg = arg->next) if (lcaseName == lowerCase(arg->name())) return arg;
	return NULL;
}

/*!
 * \brief Set method pointer
 */
void Command::setPointer(CommandPointer ptr)
{
	ptr_ = ptr;
}

/*!
 * \brief Return method pointer
 */
CommandPointer Command::commandPointer() const
{
	return ptr_;
}

/*!
 * \brief Set default Argument values
 */
void Command::setDefaultArguments()
{
	for (Argument* arg = arguments_.first(); arg != NULL; arg = arg->next) arg->setDefaultValue();
}

/*!
 * \brief Print formatted Command info
 */
void Command::print()
{
	Dnchar s = name_;
	s += ':';
	if (arguments_.nItems() > 0) for (Argument* arg = arguments_.first(); arg != NULL; arg = arg->next) s.strcatf(" %s=%s", arg->name(), arg->asConstChar());
	
	msg.print("%s\n", s.get());
}

/*
// Simulation Step
*/

/*!
 * \brief Constructor
 * \details Constructor for Step.
 */
Step::Step()
{
	command_ = NULL;
	iterations_ = 1;
}

/*!
 * \brief Clear step data
 */
void Step::clear()
{
	iterations_ = 1;
	command_ = NULL;
	argumentValues_.clear();
}

/*!
 * \brief Set command to perform
 */
void Step::setCommand(Command* cmd)
{
	command_ = cmd;
}

/*!
 * \brief Return command to perform
 */
Command* Step::command()
{
	return command_;
}

/*!
 * \brief Set argument value
 */
bool Step::setArgumentValue(const char* argumentName, const char* value)
{
	// Find named argument
	if (command_ == NULL)
	{
		msg.error("NULL_POINTER - NULL Command pointer found in Step::setArgumentValue().\n");
		return false;
	}
	Argument* arg = command_->argument(argumentName);
	if (arg == NULL)
	{
		msg.error("Command '%s' has no argument named '%s'.\n", command_->name(), argumentName);
		return false;
	}

	// Is this argument already in our list?
	if (argumentValues_.contains(arg))
	{
		msg.error("Argument '%s' to Command '%s' has been set more than once.\n", argumentName, command_->name());
		return false;
	}
	
	// Add new entry
	argumentValues_.add(arg, value);
	
	return true;
}

/*!
 * \brief Return stored argument values
 */
RefListItem<Argument,Dnchar>* Step::argumentValues()
{
	return argumentValues_.first();
}

/*!
 * \brief Prepare for execution
 */
bool Step::prepare()
{
	// Check for valid Command_
	if (command_ == NULL)
	{
		msg.error("NULL_POINTER - NULL Function pointer found in Step::prepare().\n");
		return false;
	}

	// Reset to default argument values
	command_->setDefaultArguments();

	// Poke any provided arguments into the Command
	for (RefListItem<Argument,Dnchar>* refArg = argumentValues_.first(); refArg != NULL; refArg = refArg->next) refArg->item->setValue(refArg->data.get());
	
	return true;
}

/*!
 * \brief Return pointer to command method
 */
CommandPointer Step::commandPointer()
{
	// Check for valid command_
	if (command_ == NULL)
	{
		msg.error("NULL_POINTER - NULL Command pointer found in Step::commandPointer().\n");
		return false;
	}
	return command_->commandPointer();
}

/*!
 * \brief Set number of times to run Command
 */
void Step::setIterations(int n)
{
	iterations_ = n;
}

/*!
 * \brief Return number of times to run Command
 */
int Step::iterations()
{
	return iterations_;
}

/*
// Parallel Comms
*/

/*!
 * \brief Broadcast data from Master to all Slaves
 */
bool Step::broadcast(const List<Command>& commands)
{
#ifdef PARALLEL
	// Master broadcasts name of Command for Slaves to find
	Dnchar funcName;
	if (Comm.master()) funcName = command_->name();
	if (!Comm.broadcast(funcName)) return false;
	for (command_ = commands.first(); command_ != NULL; command_ = command_->next) if (funcName == command_->name()) break;

	if (command_ == NULL)
	{
		msg.error("Process couldn't find a Command named '%s'.\n", funcName.get());
		return false;
	}

	// Master broadcasts number of arguments to expect
	int nArgs = argumentValues_.nItems();
	if (!Comm.broadcast(&nArgs, 1)) return false;
	Dnchar argName, argValue;
	if (Comm.master())
	{
		for (RefListItem<Argument,Dnchar>* argRef = argumentValues_.first(); argRef != NULL; argRef = argRef->next)
		{
			// Master broadcasts argument name and value
			argName = argRef->item->name();
			argValue = argRef->data;
			if (!Comm.broadcast(argName)) return false;
			if (!Comm.broadcast(argValue)) return false;
		}
	}
	else
	{
		for (int n=0; n<nArgs; ++n)
		{
			if (!Comm.broadcast(argName)) return false;
			if (!Comm.broadcast(argValue)) return false;
			if (!setArgumentValue(argName.get(), argValue.get())) return false;
		}
	}
	
	// Broadcast number of iterations
	if (!Comm.broadcast(&iterations_, 1)) return false;
#endif
	return true;
}
