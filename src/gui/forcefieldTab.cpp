// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/forcefieldTab.h"
#include "classes/atomType.h"
#include "classes/speciesAngle.h"
#include "classes/speciesBond.h"
#include "classes/speciesImproper.h"
#include "classes/speciesTorsion.h"
#include "gui/delegates/comboList.h"
#include "gui/delegates/exponentialSpin.h"
#include "gui/gui.h"
#include "gui/helpers/comboPopulator.h"
#include "gui/render/renderableData1D.h"
#include "gui/widgets/elementSelector.h"
#include "main/dissolve.h"
#include <QInputDialog>
#include <QListWidgetItem>
#include <QMessageBox>

ForcefieldTab::ForcefieldTab(DissolveWindow *dissolveWindow, Dissolve &dissolve, MainTabsWidget *parent, const QString title)
    : MainTab(dissolveWindow, dissolve, parent, title, this), atomTypesModel_(dissolve.coreData()),
      pairPotentialModel_(dissolve.pairPotentials()), pairPotentialOverrideModel_(dissolve.coreData().pairPotentialOverrides()),
      masterBondsTableModel_(dissolve.coreData()), masterAnglesTableModel_(dissolve.coreData()),
      masterTorsionsTableModel_(dissolve.coreData()), masterImpropersTableModel_(dissolve.coreData())
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
    ui_.MasterBondsTable->setModel(&masterBondsTableModel_);
    ui_.MasterAnglesTable->setModel(&masterAnglesTableModel_);
    ui_.MasterTorsionsTable->setModel(&masterTorsionsTableModel_);
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
    connect(ui_.MasterBondsTable->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(masterBondsSelectionChanged(const QItemSelection &, const QItemSelection &)));
    connect(ui_.MasterAnglesTable->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(masterAnglesSelectionChanged(const QItemSelection &, const QItemSelection &)));
    connect(ui_.MasterTorsionsTable->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(masterTorsionsSelectionChanged(const QItemSelection &, const QItemSelection &)));
    connect(ui_.MasterImpropersTable->selectionModel(),
            SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this,
            SLOT(masterImpropersSelectionChanged(const QItemSelection &, const QItemSelection &)));

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

    // Ensure fonts for table headers are set correctly and the headers themselves are visible
    ui_.PairPotentialsTable->horizontalHeader()->setFont(font());
    ui_.PairPotentialsTable->horizontalHeader()->setVisible(true);
    ui_.PairPotentialsTable->setItemDelegateForColumn(
        PairPotentialModel::Columns::ShortRangeFormColumn,
        new ComboListDelegate(this, new ComboEnumOptionsItems<Functions1D::Form>(Functions1D::forms())));
    ui_.PairPotentialsTable->setModel(&pairPotentialModel_);
    connect(&pairPotentialModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)), this,
            SLOT(pairPotentialDataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)));

    DataViewer *viewer = ui_.PairPotentialsPlotWidget->dataViewer();
    viewer->view().axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
    viewer->view().axes().setTitle(1, "U, kj/mol");
    viewer->view().axes().setRange(1, -100.0, 100.0);

    connect(&pairPotentialModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)), this,
            SLOT(pairPotentialDataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)));
    connect(ui_.PairPotentialsTable->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(pairPotentialSelectionChanged(const QItemSelection &, const QItemSelection &)));

    /*
     * Pair Potential Overrides
     */

    // Set delegates for table columns
    ui_.OverridesTable->setItemDelegateForColumn(
        PairPotentialOverrideModel::ColumnData::OverrideType,
        new ComboListDelegate(this, new ComboEnumOptionsItems<PairPotentialOverride::PairPotentialOverrideType>(
                                        PairPotentialOverride::pairPotentialOverrideTypes())));
    ui_.OverridesTable->setItemDelegateForColumn(
        PairPotentialOverrideModel::ColumnData::Form,
        new ComboListDelegate(this, new ComboEnumOptionsItems<Functions1D::Form>(Functions1D::forms())));
    ui_.OverridesTable->setModel(&pairPotentialOverrideModel_);
    connect(&pairPotentialOverrideModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)),
            this, SLOT(overrideDataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)));
    connect(ui_.OverridesTable->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(overrideSelectionChanged(const QItemSelection &, const QItemSelection &)));
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
    dissolve_.updatePairPotentials();

    resetPairPotentialModel();
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

    masterBondsTableModel_.reset();
    masterAnglesTableModel_.reset();
    masterTorsionsTableModel_.reset();
    masterImpropersTableModel_.reset();
    ui_.MasterBondsTable->resizeColumnsToContents();
    ui_.MasterAnglesTable->resizeColumnsToContents();
    ui_.MasterTorsionsTable->resizeColumnsToContents();
    ui_.MasterImpropersTable->resizeColumnsToContents();

    // AtomTypes Table
    atomTypesModel_.setData(dissolve_.coreData().atomTypes());
    ui_.AtomTypesTable->resizeColumnsToContents();

    // PairPotentials
    resetPairPotentialModel();
    ui_.UseCombinationRulesCheck->setChecked(dissolve_.useCombinationRules());
    ui_.PairPotentialRangeButton->setText(QString::number(dissolve_.pairPotentialRange()) + " Å");
    ui_.PairPotentialDeltaButton->setText(QString::number(dissolve_.pairPotentialDelta()) + " Å");
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
    ui_.CoulombTruncationCombo->setCurrentIndex(PairPotential::coulombTruncationScheme());

    // Pair Potential Overrides
    pairPotentialOverrideModel_.reset();

    refreshLocker.unlock();
}

