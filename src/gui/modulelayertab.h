/*
	*** Module Layer Tab
	*** src/gui/moduleayertab.h
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

#ifndef DISSOLVE_MODULELAYERTAB_H
#define DISSOLVE_MODULELAYERTAB_H

#include "gui/ui_modulelayertab.h"
#include "gui/maintab.h"
#include "gui/widgets/subwidget.h"

// Forward Declarations
class ModuleChart;
class ModulePalette;

// Processing Tab
class ModuleLayerTab : public QWidget, public ListItem<ModuleLayerTab>, public MainTab
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	ModuleLayerTab(DissolveWindow* dissolveWindow, Dissolve& dissolve, QTabWidget* parent, const char* title, ModuleLayer* layer);
	~ModuleLayerTab();
	// Main form declaration
	Ui::ModuleLayerTab ui;


	/*
	 * Data
	 */
	public:
	// Return tab type
	const char* tabType() const;


	/*
	 * ModuleLayer Target
	 */
	private:
	// ModuleLayer data to display
	ModuleLayer* moduleLayer_;

	public:
	// Return displayed ModuleLayer
	const ModuleLayer* moduleLayer() const;


	/*
	 * Widgets
	 */
	public slots:
	void on_EnabledButton_clicked(bool checked);
	void on_FrequencySpin_valueChanged(int value);


	/*
	 * Update
	 */
	protected:
	// Update controls in tab
	void updateControls();
	// Disable sensitive controls within tab
	void disableSensitiveControls();
	// Enable sensitive controls within tab
	void enableSensitiveControls();


	/*
	 * State
	 */
	public:
	// Read widget state through specified LineParser
	bool readState(LineParser& parser, const CoreData& coreData);
	// Write widget state through specified LineParser
	bool writeState(LineParser& parser);
};

#endif
