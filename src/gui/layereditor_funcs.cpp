/*
	*** Layer Editor Functions
	*** src/gui/layereditor_funcs.cpp
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

#include "gui/layereditor.h"
#include "gui/gui.h"
#include "gui/modulechart.hui"
#include "gui/widgets/mimetreewidgetitem.h"
#include "main/dissolve.h"
#include "base/lineparser.h"
#include "templates/variantpointer.h"

// Constructor / Destructor
LayerEditor::LayerEditor(QWidget* parent) : QWidget(parent)
{
	ui.setupUi(this);

	chartWidget_ = NULL;

	refreshing_ = false;
}

LayerEditor::~LayerEditor()
{
}

/*
 * Setup
 */

// Setup up the LayerEditor for the specified Module list
bool LayerEditor::setUp(DissolveWindow* dissolveWindow, ModuleLayer* moduleLayer, Configuration* localConfiguration)
{
	dissolveWindow_ = dissolveWindow;
	moduleLayer_ = moduleLayer;
	localConfiguration_ = localConfiguration;

	// Create a ModuleChart widget and set its source list
	chartWidget_ = new ModuleChart(dissolveWindow, *moduleLayer_, localConfiguration_);
	chartWidget_->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Minimum);
	ui.LayerScrollArea->setWidget(chartWidget_);

	// Add MimeTreeWidgetItems for each Module, adding them to a parent category item
	moduleCategories_.clear();
	ListIterator<Module> moduleIterator(dissolveWindow->dissolve().masterModules());
	while (const Module* module = moduleIterator.iterate())
	{
		// Check that the category is not 'HIDDEN' (in which case we don't show it)
		if (DissolveSys::sameString("HIDDEN", module->category())) continue;

		// Find category for this Module (if it exists) or create a new one
		MimeTreeWidgetItem* categoryItem = NULL;
		RefListIterator<MimeTreeWidgetItem,CharString> categoryIterator(moduleCategories_);
		while (categoryItem = categoryIterator.iterate()) if (DissolveSys::sameString(module->category(), categoryIterator.currentData())) break;
		if (categoryItem == NULL)
		{
			categoryItem = new MimeTreeWidgetItem((QTreeWidget*)NULL, 1000);
			categoryItem->setText(0, module->category());
			categoryItem->setFlags(Qt::ItemIsEnabled);
			moduleCategories_.add(categoryItem, module->category());
		}

		// Create item for the Module
		MimeTreeWidgetItem* item = new MimeTreeWidgetItem(categoryItem, 1000);
		item->setIcon(0, ModuleChartModuleBlock::modulePixmap(module));
		item->setText(0, module->type());
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsDragEnabled);
		item->setData(0, Qt::UserRole, VariantPointer<const Module>(module));
		item->setToolTip(0, module->brief());
		item->addMimeString(MimeString::ModuleType, module->type());
	}

	// Populate the available Modules tree with the categories we now have
	ui.AvailableModulesTree->clear();
	RefListIterator<MimeTreeWidgetItem,CharString> categoryIterator(moduleCategories_);
	while (MimeTreeWidgetItem* categoryItem = categoryIterator.iterate()) ui.AvailableModulesTree->addTopLevelItem(categoryItem);
	ui.AvailableModulesTree->resizeColumnToContents(0);
	ui.AvailableModulesTree->sortByColumn(0, Qt::AscendingOrder);
	ui.AvailableModulesTree->setSortingEnabled(true);
	ui.AvailableModulesTree->expandAll();

	// Hide palette group initially
// 	ui.PaletteGroup->setVisible(false);

	updateControls();
}

/*
 * Update
 */

// Update controls in tab
void LayerEditor::updateControls()
{
	if (!chartWidget_) return;

	refreshing_ = true;

	chartWidget_->updateControls();

	refreshing_ = false;
}

// Disable sensitive controls within tab, ready for main code to run
void LayerEditor::disableSensitiveControls()
{
	ui.AvailableModulesTree->setEnabled(false);
	chartWidget_->disableSensitiveControls();
}

// Enable sensitive controls within tab, ready for main code to run
void LayerEditor::enableSensitiveControls()
{
	ui.AvailableModulesTree->setEnabled(true);
	chartWidget_->enableSensitiveControls();
}

/*
 * Widget Functions
 */

void LayerEditor::on_AvailableModulesTree_itemDoubleClicked(QTreeWidgetItem* item)
{
	if (!moduleLayer_) return;

	// Get the Module associated to the double-clicked item
	const Module* module = VariantPointer<const Module>(item->data(0, Qt::UserRole));
	if (!module) return;

	// Create a new instance of the Module
	Module* newInstance = dissolveWindow_->dissolve().createModuleInstance(module->type());
	newInstance->setConfigurationLocal(localConfiguration_);

	// Set Configuration targets as appropriate
	if (newInstance->nTargetableConfigurations() != 0)
	{
		if (localConfiguration_) newInstance->addTargetConfiguration(localConfiguration_);
		else
		{
			ListIterator<Configuration> configIterator(dissolveWindow_->dissolve().configurations());
			while (Configuration* cfg = configIterator.iterate())
			{
				newInstance->addTargetConfiguration(cfg);
				if ((newInstance->nTargetableConfigurations() != -1) && (newInstance->nTargetableConfigurations() == newInstance->nTargetConfigurations())) break;
			}
		}
	}

	moduleLayer_->add(newInstance);

	updateControls();

	// Flag that the current data has changed
	dissolveWindow_->setModified();
}

/*
 * State
 */

// Write widget state through specified LineParser
bool LayerEditor::writeState(LineParser& parser)
{
	if ((!chartWidget_) || (!chartWidget_->writeState(parser))) return false;

	return true;
}

// Read widget state through specified LineParser
bool LayerEditor::readState(LineParser& parser)
{
	if ((!chartWidget_) || (!chartWidget_->readState(parser))) return false;

	return true;
}
