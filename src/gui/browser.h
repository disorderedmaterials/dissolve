/*
	*** Browser Window
	*** src/gui/browser.h
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

#ifndef DUQ_BROWSERWINDOW_H
#define DUQ_BROWSERWINDOW_H

#include "gui/ui_browser.h"
#include "gui/subwidget.h"

// Forward Declarations
class DUQ;
class DUQWindow;

class BrowserWidget : public SubWidget
{
	// All Qt declarations must include this macro
	Q_OBJECT

	public:
	// Constructor
	BrowserWidget(QWidget* parent, DUQWindow& monitorWindow, DUQ& duq);
	// Main form declaration
	Ui::BrowserWidget ui;
	// Browser Data Types
	enum BrowserDataType { AtomTypeDataType=1000, ConfigurationDataType, ModuleDataType, PairPotentialType, SpeciesDataType, MasterTermsDataType };

	private:
	// Whether window is currently refreshing
	bool refreshing_;


	/*
	 * SubWidget Reimplementations / Definitions
	 */
	protected:
	void closeEvent(QCloseEvent* event);

	public:
	// Update controls within widget
	void updateControls();
	// Disable sensitive controls within widget, ready for main code to run
	void disableSensitiveControls();
	// Enable sensitive controls within widget, ready for main code to run
	void enableSensitiveControls();
	// Return string specifying widget type
	const char* widgetType();
	// Write widget state through specified LineParser
	bool writeState(LineParser& parser);
	// Read widget state through specified LineParser
	bool readState(LineParser& parser);


	/*
	 * Object References
	 */
	private:
	// DUQ reference
	DUQ& duq_;
	// DUQWindow reference
	DUQWindow& duqWindow_;


	/*
	 * Widgets / Functions
	 */
	signals:
	void windowClosed(void*);

	public slots:
	// Object tree double-clicked
	void on_BrowserTree_itemDoubleClicked(QTreeWidgetItem * item, int column);
};

#endif
