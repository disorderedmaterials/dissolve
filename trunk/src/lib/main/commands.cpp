/*
	*** dUQ - Commands
	*** src/lib/main/commands.cpp
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

#include "main/duq.h"
#include "base/comms.h"
#include <string.h>

/*!
 * \brief Register method functions
 * \details All simulation methods defined in dUQ must be registered as a Command in the CommandLibrary so that they may be used
 * within the Simulation block of input files, and through the interactive shell mode. Any variable arguments for the method must also be added
 * at this point.
 */
bool DUQ::registerCommands()
{
	// Standard Commands
	// -- EndLoop
	if (!registerCommand("endloop", "", &DUQ::commandEndLoop)) return false;
	// -- Energy
	if (!registerCommand("energy", "e", &DUQ::commandEnergy)) return false;
	// -- Help
	if (!registerCommand("help", "h", &DUQ::commandHelp)) return false;
	// -- LoadConfig
	if (!registerCommand("loadconfig", "l", &DUQ::commandLoadConfig)) return false;
	if (!addCommandArgument("loadconfig", "name", "Configuration file to load", "configuration.xyz")) return false;
	// -- Loop
	if (!registerCommand("loop", "", &DUQ::commandLoop)) return false;
	// -- Multiplier
	if (!registerCommand("multiplier", "mult", &DUQ::commandMultiplier)) return false;
	if (!addCommandArgument("multiplier", "n", "New system multiplier", "-1")) return false;
	// -- Quit
	if (!registerCommand("quit", "q", &DUQ::commandQuit)) return false;
	// -- RMSE
	if (!registerCommand("rmse", "rmse", &DUQ::commandRMSE)) return false;
	// -- SaveDLPOLY
	if (!registerCommand("savedlpoly", "", &DUQ::commandSaveDLPOLY)) return false;
	if (!addCommandArgument("savedlpoly", "name", "Filename to save under", "dlpoly.CONFIG")) return false;
	// -- SaveRDFs
	if (!registerCommand("saverdfs", "", &DUQ::commandSaveRDFs)) return false;
	if (!addCommandArgument("saverdfs", "source", "Which RDF information to save", "all")) return false;
	if (!addCommandArgument("saverdfs", "base", "Root name of RDFs to write out (will be appended by identifying name)", "current")) return false;
	// -- SavePP
	if (!registerCommand("savepp", "", &DUQ::commandSavePairPotentials)) return false;
	if (!addCommandArgument("savepp", "base", "Root name of PairPotentials to write out (will be appended by '-TYPE-TYPE.pp'", "pairpotential")) return false;
	// -- SaveSQ
	if (!registerCommand("savesq", "", &DUQ::commandSaveSQ)) return false;
	if (!addCommandArgument("savesq", "source", "Which RDF information to save", "all")) return false;
	if (!addCommandArgument("savesq", "base", "Root name of S(Q) to write out (will be appended by identifying name)", "current")) return false;
	// -- SaveXYZ
	if (!registerCommand("savexyz", "s", &DUQ::commandSaveXYZ)) return false;
	if (!addCommandArgument("savexyz", "name", "Filename to save under", "configuration.xyz")) return false;
	// -- Set
	if (!registerCommand("set", "", &DUQ::commandSet)) return false;
	if (!addCommandArgument("set", "command", "Command containing target parameter", "")) return false;
	if (!addCommandArgument("set", "parameter", "Target parameter", "")) return false;
	if (!addCommandArgument("set", "value", "Value of parameter", "")) return false;
	// -- Shake
	if (!registerCommand("shake", "sh", &DUQ::commandShake)) return false;
	if (!addCommandArgument("shake", "loop", "Type of loop to employ (if any)", "")) return false;
	// -- Temperature
	if (!registerCommand("temperature", "temp,t", &DUQ::commandTemperature)) return false;
	if (!addCommandArgument("temperature", "value", "New temperature to use in simulation methods", "")) return false;
	// -- Test
	if (!registerCommand("test", "", &DUQ::commandTest)) return false;

	// AtomShake
	if (!registerCommand("atomshake", "as", &DUQ::atomShake)) return false;
	if (!addCommandArgument("atomshake", "n", "Number of shakes to perform per Atom", "1")) return false;
	if (!addCommandArgument("atomshake", "cut", "Cutoff radius to use in energy calculation (-1.0 to use PairPotential cutoff)", "-1.0")) return false;
	if (!addCommandParameter("atomshake", "rate", "Target acceptance rate for moves", "0.333")) return false;
	if (!addCommandParameter("atomshake", "maxtrans", "Size of maximum allowed translation in move (updated automatically)", "0.1")) return false;

	// GrainShake
	if (!registerCommand("grainshake", "gs", &DUQ::grainShake)) return false;
	if (!addCommandArgument("grainshake", "n", "Number of shakes to perform per Grain", "1")) return false;
	if (!addCommandArgument("grainshake", "cut", "Cutoff radius to use in energy calculation (-1.0 to use PairPotential cutoff)", "-1.0")) return false;
	if (!addCommandParameter("grainshake", "rate", "Target acceptance rate for moves", "0.333")) return false;
	if (!addCommandParameter("grainshake", "maxrot", "Size of maximum allowed rotation in move (updated automatically)", "30.0")) return false;
	if (!addCommandParameter("grainshake", "maxtrans", "Size of maximum allowed translation in move (updated automatically)", "1.0")) return false;

	// Twist
	if (!registerCommand("twist", "tw", &DUQ::twist)) return false;
	if (!addCommandArgument("twist", "n", "Number of twists to attempt per Bond", "1")) return false;
	if (!addCommandArgument("twist", "cut", "Cutoff radius to use in energy calculation (-1.0 to use PairPotential cutoff)", "-1.0")) return false;

	// Perturb Potential
	if (!registerCommand("perturb", "ptb", &DUQ::perturb)) return false;

	// Pair Correlations
	if (!registerCommand("pair", "p", &DUQ::calculatePairCorrelations)) return false;

	// MD
	if (!registerCommand("md", "", &DUQ::md)) return false;
	if (!addCommandArgument("md", "n", "Number of MD steps to do", "100")) return false;
	if (!addCommandArgument("md", "cut", "Cutoff radius to use in force calculation (-1.0 to use PairPotential cutoff)", "-1.0")) return false;
	if (!addCommandArgument("md", "deltat", "Timestep to use (ps)", "0.0005")) return false;
	if (!addCommandParameter("md", "trajectory", "Name of xyz trajectory file to write", "")) return false;
	if (!addCommandParameter("md", "energy", "Whether to calculate total energy at each step", "0")) return false;
	if (!addCommandParameter("md", "freq", "Frequency to write step summary", "1")) return false;
	
	return true;
}

