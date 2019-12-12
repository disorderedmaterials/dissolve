/*
	*** Module List Editor Functions
	*** src/gui/modulelisteditor_funcs.cpp
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

#include "gui/modulelisteditor.h"
#include "gui/gui.h"
#include "gui/charts/modulelist.h"
#include "gui/charts/moduleblock.h"
#include "gui/widgets/mimetreewidgetitem.h"
#include "main/dissolve.h"
#include "base/lineparser.h"
#include "templates/variantpointer.h"

// Constructor / Destructor
ModuleListEditor::ModuleListEditor(QWidget* parent) : QWidget(parent)
{
	ui_.setupUi(this);

	chartWidget_ = NULL;

	refreshing_ = false;
}

ModuleListEditor::~ModuleListEditor()
{
}

/*
 * Setup
 */

// Setup up the ModuleListEditor for the specified Module list
bool ModuleListEditor::setUp(DissolveWindow* dissolveWindow, ModuleLayer* moduleLayer, Configuration* localConfiguration)
{
	dissolveWindow_ = dissolveWindow;
	moduleLayer_ = moduleLayer;
	localConfiguration_ = localConfiguration;

	// Create a ModuleListChart widget and set its source list
	chartWidget_ = new ModuleListChart(moduleLayer_, dissolveWindow_->dissolve(), localConfiguration_);
	chartWidget_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	ui_.ChartScrollArea->setWidget(chartWidget_);
	ui_.ChartScrollArea->setWidgetResizable(true);
	ui_.ChartScrollArea->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
	connect(chartWidget_, SIGNAL(dataModified()), dissolveWindow_, SLOT(setModified()));
	connect(chartWidget_, SIGNAL(blockDoubleClicked(const QString&)), dissolveWindow_, SLOT(showModuleTab(const QString&)));
	connect(chartWidget_, SIGNAL(blockRemoved(const QString&)), dissolveWindow_, SLOT(removeModuleTab(const QString&)));
	connect(chartWidget_, SIGNAL(blockSelectionChanged(const QString&)), this, SLOT(blockSelectionChanged(const QString&)));

	// Add MimeTreeWidgetItems for each Module, adding them to a parent category item
	moduleCategories_.clear();
	ListIterator<Module> moduleIterator(dissolveWindow->dissolve().masterModules());
	while (const Module* module = moduleIterator.iterate())
	{
		// Check that the category is not 'HIDDEN' (in which case we don't show it)
		if (DissolveSys::sameString("HIDDEN", module->category())) continue;

		// Find category for this Module (if it exists) or create a new one
		MimeTreeWidgetItem* categoryItem = NULL;
		RefDataListIterator<MimeTreeWidgetItem,CharString> categoryIterator(moduleCategories_);
		while (categoryItem = categoryIterator.iterate()) if (DissolveSys::sameString(module->category(), categoryIterator.currentData())) break;
		if (categoryItem == NULL)
		{
			categoryItem = new MimeTreeWidgetItem((QTreeWidget*)NULL, 1000);
			categoryItem->setText(0, module->category());
			categoryItem->setFlags(Qt::ItemIsEnabled);
			moduleCategories_.append(categoryItem, module->category());
		}

		// Create item for the Module
		MimeTreeWidgetItem* item = new MimeTreeWidgetItem(categoryItem, 1000);
		item->setIcon(0, ModuleBlock::modulePixmap(module));
		item->setText(0, module->type());
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsDragEnabled);
		item->setData(0, Qt::UserRole, VariantPointer<const Module>(module));
		item->setToolTip(0, module->brief());
		item->addMimeString(MimeString::ModuleType, module->type());
	}

	// Populate the available Modules tree with the categories we now have
	ui_.AvailableModulesTree->clear();
	RefDataListIterator<MimeTreeWidgetItem,CharString> categoryIterator(moduleCategories_);
	while (MimeTreeWidgetItem* categoryItem = categoryIterator.iterate()) ui_.AvailableModulesTree->addTopLevelItem(categoryItem);
	ui_.AvailableModulesTree->resizeColumnToContents(0);
	ui_.AvailableModulesTree->sortByColumn(0, Qt::AscendingOrder);
	ui_.AvailableModulesTree->setSortingEnabled(true);
	ui_.AvailableModulesTree->expandAll();

	// Set up the ControlsWidget, and hide it initially
	ui_.ControlsWidget->setUp(dissolveWindow_);
	ui_.ControlsWidget->setVisible(false);
	connect(ui_.ControlsWidget, SIGNAL(dataModified()), dissolveWindow_, SLOT(setModified()));

	// Hide palette group initially
// 	ui_.PaletteGroup->setVisible(false);

	updateControls();
}

/*
 * Update
 */

// Update controls in tab
void ModuleListEditor::updateControls()
{
	if (!chartWidget_) return;

	refreshing_ = true;

	chartWidget_->updateControls();
	ui_.ControlsWidget->updateControls();

	refreshing_ = false;
}

// Disable sensitive controls within tab
void ModuleListEditor::disableSensitiveControls()
{
	ui_.AvailableModulesTree->setEnabled(false);
	chartWidget_->disableSensitiveControls();
	ui_.ControlsWidget->disableSensitiveControls();
}

// Enable sensitive controls within tab
void ModuleListEditor::enableSensitiveControls()
{
	ui_.AvailableModulesTree->setEnabled(true);
	chartWidget_->enableSensitiveControls();
	ui_.ControlsWidget->enableSensitiveControls();
}

/*
 * Widget Functions
 */

void ModuleListEditor::blockSelectionChanged(const QString& blockIdentifier)
{
	if (blockIdentifier.isEmpty())
	{
		ui_.ControlsWidget->setModule(NULL, NULL);
		ui_.ControlsWidget->setVisible(false);
		return;
	}

	// Get Module by unique name
	Module* module = dissolveWindow_->dissolve().findModuleInstance(qPrintable(blockIdentifier));
	if (!module)
	{
		ui_.ControlsWidget->setModule(NULL, NULL);
		ui_.ControlsWidget->setVisible(false);
		return;
	}

	ui_.ControlsWidget->setModule(module, &dissolveWindow_->dissolve());
	ui_.ControlsWidget->setVisible(true);
}

void ModuleListEditor::on_AvailableModulesTree_itemDoubleClicked(QTreeWidgetItem* item)
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

	moduleLayer_->own(newInstance);

	updateControls();

	// Flag that the current data has changed
	dissolveWindow_->setModified();
}

/*
 * State
 */

// Write widget state through specified LineParser
bool ModuleListEditor::writeState(LineParser& parser)
{
	if ((!chartWidget_) || (!chartWidget_->writeState(parser))) return false;

	return true;
}

// Read widget state through specified LineParser
bool ModuleListEditor::readState(LineParser& parser)
{
	if ((!chartWidget_) || (!chartWidget_->readState(parser))) return false;

	return true;
}
