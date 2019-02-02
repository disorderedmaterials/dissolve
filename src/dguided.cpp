/*
	*** Dissolve Guide Editor (dguided)
	*** src/dguided.cpp
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
#include "gui/dguided.h"
#include <time.h>
#include <ctime>
#include <clocale>
#include <stdlib.h>
#include <QSurfaceFormat>

int main(int argc, char **argv)
{
	// Parse CLI options...
	int n = 1;
	CharString guideFile;
	while (n < argc)
	{
		if (argv[n][0] == '-')
		{
			// Command-line switch
			switch (argv[n][1])
			{
				case ('h'):
					printf("DGuidEd version %s\n\nAvailable CLI options are:\n\n", DISSOLVEVERSION);
					printf("\t-v\t\tVerbose mode - be a little more descriptive throughout\n");
					ProcessPool::finalise();
					return 0;
					break;
				case ('v'):
					Messenger::setVerbose(true);
					Messenger::printVerbose("Verbose mode enabled.\n");
					break;
				default:
					printf("Unrecognised command-line switch '%s'.\n", argv[n]);
					printf("Run with -h to see available switches.\n");
					return 1;
					break;
			}
		}
		else
		{
			// Input filename?
			if (guideFile.isEmpty()) guideFile = argv[n];
			else
			{
				printf("Error: More than one input file specified?\n");
				return 1;
			}
		}

		++n;
	}

	// Print GPL license information
	Messenger::print("DGuidEd version %s, Copyright (C) 2012-2019 T. Youngs.\n", DISSOLVEVERSION);
	Messenger::print("Source repository: %s.\n", DISSOLVEREPO);
	Messenger::print("DGuidEd comes with ABSOLUTELY NO WARRANTY.\n");
	Messenger::print("This is free software, and you are welcome to redistribute it under certain conditions.\n");
	Messenger::print("For more details read the GPL at <http://www.gnu.org/copyleft/gpl.html>.\n");

	/*
	 * Create and launch GUI
	 */

	// Create the main QApplication */
	QApplication app(argc, argv);
	QCoreApplication::setOrganizationName("ProjectAten");
	QCoreApplication::setOrganizationDomain("www.projectaten.com");
	QCoreApplication::setApplicationName("DGuidEd");

	// Set high DPI pixmaps
	app.setAttribute(Qt::AA_UseHighDpiPixmaps, true);

	// Ensure that the C locale is set, otherwise printf() and friends may not use dot for the radix point
	setlocale(LC_NUMERIC,"C");

	// Create the main window
	DGuidEdWindow dGuidEdWindow;

	// If an input file was specified, load it here
	if ((!guideFile.isEmpty()) && (!dGuidEdWindow.openFile(guideFile))) return 1;

	// Update and show the main window
	dGuidEdWindow.show();

	int result = app.exec();

	// Done.
	return result;
}

