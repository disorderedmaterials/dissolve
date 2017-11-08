/*
	*** Browser Window
	*** src/gui/browser.h
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

#ifndef DUQ_BROWSERWINDOW_H
#define DUQ_BROWSERWINDOW_H

#include "gui/ui_browser.h"

// Forward Declarations
class DUQ;
class MonitorWindow;

class BrowserWindow : public QWidget
{
	// All Qt declarations must include this macro
	Q_OBJECT

	public:
	// Constructor
	BrowserWindow(MonitorWindow& monitorWindow, DUQ& duq);
	// Main form declaration
	Ui::BrowserWindow ui;
	// Browser Data Types
	enum BrowserDataType { AtomTypeDataType=1000, ConfigurationDataType, ModuleDataType, SpeciesDataType };

	private:
	// Whether window is currently refreshing
	bool refreshing_;

	protected:
	// Window close event
	void closeEvent(QCloseEvent* event);

	public:
	// Update window contents
	void updateWindow();


	/*
	 * Object References
	 */
	private:
	// DUQ reference
	DUQ& duq_;
	// MonitorWindow reference
	MonitorWindow& monitorWindow_;


	/*
	 * Widgets / Functions
	 */
	private:

	public slots:
	// Object tree double-clicked
	void on_BrowserTree_itemDoubleClicked(QTreeWidgetItem * item, int column);
};

#endif