// Prevent editing within tab
void ForcefieldTab::preventEditing() { setEnabled(false); }

// Allow editing within tab
void ForcefieldTab::allowEditing() { setEnabled(true); }

// Set the current tab
void ForcefieldTab::setTab(int index) { ui_.Tabs->setCurrentIndex(index); }

/*
 * Atom Types
 */

void ForcefieldTab::on_AtomTypeDuplicateButton_clicked(bool checked)
{
    auto index = ui_.AtomTypesTable->currentIndex();
    if (!index.isValid())
        return;

    // Get selected atomtype
    auto at = atomTypesModel_.rawData(index);
    assert(at);

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

    dissolve_.updatePairPotentials();
    resetPairPotentialModel();

    updateControls();

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

    dissolve_.updatePairPotentials();
    resetPairPotentialModel();

    updateControls();

    dissolveWindow_->setModified();
}

void ForcefieldTab::on_AtomTypeRemoveButton_clicked(bool checked)
{
    QMessageBox queryBox;
    queryBox.setWindowTitle(QString("Remove Atom Type"));
    queryBox.setText(QString("This operation cannot be undone!"));
    queryBox.setInformativeText("Proceed?");
    queryBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    queryBox.setDefaultButton(QMessageBox::No);

    if (queryBox.exec() != QMessageBox::Yes)
        return;

    auto index = ui_.AtomTypesTable->currentIndex();
    if (!index.isValid())
        return;

    // Get selected atomtype
    auto at = atomTypesModel_.rawData(index);
    assert(at);

    dissolve_.coreData().removeAtomType(at);

    Locker refreshLocker(refreshLock_);

    atomTypesModel_.setData(dissolve_.coreData().atomTypes());
    ui_.AtomTypesTable->resizeColumnsToContents();

    dissolve_.updatePairPotentials();
    resetPairPotentialModel();

    dissolveWindow_->setModified();
}

void ForcefieldTab::atomTypeSelectionChanged(const QItemSelection &current, const QItemSelection &previous)
{
    ui_.AtomTypeRemoveButton->setEnabled(!current.empty());
    ui_.AtomTypeDuplicateButton->setEnabled(!current.empty());
}

