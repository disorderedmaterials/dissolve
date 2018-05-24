/*
	*** dUQ GUI Main
	*** src/duq-gui.cpp
	Copyright T. Youngs 2012-2018

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
#include "module/registry.h"
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

	// Instantiate main class
	DUQ dUQ;

	// Parse CLI options...
	int n = 1;
	CharString inputFile;
	bool ignoreRestart = false, ignoreLayout = false;
	while (n < argc)
	{
		if (argv[n][0] == '-')
		{
			// Command-line switch
			switch (argv[n][1])
			{
				case ('h'):
					printf("dUQ version %s\n\nAvailable CLI options are:\n\n", DUQVERSION);
					printf("\t-a\t\tAuto-add dependent Modules if they are not present already\n");
					printf("\t-i\t\tIgnore restart file\n");
					printf("\t-I\t\tIgnore GUI state file\n");
					printf("\t-q\t\tQuiet mode - print no output\n");
					printf("\t-v\t\tVerbose mode - be a little more descriptive throughout\n");
					ProcessPool::finalise();
					return 0;
					break;
				case ('a'):
					dUQ.setAutoAddDependentModules(true);
					break;
				case ('i'):
					Messenger::print("Restart file (if it exists) will be ignored.\n");
					ignoreRestart = true;
					break;
				case ('I'):
					Messenger::print("GUI layout file (if it exists) will be ignored.\n");
					ignoreLayout = true;
					break;
				case ('q'):
					Messenger::setQuiet(true);
					break;
				case ('v'):
					Messenger::setVerbose(true);
					Messenger::printVerbose("Verbose mode enabled.\n");
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
#ifdef PARALLEL
	Messenger::print("Monitor PARALLEL version %s, Copyright (C) 2012-2018 T. Youngs.\n", DUQVERSION);
#else
	Messenger::print("Monitor SERIAL version %s, Copyright (C) 2012-2018 T. Youngs.\n", DUQVERSION);
#endif
	Messenger::print("Source repository: %s.\n", DUQREPO);
	Messenger::print("dUQ comes with ABSOLUTELY NO WARRANTY.\n");
	Messenger::print("This is free software, and you are welcome to redistribute it under certain conditions.\n");
	Messenger::print("For more details read the GPL at <http://www.gnu.org/copyleft/gpl.html>.\n");

	// Load external datafiles (master only)
	Messenger::banner("Load External Data");
	if (!MPIRunMaster(dUQ.worldPool(), dUQ.loadDataFiles()))
	{
		ProcessPool::finalise();
		return 1;
	}

	// Broadcast periodic table (including isotope and parameter data)
	if (!periodicTable.broadcast(dUQ.worldPool()))
	{
		ProcessPool::finalise();
		return 1;
	}

	// Register modules and print info
	Messenger::banner("Register Modules");
	ModuleRegistry moduleRegistry;
	if (!ModuleList::printMasterModuleInformation())
	{
		ProcessPool::finalise();
		return 1;
	}

	/*
	 * Create and launch GUI
	 */

	// Create the main QApplication */
	QApplication app(argc, argv);
	QCoreApplication::setOrganizationName("ProjectAten");
	QCoreApplication::setOrganizationDomain("www.projectaten.com");
	QCoreApplication::setApplicationName("DUQ GUI");

	// Tweak the default QSurfaceFormat
	QSurfaceFormat surfaceFormat;
	surfaceFormat.setSamples(4);
	QSurfaceFormat::setDefaultFormat(surfaceFormat);

	// Set native siblings attribute to prevent odd rendering artefacts on some systems
	app.setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);

	// Set high DPI pixmaps
	app.setAttribute(Qt::AA_UseHighDpiPixmaps, true);

	// Ensure that the C locale is set, otherwise printf() and friends may not use dot for the radix point
	setlocale(LC_NUMERIC,"C");

	// Create the main window
	DUQWindow duqWindow(dUQ);

	// If an input file was specified, load it here
	if ((!inputFile.isEmpty()) && (!duqWindow.openFile(inputFile, ignoreRestart, ignoreLayout)))
	{
		ProcessPool::finalise();
		return 1;
	}

	// Update and show the main window
	duqWindow.updateControls();
	duqWindow.addOutputHandler();
	duqWindow.show();

	int result = app.exec();

	// Clear all data
	dUQ.clear();

	// End parallel communication
	ProcessPool::finalise();

	// Done.
	return result;
}

