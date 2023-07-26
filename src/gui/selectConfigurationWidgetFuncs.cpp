// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/configuration.h"
#include "classes/coreData.h"
#include "gui/helpers/listWidgetUpdater.h"
#include "gui/selectConfigurationWidget.h"
#include "templates/variantPointer.h"

SelectConfigurationWidget::SelectConfigurationWidget(QWidget *parent) : QWidget(parent)
{
    ui_.setupUi(this);

    connect(ui_.ConfigurationList->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(selectionChanged(const QItemSelection &, const QItemSelection &)));

    coreData_ = nullptr;
    minimumSelectionSize_ = 1;
    maximumSelectionSize_ = 1;

    refreshing_ = false;
}

SelectConfigurationWidget::~SelectConfigurationWidget() {}

/*
 * Data
 */

// Set target Configuration data
void SelectConfigurationWidget::setConfiguration(const std::vector<std::unique_ptr<Configuration>> &configuration)
{
    configurationModel_.setData(configuration);
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
                                                                               coreData_->configurations());
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
    for (auto n = 0; n < ui_.ConfigurationList->count(); ++n)
    {
        QListWidgetItem *item = ui_.ConfigurationList->item(n);

        if (item->isSelected())
            ++count;
    }

    return count;
}

// Return the currently-selected Configuration
std::vector<Configuration *> SelectConfigurationWidget::currentConfiguration() const
{
    std::vector<Configuration *> selection;

    // Loop over items in the list and construct the selection
    for (auto n = 0; n < ui_.ConfigurationList->count(); ++n)
    {
        QListWidgetItem *item = ui_.ConfigurationList->item(n);

        if (item->isSelected())
            selection.push_back(VariantPointer<Configuration>(item->data(Qt::UserRole)));
    }

    return selection;
}
