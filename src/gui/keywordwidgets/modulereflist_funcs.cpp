/*
	*** Keyword Widget - Module RefList
	*** src/gui/keywordwidgets/modulereflist_funcs.cpp
	Copyright T. Youngs 2012-2020

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

#include "gui/keywordwidgets/modulereflist.h"
#include "gui/keywordwidgets/dropdown.h"
#include "gui/helpers/listwidgetupdater.h"
#include "module/module.h"
#include "classes/coredata.h"
#include "genericitems/listhelper.h"
#include <QHBoxLayout>
#include <QComboBox>
#include <QString>

// Constructor
ModuleRefListKeywordWidget::ModuleRefListKeywordWidget(QWidget* parent, KeywordBase* keyword, const CoreData& coreData) : KeywordDropDown(this), KeywordWidgetBase(coreData)
{
	// Create and set up the UI for our widget in the drop-down's widget container
	ui_.setupUi(dropWidget());

	// Connect signals / slots
	connect(ui_.SelectionList, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(itemChanged(QListWidgetItem*)));

	// Cast the pointer up into the parent class type
	keyword_ = dynamic_cast<ModuleRefListKeyword*>(keyword);
	if (!keyword_) Messenger::error("Couldn't cast base keyword into ModuleRefListKeyword.\n");
	else
	{
		// Set current information
		updateWidgetValues(coreData_);
	}
}

/*
 * Widgets
 */

// Selection list update function
void ModuleRefListKeywordWidget::updateSelectionRow(int row, Module* module, bool createItem)
{
	// Grab the target reference list
	RefList<Module>& selection = keyword_->data();

	QListWidgetItem* item;
	if (createItem)
	{
		item = new QListWidgetItem(module->uniqueName());
		item->setData(Qt::UserRole, VariantPointer<Module>(module));
		item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
		ui_.SelectionList->insertItem(row, item);
	}
	else item = ui_.SelectionList->item(row);
	item->setCheckState(selection.contains(module) ? Qt::Checked : Qt::Unchecked);
}

// List item changed
void ModuleRefListKeywordWidget::itemChanged(QListWidgetItem* item)
{
	if (refreshing_) return;

	updateKeywordData();

	updateSummaryText();

	emit(keywordValueChanged(keyword_->optionMask()));
}

/*
 * Update
 */

// Update value displayed in widget
void ModuleRefListKeywordWidget::updateValue()
{
	updateWidgetValues(coreData_);
}

// Update widget values data based on keyword data
void ModuleRefListKeywordWidget::updateWidgetValues(const CoreData& coreData)
{
	refreshing_ = true;

	// Get a RefList of current Modules that are of the correct type
	RefList<Module> availableModules = coreData.findModules(keyword_->moduleTypes());

	// Update the list widget
	ListWidgetUpdater<ModuleRefListKeywordWidget,Module> listUpdater(ui_.SelectionList, availableModules, this, &ModuleRefListKeywordWidget::updateSelectionRow);

	updateSummaryText();

	refreshing_ = false;
}

// Update keyword data based on widget values
void ModuleRefListKeywordWidget::updateKeywordData()
{
	// Loop over items in the QListWidget, adding the associated Modules for any that are checked
	RefList<Module> newSelection;
	for (int n=0; n<ui_.SelectionList->count(); ++n)
	{
		QListWidgetItem* item = ui_.SelectionList->item(n);
		if (item->checkState() == Qt::Checked) newSelection.append(VariantPointer<Module>(item->data(Qt::UserRole)));
	}

	// Update the data
	keyword_->setData(newSelection);
}

// Update summary text
void ModuleRefListKeywordWidget::updateSummaryText()
{
	// Create summary text for the KeywordDropDown button
	RefList<Module>& selection = keyword_->data();
	if (selection.nItems() == 0) setSummaryText("<None>");
	else
	{
		CharString summaryText;
		bool first = true;
		for (Module* module : selection)
		{
			if (first) summaryText = module->uniqueName();
			else summaryText.strcatf(", %s", module->uniqueName());
			first = false;
		}
		setSummaryText(summaryText);
	}
}
