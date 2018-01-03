/*
	*** Monitor Window
	*** src/gui/monitor.h
	Copyright T. Youngs 2007-2018

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

#ifndef DUQ_MONITORWINDOW_H
#define DUQ_MONITORWINDOW_H

#include "gui/ui_monitor.h"
#include "gui/subwindow.h"
#include "gui/outputhandler.hui"
#include "gui/thread.hui"
#include "base/charstring.h"
#include "templates/list.h"

// Forward Declarations
class DUQ;
class QMdiSubWindow;

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
	private:
	// Whether window is currently refreshing
	bool refreshing_;
	// Whether window has been shown
	bool shown_;
	// Output handler for messaging in GUI
	GUIOutputHandler outputHandler_;

	public slots:
	// Refresh all displayed widgets
	void updateWidgets();


	/*
	 * Run Control
	 */
	private:
	// Thread controller
	DUQThreadController threadController_;

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
	 * Sub-window Managemnet
	 */
	private:
	// List of current MDI sub-windows
	List<SubWindow> subWindows_;

	private:
	// Find SubWindow from specified data pointer
	SubWindow* subWindow(void* data);

	public:
	// Return window for specified data (as pointer), if it exists
	QMdiSubWindow* currentWindow(void* windowContents);
	// Add window for widget containing specified data (as pointer)
	QMdiSubWindow* addWindow(SubWidget* widget, void* windowContents, const char* windowTitle);

	public slots:
	// Remove window for specified data (as pointer), removing it from the list
	bool removeWindow(void* windowContents);


	/*
	 * Window State
	 */
	private:
	// Filename containing current window state
	CharString windowStateFilename_;

	public:
	// Save current window state
	bool saveWindowState();
	// Load window state
	bool loadWindowState();


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
