/*
	*** Keyword Widget - Module Reference List
	*** src/gui/keywordwidgets/modulereferencelist_funcs.cpp
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

#include "gui/keywordwidgets/modulereferencelist.h"
#include "gui/keywordwidgets/dropdown.h"
#include "gui/helpers/listwidgetupdater.h"
#include "module/module.h"
#include "classes/coredata.h"
#include "genericitems/listhelper.h"
#include <QHBoxLayout>
#include <QComboBox>
#include <QString>

// Constructor
ModuleReferenceListKeywordWidget::ModuleReferenceListKeywordWidget(QWidget* parent, KeywordBase* keyword, const CoreData& coreData, GenericList& moduleData, const char* prefix) : KeywordDropDown(this), KeywordWidgetBase(coreData, moduleData, prefix)
{
	// Create and set up the UI for our widget in the drop-down's widget container
	ui.setupUi(dropWidget());

	// Connect signals / slots
	connect(ui.SelectionList, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(itemChanged(QListWidgetItem*)));

	// Cast the pointer up into the parent class type
	keyword_ = dynamic_cast<ModuleReferenceListKeyword*>(keyword);
	if (!keyword_) Messenger::error("Couldn't cast base module keyword into ModuleReferenceListKeyword.\n");
	else
	{
		// Set current information
		updateWidgetValues(coreData_);
	}
}

/*
 * Signals / Slots
 */

// Selection list update function
void ModuleReferenceListKeywordWidget::updateSelectionRow(int row, Module* module, bool createItem)
{
	// Grab the target reference list
	RefList<Module>& selection = keyword_->data();

	QListWidgetItem* item;
	if (createItem)
	{
		item = new QListWidgetItem(module->uniqueName());
		item->setData(Qt::UserRole, VariantPointer<Module>(module));
		item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
		ui.SelectionList->insertItem(row, item);
	}
	else item = ui.SelectionList->item(row);
	item->setCheckState(selection.contains(module) ? Qt::Checked : Qt::Unchecked);
}

// Function type combo changed
void ModuleReferenceListKeywordWidget::itemChanged(QListWidgetItem* item)
{
	if (refreshing_) return;

	updateKeywordData();

	updateSummaryText();

	emit(keywordValueChanged());
}

/*
 * Update
 */

// Update value displayed in widget
void ModuleReferenceListKeywordWidget::updateValue()
{
	refreshing_ = true;

	// Check to see if the associated Keyword may have been stored/updated in the specified moduleData
	/* Not Relevant */

	refreshing_ = false;

	updateWidgetValues(coreData_);
}

// Update widget values data based on keyword data
void ModuleReferenceListKeywordWidget::updateWidgetValues(const CoreData& coreData)
{
	refreshing_ = true;

	// Get a RefList of current Modules that are of the correct type
	RefList<Module> availableModules = coreData.findModules(keyword_->moduleTypes());

	// Update the list widget
	ListWidgetUpdater<ModuleReferenceListKeywordWidget,Module> listUpdater(ui.SelectionList, availableModules, this, &ModuleReferenceListKeywordWidget::updateSelectionRow);

	updateSummaryText();

	refreshing_ = false;
}

// Update keyword data based on widget values
void ModuleReferenceListKeywordWidget::updateKeywordData()
{
	// Loop over items in the QListWidget, adding the associated Modules for any that are checked
	RefList<Module> newSelection;
	for (int n=0; n<ui.SelectionList->count(); ++n)
	{
		QListWidgetItem* item = ui.SelectionList->item(n);
		if (item->checkState() == Qt::Checked) newSelection.append(VariantPointer<Module>(item->data(Qt::UserRole)));
	}

	// Update the data
	keyword_->setData(newSelection);
}

// Update summary text
void ModuleReferenceListKeywordWidget::updateSummaryText()
{
	// Create summary text for the KeywordDropDown button
	RefList<Module>& selection = keyword_->data();
	if (selection.nItems() == 0) setSummaryText("<None>");
	else
	{
		CharString summaryText;
		RefListIterator<Module> moduleIterator(selection);
		while (Module* module = moduleIterator.iterate())
		{
			if (moduleIterator.isFirst()) summaryText = module->uniqueName();
			else summaryText.strcatf(", %s", module->uniqueName());
		}
		setSummaryText(summaryText);
	}
}
