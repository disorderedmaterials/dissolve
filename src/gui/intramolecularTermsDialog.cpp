// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/species.h"
#include "gui.h"
#include "gui/delegates/exponentialSpin.h"
#include "gui/delegates/intraFormCombo.h"
#include "gui/intramolecularTermsDialog.h"
#include "main/dissolve.h"
#include <QWidget>

IntramolecularTermsDialog::IntramolecularTermsDialog(QWidget *parent, Dissolve &dissolve, Species *species,
                                                     DissolveWindow *dissolveWindow)
    : dissolve_(dissolve), bonds_(species->bonds(), dissolve.coreData()), angles_(species->angles(), dissolve.coreData()),
      torsions_(species->torsions(), dissolve.coreData()), impropers_(species->impropers(), dissolve.coreData())
{

    ui_.setupUi(this);
    setFont(parent->font());

    // -- Geometry tables
    ui_.BondTable->setItemDelegateForColumn(
        2, new IntraFormComboDelegate(this, new ComboEnumOptionsItems<BondFunctions::Form>(BondFunctions::forms()),
                                      dissolve_.coreData().masterBonds()));
    ui_.AngleTable->setItemDelegateForColumn(
        3, new IntraFormComboDelegate(this, new ComboEnumOptionsItems<AngleFunctions::Form>(AngleFunctions::forms()),
                                      dissolve_.coreData().masterAngles()));
    ui_.TorsionTable->setItemDelegateForColumn(
        4, new IntraFormComboDelegate(this, new ComboEnumOptionsItems<TorsionFunctions::Form>(TorsionFunctions::forms()),
                                      dissolve_.coreData().masterTorsions()));
    ui_.TorsionTable->setItemDelegateForColumn(6, new ExponentialSpinDelegate(this));
    ui_.TorsionTable->setItemDelegateForColumn(7, new ExponentialSpinDelegate(this));
    ui_.ImproperTable->setItemDelegateForColumn(
        4, new IntraFormComboDelegate(this, new ComboEnumOptionsItems<TorsionFunctions::Form>(TorsionFunctions::forms()),
                                      dissolve_.coreData().masterImpropers()));

    ui_.BondTable->setModel(&bonds_);
    ui_.BondTable->horizontalHeader()->setFont(parent->font());
    ui_.BondTable->horizontalHeader()->setVisible(true);
    ui_.BondTable->resizeColumnsToContents();
    ui_.AngleTable->setModel(&angles_);
    ui_.AngleTable->horizontalHeader()->setFont(parent->font());
    ui_.AngleTable->horizontalHeader()->setVisible(true);
    ui_.AngleTable->resizeColumnsToContents();
    ui_.TorsionTable->setModel(&torsions_);
    ui_.TorsionTable->horizontalHeader()->setFont(parent->font());
    ui_.TorsionTable->horizontalHeader()->setVisible(true);
    ui_.TorsionTable->resizeColumnsToContents();
    ui_.ImproperTable->setModel(&impropers_);
    ui_.ImproperTable->horizontalHeader()->setFont(parent->font());
    ui_.ImproperTable->horizontalHeader()->setVisible(true);
    ui_.ImproperTable->resizeColumnsToContents();

    connect(&bonds_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), dissolveWindow, SLOT(setModified()));
    connect(&angles_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), dissolveWindow, SLOT(setModified()));
    connect(&torsions_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), dissolveWindow, SLOT(setModified()));
    connect(&impropers_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), dissolveWindow, SLOT(setModified()));
}

// Finish
void IntramolecularTermsDialog::on_OKButton_clicked(bool checked) { accept(); }
