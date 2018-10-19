/*
	*** Dissolve Main
	*** src/main.cpp
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

#include "version.h"
#include "base/messenger.h"
#include "main/dissolve.h"
#include "module/registry.h"
#include "base/processpool.h"
#include <time.h>
#include <ctime>
#include <stdlib.h>

int main(int argc, char **argv)
{
#ifdef PARALLEL
	// Initialise parallel communication
	ProcessPool::initialiseMPI(&argc, &argv);
#endif

	// Instantiate main class
	Dissolve dissolve;

	// Parse CLI options...
	int n = 1;
	CharString inputFile, redirectFileName, restartDataFile;
	int nIterations = 5;
	bool ignoreRestart = false, dontWriteRestart = false;
	while (n < argc)
	{
		if (argv[n][0] == '-')
		{
			// Command-line switch
			switch (argv[n][1])
			{
				case ('h'):
#ifdef PARALLEL
					printf("Dissolve PARALLEL version %s, Copyright (C) 2012-2018 T. Youngs.\n\n", DISSOLVEVERSION);
#else
					printf("Dissolve SERIAL version %s, Copyright (C) 2012-2018 T. Youngs.\n\n", DISSOLVEVERSION);
#endif
					printf("Recognised CLI options are:\n\n");
					printf("\t-c\t\tCheck input and set-up only - don't perform any main-loop iterations\n");
					printf("\t-i\t\tIgnore restart file\n");
					printf("\t-m\t\tRestrict output to be from the master process alone (parallel code only)\n");
					printf("\t-n <iterations>\tRun for the specified number of main loop iterations, then stop\n");
					printf("\t-q\t\tQuiet mode - print no output\n");
					printf("\t-r <file>\tRedirect output from all process to 'file.N', where N is the process rank\n");
					printf("\t-s\t\tPerform single main loop iteration and then quit\n");
					printf("\t-t <file>\tLoad restart data from specified file, rather than the associated restart file\n");
					printf("\t-v\t\tVerbose mode - be a little more descriptive throughout\n");
					printf("\t-x\t\tDon't write any restart information (but still read in the restart file if present)\n");
					ProcessPool::finalise();
					Messenger::ceaseRedirect();
					return 1;
					break;
				case ('c'):
					nIterations = 0;
					Messenger::print("System input and set-up will be checked, then Dissolve will exit.\n");
					break;
				case ('i'):
					Messenger::print("Restart file (if it exists) will be ignored.\n");
					ignoreRestart = true;
					break;
				case ('m'):
					Messenger::setMasterOnly(true);
					break;
				case ('n'):
					++n;
					if (n == argc)
					{
						Messenger::error("Expected number of iterations.\n");
						Messenger::ceaseRedirect();
						return 1;
					}
					nIterations = atoi(argv[n]);
					Messenger::print("%i main-loop iterations will be performed, then Dissolve will exit.\n", nIterations);
					break;
				case ('q'):
					Messenger::setQuiet(true);
					break;
				case ('r'):
					// Next argument is filename
					++n;
					if (n == argc)
					{
						Messenger::error("Expected redirection filename.\n");
						Messenger::ceaseRedirect();
						return 1;
					}
					redirectFileName.sprintf("%s.%i", argv[n], ProcessPool::worldRank());
					Messenger::enableRedirect(redirectFileName.get());
					break;
				case ('s'):
					Messenger::print("Single main-loop iteration will be performed, then Dissolve will exit.\n");
					nIterations = 1;
					break;
				case ('t'):
					// Next argument is filename
					++n;
					if (n == argc)
					{
						Messenger::error("Expected restart data filename.\n");
						Messenger::ceaseRedirect();
						return 1;
					}
					restartDataFile = argv[n];
					Messenger::print("Restart data will be loaded from '%s'.\n", restartDataFile.get());
					break;
				case ('v'):
					Messenger::setVerbose(true);
					Messenger::printVerbose("Verbose mode enabled.\n");
					break;
				case ('x'):
					dontWriteRestart = true;
					Messenger::print("No restart file will be written.\n");
					break;
				default:
					printf("Unrecognised command-line switch '%s'.\n", argv[n]);
					printf("Run with -h to see available switches.\n");
					Messenger::ceaseRedirect();
					ProcessPool::finalise();
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
				ProcessPool::finalise();
				Messenger::ceaseRedirect();
				return 1;
			}
		}

		++n;
	}

	// Print GPL license information
#ifdef PARALLEL
	Messenger::print("Dissolve PARALLEL version %s, Copyright (C) 2012-2018 T. Youngs.\n", DISSOLVEVERSION);
#else
	Messenger::print("Dissolve SERIAL version %s, Copyright (C) 2012-2018 T. Youngs.\n", DISSOLVEVERSION);
#endif
	Messenger::print("Source repository: %s.\n", DISSOLVEREPO);
	Messenger::print("Dissolve comes with ABSOLUTELY NO WARRANTY.\n");
	Messenger::print("This is free software, and you are welcome to redistribute it under certain conditions.\n");
	Messenger::print("For more details read the GPL at <http://www.gnu.org/copyleft/gpl.html>.\n");

	// Register modules and print info
	Messenger::banner("Register Modules");
	ModuleRegistry moduleRegistry;
	if (!ModuleList::printMasterModuleInformation())
	{
		ProcessPool::finalise();
		Messenger::ceaseRedirect();
		return 1;
	}

	// Load input file
	// If no input file was provided, exit here
	Messenger::banner("Parse Input File");
	if (inputFile.isEmpty())
	{
		Messenger::print("No input file provided. Nothing more to do.\n");
		ProcessPool::finalise();
		Messenger::ceaseRedirect();
		return 1;
	}
	if (!dissolve.loadInput(inputFile))
	{
		ProcessPool::finalise();
		Messenger::ceaseRedirect();
		return 1;
	}

	// Load restart file if it exists
	Messenger::banner("Parse Restart File");
	if (!ignoreRestart)
	{
		// We may have been provided the name of a restart file to read in...
		CharString restartFile;
		if (restartDataFile.isEmpty()) restartFile.sprintf("%s.restart", inputFile.get());
		else restartFile = restartDataFile;
		
		if (DissolveSys::fileExists(restartFile))
		{
			Messenger::print("Restart file '%s' exists and will be loaded.\n", restartFile.get());
			if (!dissolve.loadRestart(restartFile.get()))
			{
				Messenger::error("Restart file contained errors.\n");
				ProcessPool::finalise();
				Messenger::ceaseRedirect();
				return 1;
			}
		}
		else Messenger::print("Restart file '%s' does not exist.\n", restartFile.get());
	}
	else Messenger::print("Restart file (if it exists) will be ignored.\n");

	// Prepare for run
	if (!dissolve.setUp())
	{
		ProcessPool::finalise();
		Messenger::ceaseRedirect();
		return 1;
	}

#ifdef PARALLEL
	Messenger::print("This is process rank %i of %i processes total.\n", ProcessPool::worldRank(), ProcessPool::nWorldProcesses());
#endif

	// Set restart file frequency to 0 if 'dontWriteRestart' is set
	if (dontWriteRestart) dissolve.setRestartFileFrequency(0);

	// Run main simulation
	bool result = dissolve.iterate(nIterations);

	// Print timing information
	dissolve.printTiming();

	// Clear all data
	dissolve.clear();

	if (result) Messenger::print("Dissolve is done.\n");
	else Messenger::print("Dissolve is done, but with errors.\n");

	// End parallel communication
	ProcessPool::finalise();

	// Stop redirecting
	Messenger::ceaseRedirect();

	// Done.
	return (result ? 0 : 1);
}
