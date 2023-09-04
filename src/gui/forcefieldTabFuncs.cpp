// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/atomType.h"
#include "classes/speciesAngle.h"
#include "classes/speciesBond.h"
#include "classes/speciesImproper.h"
#include "classes/speciesTorsion.h"
#include "gui/delegates/comboList.hui"
#include "gui/delegates/exponentialSpin.hui"
#include "gui/forcefieldTab.h"
#include "gui/gui.h"
#include "gui/helpers/comboPopulator.h"
#include "gui/render/renderableData1D.h"
#include "gui/widgets/elementSelector.hui"
#include "main/dissolve.h"
#include <QListWidgetItem>

ForcefieldTab::ForcefieldTab(DissolveWindow *dissolveWindow, Dissolve &dissolve, MainTabsWidget *parent, const QString title)
    : MainTab(dissolveWindow, dissolve, parent, title, this), atomTypesModel_(dissolve.coreData()),
      pairPotentialModel_(dissolve.pairPotentials())
{
    ui_.setupUi(this);

    Locker refreshLocker(refreshLock_);

    /*
     * Master Terms
     */

    // Set item delegates for tables
    // -- Functional Forms
    ui_.MasterBondsTable->setItemDelegateForColumn(
        1, new ComboListDelegate(this, new ComboEnumOptionsItems<BondFunctions::Form>(BondFunctions::forms())));
    ui_.MasterAnglesTable->setItemDelegateForColumn(
        1, new ComboListDelegate(this, new ComboEnumOptionsItems<AngleFunctions::Form>(AngleFunctions::forms())));
    ui_.MasterTorsionsTable->setItemDelegateForColumn(
        1, new ComboListDelegate(this, new ComboEnumOptionsItems<TorsionFunctions::Form>(TorsionFunctions::forms())));
    ui_.MasterImpropersTable->setItemDelegateForColumn(
        1, new ComboListDelegate(this, new ComboEnumOptionsItems<TorsionFunctions::Form>(TorsionFunctions::forms())));

    // Set models for tables
    masterBondsTableModel_.setSourceData(dissolve_.coreData().masterBonds());
    ui_.MasterBondsTable->setModel(&masterBondsTableModel_);
    masterAnglesTableModel_.setSourceData(dissolve_.coreData().masterAngles());
    ui_.MasterAnglesTable->setModel(&masterAnglesTableModel_);
    masterTorsionsTableModel_.setSourceData(dissolve_.coreData().masterTorsions());
    ui_.MasterTorsionsTable->setModel(&masterTorsionsTableModel_);
    masterImpropersTableModel_.setSourceData(dissolve_.coreData().masterImpropers());
    ui_.MasterImpropersTable->setModel(&masterImpropersTableModel_);

    // Ensure fonts for table headers are set correctly and the headers themselves are visible
    ui_.MasterBondsTable->horizontalHeader()->setFont(font());
    ui_.MasterBondsTable->horizontalHeader()->setVisible(true);
    ui_.MasterAnglesTable->horizontalHeader()->setFont(font());
    ui_.MasterAnglesTable->horizontalHeader()->setVisible(true);
    ui_.MasterTorsionsTable->horizontalHeader()->setFont(font());
    ui_.MasterTorsionsTable->horizontalHeader()->setVisible(true);
    ui_.MasterImpropersTable->horizontalHeader()->setFont(font());
    ui_.MasterImpropersTable->horizontalHeader()->setVisible(true);

    connect(&masterBondsTableModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), this,
            SLOT(masterBondsDataChanged(const QModelIndex &, const QModelIndex &)));
    connect(&masterAnglesTableModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), this,
            SLOT(masterAnglesDataChanged(const QModelIndex &, const QModelIndex &)));
    connect(&masterTorsionsTableModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), this,
            SLOT(masterTorsionsDataChanged(const QModelIndex &, const QModelIndex &)));
    connect(&masterImpropersTableModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), this,
            SLOT(masterImpropersDataChanged(const QModelIndex &, const QModelIndex &)));

    /*
     * Atom Types
     */

    // Set item delegates for tables
    // -- Short Range Functional Forms
    ui_.AtomTypesTable->setItemDelegateForColumn(
        3, new ComboListDelegate(this, new ComboEnumOptionsItems<ShortRangeFunctions::Form>(ShortRangeFunctions::forms())));

    // Ensure fonts for table headers are set correctly and the headers themselves are visible
    ui_.AtomTypesTable->horizontalHeader()->setFont(font());
    ui_.AtomTypesTable->horizontalHeader()->setVisible(true);
    ui_.AtomTypesTable->setModel(&atomTypesModel_);
    connect(&atomTypesModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)), this,
            SLOT(atomTypeDataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)));
    connect(ui_.AtomTypesTable->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(atomTypeSelectionChanged(const QItemSelection &, const QItemSelection &)));
    /*
     * Pair Potentials
     */

    // Set up combo delegates
    ComboEnumOptionsPopulator coulPopulator(ui_.CoulombTruncationCombo, PairPotential::coulombTruncationSchemes());
    ComboEnumOptionsPopulator shortPopulator(ui_.ShortRangeTruncationCombo, PairPotential::shortRangeTruncationSchemes());

    // Set sensible lower limits and steps for range and delta
    ui_.PairPotentialRangeSpin->setRange(1.0, 1.0e5);
    ui_.PairPotentialRangeSpin->setSingleStep(1.0);
    ui_.PairPotentialDeltaSpin->setRange(0.001, 1.0);
    ui_.PairPotentialDeltaSpin->setSingleStep(0.001);

    // Ensure fonts for table headers are set correctly and the headers themselves are visible
    ui_.PairPotentialsTable->horizontalHeader()->setFont(font());
    ui_.PairPotentialsTable->horizontalHeader()->setVisible(true);
    ui_.PairPotentialsTable->setModel(&pairPotentialModel_);

    DataViewer *viewer = ui_.PairPotentialsPlotWidget->dataViewer();
    viewer->view().axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
    viewer->view().axes().setTitle(1, "U, kj/mol");
    viewer->view().axes().setRange(1, -100.0, 100.0);

    connect(&pairPotentialModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)), this,
            SLOT(pairPotentialDataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)));
    connect(ui_.PairPotentialsTable->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(pairPotentialSelectionChanged(const QItemSelection &, const QItemSelection &)));
}

