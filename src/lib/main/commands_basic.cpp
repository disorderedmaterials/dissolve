/*
	*** dUQ - Basic Commands
	*** src/lib/main/commands_basic.cpp
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
#include "base/sysfunc.h"
#include "classes/atom.h"
#include "classes/box.h"
#include <string.h>

/*!
 * \brief 'EndLoop' Command
 * \details Mark the end of a loop in the simulation steps
 */
CommandReturnValue DUQ::commandEndLoop(Configuration& cfg)
{
	// Nothing to do here - DUQ::runSimulation() recognises this command and 
	// will jump to the previously-stored 'Loop' step.
	return CommandFail;
}

/*!
 * \brief 'Energy' Command
 * \details Calculate and display total energy
 */
CommandReturnValue DUQ::commandEnergy(Configuration& cfg)
{
	totalEnergy(cfg);
	return CommandSuccess;	
}

/*!
 * \brief 'Help' Command
 * \details Display help for all commands
 */
CommandReturnValue DUQ::commandHelp(Configuration& cfg)
{
	// Create a sorted alphabetic list of available function
	RefList<Command,int> sortedList;
	RefListItem<Command,int>* cmdRef;
	Command* command;
	LineParser parser;
	Dnchar lcase;
	int value;
	for (command = commands_.first(); command != NULL; command = command->next)
	{
		lcase = lowerCase(command->name());
		// Find first entry in sorted list which is 'greater than' the current command name
		for (cmdRef = sortedList.first(); cmdRef != NULL; cmdRef = cmdRef->next)
		{
			value = strcmp(lowerCase(cmdRef->item->name()), lcase.get());
			if (value > 0) break;
		}
		// Insert into list
		sortedList.addBefore(cmdRef, command);
	}

	// Print sorted list
	Dnchar s;
	for (cmdRef = sortedList.first(); cmdRef != NULL; cmdRef = cmdRef->next)
	{
		command = cmdRef->item;

		// Create name and alias line
		s.sprintf("   %-12s", command->name());
		parser.getArgsDelim(LineParser::Defaults, command->shortNames());
		if (parser.nArgs() > 0)
		{
			s.strcatf("  [");
			for (int n=0; n<parser.nArgs(); ++n) s.strcatf("%s%s", n == 0 ? "" : ", ", parser.argc(n));
			s.strcatf("]");
		}
		msg.print("%s\n", s.get());

		// Print argument information
		for (Argument* arg = command->arguments(); arg != NULL; arg = arg->next)
		{
			if (arg == command->arguments()) msg.print("\t\tArgs:  %-8s   %s (default = %s)\n", arg->name(), arg->description(), arg->defaultValue());
			else msg.print("\t\t       %-8s   %s (default = %s)\n", arg->name(), arg->description(), arg->defaultValue());
		}
		
		// Print description
		// TODO
		
		msg.print("\n");
	}

	return CommandSuccess;
}

/*!
 * \brief 'LoadConfig' Command
 * \details Load configuration from file
 */
CommandReturnValue DUQ::commandLoadConfig(Configuration& cfg)
{
	bool result;
	Dnchar fileName = commandArgumentAsConstChar("loadconfig", "name", result);
	if (!result) return CommandFail;
	
// 	if (!MPIRunMaster(cfg.loadInitialCoordinates(fileName))) return CommandFail;
// 	if (!cfg.broadcastCoordinates()) return CommandFail;
// 	updateGrains(cfg);
	return CommandFail;

	return CommandSuccess;
}

/*!
 * \brief 'Loop' Command
 * \details Mark the start of a loop in the simulation steps
 */
CommandReturnValue DUQ::commandLoop(Configuration& cfg)
{
	// Nothing to do here - DUQ::runSimulation() recognises this command and 
	// will remember the Step in which it occurs
	return CommandFail;
}

/*!
 * \brief 'Multiplier' Command
 * \details Adjust the system multiplier and regenerate setup
 */
CommandReturnValue DUQ::commandMultiplier(Configuration& cfg)
{
	bool result;
	int newMultiplier = commandArgumentAsInteger("multiplier", "n", result);
	if (!result) return CommandFail;
	
	// Just print current multiplier?
	if (newMultiplier < 0)
	{
		msg.print("Current system multiplier is %i (giving %i Atoms in the configuration).\n", multiplier_, configuration_.nAtoms());
		return CommandSuccess;
	}

	// Print a warning
	// Get string from user and parse it
	bool response = getBooleanResponse("WARNING - Changing the system multiplier will destroy the current configuration. Continue (Y/n)?", true, true);
	if (!response) return CommandSuccess;

	// Proceed...
	multiplier_ = newMultiplier;
	
	// TODO - Need to clear old config/setup first...
	return CommandFail;
}

