// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/atomtype.h"
#include "gui/gui.h"
#include "gui/helpers/tablewidgetupdater.h"
#include "gui/selectforcefielddialog.h"
#include "gui/speciestab.h"
#include "main/dissolve.h"
#include "templates/variantpointer.h"
#include <QMessageBox>

/*
 * Private Functions
 */

// Return valid AtomType names for specified model index in the SpeciesAtomTable
std::vector<std::string> SpeciesTab::validAtomTypeNames(const QModelIndex &index)
{
    // The QModelIndex should represent an AtomType for a SpeciesAtom, so the column should be 1
    assert(index.column() == 1);

    // The row of the QModelIndex represents the SpecieAtom index in the Species
    auto &i = species_->atom(index.row());

    // Construct valid names list
    std::vector<std::string> validNames;
    for (auto &at : dissolve_.atomTypes())
        if (at->Z() == i.Z())
            validNames.emplace_back(at->name());

    return validNames;
}

/*
 * Private Slots
 */

// Update atom table selection
void SpeciesTab::updateAtomTableSelection()
{
    SpeciesAtom *i;

    auto selection = QItemSelection();
    // Set atom selection in table to reflect the atom data
    for (auto n = 0; n < atoms_.rowCount(); ++n)
    {
        i = atoms_.data(atoms_.index(n, 0), Qt::UserRole).value<SpeciesAtom *>();
        if (i->isSelected())
            selection.select(atoms_.index(n, 0), atoms_.index(n, 5));
    }
    ui_.AtomTable->selectionModel()->select(selection, QItemSelectionModel::ClearAndSelect);
}

void SpeciesTab::updateUnderlyingAtomSelection()
{
    if (refreshLock_.isLocked())
        return;

    SpeciesAtom *i;

    // Set atom selection in viewer to be same as the table
    for (auto n = 0; n < atoms_.rowCount(); ++n)
    {
        i = atoms_.data(atoms_.index(n, 0), Qt::UserRole).value<SpeciesAtom *>();
        if (ui_.AtomTable->selectionModel()->isSelected(atoms_.index(n, 0)))
            species_->selectAtom(i);
        else
            species_->deselectAtom(i);
    }

    // Recreate selection primitive and update viewer
    ui_.ViewerWidget->speciesViewer()->recreateSelectionPrimitive();
    ui_.ViewerWidget->updateStatusBar();
}

/*
 * Public Functions
 */

// Update Geometry tab
// void SpeciesTab::updateGeometryTab()
// {
//     Locker refreshLocker(refreshLock_);

//     if (!species_)
//         ui_.AtomTable->clearContents();
//     else
//         TableWidgetUpdater<SpeciesTab, SpeciesAtom> speciesAtomUpdater(ui_.AtomTable, species_->atoms(), this,
//                                                                        &SpeciesTab::updateAtomTableRow);

//     // -- Intramolecular terms
//     if (!species_)
//     {
//         ui_.BondTable->clearContents();
//         ui_.AngleTable->clearContents();
//         ui_.TorsionTable->clearContents();
//         ui_.ImproperTable->clearContents();
//     }
//     else
//     {
//         TableWidgetUpdater<SpeciesTab, SpeciesBond> bondUpdater(ui_.BondTable, species_->bonds(), this,
//                                                                 &SpeciesTab::updateBondTableRow);
//         TableWidgetUpdater<SpeciesTab, SpeciesAngle> angleUpdater(ui_.AngleTable, species_->angles(), this,
//                                                                   &SpeciesTab::updateAngleTableRow);
//         TableWidgetUpdater<SpeciesTab, SpeciesTorsion> torsionUpdater(ui_.TorsionTable, species_->torsions(), this,
//                                                                       &SpeciesTab::updateTorsionTableRow);
//         TableWidgetUpdater<SpeciesTab, SpeciesImproper> improperUpdater(ui_.ImproperTable, species_->impropers(), this,
//                                                                         &SpeciesTab::updateImproperTableRow);
//     }
//     ui_.BondTable->resizeColumnsToContents();
//     ui_.AngleTable->resizeColumnsToContents();
//     ui_.TorsionTable->resizeColumnsToContents();
//     ui_.ImproperTable->resizeColumnsToContents();
// }
