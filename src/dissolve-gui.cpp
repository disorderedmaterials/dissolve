/*
	*** Dissolve GUI Main
	*** src/dissolve-gui.cpp
	Copyright T. Youngs 2012-2019

	This file is part of dissolve.

	Dissolve is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Dissolve is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "version.h"
#include "base/messenger.h"
#include "main/dissolve.h"
#include "base/processpool.h"
#include "gui/gui.h"
#include <time.h>
#include <ctime>
#include <clocale>
#include <stdlib.h>
#include <QSurfaceFormat>

int main(int argc, char **argv)
{
#ifdef PARALLEL
	// Initialise parallel communication
	ProcessPool::initialiseMPI(&argc, &argv);
#endif

	// Instantiate main classes
	CoreData coreData;
	Dissolve dissolve(coreData);

	// Parse CLI options...
	int n = 1;
	CharString inputFile, restartFile;
	int nIterations = 0;
	bool ignoreRestart = false, ignoreLayout = false, dontWriteRestart = false;
	while (n < argc)
	{
		if (argv[n][0] == '-')
		{
			// Command-line switch
			switch (argv[n][1])
			{
				case ('h'):
					printf("Dissolve version %s\n\nAvailable CLI options are:\n\n", DISSOLVEVERSION);
					printf("\t-h\t\tPrint what you're reading now\n");
					printf("\t-i\t\tIgnore restart file\n");
					printf("\t-I\t\tIgnore GUI state file\n");
					printf("\t-q\t\tQuiet mode - print no output\n");
					printf("\t-v\t\tVerbose mode - be a little more descriptive throughout\n");
					printf("\t-x\t\tDon't write restart or heartbeat files (but still read in the restart file if present)\n");
					ProcessPool::finalise();
					return 0;
					break;
				case ('i'):
					Messenger::print("Restart file (if it exists) will be ignored.\n");
					ignoreRestart = true;
					break;
				case ('I'):
					Messenger::print("GUI layout file (if it exists) will be ignored.\n");
					ignoreLayout = true;
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
					Messenger::print("%i main-loop iterations will be performed, then the GUI will be launched.\n", nIterations);
					break;
				case ('q'):
					Messenger::setQuiet(true);
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
					restartFile = argv[n];
					Messenger::print("Restart data will be loaded from '%s'.\n", restartFile.get());
					break;
				case ('v'):
					Messenger::setVerbose(true);
					Messenger::printVerbose("Verbose mode enabled.\n");
					break;
				case ('x'):
					dontWriteRestart = true;
					dissolve.setWriteHeartBeat(false);
					Messenger::print("No restart or heartbeat files will be written.\n");
					break;
				default:
					printf("Unrecognised command-line switch '%s'.\n", argv[n]);
					printf("Run with -h to see available switches.\n");
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
				return 1;
			}
		}

		++n;
	}

	// Print GPL license information
	Messenger::print("Dissolve-GUI version %s, Copyright (C) 2012-2019 T. Youngs.\n", DISSOLVEVERSION);
	Messenger::print("Source repository: %s.\n", DISSOLVEREPO);
	Messenger::print("Dissolve comes with ABSOLUTELY NO WARRANTY.\n");
	Messenger::print("This is free software, and you are welcome to redistribute it under certain conditions.\n");
	Messenger::print("For more details read the GPL at <http://www.gnu.org/copyleft/gpl.html>.\n");

	// Register master Modules
	Messenger::banner("Available Modules");
	if (!dissolve.registerMasterModules())
	{
		ProcessPool::finalise();
		return 1;
	}

	// Set restart file frequency to 0 if 'dontWriteRestart' is set
	if (dontWriteRestart) dissolve.setRestartFileFrequency(0);
    

	/*
	 * Create and launch GUI
	 */

	// Create the main QApplication */
	QApplication app(argc, argv);
	QCoreApplication::setOrganizationName("ProjectAten");
	QCoreApplication::setOrganizationDomain("www.projectaten.com");
	QCoreApplication::setApplicationName("Dissolve-GUI");

	// Set native siblings attribute to prevent odd rendering artefacts on some systems
	app.setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);

	// Set high DPI pixmaps
	app.setAttribute(Qt::AA_UseHighDpiPixmaps, true);

	// Ensure that the C locale is set, otherwise printf() and friends may not use dot for the radix point
	setlocale(LC_NUMERIC,"C");

	// Create the main window
	DissolveWindow dissolveWindow(dissolve);

	// If an input file was specified, load it here
	if ((!inputFile.isEmpty()) && (!dissolveWindow.openFileFromCLI(inputFile, restartFile, ignoreRestart, ignoreLayout)))
	{
		ProcessPool::finalise();
		return 1;
	}

	// Iterate before launching the GUI?
	if (nIterations > 0)
	{
		// Prepare for run
		if (!dissolve.prepare()) return 1;

		// Run main simulation
		bool result = dissolve.iterate(nIterations);
		if (!result) return 1;
	}

	// Update and show the main window
	dissolveWindow.fullUpdate();
	dissolveWindow.addOutputHandler();
	dissolveWindow.show();

	int result = app.exec();

	// Clear all data
	dissolve.clear();

	// End parallel communication
	ProcessPool::finalise();

	// Done.
	return result;
}

