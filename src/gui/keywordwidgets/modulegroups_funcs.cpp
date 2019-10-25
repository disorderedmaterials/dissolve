/*
	*** Keyword Widget - Module Groups
	*** src/gui/keywordwidgets/modulegroups_funcs.cpp
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

#include "gui/keywordwidgets/modulegroups.h"
#include "gui/keywordwidgets/dropdown.h"
#include "gui/helpers/tablewidgetupdater.h"
#include "module/module.h"
#include "module/group.h"
#include "module/groups.h"
#include "classes/coredata.h"
#include "genericitems/listhelper.h"
#include <QHBoxLayout>
#include <QComboBox>
#include <QString>

// Constructor
ModuleGroupsKeywordWidget::ModuleGroupsKeywordWidget(QWidget* parent, KeywordBase* keyword, const CoreData& coreData) : KeywordDropDown(this), KeywordWidgetBase(coreData)
{
	// Create and set up the UI for our widget in the drop-down's widget container
	ui_.setupUi(dropWidget());

	// Connect signals / slots
	connect(ui_.SelectionTable, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(itemChanged(QTableWidgetItem*)));

	// Cast the pointer up into the parent class type
	keyword_ = dynamic_cast<ModuleGroupsKeyword*>(keyword);
	if (!keyword_) Messenger::error("Couldn't cast base keyword into ModuleGroupsKeyword.\n");
	else
	{
		// Set current information
		updateWidgetValues(coreData_);
	}
}

/*
 * Widgets
 */

// Selection table update function
void ModuleGroupsKeywordWidget::updateSelectionRow(int row, Module* module, bool create)
{
	// Grab the target groups
	ModuleGroups& groups = keyword_->data();

	QTableWidgetItem* item;

	// Module name
	if (create)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<Module>(module));
		item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
		ui_.SelectionTable->setItem(row, 0, item);
	}
	else item = ui_.SelectionTable->item(row, 0);
	item->setText(module->uniqueName());
	item->setCheckState(groups.contains(module) ? Qt::Checked : Qt::Unchecked);

	// Module group
	if (create)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<Module>(module));
		ui_.SelectionTable->setItem(row, 1, item);
	}
	else item = ui_.SelectionTable->item(row, 1);
	item->setText(groups.groupName(module));
}

// Function type combo changed
void ModuleGroupsKeywordWidget::itemChanged(QTableWidgetItem* item)
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
void ModuleGroupsKeywordWidget::updateValue()
{
	updateWidgetValues(coreData_);
}

// Update widget values data based on keyword data
void ModuleGroupsKeywordWidget::updateWidgetValues(const CoreData& coreData)
{
	refreshing_ = true;

	// Get a RefList of current Modules that are of the correct type
	RefList<Module> availableModules = coreData.findModules(keyword_->data().allowedModuleTypes());

	// Update the list widget
	TableWidgetRefListUpdater<ModuleGroupsKeywordWidget,Module> tableUpdater(ui_.SelectionTable, availableModules, this, &ModuleGroupsKeywordWidget::updateSelectionRow);

	ui_.SelectionTable->resizeColumnToContents(0);

	updateSummaryText();

	refreshing_ = false;
}

// Update keyword data based on widget values
void ModuleGroupsKeywordWidget::updateKeywordData()
{
	// Loop over items in the QListWidget, adding the associated Modules/groups for any that are checked
	ModuleGroups newGroups;
	newGroups.setAllowedModuleTypes(keyword_->data().allowedModuleTypes());
	for (int n=0; n<ui_.SelectionTable->rowCount(); ++n)
	{
		// Get selection status and Module pointer from first column
		QTableWidgetItem* item = ui_.SelectionTable->item(n, 0);
		Module* module = VariantPointer<Module>(item->data(Qt::UserRole));
		if ((!module) || (item->checkState() != Qt::Checked)) continue;

		// Second column contains group name
		QString group = ui_.SelectionTable->item(n, 1)->text();
		newGroups.addModule(module, qPrintable(group));
	}

	// Update the data
	keyword_->setData(newGroups);
}

// Update summary text
void ModuleGroupsKeywordWidget::updateSummaryText()
{
	// Create summary text for the KeywordDropDown button
	ModuleGroups& groups = keyword_->data();
	if (groups.modules().nItems() == 0) setSummaryText("<None>");
	else
	{
		CharString summaryText;
		ListIterator<ModuleGroup> groupIterator(groups.groups());
		while (ModuleGroup* group = groupIterator.iterate())
		{
			if (groupIterator.isFirst()) summaryText.sprintf("%i (%s)", group->nModules(), group->name());
			else summaryText.strcatf(", %i (%s)", group->nModules(), group->name());
		}
		setSummaryText(summaryText);
	}
}