/*!
 * \brief Register a new Command in the library
 */
Command* DUQ::registerCommand(const char* name, const char* shortNames, CommandPointer ptr)
{
	Command* func = command(name);
	if (func != NULL)
	{
		msg.error("Tried to register a command '%s', but one with the same name already exists.\n", name);
		return false;
	}

	// Create a new command
	func = commands_.add();
	func->setName(name);
	func->setShortNames(shortNames);
	func->setPointer(ptr);

	if (strlen(shortNames) < 2) msg.print("Successfully registered '%s' command\n", name);
	else msg.print("Successfully registered '%s' command (also known as %s)\n", name, shortNames);

	return func;
}

/*!
 * \brief Add an argument to a Command
 */
Argument* DUQ::addCommandArgument(const char* commandName, const char* argumentName, const char* description, const char* defaultValue)
{
	Command* func = command(commandName);
	if (func == NULL)
	{
		msg.error("Tried to add an argument to the unknown Command '%s'.\n", commandName);
		return false;
	}

	return func->addArgument(argumentName, description, defaultValue);
}

/*!
 * \brief Add a parameter to a Command
 */
Argument* DUQ::addCommandParameter(const char* commandName, const char* parameterName, const char* description, const char* startingValue)
{
	Command* func = command(commandName);
	if (func == NULL)
	{
		msg.error("Tried to add an argument to the unknown Command '%s'.\n", commandName);
		return false;
	}

	return func->addParameter(parameterName, description, startingValue);
}

/*!
 * \brief Return first Command in list
 */
Command* DUQ::commands()
{
	return commands_.first();
}

/*!
 * \brief Find and return named Command
 */
Command* DUQ::command(const char* name)
{
	LineParser parser;

	for (Command* cmd = commands_.first(); cmd != NULL; cmd = cmd->next)
	{
		if (strcmp(name, cmd->name()) == 0) return cmd;
	
		// Perhaps a short name matches?
		parser.getArgsDelim(LineParser::Defaults, cmd->shortNames());
		for (int n=0; n<parser.nArgs(); ++n) if (strcmp(name, parser.argc(n)) == 0) return cmd;
	}

	return NULL;
}