/*!
 * \brief 'Quit' Command
 * \details Exit the program immediately.
 */
CommandReturnValue DUQ::commandQuit(Configuration& cfg)
{
	// TODO Check for changes?
	msg.print("Exiting.\n");
	return CommandSuccess;
}

/*!
 * \brief 'RMSE' Command
 * \details Calculate the current RMSE between Sample total F(Q) and their reference data.
 * Total RDFs are automatically calculated if necessary.
 */
CommandReturnValue DUQ::commandRMSE(Configuration& cfg)
{
	// Calculate partials
	if (cfg.changeCount() != partialsChangeCount_) calculatePairCorrelations(cfg);

	// Calculate RMSE over all samples
	double rmse = 0.0, sampleRMSE;
	for (Sample* sam = samples_.first(); sam != NULL; sam = sam->next)
	{
		if (!sam->hasReferenceData()) continue;
		
		sampleRMSE = sam->referenceRMSE(rmseDeltaQ_);
		msg.print("--> Sample '%s' has RMSE %12.5e barns/sr/atom.\n", sam->name(), sampleRMSE);
		rmse += sampleRMSE;
	}

	msg.print("--> Total RMSE over all Samples is %12.5e barns/sr/atom.\n", rmse);
	return CommandSuccess;
}

/*!
 * \brief 'SaveDLPOLY' Command
 * \details Save DLPOLY CONFIG of configuration
 */
CommandReturnValue DUQ::commandSaveDLPOLY(Configuration& cfg)
{
	bool result;
	Dnchar fileName = commandArgumentAsConstChar("savedlpoly", "name", result);
	if (!result) return CommandFail;
	saveConfigurationDLPOLY(cfg, fileName);
	return CommandSuccess;
}

/*!
 * \brief 'SaveRDFs' Command
 * \details Save RDF data
 */
CommandReturnValue DUQ::commandSaveRDFs(Configuration& cfg)
{
	bool result;
	Dnchar source = commandArgumentAsConstChar("saverdfs", "source", result);
	if (!result) return CommandFail;
	Dnchar baseName = commandArgumentAsConstChar("saverdfs", "base", result);
	if (!result) return CommandFail;

	if (source[0] == '?')
	{
// 		msg.print("Possible 
	}
	else if (source == "all")
	{
		// Unweighted RDFs
		saveRDFs(baseName);

		// Loop over samples
		for (Sample* sam = samples_.first(); sam != NULL; sam = sam->next) sam->saveRDFs(baseName);
	}
	else
	{
		msg.print("Not implemented yet...\n");
		return CommandFail;
		// Find sample name specified
// 		Dnchar lcase = lowerCase(source);
	}

	return CommandSuccess;
}

/*!
 * \brief 'SavePP' Command
 * \details Save current PairPotentials
 */
CommandReturnValue DUQ::commandSavePairPotentials(Configuration& cfg)
{
	bool result;
	Dnchar baseName = commandArgumentAsConstChar("savepp", "base", result);
	if (!result) return CommandFail;
	if (!MPIRunMaster(savePairPotentials(baseName))) return CommandFail;
	return CommandSuccess;
}

/*!
 * \brief 'SaveSQ' Command
 * \details Save last calculated partial S(Q)
 */
CommandReturnValue DUQ::commandSaveSQ(Configuration& cfg)
{
	bool result;
	Dnchar source = commandArgumentAsConstChar("savesq", "source", result);
	if (!result) return CommandFail;
	Dnchar baseName = commandArgumentAsConstChar("savesq", "base", result);
	if (!result) return CommandFail;

	if (source[0] == '?')
	{
// 		msg.print("Possible 
	}
	else if (source == "all")
	{
		// Unweighted S(Q)
		saveSQ(baseName);

		// Loop over Samples
		for (Sample* sam = samples_.first(); sam != NULL; sam = sam->next) sam->saveSQ(baseName);
	}
	else
	{
		msg.print("Not implemented yet...\n");
		return CommandFail;
		// Find sample name specified
// 		Dnchar lcase = lowerCase(source);
	}
	return CommandSuccess;
}

/*!
 * \brief 'SaveXYZ' Command
 * \details Save XYZ of configuration
 */
CommandReturnValue DUQ::commandSaveXYZ(Configuration& cfg)
{
	bool result;
	Dnchar fileName = commandArgumentAsConstChar("savexyz", "name", result);
	if (!result) return CommandFail;
	saveConfigurationXYZ(cfg, fileName);
	return CommandSuccess;
}

