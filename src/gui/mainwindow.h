/*
	*** Main Window
	*** src/gui/mainwindow.h
	Copyright T. Youngs 2007-2017

	This file is part of DUQ.

	DUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	DUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with DUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ATEN_MAINWINDOW_H
#define ATEN_MAINWINDOW_H

#include <QtWidgets/QButtonGroup>
#include <QSettings>
#include <QMenu>
#include "gui/ui_mainwindow.h"

// Forward Declarations
class DUQ;

class MonitorWindow : public QMainWindow
{
	// All Qt declarations must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	MonitorWindow(DUQ& duq);
	~MonitorWindow();
	// Main form declaration
	Ui::MonitorWindow ui;

	protected:
	void closeEvent(QCloseEvent* event);
	void resizeEvent(QResizeEvent* event);


	/*
	 * DUQ Reference
	 */
	private:
	// DUQ reference
	DUQ& duq_;

	public:
	// Return reference to DUQ
	DUQ& duq();


	/*
	 * Update Functions
	 */
	public:
	// Update Targets 
	enum UpdateTarget { DefaultTarget = 0, AllTargets = 65535 };

	private:
	// Whether window is currently refreshing
	bool refreshing_;
	// Whether window has been shown
	bool shown_;

	private:
	// Refresh main window
	void updateMainWindow();

	public slots:
	// Refreshes specified (or all) dock widgets
	void updateWidgets(int targets = MonitorWindow::DefaultTarget);
};

#endif
