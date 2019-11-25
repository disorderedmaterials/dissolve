/*
	*** Main Tab
	*** src/gui/maintab.h
	Copyright T. Youngs 2012-2019

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

#ifndef DISSOLVE_MAINTAB_H
#define DISSOLVE_MAINTAB_H

#include "module/module.h"
#include "base/charstring.h"
#include "base/enumoptions.h"
#include "templates/list.h"
#include "templates/reflist.h"
#include <QString>

// Forward Declarations
class Dissolve;
class DissolveWindow;
class LineParser;
class QLayout;
class QMdiArea;
class QTabWidget;
class QWidget;

// Base Tab
class MainTab
{
	public:
	// Constructor / Destructor
	MainTab(DissolveWindow* dissolveWindow, Dissolve& dissolve, QTabWidget* parent, const char* title, QWidget* page);
	virtual ~MainTab();
	// Tab Types
	enum TabType { ConfigurationTabType, ForcefieldTabType, LayerTabType, ModuleTabType, SpeciesTabType, WorkspaceTabType };
	// Return enum options for TabType
	static EnumOptions<MainTab::TabType> tabTypes();


	/*
	 * Data
	 */
	protected:
	// Reference to Dissolve
	Dissolve& dissolve_;
	// Pointer to main window
	DissolveWindow* dissolveWindow_;
	// Tab widget in which this tab is contained
	QTabWidget* tabWidget_;
	// Page widget
	QWidget* page_;
	// Unique title (name) of tab
	CharString title_;

	protected:
	// Raise suitable dialog for entering / checking new tab name
	virtual QString getNewTitle(bool& ok);

	public:
	// Return tab type
	virtual TabType type() const = 0;
	// Return page widget
	QWidget* page() const;
	// Return whether the title of the tab can be changed
	virtual bool canChangeTitle() const;
	// Rename tab through suitable dialog / widget
	bool rename();
	// Return title of tab
	const char* title() const;


	/*
	 * Update
	 */
	protected:
	// Whether the tab is currently refreshing
	bool refreshing_;

	public:
	// Update controls in tab
	virtual void updateControls() = 0;
	// Disable sensitive controls within the tab
	virtual void disableSensitiveControls() = 0;
	// Enable sensitive controls within the tab
	virtual void enableSensitiveControls() = 0;


	/*
	 * State
	 */
	public:
	// Read widget state through specified LineParser
	virtual bool readState(LineParser& parser, const CoreData& coreData) = 0;
	// Write widget state through specified LineParser
	virtual bool writeState(LineParser& parser) = 0;
};

#endif
