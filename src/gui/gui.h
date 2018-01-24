/*
	*** Monitor Window
	*** src/gui/monitor.h
	Copyright T. Youngs 2012-2018

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

#ifndef DUQ_MAINWINDOW_H
#define DUQ_MAINWINDOW_H

#include "gui/ui_gui.h"
#include "gui/subwindow.h"
#include "gui/outputhandler.hui"
#include "gui/thread.hui"
#include "gui/maintab.h"
#include "base/charstring.h"
#include "templates/list.h"

// Forward Declarations
class BrowserWidget;
class DUQ;
class QMdiSubWindow;

class DUQWindow : public QMainWindow
{
	// All Qt declarations must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	DUQWindow(DUQ& duq);
	~DUQWindow();
	// Main form declaration
	Ui::DUQWindow ui;

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
	* File
	*/
	public:
	bool openFile(const char* inputFile, bool ignoreRestartFile, bool ignoreLayoutFile);


	/*
	 * Update Functions
	 */
	private:
	// Whether window is currently refreshing
	bool refreshing_;
	// Whether window has been shown
	bool shown_;
	// Output handler for messaging in GUI
	GUIOutputHandler outputHandler_;

	public slots:
	// Refresh all controls
	void updateControls();


	/*
	 * Run Control
	 */
	public:
	// dUQ State Enum
	enum DUQState {
		RunningState,		/* dUQ is currently running in the GUI */
		StoppedState,		/* dUQ is currently stopped in the GUI */
		MonitoringState,	/* dUQ is running in the background, and we are monitoring it */
		nDUQStates
	};

	private:
	// Thread controller
	DUQThreadController threadController_;
	// Current state of dUQ
	DUQState duqState_;

	public:
	// Return current state of dUQ
	DUQState duqState() const;

	public slots:
	// Set widgets ready for the main code to be run
	void setWidgetsForRun();
	// Set widgets after the main code has been run
	void setWidgetsAfterRun();
	// All iterations requested are complete
	void iterationsComplete();

	signals:
	void iterate(int);
	void stopIterating();


	/*
	 * Tab Management
	 */
	private:
	// List of all displayed tabs
	List<MainTab> tabs_;

	private:
	// Clear all tabs
	void clearAllTabs();
	// Add setup tab
	void addSetupTab();
	// Add on tabs for all current Configurations
	void addConfigurationTabs();
	// Add on an empty workspace tab
	MainTab* addWorkspaceTab(const char* name);
	// Find named tab
	MainTab* findTab(const char* name);


	/*
	 * Sub-window Management
	 */


	/*
	 * Window State
	 */
	private:
	// Filename containing current window layout
	CharString windowLayoutFilename_;

	public:
	// Save current window layout
	bool saveWindowLayout();
	// Load window state
	bool loadWindowLayout();


	/*
	 * Widget Slots
	 */
	private slots: 
	void on_ControlRunButton_clicked(bool checked);
	void on_ControlStepButton_clicked(bool checked);
	void on_ControlStepFiveButton_clicked(bool checked);
	void on_ControlPauseButton_clicked(bool checked);
	void on_ControlReloadButton_clicked(bool checked);
};

#endif