/*
 * MainTab Reimplementations
 */

// Return tab type
MainTab::TabType ForcefieldTab::type() const { return MainTab::TabType::Forcefield; }

// Return whether the tab can be closed
bool ForcefieldTab::canClose() const { return false; }

/*
 * Update
 */

// Update all pair potentials
void ForcefieldTab::updatePairPotentials()
{
    // Mute output
    Messenger::mute();

    dissolve_.regeneratePairPotentials();

    resetPairPotentialModel();

    // Reinstate output
    Messenger::unMute();
}

// Reset pair potential model
void ForcefieldTab::resetPairPotentialModel()
{
    auto selectedIndices = ui_.PairPotentialsTable->selectionModel()->selection().indexes();

    pairPotentialModel_.reset();
    ui_.PairPotentialsTable->resizeColumnsToContents();

    for (auto &index : selectedIndices)
        ui_.PairPotentialsTable->selectionModel()->select(index, QItemSelectionModel::Select);
}

// Update controls in tab
void ForcefieldTab::updateControls()
{
    Locker refreshLocker(refreshLock_);

    masterBondsTableModel_.setSourceData(dissolve_.coreData().masterBonds());
    masterAnglesTableModel_.setSourceData(dissolve_.coreData().masterAngles());
    masterTorsionsTableModel_.setSourceData(dissolve_.coreData().masterTorsions());
    masterImpropersTableModel_.setSourceData(dissolve_.coreData().masterImpropers());
    ui_.MasterBondsTable->resizeColumnsToContents();
    ui_.MasterAnglesTable->resizeColumnsToContents();
    ui_.MasterTorsionsTable->resizeColumnsToContents();
    ui_.MasterImpropersTable->resizeColumnsToContents();

    // AtomTypes Table
    atomTypesModel_.setData(dissolve_.coreData().atomTypes());
    ui_.AtomTypesTable->resizeColumnsToContents();

    // PairPotentials
    ui_.PairPotentialRangeSpin->setValue(dissolve_.pairPotentialRange());
    ui_.PairPotentialDeltaSpin->setValue(dissolve_.pairPotentialDelta());
    ui_.AutomaticChargeSourceCheck->setChecked(dissolve_.automaticChargeSource());
    ui_.PairPotentialsAtomTypeChargesRadio->setDisabled(dissolve_.automaticChargeSource());
    ui_.PairPotentialsSpeciesAtomChargesRadio->setDisabled(dissolve_.automaticChargeSource());
    ui_.ForceChargeSourceCheck->setChecked(dissolve_.forceChargeSource());
    ui_.ForceChargeSourceCheck->setDisabled(dissolve_.automaticChargeSource());
    if (dissolve_.atomTypeChargeSource())
        ui_.PairPotentialsAtomTypeChargesRadio->setChecked(true);
    else
        ui_.PairPotentialsSpeciesAtomChargesRadio->setChecked(true);
    ui_.ShortRangeTruncationCombo->setCurrentIndex(PairPotential::shortRangeTruncationScheme());
    ui_.ShortRangeTruncationWidthSpin->setValue(PairPotential::shortRangeTruncationWidth());
    ui_.ShortRangeTruncationWidthSpin->setEnabled(PairPotential::shortRangeTruncationScheme() ==
                                                  PairPotential::CosineShortRangeTruncation);
    ui_.CoulombTruncationCombo->setCurrentIndex(PairPotential::coulombTruncationScheme());

    refreshLocker.unlock();
}

