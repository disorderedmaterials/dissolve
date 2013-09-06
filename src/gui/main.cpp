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
#include "main/flags.h"
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
		QCoreApplication::setOrganizationDomain("www.projectaten.net");
		QCoreApplication::setApplicationName("dUQ");
		
		QFont font = QApplication::font();
		font.setPointSize(10.0);
		QApplication::setFont(font);

#if QT_VERSION >= 0x040600
		QGL::setPreferredPaintEngine(QPaintEngine::OpenGL);
#endif

		/* Tweak the default QGLFormat */
		QGLFormat::defaultFormat().setSampleBuffers(true);

		/* Initialise our icon resource */
		Q_INIT_RESOURCE(icons);

		/* Create main dUQ object */
		DUQObject dUQ;

		/* Load external datafiles */
		if (!MPIRunMaster(dUQ.loadDataFiles()))
		{
			Comm.finalise();
			return 1;
		}
		
		/* Broadcast periodic table (including isotope and parameter data) */
		if (!periodicTable.broadcast())
		{
			Comm.finalise();
			return 1;
		}

		/* Register commands */
		msg.print("Registering commands...\n");
		if (!dUQ.registerCommands())
		{
			Comm.finalise();
			return false;
		}

		/* Load existing input file (if specified) */
		Flags::clearAll();
		if (argc == 2)
		{
			if (!dUQ.loadInput(argv[1]))
			{
				Comm.decide(false);
				Comm.finalise();
				return 1;
			}
			else Comm.decide(true);
		}
		else Comm.decide(true);

		/* Create the main window */
		MainWindow mainWindow(dUQ);

		/* Refresh GUI elements */
		mainWindow.refresh(65535);
		Flags::clearAll();

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
		if (!periodicTable.broadcast())
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

		// Wait for a decision here (master loading input file)
		if (!Comm.decision())
		{
			Comm.finalise();
			return -1;
		}

		// Now, enter interactive mode and wait for commands...
		dUQ.enterMessageLoop();
	}
	
	return result;
}

