// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/lock.h"
#include "gui/models/cifAssemblyModel.h"
#include "gui/ui_importCIFDialog.h"
#include "gui/wizard.h"
#include "io/import/cif.h"
#include "main/dissolve.h"
#include <QDialog>

// Forward Declarations
class Dissolve;
class Species;

// Import CIF Dialog
class ImportCIFDialog : public QDialog
{
    Q_OBJECT

    public:
    ImportCIFDialog(QWidget *parent, Dissolve &dissolve);
    ~ImportCIFDialog() = default;

    private:
    // Main form declaration
    Ui::ImportCIFDialog ui_;
    // Model for CIF assemblies
    CIFAssemblyModel cifAssemblyModel_;
    // Temporary core data
    CoreData temporaryCoreData_;

    /*
     * Data
     */
    private:
    // Main Dissolve object
    Dissolve &dissolve_;
    // CIF Handler
    CIFHandler cifHandler_;
    // NETA for moiety removal
    NETADefinition moietyNETA_;

    private:
    // Create / check NETA definition for moiety removal
    bool createMoietyRemovalNETA(std::string definition);

    /*
     * UI
     */
    private:
    // Widget update lock
    Lock widgetsUpdating_;
    // Update all controls
    void updateWidgets();
    // Update density label
    void updateDensityLabel();

    private Q_SLOTS:
    // CIF File / Information
    void on_InputFileEdit_editingFinished();
    void on_InputFileSelectButton_clicked(bool checked);
    void on_SpaceGroupsCombo_currentIndexChanged(int index);
    // Overlap
    void on_NormalOverlapToleranceRadio_clicked(bool checked);
    void on_LooseOverlapToleranceRadio_clicked(bool checked);
    // Bonding
    void on_CalculateBondingRadio_clicked(bool checked);
    void on_BondingPreventMetallicCheck_clicked(bool checked);
    void on_BondFromCIFRadio_clicked(bool checked);
    // CleanUp
    void on_MoietyRemoveAtomicsCheck_clicked(bool checked);
    void on_MoietyRemoveWaterCheck_clicked(bool checked);
    void on_MoietyRemoveByNETACheck_clicked(bool checked);
    void on_MoietyNETARemovalEdit_textEdited(const QString &text);
    void on_MoietyNETARemoveFragmentsCheck_clicked(bool checked);
    // Supercell
    void on_RepeatASpin_valueChanged(int value);
    void on_RepeatBSpin_valueChanged(int value);
    void on_RepeatCSpin_valueChanged(int value);
    // Preview
    void on_DensityUnitsCombo_currentIndexChanged(int index);
    // Output
    void on_OutputMolecularRadio_clicked(bool checked);
    void on_OutputFrameworkRadio_clicked(bool checked);
    void on_OutputSupermoleculeRadio_clicked(bool checked);
    // Dialog
    void on_OKButton_clicked(bool checked);
    void on_CancelButton_clicked(bool checked);
};
