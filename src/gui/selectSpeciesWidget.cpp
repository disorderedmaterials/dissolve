// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/selectSpeciesWidget.h"
#include "classes/coreData.h"
#include "classes/species.h"

SelectSpeciesWidget::SelectSpeciesWidget(QWidget *parent) : QWidget(parent)
{
    ui_.setupUi(this);

    speciesFilterProxy_.setSourceModel(&speciesModel_);
    ui_.SpeciesList->setModel(&speciesFilterProxy_);

    connect(ui_.SpeciesList->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this,
            SLOT(selectionChanged(const QItemSelection &, const QItemSelection &)));

    reset(1, 1);

    refreshing_ = false;
}

/*
 * Data
 */

// Set flags for the filter proxy
void SelectSpeciesWidget::setFilterProxyFlags(const Flags<SpeciesFilterProxy::FilterFlags> &flags)
{
    speciesFilterProxy_.setFlags(flags);
}

// Set target Species data
void SelectSpeciesWidget::setSpecies(const std::vector<std::unique_ptr<Species>> &species) { speciesModel_.setData(species); }

// Reset widget, applying specified min and max limits to selection
void SelectSpeciesWidget::reset(int minSize, std::optional<int> maxSize)
{
    minimumSelectionSize_ = minSize;
    maximumSelectionSize_ = maxSize;

    ui_.SpeciesList->clearSelection();

    // Set the correct selection behaviour and enable/disable select all / none buttons as appropriate
    ui_.SpeciesList->setSelectionMode(!maximumSelectionSize_.has_value() || maximumSelectionSize_.value() > 1
                                          ? QAbstractItemView::ExtendedSelection
                                          : QAbstractItemView::SingleSelection);
    ui_.SelectionControls->setVisible(maximumSelectionSize_.has_value() && maximumSelectionSize_.value() > 1);
}

/*
 * Update
 */
void SelectSpeciesWidget::selectionChanged(const QItemSelection &, const QItemSelection &)
{
    Q_EMIT(speciesSelectionChanged(isSelectionValid()));
}

void SelectSpeciesWidget::on_SelectNoneButton_clicked(bool checked) { ui_.SpeciesList->clearSelection(); }

void SelectSpeciesWidget::on_SelectAllButton_clicked(bool checked) { ui_.SpeciesList->selectAll(); }

void SelectSpeciesWidget::on_SpeciesList_doubleClicked(const QModelIndex &index) { Q_EMIT(speciesDoubleClicked()); }

// Return whether number of selected items is valid
bool SelectSpeciesWidget::isSelectionValid() const
{
    auto count = nSelected();

    if (count < minimumSelectionSize_)
        return false;

    return !maximumSelectionSize_.has_value() || count <= maximumSelectionSize_.value();
}

// Return number of species currently selected
int SelectSpeciesWidget::nSelected() const { return ui_.SpeciesList->selectionModel()->selectedIndexes().size(); }

// Return the currently-selected Species
std::vector<const Species *> SelectSpeciesWidget::selection() const
{
    std::vector<const Species *> selection;

    for (auto &index : ui_.SpeciesList->selectionModel()->selectedIndexes())
        selection.push_back(speciesModel_.data(speciesFilterProxy_.mapToSource(index), Qt::UserRole).value<const Species *>());

    return selection;
}
