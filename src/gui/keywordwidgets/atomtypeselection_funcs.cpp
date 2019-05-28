/*
	*** Keyword Widget - AtomTypeSelection
	*** src/gui/keywordwidgets/atomtypeselection_funcs.cpp
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

#include "gui/keywordwidgets/atomtypeselection.h"
#include "gui/keywordwidgets/dropdown.h"
#include "gui/helpers/listwidgetupdater.h"
#include "classes/atomtype.h"
#include "classes/atomtypelist.h"
#include "classes/coredata.h"
#include "genericitems/listhelper.h"
#include <QHBoxLayout>
#include <QComboBox>
#include <QString>

// Constructor
AtomTypeSelectionKeywordWidget::AtomTypeSelectionKeywordWidget(QWidget* parent, ModuleKeywordBase* keyword, const CoreData& coreData, GenericList& moduleData, const char* prefix) : KeywordDropDown(this), KeywordWidgetBase(coreData, moduleData, prefix)
{
	// Create and set up the UI for our widget in the drop-down's widget container
	ui.setupUi(dropWidget());

	// Connect signals / slots
	connect(ui.SelectionList, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(itemChanged(QListWidgetItem*)));

	// Cast the pointer up into the parent class type
	keyword_ = dynamic_cast<AtomTypeSelectionModuleKeyword*>(keyword);
	if (!keyword_) Messenger::error("Couldn't cast base module keyword into AtomTypeSelectionModuleKeyword.\n");
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
void AtomTypeSelectionKeywordWidget::updateSelectionRow(int row, AtomType* atomType, bool createItem)
{
	// Grab the target AtomTypeSelection
	AtomTypeList& selection = keyword_->data();

	QListWidgetItem* item;
	if (createItem)
	{
		item = new QListWidgetItem(atomType->name());
		item->setData(Qt::UserRole, VariantPointer<AtomType>(atomType));
		item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
		ui.SelectionList->insertItem(row, item);
	}
	else item = ui.SelectionList->item(row);
	item->setCheckState(selection.contains(atomType) ? Qt::Checked : Qt::Unchecked);
}

// Function type combo changed
void AtomTypeSelectionKeywordWidget::itemChanged(QListWidgetItem* item)
{
	if (refreshing_) return;

	updateKeywordData();

	updateSummaryText();

	emit(keywordValueChanged());
}

/*
 * Update
 */

// Update value displayed in widget, using specified source if necessary
void AtomTypeSelectionKeywordWidget::updateValue()
{
	refreshing_ = true;

	// Check to see if the associated Keyword may have been stored/updated in the specified moduleData
	if ((keyword_->genericItemFlags()&GenericItem::InRestartFileFlag) && moduleData_.contains(keyword_->keyword(), modulePrefix_))
	{
		// Retrieve the item from the list and set our widgets
		keyword_->selection() = GenericListHelper<AtomTypeList>::value(moduleData_, keyword_->keyword(), modulePrefix_);
	}

	refreshing_ = false;

	updateWidgetValues(coreData_);
}

// Update widget values data based on keyword data
void AtomTypeSelectionKeywordWidget::updateWidgetValues(const CoreData& coreData)
{
	refreshing_ = true;
	
	// Update the list against the global AtomType list
	ListWidgetUpdater<AtomTypeSelectionKeywordWidget,AtomType> listUpdater(ui.SelectionList, coreData_.constAtomTypes(), this, &AtomTypeSelectionKeywordWidget::updateSelectionRow);

	updateSummaryText();

	refreshing_ = false;
}

// Update keyword data based on widget values
void AtomTypeSelectionKeywordWidget::updateKeywordData()
{
	// Loop over items in the QListWidget, adding the associated AtomTypes for any that are checked
	AtomTypeList newSelection;
	for (int n=0; n<ui.SelectionList->count(); ++n)
	{
		QListWidgetItem* item = ui.SelectionList->item(n);
		if (item->checkState() == Qt::Checked) newSelection.add( (AtomType*) VariantPointer<AtomType>(item->data(Qt::UserRole)));
	}
	keyword_->setData(newSelection);
}

// Update summary text
void AtomTypeSelectionKeywordWidget::updateSummaryText()
{
	// Create summary text for the KeywordDropDown button
	AtomTypeList& selection = keyword_->data();
	if (selection.nItems() == 0) setSummaryText("<None>");
	else
	{
		CharString summaryText;
		ListIterator<AtomTypeData> atomTypeIterator(selection.types());
		while (AtomTypeData* atd = atomTypeIterator.iterate())
		{
			if (atomTypeIterator.isFirst()) summaryText = atd->atomTypeName();
			else summaryText.strcatf(", %s", atd->atomTypeName());
		}
		setSummaryText(summaryText);
	}
}
