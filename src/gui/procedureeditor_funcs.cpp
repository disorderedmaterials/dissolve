/*
	*** Procedure Editor Functions
	*** src/gui/procedureeditor_funcs.cpp
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

#include "gui/procedureeditor.h"
#include "gui/gui.h"
#include "gui/charts/procedure.h"
#include "gui/widgets/mimetreewidgetitem.h"
#include "main/dissolve.h"
#include "base/lineparser.h"
#include "templates/variantpointer.h"

// Constructor / Destructor
ProcedureEditor::ProcedureEditor(QWidget* parent) : QWidget(parent)
{
	ui_.setupUi(this);

	procedure_ = NULL;
	chart_ = NULL;

	refreshing_ = false;
}

ProcedureEditor::~ProcedureEditor()
{
}

/*
 * Procedure Target
 */

// Set up to display / edit the target Procedure
void ProcedureEditor::setUp(Procedure* procedure, const CoreData& coreData)
{
	procedure_ = procedure;

	// Create the ProcedureChart and add it to the scroll area
	chart_ = new ProcedureChart(procedure, coreData);
	chart_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Ignored);
	ui_.ProcedureScrollArea->setWidget(chart_);
	ui_.ProcedureScrollArea->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

	// Connect signals / slots
	connect(chart_, SIGNAL(dataModified()), this, SLOT(chartDataModified()));

	updateControls();
}

// 	// Add MimeTreeWidgetItems for each Module, adding them to a parent category item
// 	moduleCategories_.clear();
// 	ListIterator<Module> moduleIterator(dissolveWindow->dissolve().masterModules());
// 	while (const Module* module = moduleIterator.iterate())
// 	{
// 		// Check that the category is not 'HIDDEN' (in which case we don't show it)
// 		if (DissolveSys::sameString("HIDDEN", module->category())) continue;
// 
// 		// Find category for this Module (if it exists) or create a new one
// 		MimeTreeWidgetItem* categoryItem = NULL;
// 		RefDataListIterator<MimeTreeWidgetItem,CharString> categoryIterator(moduleCategories_);
// 		while (categoryItem = categoryIterator.iterate()) if (DissolveSys::sameString(module->category(), categoryIterator.currentData())) break;
// 		if (categoryItem == NULL)
// 		{
// 			categoryItem = new MimeTreeWidgetItem((QTreeWidget*)NULL, 1000);
// 			categoryItem->setText(0, module->category());
// 			categoryItem->setFlags(Qt::ItemIsEnabled);
// 			moduleCategories_.append(categoryItem, module->category());
// 		}
// 
// 		// Create item for the Module
// 		MimeTreeWidgetItem* item = new MimeTreeWidgetItem(categoryItem, 1000);
// 		item->setIcon(0, ModuleChartModuleBlock::modulePixmap(module));
// 		item->setText(0, module->type());
// 		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsDragEnabled);
// 		item->setData(0, Qt::UserRole, VariantPointer<const Module>(module));
// 		item->setToolTip(0, module->brief());
// 		item->addMimeString(MimeString::ModuleType, module->type());
// 	}
// 
// 	// Populate the available Modules tree with the categories we now have
// 	ui_.AvailableModulesTree->clear();
// 	RefDataListIterator<MimeTreeWidgetItem,CharString> categoryIterator(moduleCategories_);
// 	while (MimeTreeWidgetItem* categoryItem = categoryIterator.iterate()) ui_.AvailableModulesTree->addTopLevelItem(categoryItem);
// 	ui_.AvailableModulesTree->resizeColumnToContents(0);
// 	ui_.AvailableModulesTree->sortByColumn(0, Qt::AscendingOrder);
// 	ui_.AvailableModulesTree->setSortingEnabled(true);
// 	ui_.AvailableModulesTree->expandAll();

	// Hide palette group initially
// 	ui_.PaletteGroup->setVisible(false);

/*
 * Update
 */

// Update controls in tab
void ProcedureEditor::updateControls()
{
	refreshing_ = true;

	chart_->updateControls();

	refreshing_ = false;
}

// Disable sensitive controls within tab, ready for main code to run
void ProcedureEditor::disableSensitiveControls()
{
// 	ui_.AvailableModulesTree->setEnabled(false);
	chart_->disableSensitiveControls();
}

// Enable sensitive controls within tab, ready for main code to run
void ProcedureEditor::enableSensitiveControls()
{
// 	ui_.AvailableModulesTree->setEnabled(true);
	chart_->enableSensitiveControls();
}

/*
 * Widget Functions
 */

// void ProcedureEditor::on_AvailableModulesTree_itemDoubleClicked(QTreeWidgetItem* item)
// {
// 	if (!moduleProcedure_) return;
// 
// 	// Get the Module associated to the double-clicked item
// 	const Module* module = VariantPointer<const Module>(item->data(0, Qt::UserRole));
// 	if (!module) return;
// 
// 	// Create a new instance of the Module
// 	Module* newInstance = dissolveWindow_->dissolve().createModuleInstance(module->type());
// 	newInstance->setConfigurationLocal(localConfiguration_);
// 
// 	// Set Configuration targets as appropriate
// 	if (newInstance->nTargetableConfigurations() != 0)
// 	{
// 		if (localConfiguration_) newInstance->addTargetConfiguration(localConfiguration_);
// 		else
// 		{
// 			ListIterator<Configuration> configIterator(dissolveWindow_->dissolve().configurations());
// 			while (Configuration* cfg = configIterator.iterate())
// 			{
// 				newInstance->addTargetConfiguration(cfg);
// 				if ((newInstance->nTargetableConfigurations() != -1) && (newInstance->nTargetableConfigurations() == newInstance->nTargetConfigurations())) break;
// 			}
// 		}
// 	}
// 
// 	moduleProcedure_->own(newInstance);
// 
// 	updateControls();
// 
// 	// Flag that the current data has changed
// 	dissolveWindow_->setModified();
// }

/*
 * State
 */

// Write widget state through specified LineParser
bool ProcedureEditor::writeState(LineParser& parser)
{
	if (!chart_->writeState(parser)) return false;

	return true;
}

// Read widget state through specified LineParser
bool ProcedureEditor::readState(LineParser& parser)
{
	if (!chart_->readState(parser)) return false;

	return true;
}

/*
 * Signals / Slots
 */

// Data displayed in the chart has been modified
void ProcedureEditor::chartDataModified()
{
	emit(dataModified());
}