/*!
 * \brief Return specified Command Argument
 */
Argument* DUQ::commandArgument(const char* commandName, const char* argumentName)
{
	Command* cmd = command(commandName);
	if (cmd == NULL)
	{
		msg.error("Couldn't find named Command '%s' in DUQ::commandArgument().\n", commandName);
		return NULL;
	}

	Argument* arg = cmd->argument(argumentName);
	if (arg == NULL)
	{
		msg.error("Couldn't find named argument '%s' in Command '%s' in DUQ::commandArgument().\n", argumentName, commandName);
		return NULL;
	}
	
	return arg;
}

/*!
 * \brief Return Command Argument specified as const char*
 */
const char* DUQ::commandArgumentAsConstChar(const char* commandName, const char* argumentName, bool& result)
{
	static const char* dummy = "NULL";
	Argument* arg = commandArgument(commandName, argumentName);
	if (arg == NULL)
	{
		msg.error("Couldn't find named Argument '%s' in Command '%s' in DUQ::argumentAsConstChar().\n", argumentName, commandName);
		result = false;
		return dummy;
	}
	result = true;
	return arg->asConstChar();
}

/*!
 * \brief Return Command Argument specified as integer
 */
int DUQ::commandArgumentAsInteger(const char* commandName, const char* argumentName, bool& result)
{
	Argument* arg = commandArgument(commandName, argumentName);
	if (arg == NULL)
	{
		msg.error("Couldn't find named Argument '%s' in Command '%s' in DUQ::argumentAsInteger().\n", argumentName, commandName);
		result = false;
		return 0;
	}
	result = true;
	return arg->asInteger();
}

/*!
 * \brief Return Command Argument specified as double
 */
double DUQ::commandArgumentAsDouble(const char* commandName, const char* argumentName, bool& result)
{
	Argument* arg = commandArgument(commandName, argumentName);
	if (arg == NULL)
	{
		msg.error("Couldn't find named Argument '%s' in Command '%s' in DUQ::argumentAsDouble().\n", argumentName, commandName);
		result = false;
		return 0.0;
	}
	result = true;
	return arg->asDouble();
}

/*!
 * \brief Return parameter for specified command
 */
Argument* DUQ::commandParameter(const char* commandName, const char* parameterName)
{
	Command* cmd = command(commandName);
	if (cmd == NULL)
	{
		msg.error("Couldn't find named Command '%s' in DUQ::commandParameter().\n", commandName);
		return NULL;
	}
	
	Argument* arg = cmd->parameter(parameterName);
	if (arg == NULL)
	{
		msg.error("Couldn't find named parameter '%s' in Command '%s' in DUQ::commandParameter().\n", parameterName, commandName);
		return NULL;
	}
	return arg;
}

/*!
 * \brief Return Command parameter specified as const char*
 */
const char* DUQ::commandParameterAsConstChar(const char* commandName, const char* parameterName, bool& result)
{
	static const char* dummy = "NULL";
	Argument* arg = commandParameter(commandName, parameterName);
	if (arg == NULL)
	{
		msg.error("Couldn't find named parameter '%s' in Command '%s' in DUQ::argumentAsConstChar().\n", parameterName, commandName);
		result = false;
		return dummy;
	}
	result = true;
	return arg->asConstChar();
}

/*!
 * \brief Return Command parameter specified as integer
 */
int DUQ::commandParameterAsInteger(const char* commandName, const char* parameterName, bool& result)
{
	Argument* arg = commandParameter(commandName, parameterName);
	if (arg == NULL)
	{
		msg.error("Couldn't find named parameter '%s' in Command '%s' in DUQ::argumentAsInteger().\n", parameterName, commandName);
		result = false;
		return 0;
	}
	result = true;
	return arg->asInteger();
}

/*!
 * \brief Return Command parameter specified as double
 */
double DUQ::commandParameterAsDouble(const char* commandName, const char* parameterName, bool& result)
{
	Argument* arg = commandParameter(commandName, parameterName);
	if (arg == NULL)
	{
		msg.error("Couldn't find named parameter '%s' in Command '%s' in DUQ::argumentAsDouble().\n", parameterName, commandName);
		result = false;
		return 0.0;
	}
	result = true;
	return arg->asDouble();
}
