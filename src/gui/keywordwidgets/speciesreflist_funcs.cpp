/*
    *** Keyword Widget - Species RefList
    *** src/gui/keywordwidgets/speciesreflist_funcs.cpp
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

#include "classes/coredata.h"
#include "classes/species.h"
#include "genericitems/listhelper.h"
#include "gui/helpers/listwidgetupdater.h"
#include "gui/keywordwidgets/speciesreflist.h"
#include <QComboBox>
#include <QHBoxLayout>
#include <QString>

SpeciesRefListKeywordWidget::SpeciesRefListKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData)
    : KeywordDropDown(this), KeywordWidgetBase(coreData)
{
    // Create and set up the UI for our widget in the drop-down's widget container
    ui_.setupUi(dropWidget());

    // Connect signals / slots
    connect(ui_.SelectionList, SIGNAL(itemChanged(QListWidgetItem *)), this, SLOT(itemChanged(QListWidgetItem *)));

    // Cast the pointer up into the parent class type
    keyword_ = dynamic_cast<SpeciesRefListKeyword *>(keyword);
    if (!keyword_)
        Messenger::error("Couldn't cast base keyword '{}' into SpeciesRefListKeyword.\n", keyword->name());
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
void SpeciesRefListKeywordWidget::updateSelectionRow(int row, Species *sp, bool createItem)
{
    // Grab the target reference list
    auto &selection = keyword_->data();

    QListWidgetItem *item;
    if (createItem)
    {
        item = new QListWidgetItem(QString::fromStdString(std::string(sp->name())));
        item->setData(Qt::UserRole, VariantPointer<Species>(sp));
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        ui_.SelectionList->insertItem(row, item);
    }
    else
        item = ui_.SelectionList->item(row);
    item->setCheckState(selection.contains(sp) ? Qt::Checked : Qt::Unchecked);
}

// List item changed
void SpeciesRefListKeywordWidget::itemChanged(QListWidgetItem *item)
{
    if (refreshing_)
        return;

    updateKeywordData();

    updateSummaryText();

    emit(keywordValueChanged(keyword_->optionMask()));
}

/*
 * Update
 */

// Update value displayed in widget
void SpeciesRefListKeywordWidget::updateValue() { updateWidgetValues(coreData_); }

// Update widget values data based on keyword data
void SpeciesRefListKeywordWidget::updateWidgetValues(const CoreData &coreData)
{
    refreshing_ = true;

    // Update the list against the global Species list
    ListWidgetUpdater<SpeciesRefListKeywordWidget, Species> listUpdater(ui_.SelectionList, coreData_.constSpecies(), this,
                                                                        &SpeciesRefListKeywordWidget::updateSelectionRow);

    updateSummaryText();

    refreshing_ = false;
}

// Update keyword data based on widget values
void SpeciesRefListKeywordWidget::updateKeywordData()
{
    // Loop over items in the QListWidget, adding the associated Speciess for any that are checked
    RefList<Species> newSelection;
    for (int n = 0; n < ui_.SelectionList->count(); ++n)
    {
        QListWidgetItem *item = ui_.SelectionList->item(n);
        if (item->checkState() == Qt::Checked)
            newSelection.append(VariantPointer<Species>(item->data(Qt::UserRole)));
    }
    keyword_->setData(newSelection);
}

// Update summary text
void SpeciesRefListKeywordWidget::updateSummaryText()
{
    // Create summary text for the KeywordDropDown button
    auto &selection = keyword_->data();
    if (selection.nItems() == 0)
        setSummaryText("<None>");
    else
    {
        std::string summaryText;
        for (Species *sp : selection)
            summaryText += fmt::format("{}{}", summaryText.empty() ? "" : ", ", sp->name());

        setSummaryText(QString::fromStdString(summaryText));
    }
}
