/*
	*** dUQ - Simulation
	*** src/lib/main/simulation.cpp
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

#include "main/duq.h"
#include "base/comms.h"
#include "base/sysfunc.h"
#include "version.h"
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <csignal>

/*!
 * \brief Attempt to parse supplied line into a Step
 */
bool DUQ::parseStep(Step* step, const char* line)
{
	LineParser parser;
	parser.getArgsDelim(LineParser::Defaults, line);
	if (parser.nArgs() == 0) return false;

	// Check for NULL pointer
	if (step == NULL)
	{
		msg.error("NULL_POINTER - NULL Step pointer passed to DUQ::parseStep().\n");
		return false;
	}
	step->clear();
	
	// The first item might be a number, specifying a repeat value
	int argId = 0;
	if (parser.argi(argId) > 0)
	{
		step->setIterations(parser.argi(argId));
		++argId;
	}

	// Check here to make sure that we have another argument to check...
	if (argId == parser.nArgs()) return false;

	// Search for the requested command
	Command* cmd = command(parser.argc(argId));
	if (cmd == NULL)
	{
		msg.print("'%s' is not a valid command name - type 'help' or 'h' for a list of available commands.\n", parser.argc(argId));
		return false;
	}
	++argId;

	// Set Step information
	step->setCommand(cmd);
	int nAssignments = 0;
	Argument* args = cmd->arguments();
	for (int n=argId; n<parser.nArgs(); ++n)
	{
		// Check for an assignment
		bool assignment = index(parser.argc(n), '=') != NULL;
		if ((assignment && (nAssignments < 0)) || ( (!assignment) & (nAssignments > 0)))
		{
			msg.error("Can't mix argument assignments (arg=value) with straight value lists.\n");
			return false;
		}
		
		if (assignment)
		{
			if (!step->setArgumentValue(beforeChar(parser.argc(n),'='), afterChar(parser.argc(n), '=')))
			{
				msg.error("Couldn't set argument in command '%s'.\n", cmd->name());
				return false;
			}
		}
		else
		{
			if (args == NULL)
			{
				msg.error("Too many arguments given to command '%s'.\n", cmd->name());
				return false;
			}
			if (!step->setArgumentValue(args->name(), parser.argc(n)))
			{
				msg.error("Couldn't set argument '%s' in command '%s'.\n", args->name(), cmd->name());
				return false;
			}
		}
		
		nAssignments += (assignment ? 1 : -1);
		args = args->next;
	}
	
	return true;
}

void my_handler(int s){
           printf("XXX Caught signal %d\n",s);
//            exit(1); 

}

/*!
 * \brief Execute Step list
 * \details Execute the Step list provided until a Step fails, we abort, or we reach the end of the list.
 */
CommandReturnValue DUQ::executeSteps(Step* firstStep)
{
	// Loop over sup
	bool result, error = false;
	Step* interleavedLoopStart = NULL;
	Step* step = firstStep;
	while (step != NULL)
	{
		// Quit?
		if (step->commandPointer() == &DUQ::commandQuit) return CommandQuit;

		// Start of Loop marker?
		if (step->commandPointer() == &DUQ::commandLoop)
		{
			printf("LOOPNITERS = %i\n", step->iterations());
			interleavedLoopStart = step;
			
			step = step->next;
			continue;
		}

		// Loop End marker?
		if (step->commandPointer() == &DUQ::commandEndLoop)
		{
			if (interleavedLoopStart == NULL)
			{
				msg.error("EndLoop encountered, but no Loop start was set.\n");
				return CommandFail;
			}
			step = interleavedLoopStart;
			continue;
		}
		
		if (step->prepare())
		{
			// Function arguments are now prepped, so print data and execute command...
			step->command()->print();
			for (int n=0; n<step->iterations(); ++n) 
			{
				if (step->iterations() > 1) msg.print("Iteration %i of %i\n", n+1, step->iterations());
				result = ((this)->*(step->commandPointer()))(configuration_);
				if (!result) return CommandFail;
			}
		}
		else return CommandFail;

		// Next step...
		step = step->next;
	}

	return CommandSuccess;
}

/*!
 * \brief Register a change in the total energy of the system
 */
void DUQ::registerEnergyChange(double deltaE)
{
	energyChange_ += deltaE;
	energyChanged_ = true;
}

/*!
 * \brief Accumulate current energy change into energyData_
 */
void DUQ::accumulateEnergyChange()
{
	// If energy hasn't changed, don't do anything
	if (!energyChanged_) return;

	// Get index of new point to add
	double x = energyData_.nPoints()+1.0;

	// Determine y value of new point...
	double y = energyData_.nPoints() == 0 ? energyChange_ : energyData_.y(energyData_.nPoints()-1) + energyChange_;

	// Add point and signal change
	energyData_.addPoint(x, y);
	energyChange_ = 0.0;
	energyChanged_ = false;
}

