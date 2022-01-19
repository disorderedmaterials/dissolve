// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "classes/atomtype.h"
#include "classes/coredata.h"
#include "gui/helpers/listwidgetupdater.h"
#include "gui/selectatomtypewidget.h"

SelectAtomTypeWidget::SelectAtomTypeWidget(QWidget *parent) : QWidget(parent)
{
    ui_.setupUi(this);

    atomTypeFilterProxy_.setSourceModel(&atomTypeModel_);
    ui_.AtomTypeList->setModel(&atomTypeFilterProxy_);

    connect(ui_.AtomTypeList->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this,
            SLOT(selectionChanged(const QItemSelection &, const QItemSelection &)));
}

/*
 * Data
 */

// Set filter element
void SelectAtomTypeWidget::setFilterElement(Elements::Element Z) { atomTypeFilterProxy_.setFilterElement(Z); }

// Clear filter element
void SelectAtomTypeWidget::clearFilterElement() { atomTypeFilterProxy_.clearFilterElement(); }

// Set target AtomType data
void SelectAtomTypeWidget::setAtomTypes(const std::vector<std::shared_ptr<AtomType>> &atomTypes)
{
    atomTypeModel_.setData(atomTypes);
}

// Reset widget
void SelectAtomTypeWidget::reset() { ui_.AtomTypeList->clearSelection(); }

/*
 * Update
 */
void SelectAtomTypeWidget::selectionChanged(const QItemSelection &, const QItemSelection &)
{
    emit(atomTypeSelectionChanged(currentAtomType() != nullptr));
}

void SelectAtomTypeWidget::on_AtomTypeList_doubleClicked(const QModelIndex &index) { emit(atomTypeDoubleClicked()); }

// Return the currently-selected AtomType
const std::shared_ptr<AtomType> SelectAtomTypeWidget::currentAtomType() const
{
    auto currentIndex = ui_.AtomTypeList->selectionModel()->currentIndex();
    if (!currentIndex.isValid())
        return nullptr;

    return atomTypeModel_.rawData(atomTypeFilterProxy_.mapToSource(currentIndex));
}
