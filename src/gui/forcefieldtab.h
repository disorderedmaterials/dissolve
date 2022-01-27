// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "classes/atomtype.h"
#include "classes/pairpotential.h"
#include "gui/maintab.h"
#include "gui/models/atomTypeModel.h"
#include "gui/models/masterTermModel.h"
#include "gui/models/pairPotentialModel.h"
#include "gui/ui_forcefieldtab.h"

// Forcefield Tab
class ForcefieldTab : public QWidget, public MainTab
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    ForcefieldTab(DissolveWindow *dissolveWindow, Dissolve &dissolve, MainTabsWidget *parent, QString title);
    ~ForcefieldTab() override = default;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::ForcefieldTab ui_;
    // Models
    AtomTypeModel atoms_;
    PairPotentialModel pairs_;

    /*
     * MainTab Reimplementations
     */
    public:
    // Return tab type
    MainTab::TabType type() const override;
    // Return whether the tab can be closed
    bool canClose() const override;

    /*
     * Update
     */
    private:
    // Table models
    MasterTermBondModel masterBondsTableModel_;
    MasterTermAngleModel masterAnglesTableModel_;
    MasterTermTorsionModel masterTorsionsTableModel_;
    MasterTermImproperModel masterImpropersTableModel_;

    private:
    // Update all pair potentials
    void updatePairPotentials();

    protected:
    // Update controls in tab
    void updateControls() override;
    // Disable sensitive controls within tab
    void disableSensitiveControls() override;
    // Enable sensitive controls within tab
    void enableSensitiveControls() override;

    /*
     * Signals / Slots
     */
    private:
    // Signal that some AtomType parameter has been modified, so pair potentials should be regenerated
    void atomTypeDataModified();

    private slots:
    // Atom Types
    void on_AtomTypeAddButton_clicked(bool checked);
    void on_AtomTypeRemoveButton_clicked(bool checked);
    // Pair Potentials
    void on_PairPotentialRangeSpin_valueChanged(double value);
    void on_PairPotentialDeltaSpin_valueChanged(double value);
    void on_PairPotentialsAtomTypeChargesRadio_clicked(bool checked);
    void on_PairPotentialsSpeciesAtomChargesRadio_clicked(bool checked);
    void on_ShortRangeTruncationCombo_currentIndexChanged(int index);
    void on_CoulombTruncationCombo_currentIndexChanged(int index);
    void on_AutomaticChargeSourceCheck_clicked(bool checked);
    void on_ForceChargeSourceCheck_clicked(bool checked);
    void on_RegenerateAllPairPotentialsButton_clicked(bool checked);
    void on_AutoUpdatePairPotentialsCheck_clicked(bool checked);
    void pairPotentialTableRowChanged(const QModelIndex &current, const QModelIndex &previous);
    // Master Terms
    void masterBondsDataChanged(const QModelIndex &, const QModelIndex &);
    void masterAnglesDataChanged(const QModelIndex &, const QModelIndex &);
    void masterTorsionsDataChanged(const QModelIndex &, const QModelIndex &);
    void masterImpropersDataChanged(const QModelIndex &, const QModelIndex &);
    void on_MasterTermAddBondButton_clicked(bool checked);
    void on_MasterTermRemoveBondButton_clicked(bool checked);
    void on_MasterTermAddAngleButton_clicked(bool checked);
    void on_MasterTermRemoveAngleButton_clicked(bool checked);
    void on_MasterTermAddTorsionButton_clicked(bool checked);
    void on_MasterTermRemoveTorsionButton_clicked(bool checked);
    void on_MasterTermAddImproperButton_clicked(bool checked);
    void on_MasterTermRemoveImproperButton_clicked(bool checked);
};