void ForcefieldTab::atomTypeDataChanged(const QModelIndex &current, const QModelIndex &previous, const QVector<int> &)
{
    ui_.AtomTypesTable->resizeColumnsToContents();

    dissolve_.updatePairPotentials();
    resetPairPotentialModel();

    dissolveWindow_->setModified();
}

/*
 * Pair Potentials
 */

void ForcefieldTab::on_PairPotentialRangeButton_clicked(bool checked)
{
    bool ok = false;
    auto newRange = QInputDialog::getDouble(this, "Set Pair Potential Range",
                                            "Enter a new pair potential range (in Angstroms).\nWarning: Setting this will "
                                            "force all current processing data to be cleared!",
                                            dissolve_.pairPotentialRange(), 5.0, 50.0, 1, &ok);
    if (!ok || newRange == dissolve_.pairPotentialRange())
        return;

    dissolve_.setPairPotentialRange(newRange);
    dissolve_.processingModuleData().clearAll();

    updatePairPotentials();
    resetPairPotentialModel();

    dissolveWindow_->fullUpdate();

    dissolveWindow_->setModified();
}

void ForcefieldTab::on_PairPotentialDeltaButton_clicked(bool checked)
{
    bool ok = false;
    auto newDelta = QInputDialog::getDouble(this, "Set Pair Potential Delta",
                                            "Enter a new pair potential delta (in Angstroms).\nWarning: Setting this will "
                                            "force all current processing data to be cleared!",
                                            dissolve_.pairPotentialDelta(), 0.0001, 0.1, 4, &ok);
    if (!ok || newDelta == dissolve_.pairPotentialDelta())
        return;

    dissolve_.setPairPotentialDelta(newDelta);
    dissolve_.processingModuleData().clearAll();

    updatePairPotentials();
    resetPairPotentialModel();

    dissolveWindow_->fullUpdate();

    dissolveWindow_->setModified();
}

void ForcefieldTab::on_PairPotentialsAtomTypeChargesRadio_clicked(bool checked)
{
    if (refreshLock_.isLocked())
        return;

    dissolve_.setAtomTypeChargeSource(checked);

    updatePairPotentials();
    updateControls();

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

    updatePairPotentials();
    updateControls();

    dissolveWindow_->setModified();
}

void ForcefieldTab::on_CoulombTruncationCombo_currentIndexChanged(int index)
{
    if (refreshLock_.isLocked())
        return;

    PairPotential::setCoulombTruncationScheme((PairPotential::CoulombTruncationScheme)index);

    updatePairPotentials();
    updateControls();

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

void ForcefieldTab::on_UseCombinationRulesCheck_clicked(bool checked)
{
    if (refreshLock_.isLocked())
        return;

    // Make sure this is really what the user wants to do - we will potentially destroy user-defined data otherwise
    if (QMessageBox::warning(this, checked ? "Use Combination Rules?" : "Disable Combination Rules?",
                             QString(checked
                                         ? "This will enable automatic generation of pair potentials by employing combination "
                                           "rules.\nThis will immediately overwrite any user-defined potentials in the list."
                                         : "This will disable automatic generation and update of pair potentials with "
                                           "combination rules, meaning you will need to define / adjust them by hand.") +
                                 "\nAre you sure this is what you want to do?",
                             QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No,
                             QMessageBox::StandardButton::No) == QMessageBox::StandardButton::No)
    {
        Locker refreshLocker(refreshLock_);
        ui_.UseCombinationRulesCheck->setChecked(!checked);
        return;
    }

    dissolve_.setUseCombinationRules(checked);
    dissolve_.updatePairPotentials();
    pairPotentialModel_.setEditable(!checked);
    resetPairPotentialModel();
    dissolveWindow_->setModified();
    updateControls();
}

void ForcefieldTab::pairPotentialDataChanged(const QModelIndex &current, const QModelIndex &previous, const QVector<int> &)
{
    dissolve_.updatePairPotentials();

    updateControls();

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
        pp->shortRangePotential(), fmt::format("Short Range {}-{}", pp->nameI(), pp->nameJ()));
    ui_.PairPotentialsPlotWidget->dataViewer()
        ->createRenderable<RenderableData1D>(pp->coulombPotential(), fmt::format("Coulomb {}-{}", pp->nameI(), pp->nameJ()))
        ->setColour(StockColours::BlueStockColour);
    ui_.PairPotentialsPlotWidget->dataViewer()
        ->createRenderable<RenderableData1D>(pp->derivative(), fmt::format("Derivative {}-{}", pp->nameI(), pp->nameJ()))
        ->setColour(StockColours::RedStockColour);
}