// Prevent editing within tab
void ForcefieldTab::preventEditing() { setEnabled(false); }

// Allow editing within tab
void ForcefieldTab::allowEditing() { setEnabled(true); }

// Set the current tab
void ForcefieldTab::setTab(int index) { ui_.Tabs->setCurrentIndex(index); }

/*
 * Signals / Slots
 */

// Signal that some AtomType parameter has been modified, so pair potentials should be regenerated
void ForcefieldTab::atomTypeDataModified()
{
    dissolve_.coreData().bumpAtomTypesVersion();

    if (ui_.AutoUpdatePairPotentialsCheck->isChecked())
        updatePairPotentials();
}

void ForcefieldTab::on_AtomTypeDuplicateButton_clicked(bool checked)
{
    auto index = ui_.AtomTypesTable->currentIndex();
    if (!index.isValid())
        return;

    // Get selected atomtype
    auto at = atomTypesModel_.rawData(index);
    if (!at)
        return;

    // Generate a unique name before we duplicate
    auto newName =
        DissolveSys::uniqueName(at->name(), dissolve_.coreData().atomTypes(), [](const auto &at) { return at->name(); });
    auto newAt = dissolve_.coreData().addAtomType(at->Z());
    newAt->setName(newName);
    newAt->setCharge(at->charge());
    newAt->interactionPotential().setFormAndParameters(at->interactionPotential().form(),
                                                       at->interactionPotential().parameters());

    Locker refreshLocker(refreshLock_);

    atomTypesModel_.setData(dissolve_.coreData().atomTypes());
    ui_.AtomTypesTable->resizeColumnsToContents();

    // Re-set the current index
    ui_.AtomTypesTable->setCurrentIndex(index);

    dissolveWindow_->setModified();
}

void ForcefieldTab::on_AtomTypeAddButton_clicked(bool checked)
{
    // First, need to get target element for the new AtomType
    bool ok;
    auto Z =
        ElementSelector::getElement(this, "Element Selection", "Choose the Element for the AtomType", Elements::Unknown, &ok);
    if (!ok)
        return;

    auto at = dissolve_.coreData().addAtomType(Z);

    Locker refreshLocker(refreshLock_);

    atomTypesModel_.setData(dissolve_.coreData().atomTypes());
    ui_.AtomTypesTable->resizeColumnsToContents();

    dissolveWindow_->setModified();
}

