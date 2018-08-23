/*
	*** Workspace Tab
	*** src/gui/workspacetab.h
	Copyright T. Youngs 2012-2018

	This file is part of Dissolve.

	Dissolve is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Dissolve is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DISSOLVE_WORKSPACETAB_H
#define DISSOLVE_WORKSPACETAB_H

#include "gui/ui_workspacetab.h"
#include "gui/maintab.h"

// Forward Declarations
class TMdiArea;
class QMenu;

// Workspace Tab
class WorkspaceTab : public QWidget, public MainTab
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	WorkspaceTab(DissolveWindow* dissolveWindow, Dissolve& dissolve, QTabWidget* parent, const char* title);
	~WorkspaceTab();
	// Main form declaration
	Ui::WorkspaceTab ui;


	/*
	 * Data
	 */
	public:
	// Return tab type
	const char* tabType() const;
	// Return whether the title of the tab can be changed
	bool canChangeTitle();


	/*
	 * Update
	 */
	protected:
	// Update controls in tab
	void updateControls();
	// Disable sensitive controls within tab, ready for main code to run
	void disableSensitiveControls();
	// Enable sensitive controls within tab, ready for main code to run
	void enableSensitiveControls();


	/*
	 * MDI SubWindow Management
	 */
	private:
	// MDI area widget
	TMdiArea* mdiArea_;
	// List of all current MDI sub-windows
	List<SubWindow> subWindows_;

	private:
	// Find SubWindow from specified data pointer
	SubWindow* subWindow(void* data);

	private slots:
	// Remove subWindow with specified title
	void removeSubWindow(QString title);

	public:
	// Remove window for specified data (as pointer) from this widget's MDI area
	bool removeWindowFromMDIArea(void* windowContents);
	// Return window for specified data (as pointer) in this widget's MDI area, if it exists
	QMdiSubWindow* findMDIWindow(void* windowContents);
	// Find SubWindow by title
	SubWindow* findSubWindow(const char* title);
	// Find SubWindow by SubWidget
	SubWindow* findSubWindow(SubWidget* subWidget);
	// Find SubWindow by data content
	SubWindow* findSubWindow(void* windowData);
	// Add SubWindow for widget containing specified data (as pointer)
	SubWindow* addSubWindow(SubWidget* widget, void* windowContents);
	// Remove SubWindow specified
	void removeSubWindow(SubWindow* window);


	/*
	 * Context Menu
	 */
	private:
	// Create Module menu with specified QMenu as parent
	void createContextMenu(QMenu* parent);

	private slots:
	// Module selected on context menu
	void contextMenuModuleSelected(bool checked);
	// General widget selected on context menu
	void contextMenuWidgetSelected(bool checked);

	public:
	// Add ModuleControl widget to workspace
	SubWindow* addModuleControlWidget(Module* module);
	// Add named widget to workspace
	SubWindow* addNamedWidget(const char* widgetName, const char* title);

	public slots:
	// Custom context menu requested
	void showContextMenu(const QPoint& pos);


	/*
	 * State
	 */
	public:
	// Write widget state through specified LineParser
	bool writeState(LineParser& parser);
	// Read widget state through specified LineParser
	bool readState(LineParser& parser);
};

#endif
