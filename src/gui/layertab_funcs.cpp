/*
	*** Layer Tab Functions
	*** src/gui/layertab_funcs.cpp
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

#include "gui/layertab.h"
#include "gui/modulechart.hui"
#include "gui/gui.h"
#include "gui/getmodulelayernamedialog.h"
#include "main/dissolve.h"
#include "base/lineparser.h"

// Constructor / Destructor
LayerTab::LayerTab(DissolveWindow* dissolveWindow, Dissolve& dissolve, QTabWidget* parent, const char* title, ModuleLayer* layer) : ListItem<LayerTab>(), MainTab(dissolveWindow, dissolve, parent, CharString("Layer: %s", title), this)
{
	ui_.setupUi(this);

	moduleLayer_ = layer;

	// Set up ModuleEditor
	ui_.ModulePanel->setUp(dissolveWindow, moduleLayer_);

	refreshing_ = false;
}

LayerTab::~LayerTab()
{
}

/*
 * MainTab Reimplementations
 */

// Return tab type
MainTab::TabType LayerTab::type() const
{
	return MainTab::LayerTabType;
}

// Raise suitable dialog for entering / checking new tab name
QString LayerTab::getNewTitle(bool& ok)
{
	// Get a new, valid name for the layer
	GetModuleLayerNameDialog nameDialog(this, dissolve_.processingLayers());
	ok = nameDialog.get(moduleLayer_, moduleLayer_->name());

	if (ok)
	{
		// Rename our layer, and flag that our data has been modified
		moduleLayer_->setName(qPrintable(nameDialog.newName()));

		dissolveWindow_->setModified();
	}

	return nameDialog.newName();
}

// Return whether the title of the tab can be changed
bool LayerTab::canChangeTitle() const
{
	return true;
}

/*
 * ModuleLayer Target
 */

// Return displayed ModuleLayer
const ModuleLayer* LayerTab::moduleLayer() const
{
	return moduleLayer_;
}

/*
 * Widgets
 */

void LayerTab::on_EnabledButton_clicked(bool checked)
{
	if (refreshing_ || (!moduleLayer_)) return;

	moduleLayer_->setEnabled(checked);

	dissolveWindow_->setModified();
}

void LayerTab::on_FrequencySpin_valueChanged(int value)
{
	if (refreshing_ || (!moduleLayer_)) return;

	moduleLayer_->setFrequency(value);

	dissolveWindow_->setModified();
}

/*
 * Update
 */

// Update controls in tab
void LayerTab::updateControls()
{
	if (!moduleLayer_) return;

	refreshing_ = true;

	ui_.EnabledButton->setChecked(moduleLayer_->enabled());
	ui_.FrequencySpin->setValue(moduleLayer_->frequency());

	ui_.ModulePanel->updateControls();

	refreshing_ = false;
}

// Disable sensitive controls within tab
void LayerTab::disableSensitiveControls()
{
	ui_.EnabledButton->setEnabled(false);
	ui_.FrequencySpin->setEnabled(false);
	ui_.ModulePanel->disableSensitiveControls();
}

// Enable sensitive controls within tab
void LayerTab::enableSensitiveControls()
{
	ui_.EnabledButton->setEnabled(true);
	ui_.FrequencySpin->setEnabled(true);
	ui_.ModulePanel->enableSensitiveControls();
}

/*
 * State
 */

// Read widget state through specified LineParser
bool LayerTab::readState(LineParser& parser, const CoreData& coreData)
{
	if (!ui_.ModulePanel->readState(parser)) return false;

	return true;
}

// Write widget state through specified LineParser
bool LayerTab::writeState(LineParser& parser)
{
	if (!ui_.ModulePanel->writeState(parser)) return false;

	return true;
}