void ForcefieldTab::on_AtomTypeRemoveButton_clicked(bool checked) { Messenger::error("NOT IMPLEMENTED YET.\n"); }

void ForcefieldTab::atomTypeSelectionChanged(const QItemSelection &current, const QItemSelection &previous)
{
    ui_.AtomTypeDuplicateButton->setEnabled(!current.empty());
}

void ForcefieldTab::atomTypeDataChanged(const QModelIndex &current, const QModelIndex &previous, const QVector<int> &)
{
    ui_.AtomTypesTable->resizeColumnsToContents();

    dissolveWindow_->setModified();
}

void ForcefieldTab::on_PairPotentialRangeSpin_valueChanged(double value)
{
    if (refreshLock_.isLocked())
        return;

    dissolve_.setPairPotentialRange(value);

    if (ui_.AutoUpdatePairPotentialsCheck->isChecked())
        updatePairPotentials();

    dissolveWindow_->setModified();
}

void ForcefieldTab::on_PairPotentialDeltaSpin_valueChanged(double value)
{
    if (refreshLock_.isLocked())
        return;

    dissolve_.setPairPotentialDelta(value);

    if (ui_.AutoUpdatePairPotentialsCheck->isChecked())
        updatePairPotentials();

    dissolveWindow_->setModified();
}

void ForcefieldTab::on_PairPotentialsAtomTypeChargesRadio_clicked(bool checked)
{
    if (refreshLock_.isLocked())
        return;

    dissolve_.setAtomTypeChargeSource(checked);

    if (ui_.AutoUpdatePairPotentialsCheck->isChecked())
        updatePairPotentials();

    dissolveWindow_->setModified();
}

void ForcefieldTab::on_PairPotentialsSpeciesAtomChargesRadio_clicked(bool checked)
{
    on_PairPotentialsAtomTypeChargesRadio_clicked(false);
}

void ForcefieldTab::on_ShortRangeTruncationCombo_currentIndexChanged(int index)
{
    if (refreshLock_.isLocked())
        return;

    PairPotential::setShortRangeTruncationScheme((PairPotential::ShortRangeTruncationScheme)index);
    ui_.ShortRangeTruncationWidthSpin->setEnabled(PairPotential::shortRangeTruncationScheme() ==
                                                  PairPotential::CosineShortRangeTruncation);

    if (ui_.AutoUpdatePairPotentialsCheck->isChecked())
        updatePairPotentials();

    dissolveWindow_->setModified();
}

void ForcefieldTab::on_CoulombTruncationCombo_currentIndexChanged(int index)
{
    if (refreshLock_.isLocked())
        return;

    PairPotential::setCoulombTruncationScheme((PairPotential::CoulombTruncationScheme)index);

    if (ui_.AutoUpdatePairPotentialsCheck->isChecked() && dissolve_.atomTypeChargeSource())
        updatePairPotentials();

    dissolveWindow_->setModified();
}

void ForcefieldTab::on_AutomaticChargeSourceCheck_clicked(bool checked)
{
    if (refreshLock_.isLocked())
        return;

    dissolve_.setAutomaticChargeSource(checked);

    dissolveWindow_->setModified();

    updateControls();
}

void ForcefieldTab::on_ForceChargeSourceCheck_clicked(bool checked)
{
    if (refreshLock_.isLocked())
        return;

    dissolve_.setForceChargeSource(checked);

    dissolveWindow_->setModified();
}

void ForcefieldTab::on_RegenerateAllPairPotentialsButton_clicked(bool checked) { updatePairPotentials(); }

void ForcefieldTab::on_AutoUpdatePairPotentialsCheck_clicked(bool checked)
{
    if (checked)
        updatePairPotentials();
}

void ForcefieldTab::pairPotentialDataChanged(const QModelIndex &current, const QModelIndex &previous, const QVector<int> &)
{
    dissolveWindow_->setModified();
}

