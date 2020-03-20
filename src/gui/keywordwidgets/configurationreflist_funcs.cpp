/*
    *** Keyword Widget - Configuration RefList
    *** src/gui/keywordwidgets/configurationreflist_funcs.cpp
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

#include "classes/configuration.h"
#include "classes/coredata.h"
#include "genericitems/listhelper.h"
#include "gui/helpers/listwidgetupdater.h"
#include "gui/keywordwidgets/configurationreflist.h"
#include "module/module.h"
#include <QComboBox>
#include <QHBoxLayout>
#include <QString>

// Constructor
ConfigurationRefListKeywordWidget::ConfigurationRefListKeywordWidget(QWidget *parent, KeywordBase *keyword,
                                                                     const CoreData &coreData)
    : KeywordDropDown(this), KeywordWidgetBase(coreData)
{
    // Create and set up the UI for our widget in the drop-down's widget container
    ui_.setupUi(dropWidget());

    // Connect signals / slots
    connect(ui_.SelectionList, SIGNAL(itemChanged(QListWidgetItem *)), this, SLOT(itemChanged(QListWidgetItem *)));

    // Cast the pointer up into the parent class type
    keyword_ = dynamic_cast<ConfigurationRefListKeyword *>(keyword);
    if (!keyword_)
        Messenger::error("Couldn't cast base keyword '%s' into ConfigurationRefListKeyword.\n", keyword->name());
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
void ConfigurationRefListKeywordWidget::updateSelectionRow(int row, Configuration *sp, bool createItem)
{
    // Grab the target reference list
    RefList<Configuration> &selection = keyword_->data();

    QListWidgetItem *item;
    if (createItem)
    {
        item = new QListWidgetItem(QString::fromStdString(std::string(sp->name())));
        item->setData(Qt::UserRole, VariantPointer<Configuration>(sp));
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        ui_.SelectionList->insertItem(row, item);
    }
    else
        item = ui_.SelectionList->item(row);
    item->setCheckState(selection.contains(sp) ? Qt::Checked : Qt::Unchecked);
}

// List item changed
void ConfigurationRefListKeywordWidget::itemChanged(QListWidgetItem *item)
{
    if (refreshing_)
        return;

    refreshing_ = true;

    // Ensure that we obey any limit on maximum number of selected items
    auto nChecked = 0;
    for (int n = 0; n < ui_.SelectionList->count(); ++n)
        if (ui_.SelectionList->item(n)->checkState() == Qt::Checked)
            ++nChecked;
    switch (keyword_->maxListSize())
    {
        case (Module::ZeroTargets):
            if (nChecked != 0)
                for (int n = 0; n < ui_.SelectionList->count(); ++n)
                    ui_.SelectionList->item(n)->setCheckState(Qt::Unchecked);
            break;
        case (Module::ExactlyOneTarget):
            if (nChecked > 1)
            {
                for (int n = 0; n < ui_.SelectionList->count(); ++n)
                    if ((ui_.SelectionList->item(n)->checkState() == Qt::Checked) && (ui_.SelectionList->item(n) != item))
                        ui_.SelectionList->item(n)->setCheckState(Qt::Unchecked);
            }
            break;
        case (Module::OneOrMoreTargets):
            // Nothing to do
            break;
        default:
            if (nChecked > keyword_->maxListSize())
            {
                for (int n = 0; n < ui_.SelectionList->count(); ++n)
                {
                    if ((ui_.SelectionList->item(n)->checkState() == Qt::Checked) && (ui_.SelectionList->item(n) != item))
                    {
                        ui_.SelectionList->item(n)->setCheckState(Qt::Unchecked);
                        --nChecked;
                        if (nChecked == keyword_->maxListSize())
                            break;
                    }
                }
            }
            break;
    }

    updateKeywordData();

    updateSummaryText();

    emit(keywordValueChanged(keyword_->optionMask()));

    refreshing_ = false;
}

/*
 * Update
 */

// Update value displayed in widget
void ConfigurationRefListKeywordWidget::updateValue() { updateWidgetValues(coreData_); }

// Update widget values data based on keyword data
void ConfigurationRefListKeywordWidget::updateWidgetValues(const CoreData &coreData)
{
    refreshing_ = true;

    // Update the list against the global Configuration list
    ListWidgetUpdater<ConfigurationRefListKeywordWidget, Configuration> listUpdater(
        ui_.SelectionList, coreData_.constConfigurations(), this, &ConfigurationRefListKeywordWidget::updateSelectionRow);

    updateSummaryText();

    refreshing_ = false;
}

// Update keyword data based on widget values
void ConfigurationRefListKeywordWidget::updateKeywordData()
{
    // Loop over items in the QListWidget, adding the associated Configurations for any that are checked
    RefList<Configuration> newSelection;
    for (int n = 0; n < ui_.SelectionList->count(); ++n)
    {
        QListWidgetItem *item = ui_.SelectionList->item(n);
        if (item->checkState() == Qt::Checked)
            newSelection.append(VariantPointer<Configuration>(item->data(Qt::UserRole)));
    }
    keyword_->setData(newSelection);
}

// Update summary text
void ConfigurationRefListKeywordWidget::updateSummaryText()
{
    // Create summary text for the KeywordDropDown button
    RefList<Configuration> &selection = keyword_->data();
    if (selection.nItems() == 0)
        setSummaryText("<None>");
    else
    {
        QString summaryText;
        for (Configuration *sp : selection)
            summaryText +=
                QString("%1%2").arg(summaryText.isEmpty() ? "" : ", ").arg(QString::fromStdString(std::string(sp->name())));

        setSummaryText(summaryText);
    }
}
