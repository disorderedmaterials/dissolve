/*
	*** Keyword Widget - IsotopologueCollection
	*** src/gui/keywordwidgets/isotopologuecollection_funcs.cpp
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

#include "gui/keywordwidgets/isotopologuecollection.h"
#include "gui/keywordwidgets/dropdown.h"
#include "gui/delegates/combolist.hui"
#include "gui/delegates/exponentialspin.hui"
#include "gui/delegates/isotopologuecombo.hui"
#include "gui/delegates/usedspeciescombo.hui"
#include "gui/helpers/treewidgetupdater.h"
#include "module/module.h"
#include "classes/configuration.h"
#include "classes/coredata.h"
#include "classes/species.h"
#include "genericitems/listhelper.h"
#include "templates/variantpointer.h"

// Constructor
IsotopologueCollectionKeywordWidget::IsotopologueCollectionKeywordWidget(QWidget* parent, KeywordBase* keyword, const CoreData& coreData) : KeywordDropDown(this), KeywordWidgetBase(coreData)
{
	// Create and set up the UI for our widget in the drop-down's widget container
	ui_.setupUi(dropWidget());

	// Set delegates for table
	ui_.IsotopologueTree->setItemDelegateForColumn(2, new IsotopologueComboDelegate(this));
	ui_.IsotopologueTree->setItemDelegateForColumn(3, new ExponentialSpinDelegate(this));

	// Connect signals / slots
	connect(ui_.AutoButton, SIGNAL(clicked(bool)), this, SLOT(autoButton_clicked(bool)));
	connect(ui_.AddButton, SIGNAL(clicked(bool)), this, SLOT(addButton_clicked(bool)));
	connect(ui_.RemoveButton, SIGNAL(clicked(bool)), this, SLOT(removeButton_clicked(bool)));
	connect(ui_.IsotopologueTree, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(isotopologueTree_itemChanged(QTreeWidgetItem*,int)));
	connect(ui_.IsotopologueTree, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(isotopologueTree_currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));

	// Cast the pointer up into the parent class type
	keyword_ = dynamic_cast<IsotopologueCollectionKeyword*>(keyword);
	if (!keyword_) Messenger::error("Couldn't cast base keyword '%s' into IsotopologueCollectionKeyword.\n", keyword->name());
	else
	{
		// Set current information
		updateWidgetValues(coreData_);
	}

	// Summary text on KeywordDropDown button
	setSummaryText("Edit...");
}

/*
 * Widgets
 */

void IsotopologueCollectionKeywordWidget::autoButton_clicked(bool checked)
{
	// Complete the set
	keyword_->data().complete(keyword_->allowedConfigurations());

	// Manually flag that the keyword data has changed
	keyword_->hasBeenSet();

	updateWidgetValues(coreData_);

	emit(keywordValueChanged(keyword_->optionMask()));
}

