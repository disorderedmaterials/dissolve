// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/selectAtomTypeWidget.h"
#include "classes/atomType.h"
#include "classes/coreData.h"

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
void SelectAtomTypeWidget::setAtomTypes(const std::vector<std::shared_ptr<AtomType>> &atomTypes,
                                        OptionalReferenceWrapper<const CoreData> coreData)
{
    atomTypeModel_.setData(atomTypes, coreData);
}

// Reset widget
void SelectAtomTypeWidget::reset() { ui_.AtomTypeList->clearSelection(); }

/*
 * Update
 */
void SelectAtomTypeWidget::selectionChanged(const QItemSelection &, const QItemSelection &)
{
    Q_EMIT(atomTypeSelectionChanged(currentAtomType() != nullptr));
}

void SelectAtomTypeWidget::on_AtomTypeList_doubleClicked(const QModelIndex &index) { Q_EMIT(atomTypeDoubleClicked()); }

// Return the currently-selected AtomType
const std::shared_ptr<AtomType> SelectAtomTypeWidget::currentAtomType() const
{
    auto currentIndex = ui_.AtomTypeList->selectionModel()->currentIndex();
    if (!currentIndex.isValid())
        return nullptr;

    return atomTypeModel_.rawData(atomTypeFilterProxy_.mapToSource(currentIndex));
}
