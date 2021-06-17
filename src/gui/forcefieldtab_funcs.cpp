// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/atomtype.h"
#include "classes/speciesangle.h"
#include "classes/speciesbond.h"
#include "classes/speciesimproper.h"
#include "classes/speciestorsion.h"
#include "gui/delegates/combolist.hui"
#include "gui/delegates/exponentialspin.hui"
#include "gui/forcefieldtab.h"
#include "gui/gui.h"
#include "gui/helpers/combopopulator.h"
#include "gui/helpers/tablewidgetupdater.h"
#include "gui/render/renderabledata1d.h"
#include "gui/widgets/elementselector.hui"
#include "main/dissolve.h"
#include <QListWidgetItem>

Q_DECLARE_SMART_POINTER_METATYPE(std::shared_ptr)
Q_DECLARE_METATYPE(std::shared_ptr<AtomType>)
Q_DECLARE_METATYPE(std::shared_ptr<MasterIntra>)
Q_DECLARE_METATYPE(MasterIntra *)
Q_DECLARE_METATYPE(PairPotential *)

ForcefieldTab::ForcefieldTab(DissolveWindow *dissolveWindow, Dissolve &dissolve, MainTabsWidget *parent, const QString title)
    : MainTab(dissolveWindow, dissolve, parent, title, this), pairs_(dissolve.pairPotentials())
{
    ui_.setupUi(this);

    Locker refreshLocker(refreshLock_);

    /*
     * Master Terms
     */

    // Set item delegates for tables
    // -- Functional Forms
    ui_.MasterBondsTable->setItemDelegateForColumn(
        1, new ComboListDelegate(this, new ComboEnumOptionsItems<SpeciesBond::BondFunction>(SpeciesBond::bondFunctions())));
    ui_.MasterAnglesTable->setItemDelegateForColumn(
        1, new ComboListDelegate(this, new ComboEnumOptionsItems<SpeciesAngle::AngleFunction>(SpeciesAngle::angleFunctions())));
    ui_.MasterTorsionsTable->setItemDelegateForColumn(
        1, new ComboListDelegate(
               this, new ComboEnumOptionsItems<SpeciesTorsion::TorsionFunction>(SpeciesTorsion::torsionFunctions())));
    ui_.MasterImpropersTable->setItemDelegateForColumn(
        1, new ComboListDelegate(
               this, new ComboEnumOptionsItems<SpeciesTorsion::TorsionFunction>(SpeciesTorsion::torsionFunctions())));

    // Ensure fonts for table headers are set correctly and the headers themselves are visible=
    masterBondsTableModel_ = new MasterTermBondModel(dissolve_.coreData().masterBonds(), parent);
    ui_.MasterBondsTable->setModel(masterBondsTableModel_);
    ui_.MasterBondsTable->horizontalHeader()->setFont(font());
    ui_.MasterBondsTable->horizontalHeader()->setVisible(true);

    masterAnglesTableModel_ = new MasterTermAngleModel(dissolve_.coreData().masterAngles(), parent);
    ui_.MasterAnglesTable->setModel(masterAnglesTableModel_);
    ui_.MasterAnglesTable->horizontalHeader()->setFont(font());
    ui_.MasterAnglesTable->horizontalHeader()->setVisible(true);

    masterTorsionsTableModel_ = new MasterTermTorsionModel(dissolve_.coreData().masterTorsions(), parent);
    ui_.MasterTorsionsTable->setModel(masterTorsionsTableModel_);
    ui_.MasterTorsionsTable->horizontalHeader()->setFont(font());
    ui_.MasterTorsionsTable->horizontalHeader()->setVisible(true);

    masterImpropersTableModel_ = new MasterTermTorsionModel(dissolve_.coreData().masterImpropers(), parent);
    ui_.MasterImpropersTable->setModel(masterImpropersTableModel_);
    ui_.MasterImpropersTable->horizontalHeader()->setFont(font());
    ui_.MasterImpropersTable->horizontalHeader()->setVisible(true);

    connect(masterBondsTableModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), dissolveWindow_,
            SLOT(setModified()));
    connect(masterAnglesTableModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), dissolveWindow_,
            SLOT(setModified()));
    connect(masterTorsionsTableModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), dissolveWindow_,
            SLOT(setModified()));
    connect(masterImpropersTableModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), dissolveWindow_,
            SLOT(setModified()));

    /*
     * Atom Types
     */

    // Set item delegates for tables
    // -- Short Range Functional Forms
    ui_.AtomTypesTable->setItemDelegateForColumn(
        3, new ComboListDelegate(this, new ComboEnumOptionsItems<Forcefield::ShortRangeType>(Forcefield::shortRangeTypes())));
    // -- Charge / Parameters

    // Ensure fonts for table headers are set correctly and the headers themselves are visible
    ui_.AtomTypesTable->horizontalHeader()->setFont(font());
    ui_.AtomTypesTable->horizontalHeader()->setVisible(true);
    ui_.AtomTypesTable->setModel(&atoms_);

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
    ui_.PairPotentialsTable->setModel(&pairs_);

    DataViewer *viewer = ui_.PairPotentialsPlotWidget->dataViewer();
    viewer->view().axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
    viewer->view().axes().setTitle(1, "U, kj/mol");
    viewer->view().axes().setRange(1, -100.0, 100.0);

    connect(&pairs_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)), dissolveWindow,
            SLOT(setModified()));
}

