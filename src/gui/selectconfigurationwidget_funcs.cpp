/*
    *** Select Configuration Widget
    *** src/gui/selectconfigurationwidget_funcs.cpp
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
#include "gui/helpers/listwidgetupdater.h"
#include "gui/selectconfigurationwidget.h"
#include "templates/variantpointer.h"

SelectConfigurationWidget::SelectConfigurationWidget(QWidget *parent) : QWidget(parent)
{
    ui_.setupUi(this);

    coreData_ = nullptr;
    minimumSelectionSize_ = 1;
    maximumSelectionSize_ = 1;

    refreshing_ = false;
}

SelectConfigurationWidget::~SelectConfigurationWidget() {}

/*
 * Data
 */

// Set CoreData containing available Configuration
void SelectConfigurationWidget::setCoreData(const CoreData *coreData)
{
    coreData_ = coreData;

    updateConfigurationList();
}

// Reset widget, applying specified min and max limits to selection
void SelectConfigurationWidget::reset(int minSize, int maxSize)
{
    minimumSelectionSize_ = minSize;
    maximumSelectionSize_ = maxSize;

    ui_.ConfigurationList->clearSelection();

    // Set the correct selection behaviour and enable/disable select all / none buttons as appropriate
    if (maxSize == 1)
        ui_.ConfigurationList->setSelectionMode(QAbstractItemView::SingleSelection);
    else
        ui_.ConfigurationList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui_.SelectionControls->setVisible(maxSize != 1);
}

/*
 * Update
 */

// Update the list of Configuration
void SelectConfigurationWidget::updateConfigurationList()
{
    if (!coreData_)
    {
        ui_.ConfigurationList->clear();
        return;
    }

    ListWidgetUpdater<SelectConfigurationWidget, Configuration> speciesUpdater(ui_.ConfigurationList,
                                                                               coreData_->constConfigurations());
}

void SelectConfigurationWidget::on_SelectNoneButton_clicked(bool checked) { ui_.ConfigurationList->clearSelection(); }

void SelectConfigurationWidget::on_SelectAllButton_clicked(bool checked) { ui_.ConfigurationList->selectAll(); }

void SelectConfigurationWidget::on_ConfigurationList_itemSelectionChanged()
{
    emit(speciesSelectionChanged(isSelectionValid()));
}

void SelectConfigurationWidget::on_ConfigurationList_itemDoubleClicked(QListWidgetItem *item)
{
    if (!item)
        return;

    emit(speciesDoubleClicked());
}

// Return whether number of selected items is valid
bool SelectConfigurationWidget::isSelectionValid() const
{
    auto count = nSelected();

    if (count < minimumSelectionSize_)
        return false;
    else if (maximumSelectionSize_ == -1)
        return true;
    else
        return (count <= maximumSelectionSize_);
}

// Return number of species currently selected
int SelectConfigurationWidget::nSelected() const
{
    auto count = 0;
    for (int n = 0; n < ui_.ConfigurationList->count(); ++n)
    {
        QListWidgetItem *item = ui_.ConfigurationList->item(n);

        if (item->isSelected())
            ++count;
    }

    return count;
}

// Return the currently-selected Configuration
RefList<Configuration> SelectConfigurationWidget::currentConfiguration() const
{
    RefList<Configuration> selection;

    // Loop over items in the list and construct the selection RefList
    for (int n = 0; n < ui_.ConfigurationList->count(); ++n)
    {
        QListWidgetItem *item = ui_.ConfigurationList->item(n);

        if (item->isSelected())
            selection.append(VariantPointer<Configuration>(item->data(Qt::UserRole)));
    }

    return selection;
}
