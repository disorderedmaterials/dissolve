/*
	*** Keyword Widget - Species Reference List
	*** src/gui/keywordwidgets/speciesreferencelist_funcs.cpp
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

#include "gui/keywordwidgets/speciesreferencelist.h"
#include "gui/keywordwidgets/dropdown.h"
#include "gui/helpers/listwidgetupdater.h"
#include "classes/coredata.h"
#include "classes/species.h"
#include "genericitems/listhelper.h"
#include <QHBoxLayout>
#include <QComboBox>
#include <QString>

// Constructor
SpeciesReferenceListKeywordWidget::SpeciesReferenceListKeywordWidget(QWidget* parent, ModuleKeywordBase* keyword, const CoreData& coreData, GenericList& moduleData, const char* prefix) : KeywordDropDown(this), KeywordWidgetBase(coreData, moduleData, prefix)
{
	// Create and set up the UI for our widget in the drop-down's widget container
	ui.setupUi(dropWidget());

	// Connect signals / slots
	connect(ui.SelectionList, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(itemChanged(QListWidgetItem*)));

	// Cast the pointer up into the parent class type
	keyword_ = dynamic_cast<SpeciesReferenceListModuleKeyword*>(keyword);
	if (!keyword_) Messenger::error("Couldn't cast base module keyword '%s' into SpeciesReferenceListModuleKeyword.\n", keyword->keyword());
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
void SpeciesReferenceListKeywordWidget::updateSelectionRow(int row, Species* sp, bool createItem)
{
	// Grab the target reference list
	RefList<Species,bool>& selection = keyword_->data();

	QListWidgetItem* item;
	if (createItem)
	{
		item = new QListWidgetItem(sp->name());
		item->setData(Qt::UserRole, VariantPointer<Species>(sp));
		item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
		ui.SelectionList->insertItem(row, item);
	}
	else item = ui.SelectionList->item(row);
	item->setCheckState(selection.contains(sp) ? Qt::Checked : Qt::Unchecked);
}

// Function type combo changed
void SpeciesReferenceListKeywordWidget::itemChanged(QListWidgetItem* item)
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
void SpeciesReferenceListKeywordWidget::updateValue()
{
	refreshing_ = true;

	// Check to see if the associated Keyword may have been stored/updated in the specified moduleData
	/* Not Relevant */

	refreshing_ = false;

	updateWidgetValues(coreData_);
}

// Update widget values data based on keyword data
void SpeciesReferenceListKeywordWidget::updateWidgetValues(const CoreData& coreData)
{
	refreshing_ = true;
	
	// Update the list against the global Species list
	ListWidgetUpdater<SpeciesReferenceListKeywordWidget,Species> listUpdater(ui.SelectionList, coreData_.constSpecies(), this, &SpeciesReferenceListKeywordWidget::updateSelectionRow);

	updateSummaryText();

	refreshing_ = false;
}

// Update keyword data based on widget values
void SpeciesReferenceListKeywordWidget::updateKeywordData()
{
	// Loop over items in the QListWidget, adding the associated Speciess for any that are checked
	RefList<Species,bool> newSelection;
	for (int n=0; n<ui.SelectionList->count(); ++n)
	{
		QListWidgetItem* item = ui.SelectionList->item(n);
		if (item->checkState() == Qt::Checked) newSelection.add(VariantPointer<Species>(item->data(Qt::UserRole)));
	}
	keyword_->setData(newSelection);
}

// Update summary text
void SpeciesReferenceListKeywordWidget::updateSummaryText()
{
	// Create summary text for the KeywordDropDown button
	RefList<Species,bool>& selection = keyword_->data();
	if (selection.nItems() == 0) setSummaryText("<None>");
	else
	{
		CharString summaryText;
		RefListIterator<Species,bool> speciesIterator(selection);
		while (Species* sp = speciesIterator.iterate())
		{
			if (speciesIterator.isFirst()) summaryText = sp->name();
			else summaryText.strcatf(", %s", sp->name());
		}
		setSummaryText(summaryText);
	}
}