/*
 * Pair Potential Overrides
 */

void ForcefieldTab::on_OverrideAddButton_clicked(bool checked)
{
    pairPotentialOverrideModel_.insertRows(dissolve_.coreData().pairPotentialOverrides().size(), 1);
}

void ForcefieldTab::on_OverrideRemoveButton_clicked(bool checked)
{
    auto row = ui_.OverridesTable->currentIndex().row();
    if (row == -1)
        return;

    pairPotentialOverrideModel_.removeRows(row, 1);
}

void ForcefieldTab::on_OverrideDuplicateButton_clicked(bool checked)
{
    auto row = ui_.OverridesTable->currentIndex().row();
    if (row == -1)
        return;

    pairPotentialOverrideModel_.duplicateRow(row);
}

void ForcefieldTab::overrideDataChanged(const QModelIndex &current, const QModelIndex &previous, const QVector<int> &)
{
    dissolveWindow_->setModified();

    updatePairPotentials();
    updateControls();
}

void ForcefieldTab::overrideSelectionChanged(const QItemSelection &current, const QItemSelection &previous)
{
    ui_.OverrideDuplicateButton->setEnabled(!current.isEmpty());
    ui_.OverrideRemoveButton->setEnabled(!current.isEmpty());
}

/*
 * Master Terms
 */

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

void ForcefieldTab::masterBondsSelectionChanged(const QItemSelection &current, const QItemSelection &previous)
{
    ui_.MasterTermRemoveBondButton->setEnabled(!current.empty());
}

void ForcefieldTab::masterAnglesSelectionChanged(const QItemSelection &current, const QItemSelection &previous)
{
    ui_.MasterTermRemoveAngleButton->setEnabled(!current.empty());
}

void ForcefieldTab::masterTorsionsSelectionChanged(const QItemSelection &current, const QItemSelection &previous)
{
    ui_.MasterTermRemoveTorsionButton->setEnabled(!current.empty());
}

void ForcefieldTab::masterImpropersSelectionChanged(const QItemSelection &current, const QItemSelection &previous)
{
    ui_.MasterTermRemoveImproperButton->setEnabled(!current.empty());
}

void ForcefieldTab::on_MasterTermAddBondButton_clicked(bool checked)
{
    masterBondsTableModel_.insertRows(masterBondsTableModel_.rowCount(), 1, {});

    ui_.MasterBondsTable->resizeColumnsToContents();

    dissolveWindow_->setModified();
}

void ForcefieldTab::on_MasterTermRemoveBondButton_clicked(bool checked)
{
    QMessageBox queryBox;
    queryBox.setWindowTitle(QString("Remove Master Bond"));
    queryBox.setText(QString("This operation cannot be undone!"));
    queryBox.setInformativeText("Proceed?");
    queryBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    queryBox.setDefaultButton(QMessageBox::No);

    if (queryBox.exec() != QMessageBox::Yes)
        return;

    auto index = ui_.MasterBondsTable->currentIndex();
    if (!index.isValid())
        return;

    // Get selected master bond
    auto bond = masterBondsTableModel_.rawData(index);
    assert(bond);

    if (masterBondsTableModel_.removeRows(index.row(), 1, {}))
    {
        ui_.MasterBondsTable->resizeColumnsToContents();

        dissolveWindow_->setModified();
    }
}

