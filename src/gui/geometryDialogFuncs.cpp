// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "geometryDialog.h"
#include "gui/delegates/intraFormCombo.h"
#include "gui/delegates/exponentialSpin.hui"
#include "main/dissolve.h"
#include "classes/species.h"
#include <QWidget>

GeometryDialog::GeometryDialog(QWidget *parent, Dissolve &dissolve, Species *species) : dissolve_(dissolve), bonds_(species->bonds(), dissolve.coreData()), torsions_(species->torsions(), dissolve.coreData()), impropers_(species->impropers(), dissolve.coreData()), angles_(species->angles(), dissolve.coreData())
{

    ui_.setupUi(this);

    ui_.BondTable->setModel(&bonds_);
    ui_.BondTable->horizontalHeader()->setFont(parent->font());
    ui_.BondTable->horizontalHeader()->setVisible(true);
    ui_.AngleTable->setModel(&angles_);
    ui_.AngleTable->horizontalHeader()->setFont(parent->font());
    ui_.AngleTable->horizontalHeader()->setVisible(true);
    ui_.TorsionTable->setModel(&torsions_);
    ui_.TorsionTable->horizontalHeader()->setFont(parent->font());
    ui_.TorsionTable->horizontalHeader()->setVisible(true);
    ui_.ImproperTable->setModel(&impropers_);
    ui_.ImproperTable->horizontalHeader()->setFont(parent->font());
    ui_.ImproperTable->horizontalHeader()->setVisible(true);

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

    connect(&bonds_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(setModified()));
    connect(&angles_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(setModified()));
    connect(&torsions_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(setModified()));
    connect(&impropers_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(setModified()));


}

void GeometryDialog::setModified() { modified_ = true; }
bool GeometryDialog::modified() { return modified_; }

void GeometryDialog::on_OKButton_clicked(bool checked) { accept(); }
