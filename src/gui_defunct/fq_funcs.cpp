/*
	*** F(Q) Dialog
	*** src/gui/fq_funcs.cpp
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

#include "gui/fq.h"
#include "gui/guiduq.uih"
#include "base/sysfunc.h"

// Constructor
DUQFQDockWidget::DUQFQDockWidget(QWidget* parent, DUQObject& duq) : QDockWidget("Arse", parent), dUQ_(duq)
{
	// Call the main creation function
	ui.setupUi(this);

	// Set initial variables
	refreshing_ = false;
}

// Destructor
DUQFQDockWidget::~DUQFQDockWidget()
{
}

// Window close event
void DUQFQDockWidget::closeEvent(QCloseEvent* event)
{
}

// General refresh function
void DUQFQDockWidget::refresh(int targets)
{
	refreshing_ = true;
	ui.FQGraph->refreshData();
	refreshing_ = false;
}

/*
 * Slots
 */

void DUQFQDockWidget::on_RecalculateButton_clicked(bool checked)
{
	if (!dUQ_.setupSimulation()) return;

	dUQ_.runCommand("pair");
	
	printf("Done.\n");
	
	refresh(65535);
}