/*!
 * \brief Set absolute energy of system, after total energy calculation
 */
void DUQ::setAbsoluteEnergy(double energy)
{
	// Shift existing energy data so that the last point equals the supplied energy
	if (energyData_.nPoints() == 0) return;
	
	double y = energyData_.arrayY().last();
	if (energyChanged_)
	{
		y += energyChange_;
		energyChange_ = 0.0;
	}
	
	double delta = energy - y;
	energyData_.arrayY() += delta;
}

/*!
 * \brief Run Simulation
 * \details Execute the simulation steps provided in the input file
 */
bool DUQ::runSimulation()
{
// 	msg.print("Running Simulation (use Ctrl-C to break back to interactive mode)...\n");
	msg.print("Running Simulation...\n");

	// Set-up signal handler
// 	struct sigaction sigIntHandler;
// 	sigIntHandler.sa_handler = my_handler;
// 	sigemptyset(&sigIntHandler.sa_mask);
// 	sigIntHandler.sa_flags = SA_RESTART;
// 	sigaction(SIGINT, &sigIntHandler, NULL);
	
	CommandReturnValue returnValue = CommandFail;

	// Execute equilibration stage
	if (equilibrationSteps_.nItems() > 0)
	{
		msg.print("\nExecuting equilibration stage...\n\n");
		returnValue = executeSteps(equilibrationSteps_.first());
		if (returnValue == CommandFail) return false;
		if (returnValue == CommandQuit) return true;
	}

	// Execute main strategy
	if (strategySteps_.nItems() > 0)
	{
		msg.print("\nBeginning main strategy...\n\n");
		returnValue = executeSteps(strategySteps_.first());
	}
	
	if (returnValue == CommandFail)
	{
		msg.print("Main strategy exited with errors.\n");
		return false;
	}

	if (returnValue == CommandQuit) return true;
	
	msg.print("Finished.\n");

	return true;
}

/*!
 * \brief Enter interactive mode
 */
bool DUQ::goInteractive()
{
	// Create prompt text
	Dnchar prompt(-1, "dUQ r%s > ", DUQREVISION);
	Step step;
	Dnchar line;
	char* linePtr;

	// Initialise the History library
	if (Comm.master()) using_history();

	msg.print("Entering interactive mode...\n\n");
	while (1)
	{
		// If we are the master, get and parse the line
		if (Comm.master())
		{
			// Clear any existing Step information
			step.clear();

			// Get string from user and parse it
			linePtr = readline(prompt);
			add_history(linePtr);
			line.set(linePtr);
			free(linePtr);

			// Create Step information
			if (!parseStep(&step, line)) continue;

			// Valid function registered?
			if (step.commandPointer() == NULL) continue;

			// Quit?
			if (step.commandPointer() == &DUQ::commandQuit)
			{
				Comm.decide(false);
				break;
			}

			// Inform slaves that we have something to do and broadcast it
			Comm.decide(true);
			if (!step.broadcast(commands_)) return false;
		}
		else
		{
			// Continue or not?
			if (!Comm.decision()) break;

			// Clear any existing Step information
			step.clear();

			// Slaves get ready to receive Step information
			if (!step.broadcast(commands_)) return false;
		}

		// Execute Step
		if (step.prepare())
		{
			// Function arguments are now prepped, so print data and execute command...
			step.command()->print();
			for (int n=0; n<step.iterations(); ++n)
			{
				if (step.iterations() > 1) msg.print("Iteration %i of %i\n", n+1, step.iterations());
				bool result = ((this)->*(step.commandPointer()))(configuration_);
				if (!result)
				{
					msg.print("Failed.\n");
					break;
				}
			}
		}
		
		// Align MPI processes
		Comm.wait(DUQComm::World);
	}

	return true;
}

bool DUQ::runCommand(const char* commandString)
{
	Step step;
	
	// Create Step information
	if (!parseStep(&step, commandString)) return false;

	// Valid function registered?
	if (step.commandPointer() == NULL) return false;

	// Quit?
	if (step.commandPointer() == &DUQ::commandQuit)
	{
		Comm.decide(false);
		return false;
	}

	// Inform slaves that we have something to do and broadcast it
	Comm.decide(true);
	if (!step.broadcast(commands_)) return false;

	// Execute Step
	if (step.prepare())
	{
		// Function arguments are now prepped, so print data and execute command...
		step.command()->print();
		for (int n=0; n<step.iterations(); ++n)
		{
			if (step.iterations() > 1) msg.print("Iteration %i of %i\n", n+1, step.iterations());
			bool result = ((this)->*(step.commandPointer()))(configuration_);
			if (!result)
			{
				msg.print("Failed.\n");
				break;
			}
		}
	}
	
	// Align MPI processes
	Comm.wait(DUQComm::World);
	
	return true;
}