void ForcefieldTab::pairPotentialSelectionChanged(const QItemSelection &current, const QItemSelection &previous)
{
    ui_.PairPotentialsPlotWidget->clearRenderableData();

    if (current.isEmpty())
        return;

    auto *pp = pairPotentialModel_.data(current.indexes().front(), Qt::UserRole).value<const PairPotential *>();
    if (!pp)
        return;

    ui_.PairPotentialsPlotWidget->dataViewer()->createRenderable<RenderableData1D>(
        pp->uFull(), fmt::format("Energy {}-{}", pp->atomTypeNameI(), pp->atomTypeNameJ()));
    ui_.PairPotentialsPlotWidget->dataViewer()
        ->createRenderable<RenderableData1D>(pp->dUFull(), fmt::format("Force {}-{}", pp->atomTypeNameI(), pp->atomTypeNameJ()))
        ->setColour(StockColours::RedStockColour);
}

void ForcefieldTab::masterBondsDataChanged(const QModelIndex &, const QModelIndex &)
{
    ui_.MasterBondsTable->resizeColumnsToContents();
    dissolveWindow_->setModified();
}
void ForcefieldTab::masterAnglesDataChanged(const QModelIndex &, const QModelIndex &)
{
    ui_.MasterAnglesTable->resizeColumnsToContents();
    dissolveWindow_->setModified();
}

void ForcefieldTab::masterTorsionsDataChanged(const QModelIndex &, const QModelIndex &)
{
    ui_.MasterTorsionsTable->resizeColumnsToContents();
    dissolveWindow_->setModified();
}

void ForcefieldTab::masterImpropersDataChanged(const QModelIndex &, const QModelIndex &)
{
    ui_.MasterImpropersTable->resizeColumnsToContents();
    dissolveWindow_->setModified();
}

void ForcefieldTab::on_MasterTermAddBondButton_clicked(bool checked)
{
    dissolve_.coreData().addMasterBond(
        DissolveSys::uniqueName("NewTerm", dissolve_.coreData().masterBonds(), [](const auto &b) { return b->name(); }));
    masterBondsTableModel_.setSourceData(dissolve_.coreData().masterBonds());
    ui_.MasterBondsTable->resizeColumnsToContents();
    dissolveWindow_->setModified();
}

void ForcefieldTab::on_MasterTermRemoveBondButton_clicked(bool checked) { Messenger::error("NOT IMPLEMENTED YET.\n"); }

void ForcefieldTab::on_MasterTermAddAngleButton_clicked(bool checked)
{
    dissolve_.coreData().addMasterAngle(
        DissolveSys::uniqueName("NewTerm", dissolve_.coreData().masterAngles(), [](const auto &a) { return a->name(); }));
    masterAnglesTableModel_.setSourceData(dissolve_.coreData().masterAngles());
    ui_.MasterAnglesTable->resizeColumnsToContents();
    dissolveWindow_->setModified();
}

void ForcefieldTab::on_MasterTermRemoveAngleButton_clicked(bool checked) { Messenger::error("NOT IMPLEMENTED YET.\n"); }

void ForcefieldTab::on_MasterTermAddTorsionButton_clicked(bool checked)
{
    dissolve_.coreData().addMasterTorsion(
        DissolveSys::uniqueName("NewTerm", dissolve_.coreData().masterTorsions(), [](const auto &t) { return t->name(); }));
    masterTorsionsTableModel_.setSourceData(dissolve_.coreData().masterTorsions());
    ui_.MasterTorsionsTable->resizeColumnsToContents();
    dissolveWindow_->setModified();
}

void ForcefieldTab::on_MasterTermRemoveTorsionButton_clicked(bool checked) { Messenger::error("NOT IMPLEMENTED YET.\n"); }

void ForcefieldTab::on_MasterTermAddImproperButton_clicked(bool checked)
{
    dissolve_.coreData().addMasterImproper(
        DissolveSys::uniqueName("NewTerm", dissolve_.coreData().masterImpropers(), [](const auto &i) { return i->name(); }));
    masterImpropersTableModel_.setSourceData(dissolve_.coreData().masterImpropers());
    ui_.MasterImpropersTable->resizeColumnsToContents();
    dissolveWindow_->setModified();
}

void ForcefieldTab::on_MasterTermRemoveImproperButton_clicked(bool checked) { Messenger::error("NOT IMPLEMENTED YET.\n"); }