ForcefieldTab::~ForcefieldTab() {}

/*
 * MainTab Reimplementations
 */

// Return tab type
MainTab::TabType ForcefieldTab::type() const { return MainTab::ForcefieldTabType; }

// Return whether the tab can be closed
bool ForcefieldTab::canClose() const { return false; }

// Update controls in tab
void ForcefieldTab::updateControls()
{
    Locker refreshLocker(refreshLock_);

    masterBondsTableModel_->reset(dissolve_.coreData().masterBonds());
    masterAnglesTableModel_->reset(dissolve_.coreData().masterAngles());
    masterTorsionsTableModel_->reset(dissolve_.coreData().masterTorsions());
    masterImpropersTableModel_->reset(dissolve_.coreData().masterImpropers());

    // AtomTypes Table
    atoms_.setData(dissolve_.atomTypes());
    ui_.AtomTypesTable->resizeColumnsToContents();

    // PairPotentials
    // -- Automatically regenerate pair potentials (quietly)
    if (ui_.AutoUpdatePairPotentialsCheck->isChecked())
    {
        Messenger::mute();
        dissolve_.generatePairPotentials();
        Messenger::unMute();
    }
    ui_.PairPotentialRangeSpin->setValue(dissolve_.pairPotentialRange());
    ui_.PairPotentialDeltaSpin->setValue(dissolve_.pairPotentialDelta());
    if (dissolve_.pairPotentialsIncludeCoulomb())
        ui_.PairPotentialsIncludeCoulombRadio->setChecked(true);
    else
        ui_.PairPotentialsShortRangeOnlyRadio->setChecked(true);
    ui_.ShortRangeTruncationCombo->setCurrentIndex(PairPotential::shortRangeTruncationScheme());
    ui_.ShortRangeTruncationWidthSpin->setValue(PairPotential::shortRangeTruncationWidth());
    ui_.ShortRangeTruncationWidthSpin->setEnabled(PairPotential::shortRangeTruncationScheme() ==
                                                  PairPotential::CosineShortRangeTruncation);
    ui_.CoulombTruncationCombo->setCurrentIndex(PairPotential::coulombTruncationScheme());

    // -- Table
    // -- Get current row index before we refresh...
    pairs_.reset();
    ui_.PairPotentialsTable->resizeColumnsToContents();

    refreshLocker.unlock();
}

// Disable sensitive controls within tab
void ForcefieldTab::disableSensitiveControls() { setEnabled(false); }

// Enable sensitive controls within tab
void ForcefieldTab::enableSensitiveControls() { setEnabled(true); }

/*
 * Signals / Slots
 */

// Signal that some AtomType parameter has been modified, so pair potentials should be regenerated
void ForcefieldTab::atomTypeDataModified()
{
    dissolve_.coreData().bumpAtomTypesVersion();

    if (ui_.AutoUpdatePairPotentialsCheck->isChecked())
    {
        Messenger::mute();
        dissolve_.generatePairPotentials();
        Messenger::unMute();

        updateControls();
    }
}

void ForcefieldTab::on_AtomTypeAddButton_clicked(bool checked)
{
    // First, need to get target element for the new AtomType
    bool ok;
    auto Z =
        ElementSelector::getElement(this, "Element Selection", "Choose the Element for the AtomType", Elements::Unknown, &ok);
    if (!ok)
        return;

    std::shared_ptr<AtomType> at = dissolve_.addAtomType(Z);

    Locker refreshLocker(refreshLock_);

    atoms_.setData(dissolve_.atomTypes());
    ui_.AtomTypesTable->resizeColumnsToContents();

    dissolveWindow_->setModified();
}

