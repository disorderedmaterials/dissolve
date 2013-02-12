/*
	*** dUQ GUI Main
	*** src/gui/main.cpp
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

#include "version.h"
#include "base/messenger.h"
#include "gui/mainwindow.h"
#include <QtOpenGL/QGLFormat>

int main(int argc, char *argv[])
{
#ifdef PARALLEL
	// Initialise parallel communication
	Comm.initialise(&argc, &argv);
#endif
	int result = 1;
	if (Comm.master())
	{
		/* Create the main QApplication */
		QApplication app(argc, argv, QApplication::GuiClient);
		QCoreApplication::setOrganizationName("dUQ");
		QCoreApplication::setOrganizationDomain("www.duq.net");
		QCoreApplication::setApplicationName("dUQ");

		/* Tweak the default QGLFormat */
		QGLFormat::defaultFormat().setSampleBuffers(TRUE);

		/* Initialise our icon resource */
		Q_INIT_RESOURCE(icons);
		
		/* Create the main window */
		MainWindow mainWindow;
		
		/* Load in element and isotope data, and supplied input file (if there was one) */
		if (!mainWindow.initialise(argc == 2 ? argv[1] : NULL)) return -1;
		
		/* Start the beast... */
		
		/* Refresh GUI elements */
		mainWindow.refresh(MainWindow::AllGroups);

		/* Show and Go! */
		mainWindow.show();
		result = app.exec();
	}
	else
	{
		// Slaves create their own dUQ master structure here
		DUQ dUQ;
		
		// Load external datafiles
		if (!MPIRunMaster(dUQ.loadDataFiles()))
		{
			Comm.finalise();
			return 1;
		}
		
		// Broadcast periodic table (including isotope and parameter data)
		if (!dUQ.periodicTable().broadcast())
		{
			Comm.finalise();
			return 1;
		}

		// Register commands
		msg.print("Registering commands...\n");
		if (!dUQ.registerCommands())
		{
			Comm.finalise();
			return 1;
		}

		// Now, enter interactive mode and wait for commands...
		result = dUQ.goInteractive();
	}
	
	return result;
}