/*!
 * \brief 'Set' Command
 * \details Set a parameter value for a given command
 */
CommandReturnValue DUQ::commandSet(Configuration& cfg)
{
	bool result;
	Dnchar text;

	// Get Command
	text = commandArgumentAsConstChar("set", "command", result);
	if (!result) return CommandFail;
	if (text.isEmpty())
	{
		msg.error("No Command name given to 'set' command.\n");
		return CommandFail;
	}
	Command* cmd = command(text);
	if (cmd == NULL)
	{
		msg.error("No Command found with name/shortname '%s'.\n", text.get());
		return CommandFail;
	}

	// Get parameter
	text = commandArgumentAsConstChar("set", "parameter", result);
	if (!result) return CommandFail;
	if (text.isEmpty())
	{
		if (cmd->parameters())
		{
			msg.print("Valid parameters for Command '%s' are:.\n", cmd->name());
			for (Argument* arg = cmd->parameters(); arg != NULL; arg = arg->next) msg.print("\t\t       %-8s   %-10s (current value = %s)\n", arg->name(), arg->description(), arg->asConstChar());
		}
		else msg.print("No parameters exist for Command '%s'.\n", cmd->name());
		return CommandSuccess;
	}
	Argument* arg = cmd->parameter(text);
	if (cmd == NULL)
	{
		msg.error("No parameter names '%s' found for Command '%s'.\n", text.get(), cmd->name());
		return CommandFail;
	}
	
	// Get new value
	text = commandArgumentAsConstChar("set", "value", result);
	if (!result) return CommandFail;
	if (text.isEmpty())
	{
		msg.error("No value given to 'set' command.\n");
		return CommandFail;
	}
	arg->setValue(text);
	return CommandSuccess;
}

/*!
 * \brief Shake system, with optional loop
 */
CommandReturnValue DUQ::commandShake(Configuration& cfg)
{
	// Get loop type
	bool result, convergeCheck = false, rmseCheck = false;
	Dnchar text = commandArgumentAsConstChar("shake", "loop", result);
	if (!result) return CommandFail;
	if (!text.isEmpty())
	{
		if (text == "rmse") rmseCheck = true;
		else if (text == "energy") rmseCheck = false;
		else if (text == "<none>") convergeCheck = false;
		else
		{
			msg.error("Loop type '%s' not recognised.\n", text.get());
			return CommandFail;
		}
		convergeCheck = true;
	}
	else convergeCheck = false;
	
	// Loop variables (might not be used)
	static Array<double> rmse, energyDelta;
	int length = 5;

	bool repeat;
	do
	{
		// Reset repeat flag
		repeat = false;

		// Execute shakeSteps_ commands
		CommandReturnValue returnValue = executeSteps(shakeSteps_.first());

		// Check loop continuation
		if (convergeCheck)
		{
			// Look at quantity of interest
			if (rmseCheck)
			{
				commandRMSE(cfg);	// TEST
				rmse.add(totalRMSE_);
			}
			else energyDelta.add(energyData_.arrayY().last());
			
			Array<double>& data = (rmseCheck ? rmse : energyDelta);
			
			// Have we enough points to check for convergence?
			if (data.nItems() < length)
			{
				msg.print("--> (Shake) Not enough points for convergence check (have %i, but need %i) so repeating shake...\n", data.nItems(), length);
				repeat = true;
			}
			else
			{
				double delta = 0.0;
				for (int n=data.nItems()-length; n<data.nItems()-1; ++n) delta += data[n+1] - data[n];
				msg.print("--> (Shake) Gradient of collected data is %f.\n", delta);
				if (delta < 0)
				{
					msg.print("--> (Shake) Gradient is still negative, so continuing shake.\n");
					repeat = true;
				}
				else
				{
					msg.print("--> (Shake) Gradient is positive, so discontinuing shake loop.\n");
					repeat = false;
					
					// Reset the array here, ready for next time
					data.clear();
				}
			}
		}
		
	} while (repeat);

	return CommandSuccess;
}

/*!
 * \brief 'Temperature' Command
 */
CommandReturnValue DUQ::commandTemperature(Configuration& cfg)
{
	bool result;
	Dnchar text = commandArgumentAsConstChar("temperature", "value", result);
	if (!result) return CommandFail;
	if (text.isEmpty())
	{
		msg.print("Current simulation temperature is %0.2f K.\n", temperature_);
		return CommandSuccess;
	}
	temperature_ = text.asDouble();
	msg.print("Simulation temperature is now %0.2f K.\n", temperature_);
	return CommandSuccess;
}