void ForcefieldTab::on_MasterTermAddAngleButton_clicked(bool checked)
{
    dissolve_.coreData().addMasterAngle(
        DissolveSys::uniqueName("NewTerm", dissolve_.coreData().masterAngles(), [](const auto &a) { return a->name(); }));
    ui_.MasterAnglesTable->resizeColumnsToContents();
    dissolveWindow_->setModified();
}

void ForcefieldTab::on_MasterTermRemoveAngleButton_clicked(bool checked)
{
    QMessageBox queryBox;
    queryBox.setWindowTitle(QString("Remove Master Angle"));
    queryBox.setText(QString("This operation cannot be undone!"));
    queryBox.setInformativeText("Proceed?");
    queryBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    queryBox.setDefaultButton(QMessageBox::No);

    if (queryBox.exec() != QMessageBox::Yes)
        return;

    auto index = ui_.MasterAnglesTable->currentIndex();
    if (!index.isValid())
        return;

    // Get selected master angle
    auto angle = masterAnglesTableModel_.rawData(index);
    assert(angle);

    dissolve_.coreData().removeMasterAngle(angle);

    Locker refreshLocker(refreshLock_);

    ui_.MasterAnglesTable->resizeColumnsToContents();

    dissolveWindow_->setModified();
}

void ForcefieldTab::on_MasterTermAddTorsionButton_clicked(bool checked)
{
    dissolve_.coreData().addMasterTorsion(
        DissolveSys::uniqueName("NewTerm", dissolve_.coreData().masterTorsions(), [](const auto &t) { return t->name(); }));

    ui_.MasterTorsionsTable->resizeColumnsToContents();
    dissolveWindow_->setModified();
}

void ForcefieldTab::on_MasterTermRemoveTorsionButton_clicked(bool checked)
{
    QMessageBox queryBox;
    queryBox.setWindowTitle(QString("Remove Master Torsion"));
    queryBox.setText(QString("This operation cannot be undone!"));
    queryBox.setInformativeText("Proceed?");
    queryBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    queryBox.setDefaultButton(QMessageBox::No);

    if (queryBox.exec() != QMessageBox::Yes)
        return;

    auto index = ui_.MasterTorsionsTable->currentIndex();
    if (!index.isValid())
        return;

    // Get selected master torsion
    auto torsion = masterTorsionsTableModel_.rawData(index);
    assert(torsion);

    dissolve_.coreData().removeMasterTorsion(torsion);

    Locker refreshLocker(refreshLock_);

    ui_.MasterTorsionsTable->resizeColumnsToContents();

    dissolveWindow_->setModified();
}

void ForcefieldTab::on_MasterTermAddImproperButton_clicked(bool checked)
{
    dissolve_.coreData().addMasterImproper(
        DissolveSys::uniqueName("NewTerm", dissolve_.coreData().masterImpropers(), [](const auto &i) { return i->name(); }));

    ui_.MasterImpropersTable->resizeColumnsToContents();
    dissolveWindow_->setModified();
}

void ForcefieldTab::on_MasterTermRemoveImproperButton_clicked(bool checked)
{
    QMessageBox queryBox;
    queryBox.setWindowTitle(QString("Remove Master Improper"));
    queryBox.setText(QString("This operation cannot be undone!"));
    queryBox.setInformativeText("Proceed?");
    queryBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    queryBox.setDefaultButton(QMessageBox::No);

    if (queryBox.exec() != QMessageBox::Yes)
        return;

    auto index = ui_.MasterImpropersTable->currentIndex();
    if (!index.isValid())
        return;

    // Get selected master improper
    auto improper = masterImpropersTableModel_.rawData(index);
    assert(improper);

    dissolve_.coreData().removeMasterImproper(improper);

    Locker refreshLocker(refreshLock_);

    ui_.MasterImpropersTable->resizeColumnsToContents();

    dissolveWindow_->setModified();
}
