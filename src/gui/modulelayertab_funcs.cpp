/*
	*** ModuleLayerTab Functions
	*** src/gui/modulelayertab_funcs.cpp
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

#include "gui/modulelayertab.h"
#include "gui/modulechart.hui"
#include "gui/modulepalette.hui"
#include "gui/gui.h"
#include "main/dissolve.h"
#include "base/lineparser.h"

// Constructor / Destructor
ModuleLayerTab::ModuleLayerTab(DissolveWindow* dissolveWindow, Dissolve& dissolve, QTabWidget* parent, const char* title, ModuleLayer* layer) : ListItem<ModuleLayerTab>(), MainTab(dissolveWindow, dissolve, parent, CharString("Layer: %s", title), this)
{
	ui.setupUi(this);

	moduleLayer_ = layer;

	// Set up ModuleEditor
	ui.ModulePanel->setUp(dissolveWindow, moduleLayer_);

	refreshing_ = false;
}

ModuleLayerTab::~ModuleLayerTab()
{
}

/*
 * Data
 */

// Return tab type
const char* ModuleLayerTab::tabType() const
{
	return "ModuleLayerTab";
}

/*
 * ModuleLayer Target
 */

// Return displayed ModuleLayer
const ModuleLayer* ModuleLayerTab::moduleLayer() const
{
	return moduleLayer_;
}

/*
 * Widgets
 */

void ModuleLayerTab::on_EnabledButton_clicked(bool checked)
{
	if (refreshing_ || (!moduleLayer_)) return;

	moduleLayer_->setEnabled(checked);

	dissolveWindow_->setModified();
}

void ModuleLayerTab::on_FrequencySpin_valueChanged(int value)
{
	if (refreshing_ || (!moduleLayer_)) return;

	moduleLayer_->setFrequency(value);

	dissolveWindow_->setModified();
}

/*
 * Update
 */

// Update controls in tab
void ModuleLayerTab::updateControls()
{
	if (!moduleLayer_) return;

	refreshing_ = true;

	ui.EnabledButton->setChecked(moduleLayer_->enabled());
	ui.FrequencySpin->setValue(moduleLayer_->frequency());

	ui.ModulePanel->updateControls();

	refreshing_ = false;
}

// Disable sensitive controls within tab, ready for main code to run
void ModuleLayerTab::disableSensitiveControls()
{
	ui.EnabledButton->setEnabled(false);
	ui.FrequencySpin->setEnabled(false);
	ui.ModulePanel->disableSensitiveControls();
}

// Enable sensitive controls within tab, ready for main code to run
void ModuleLayerTab::enableSensitiveControls()
{
	ui.EnabledButton->setEnabled(true);
	ui.FrequencySpin->setEnabled(true);
	ui.ModulePanel->enableSensitiveControls();
}

/*
 * State
 */

// Read widget state through specified LineParser
bool ModuleLayerTab::readState(LineParser& parser, const CoreData& coreData)
{
	if (!ui.ModulePanel->readState(parser)) return false;

	return true;
}

// Write widget state through specified LineParser
bool ModuleLayerTab::writeState(LineParser& parser)
{
	if (!ui.ModulePanel->writeState(parser)) return false;

	return true;
}
