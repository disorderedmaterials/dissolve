// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "classes/atomtype.h"
#include "gui/gui.h"
#include "gui/helpers/tablewidgetupdater.h"
#include "gui/speciestab.h"
#include "main/dissolve.h"
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

// Atom table item changed
void SpeciesTab::atomTableDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    // Update the charge information?
    if (topLeft.column() == 5 || bottomRight.column() == 5)
        updateTotalCharges();

    // Atom type changed?
    if (topLeft.column() == 1 || bottomRight.column() == 1)
        species_->updateIsotopologues();

    dissolveWindow_->setModified();
}

// Update atom table selection
void SpeciesTab::updateAtomTableSelection()
{
    auto selection = QItemSelection();

    // Set atom selection in table to reflect the atom data
    for (auto n = 0; n < atoms_.rowCount(); ++n)
    {
        auto *i = atoms_.data(atoms_.index(n, 0), Qt::UserRole).value<SpeciesAtom *>();
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
            species_->selectAtom(i->index());
        else
            species_->deselectAtom(i->index());
    }

    // Recreate selection primitive and update viewer
    ui_.ViewerWidget->speciesViewer()->recreateSelectionPrimitive();
    ui_.ViewerWidget->updateStatusBar();
}

/*
 * Public Functions
 */

// Update total charges
void SpeciesTab::updateTotalCharges()
{
    auto errorPalette = palette();
    errorPalette.setBrush(QPalette::WindowText, Qt::red);

    auto atomCharge = species_->totalCharge(false);
    auto atomChargeOK = fabs(atomCharge) < 1.0e-5;
    ui_.TotalAtomChargeLabel->setText(atomCharge > 0.0 ? QString("+%1").arg(atomCharge) : QString::number(atomCharge));
    ui_.TotalAtomChargeLabel->setPalette(atomChargeOK ? palette() : errorPalette);
    ui_.TotalAtomChargeIndicator->setHidden(atomChargeOK);

    auto atomTypeCharge = species_->totalCharge(true);
    auto atomTypeChargeOK = fabs(atomTypeCharge) < 1.0e-5;
    ui_.TotalAtomTypeChargeLabel->setText(atomTypeCharge > 0.0 ? QString("+%1").arg(atomTypeCharge)
                                                               : QString::number(atomTypeCharge));
    ui_.TotalAtomTypeChargeLabel->setPalette(atomTypeChargeOK ? palette() : errorPalette);
    ui_.TotalAtomTypeChargeIndicator->setHidden(atomTypeChargeOK);
}

// Update Geometry tables
void SpeciesTab::updateGeometryTables()
{
    // If the Species version has changed, re-set the model data
    if (speciesVersion_ != species_->version())
    {
        bonds_.reset();
        angles_.reset();
        torsions_.reset();
        impropers_.reset();
    }
    ui_.BondTable->resizeColumnsToContents();
    ui_.AngleTable->resizeColumnsToContents();
    ui_.TorsionTable->resizeColumnsToContents();
    ui_.ImproperTable->resizeColumnsToContents();
}
