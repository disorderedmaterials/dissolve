/*
	*** LoadDataDialog Functions
	*** src/loaddatadialog_funcs.cpp
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
#include <QtCore/QTime>

/*
 * GuiDUQ
 */

// Constructor
GuiDUQ::GuiDUQ()
{
}

// Destructor
GuiDUQ::~GuiDUQ()
{
}

/*
 * Qt Slots
 */

/*!
 * \brief StartSimulation (slot)
 */
void GuiDUQ::startSimulation()
{
	// Begin the main step loop
	int result = runSimulation();

	// Finished (for whatever reason) - emit 'Stopped' signal
	emit(simulationFinished(0));
}

/*!
 * \name ReceiveDUQSignal (slot)
 */
void GuiDUQ::receiveDUQSignal(int signal, int data)
{
	receiveSignal((DUQ::Signal) signal, data);
}

/*!
 * \brief Send signal (virtual)
 */
void GuiDUQ::sendSignal(DUQ::Signal signal, int data)
{
	emit sendDUQSignal(signal, data);
}

/*
 * GuiOutputHandler
 */

/*!
 * \brief Constructor
 */
GuiOutputHandler::GuiOutputHandler()
{
	printToConsole_ = TRUE;
}

/*!
 * \brief Set whether to print to screen
 */
void GuiOutputHandler::setPrintToConsole(bool b)
{
	printToConsole_ = b;
}

/*!
 * \brief Master print routine
 * \details Basic text output printing, to either console or GUI
 */
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