void ForcefieldTab::on_AtomTypeRemoveButton_clicked(bool checked) { Messenger::error("NOT IMPLEMENTED YET.\n"); }

void ForcefieldTab::on_PairPotentialRangeSpin_valueChanged(double value)
{
    if (refreshLock_.isLocked())
        return;

    dissolve_.setPairPotentialRange(value);

    if (ui_.AutoUpdatePairPotentialsCheck->isChecked())
    {
        dissolve_.regeneratePairPotentials();
        updateControls();
    }

    dissolveWindow_->setModified();
}

void ForcefieldTab::on_PairPotentialDeltaSpin_valueChanged(double value)
{
    if (refreshLock_.isLocked())
        return;

    dissolve_.setPairPotentialDelta(value);

    if (ui_.AutoUpdatePairPotentialsCheck->isChecked())
    {
        dissolve_.regeneratePairPotentials();
        updateControls();
    }

    dissolveWindow_->setModified();
}

void ForcefieldTab::on_PairPotentialsIncludeCoulombRadio_clicked(bool checked)
{
    if (refreshLock_.isLocked())
        return;

    dissolve_.setPairPotentialsIncludeCoulomb(checked);

    if (ui_.AutoUpdatePairPotentialsCheck->isChecked())
    {
        dissolve_.regeneratePairPotentials();

        updateControls();
    }

    dissolveWindow_->setModified();
}

void ForcefieldTab::on_PairPotentialsShortRangeOnlyRadio_clicked(bool checked)
{
    on_PairPotentialsIncludeCoulombRadio_clicked(false);
}

void ForcefieldTab::on_ShortRangeTruncationCombo_currentIndexChanged(int index)
{
    if (refreshLock_.isLocked())
        return;

    PairPotential::setShortRangeTruncationScheme((PairPotential::ShortRangeTruncationScheme)index);
    ui_.ShortRangeTruncationWidthSpin->setEnabled(PairPotential::shortRangeTruncationScheme() ==
                                                  PairPotential::CosineShortRangeTruncation);

    if (ui_.AutoUpdatePairPotentialsCheck->isChecked())
    {
        dissolve_.regeneratePairPotentials();
        updateControls();
    }

    dissolveWindow_->setModified();
}

void ForcefieldTab::on_CoulombTruncationCombo_currentIndexChanged(int index)
{
    if (refreshLock_.isLocked())
        return;

    PairPotential::setCoulombTruncationScheme((PairPotential::CoulombTruncationScheme)index);

    if (ui_.AutoUpdatePairPotentialsCheck->isChecked() && dissolve_.pairPotentialsIncludeCoulomb())
    {
        dissolve_.regeneratePairPotentials();
        updateControls();
    }

    dissolveWindow_->setModified();
}

void ForcefieldTab::on_RegenerateAllPairPotentialsButton_clicked(bool checked)
{
    dissolve_.regeneratePairPotentials();

    updateControls();
}

void ForcefieldTab::on_UpdatePairPotentialsButton_clicked(bool checked)
{
    dissolve_.generatePairPotentials();

    updateControls();
}

void ForcefieldTab::on_AutoUpdatePairPotentialsCheck_clicked(bool checked)
{
    if (checked)
        on_UpdatePairPotentialsButton_clicked(false);
}

void ForcefieldTab::on_MasterTermAddBondButton_clicked(bool checked) { Messenger::error("NOT IMPLEMENTED YET.\n"); }
void ForcefieldTab::on_MasterTermRemoveBondButton_clicked(bool checked) { Messenger::error("NOT IMPLEMENTED YET.\n"); }
void ForcefieldTab::on_MasterTermAddAngleButton_clicked(bool checked) { Messenger::error("NOT IMPLEMENTED YET.\n"); }
void ForcefieldTab::on_MasterTermRemoveAngleButton_clicked(bool checked) { Messenger::error("NOT IMPLEMENTED YET.\n"); }
void ForcefieldTab::on_MasterTermAddTorsionButton_clicked(bool checked) { Messenger::error("NOT IMPLEMENTED YET.\n"); }
void ForcefieldTab::on_MasterTermRemoveTorsionButton_clicked(bool checked) { Messenger::error("NOT IMPLEMENTED YET.\n"); }
void ForcefieldTab::on_MasterTermAddImproperButton_clicked(bool checked) { Messenger::error("NOT IMPLEMENTED YET.\n"); }
void ForcefieldTab::on_MasterTermRemoveImproperButton_clicked(bool checked) { Messenger::error("NOT IMPLEMENTED YET.\n"); }
