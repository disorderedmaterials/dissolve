/*
	*** dUQ CLI Main
	*** src/cli/main.cpp
	Copyright T. Youngs 2012-2016

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

#include "version.h"
#include "base/messenger.h"
#include "main/duq.h"
#include "base/comms.h"
#include <time.h>
#include <ctime>
#include <stdlib.h>

int main(int argc, char **argv)
{
	// Instantiate main class
	DUQ dUQ;

#ifdef PARALLEL
	// Initialise parallel communication
	Comm.initialise(&argc, &argv);
#endif

	// Parse CLI options...
	int n = 1;
	Dnchar inputFile, redirectFileName;
	bool interactive = false;
	while (n < argc)
	{
		if (argv[n][0] == '-')
		{
			// Command-line switch
			switch (argv[n][1])
			{
				case ('h'):
					printf("dUQ version %s\n\nAvailable CLI options are:\n\n", DUQVERSION);
// 					printf("\t-i\t\tStart in interactive mode\n");
					printf("\t-m\t\tRestrict output to be from the master process alone (parallel code only)\n");
					printf("\t-q\t\tQuiet mode - print no output\n");
					printf("\t-r <file>\tRedirect output from all process to 'file.N', where N is the process rank\n");
					printf("\t-v\t\tVerbose mode - be a little more descriptive throughout\n");
					Comm.finalise();
					return 1;
					break;
// 				case ('i'):
// 					interactive = true;
// 					msg.print("Will start in interactive mode.\n");
// 					break;
				case ('m'):
					msg.setMasterOnly(true);
					break;
				case ('q'):
					msg.setQuiet(true);
					break;
				case ('r'):
					// Next argument is filename
					++n;
					if (n == argc)
					{
						msg.error("Expected redirection filename.\n");
						return 1;
					}
					redirectFileName.sprintf("%s.%i", argv[n], Comm.rank());
					msg.enableRedirect(redirectFileName.get());
					break;
				case ('v'):
					msg.setVerbose(true);
					msg.printVerbose("Verbose mode enabled.\n");
					break;
				default:
					printf("Unrecognised command-line switch '%s'.\n", argv[n]);
					printf("Run with -h to see available switches.\n");
					Comm.finalise();
					return 1;
					break;
			}
		}
		else
		{
			// Input filename?
			if (inputFile.isEmpty()) inputFile = argv[n];
			else
			{
				printf("Error: More than one input file specified?\n");
				Comm.finalise();
				return 1;
			}
		}

		++n;
	}

	// Print GPL license information
#ifdef PARALLEL
	msg.print("dUQ PARALLEL version %s, Copyright (C) 2012-2014 T. Youngs.\n", DUQVERSION);
#else
	msg.print("dUQ SERIAL version %s, Copyright (C) 2012-2014  T. Youngs.\n", DUQVERSION);
#endif
	msg.print("Source repository: %s.\n", DUQREPO);
	msg.print("dUQ comes with ABSOLUTELY NO WARRANTY.\n");
	msg.print("This is free software, and you are welcome to redistribute it under certain conditions.\n");
	msg.print("For more details read the GPL at <http://www.gnu.org/copyleft/gpl.html>.\n\n");

	// Load external datafiles
	if (!MPIRunMaster(dUQ.loadDataFiles()))
	{
		Comm.finalise();
		return 1;
	}
	
	// Broadcast periodic table (including isotope and parameter data)
	if (!periodicTable.broadcast())
	{
		Comm.finalise();
		return 1;
	}

	// Load input file
	msg.print("Loading input file...\n");
	if (!MPIRunMaster(dUQ.loadInput(inputFile)))
	{
		Comm.finalise();
		return 1;
	}

// 	// Check system setup
// 	if (!MPIRunMaster(dUQ.checkSetup()))
// 	{
// 		msg.print("Errors were encountered while checking the system setup.\nYou must resolve these errors before any calculation can proceed.\n");
// 		Comm.finalise();
// 		return 1;
// 	}

	// Broadcast System Setup to slaves
	if (!dUQ.broadcastSetup())
	{
		Comm.finalise();
		return 1;
	}

	// Initialise random seed
	if (dUQ.seed() == -1) srand( (unsigned)time( NULL ) );
	else srand(dUQ.seed());
	
	// Perform system setup (all processes)
	if (!dUQ.setup())
	{
		msg.print("Failed to setup Configurations.\n");
		return 1;
	}

#ifdef PARALLEL
	msg.print("This is process %i of %i total, and exists in process group %i in which it is rank %i of %i processes.\n", Comm.rank(), Comm.nProcesses(), Comm.localGroupIndex(), Comm.localGroupRank(), Comm.localGroupSize());
#endif
	
	// Enter interactive mode, or perform steps provided in input file
	int result = dUQ.go();

	// End parallel communication
	Comm.finalise();
	
	// Done.
	return result;
}