void IsotopologueCollectionKeywordWidget::addButton_clicked(bool checked)
{
	QTreeWidgetItem* item = ui_.IsotopologueTree->currentItem();

	// Determine what kind of data is selected
	if (!item)
	{
		// No item selected - add next missing configuration
		for (Configuration* cfg : keyword_->allowedConfigurations())
		{
			if (keyword_->data().contains(cfg)) continue;
			if (cfg->usedSpecies().nItems() == 0) continue;

			keyword_->data().add(cfg, cfg->usedSpecies().first()->species()->naturalIsotopologue(), 1.0);

			break;
		}
	}
	else if (!item->text(0).isEmpty())
	{
		// Configuration, so add next missing Species
		IsotopologueSet* set = VariantPointer<IsotopologueSet>(item->data(0, Qt::UserRole));
		if (!set) return;

		ListIterator<SpeciesInfo> speciesIterator(set->configuration()->usedSpecies());
		while (SpeciesInfo* spInfo = speciesIterator.iterate())
		{
			if (set->contains(spInfo->species())) continue;

			set->add(spInfo->species()->naturalIsotopologue(), 1.0);

			break;
		}
	}
	else if (!item->text(1).isEmpty())
	{
		// Species, so add next missing Isotopologue
		IsotopologueSet* set = VariantPointer<IsotopologueSet>(item->data(0, Qt::UserRole));
		if (!set) return;

		Isotopologues* topes = VariantPointer<Isotopologues>(item->data(1, Qt::UserRole));
		if (!topes) return;

		Species* sp = topes->species();

		// Natural first
		if (!topes->contains(sp->naturalIsotopologue())) set->add(sp->naturalIsotopologue(), 1.0);
		else
		{
			ListIterator<Isotopologue> topeIterator(sp->isotopologues());
			while (Isotopologue* tope = topeIterator.iterate())
			{
				if (topes->contains(tope)) continue;

				set->add(tope, 1.0);

				break;
			}
		}
	}
	else
	{
		// IsotopologueWeight, so add next missing Isotopologue
		IsotopologueSet* set = VariantPointer<IsotopologueSet>(item->data(0, Qt::UserRole));
		if (!set) return;


		IsotopologueWeight* isoWeight = VariantPointer<IsotopologueWeight>(item->data(1, Qt::UserRole));
		if (!isoWeight) return;

		Species* sp = isoWeight->isotopologue()->parent();

		Isotopologues* topes = set->isotopologues(sp);

		// Natural first
		if (!topes->contains(sp->naturalIsotopologue())) set->add(sp->naturalIsotopologue(), 1.0);
		else
		{
			ListIterator<Isotopologue> topeIterator(sp->isotopologues());
			while (Isotopologue* tope = topeIterator.iterate())
			{
				if (topes->contains(tope)) continue;

				set->add(tope, 1.0);

				break;
			}
		}
	}

	// Manually flag that the keyword data has changed
	keyword_->hasBeenSet();

	updateWidgetValues(coreData_);

	emit(keywordValueChanged(keyword_->optionMask()));
}

void IsotopologueCollectionKeywordWidget::removeButton_clicked(bool checked)
{
	QTreeWidgetItem* item = ui_.IsotopologueTree->currentItem();
	if (!item) return;

	// Determine what kind of data is selected
	if (!item->text(0).isEmpty())
	{
		IsotopologueSet* set = VariantPointer<IsotopologueSet>(item->data(0, Qt::UserRole));
		if (!set) return;
		keyword_->data().remove(set);
	}
	else if (!item->text(1).isEmpty())
	{
		IsotopologueSet* set = VariantPointer<IsotopologueSet>(item->data(0, Qt::UserRole));
		if (!set) return;

		Isotopologues* topes = VariantPointer<Isotopologues>(item->data(1, Qt::UserRole));
		if (!topes) return;
		keyword_->data().remove(set, topes->species());
	}
	else
	{
		IsotopologueSet* set = VariantPointer<IsotopologueSet>(item->data(0, Qt::UserRole));
		if (!set) return;

		IsotopologueWeight* isoWeight = VariantPointer<IsotopologueWeight>(item->data(1, Qt::UserRole));
		if (!isoWeight) return;

		keyword_->data().remove(set, isoWeight);
	}

	// Manually flag that the keyword data has changed
	keyword_->hasBeenSet();

	updateWidgetValues(coreData_);

	emit(keywordValueChanged(keyword_->optionMask()));
}

void IsotopologueCollectionKeywordWidget::isotopologueTree_itemChanged(QTreeWidgetItem* w, int column)
{
	if (refreshing_) return;

	// Get item parent, and retrieve the IsotopologueSet
	if (!w->parent()) return;
	Isotopologues* topes = VariantPointer<Isotopologues>(w->parent()->data(1, Qt::UserRole));
	if (!topes) return;

	// Get the IsotopologueWeight for the current item
	IsotopologueWeight* topeWeight = VariantPointer<IsotopologueWeight>(w->data(1, Qt::UserRole));
	if (!topeWeight) return;

	// Column of passed item tells us the type of data we need to change
	Isotopologue* iso;
	switch (column)
	{
		// Isotopologue
		case (2):
			// Find the isotopologue by name in the parent species...
			iso = topes->species()->findIsotopologue(qPrintable(w->text(column)));
			if (!iso) return;

			// Set the new Isotopologue
			topeWeight->setIsotopologue(iso);

			// Manually flag that the keyword data has changed
			keyword_->hasBeenSet();

			emit(keywordValueChanged(keyword_->optionMask()));
			break;
		// Weight
		case (3):
			topeWeight->setWeight(w->text(column).toDouble());

			// Manually flag that the keyword data has changed
			keyword_->hasBeenSet();

			emit(keywordValueChanged(keyword_->optionMask()));
			break;
		default:
			Messenger::error("Don't know what to do with data from column %i of Isotopologue table.\n", column);
			break;
	}
}

