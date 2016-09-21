/*
	*** Qt DUQ Object
	*** src/guiduq_funcs.cpp
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

#include "gui/guiduq.uih"
#include "gui/mainwindow.h"
#include <QtCore/QTime>

// DUQ Singleton Declaration
DUQObject GuiDUQ;

/*
 * DUQObject
 */

// Constructor
DUQObject::DUQObject()
{
}

// Destructor
DUQObject::~DUQObject()
{
}

// Set main window pointer
void DUQObject::setMainWindow(MainWindow* mainWindow)
{
	mainWindow_ = mainWindow;
}

/*
 * GuiOutputHandler
 */

/*
 * \brief Constructor
 */
GuiOutputHandler::GuiOutputHandler()
{
	printToConsole_ = false;
}

/*
 * \brief Set whether to print to screen
 */
void GuiOutputHandler::setPrintToConsole(bool b)
{
	printToConsole_ = b;
}

// Master print routine
void GuiOutputHandler::printText(char* source)
{
	if (printToConsole_) printf("%s", source);
	else
	{
		QString txt = QTime::currentTime().toString();
		txt += " - ";
		txt += source;
		while (txt[txt.length()-1] == '\n') txt.remove(txt.length()-1,1);
		emit(displayText(txt));
	}
}