void IsotopologueCollectionKeywordWidget::isotopologueTree_currentItemChanged(QTreeWidgetItem* currentItem, QTreeWidgetItem* previousItem)
{
	ui_.RemoveButton->setEnabled(currentItem);
}

/*
 * Update
 */

// IsotopologueTree parent (IsotopologueSet) item update function
void IsotopologueCollectionKeywordWidget::updateIsotopologueTreeRootItem(QTreeWidget* treeWidget, int topLevelItemIndex, IsotopologueSet* topeSet, bool createItem)
{
	QTreeWidgetItem* item;
	if (createItem)
	{
		item = new QTreeWidgetItem;
		item->setData(0, Qt::UserRole, VariantPointer<IsotopologueSet>(topeSet));
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		treeWidget->insertTopLevelItem(topLevelItemIndex, item);
	}
	else item = treeWidget->topLevelItem(topLevelItemIndex);

	// Set item data
	item->setText(0, topeSet->configuration()->name());

	// Update child (Isotopologues) items
	TreeWidgetUpdater<IsotopologueCollectionKeywordWidget,Isotopologues> treeUpdater(item, topeSet->isotopologues(), this, &IsotopologueCollectionKeywordWidget::updateIsotopologueTreeChildItem);
}

// IsotopologueTree child (Isotopologues) update function
void IsotopologueCollectionKeywordWidget::updateIsotopologueTreeChildItem(QTreeWidgetItem* parentItem, int childIndex, Isotopologues* topes, bool createItem)
{
	QTreeWidgetItem* item;
	if (createItem)
	{
		item = new QTreeWidgetItem;
		item->setData(0, Qt::UserRole, parentItem->data(0, Qt::UserRole));
		item->setData(1, Qt::UserRole, VariantPointer<Isotopologues>(topes));
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		parentItem->insertChild(childIndex, item);
	}
	else item = parentItem->child(childIndex);

	// Set item data
	item->setText(1, topes->species()->name());

	// Update sub-child (Isotopologue/weight) items
	TreeWidgetUpdater<IsotopologueCollectionKeywordWidget,IsotopologueWeight> subChildUpdater(item, topes->mix(), this, &IsotopologueCollectionKeywordWidget::updateIsotopologueTreeSubChildItem);
}

// IsotopologueTree sub-child (Isotopologue/double) update function
void IsotopologueCollectionKeywordWidget::updateIsotopologueTreeSubChildItem(QTreeWidgetItem* parentItem, int childIndex, IsotopologueWeight* isoWeight, bool createItem)
{
	QTreeWidgetItem* item;
	if (createItem)
	{
		item = new QTreeWidgetItem;
		item->setData(0, Qt::UserRole, parentItem->data(0, Qt::UserRole));
		item->setData(1, Qt::UserRole, VariantPointer<IsotopologueWeight>(isoWeight));
		item->setData(2, Qt::UserRole, VariantPointer<const Isotopologue>(isoWeight->isotopologue()));
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
		parentItem->insertChild(childIndex, item);
	}
	else item = parentItem->child(childIndex);
	item->setText(2, isoWeight->isotopologue()->name());
	item->setText(3, QString::number(isoWeight->weight()));
}

// Update value displayed in widget
void IsotopologueCollectionKeywordWidget::updateValue()
{
	updateWidgetValues(coreData_);
}

// Update widget values data based on keyword data
void IsotopologueCollectionKeywordWidget::updateWidgetValues(const CoreData& coreData)
{
	refreshing_ = true;

	// Update the root items of the tree with the allowed Configurations list
	TreeWidgetUpdater<IsotopologueCollectionKeywordWidget,IsotopologueSet> treeUpdater(ui_.IsotopologueTree, keyword_->data().isotopologueSets(), this, &IsotopologueCollectionKeywordWidget::updateIsotopologueTreeRootItem);

	for (int n=0; n<4; ++n) ui_.IsotopologueTree->resizeColumnToContents(n);
	ui_.IsotopologueTree->expandAll();

	ui_.RemoveButton->setEnabled(ui_.IsotopologueTree->currentItem());

	refreshing_ = false;
}

// Update keyword data based on widget values
void IsotopologueCollectionKeywordWidget::updateKeywordData()
{
	// Not used
}
