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
Q_DECLARE_METATYPE(MasterIntra *)
Q_DECLARE_METATYPE(PairPotential *)

ForcefieldTab::ForcefieldTab(DissolveWindow *dissolveWindow, Dissolve &dissolve, MainTabsWidget *parent, const QString title)
    : MainTab(dissolveWindow, dissolve, parent, title, this)
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

    // -- Parameters
    for (auto n = 2; n < 6; ++n)
    {
        ui_.MasterBondsTable->setItemDelegateForColumn(n, new ExponentialSpinDelegate(this));
        ui_.MasterAnglesTable->setItemDelegateForColumn(n, new ExponentialSpinDelegate(this));
        ui_.MasterTorsionsTable->setItemDelegateForColumn(n, new ExponentialSpinDelegate(this));
        ui_.MasterImpropersTable->setItemDelegateForColumn(n, new ExponentialSpinDelegate(this));
    }

    // Ensure fonts for table headers are set correctly and the headers themselves are visible
    ui_.MasterBondsTable->horizontalHeader()->setFont(font());
    ui_.MasterBondsTable->horizontalHeader()->setVisible(true);
    ui_.MasterAnglesTable->horizontalHeader()->setFont(font());
    ui_.MasterAnglesTable->horizontalHeader()->setVisible(true);
    ui_.MasterTorsionsTable->horizontalHeader()->setFont(font());
    ui_.MasterTorsionsTable->horizontalHeader()->setVisible(true);
    ui_.MasterImpropersTable->horizontalHeader()->setFont(font());
    ui_.MasterImpropersTable->horizontalHeader()->setVisible(true);

    /*
     * Atom Types
     */

    // Set item delegates for tables
    // -- Short Range Functional Forms
    ui_.AtomTypesTable->setItemDelegateForColumn(
        3, new ComboListDelegate(this, new ComboEnumOptionsItems<Forcefield::ShortRangeType>(Forcefield::shortRangeTypes())));
    // -- Charge / Parameters
    for (auto n = 4; n < 9; ++n)
        ui_.AtomTypesTable->setItemDelegateForColumn(n, new ExponentialSpinDelegate(this));

    // Ensure fonts for table headers are set correctly and the headers themselves are visible
    ui_.AtomTypesTable->horizontalHeader()->setFont(font());
    ui_.AtomTypesTable->horizontalHeader()->setVisible(true);

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

    DataViewer *viewer = ui_.PairPotentialsPlotWidget->dataViewer();
    viewer->view().axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
    viewer->view().axes().setTitle(1, "U, kj/mol");
    viewer->view().axes().setRange(1, -100.0, 100.0);
}

ForcefieldTab::~ForcefieldTab() {}

/*
 * MainTab Reimplementations
 */

// Return tab type
MainTab::TabType ForcefieldTab::type() const { return MainTab::ForcefieldTabType; }

// Return whether the tab can be closed
bool ForcefieldTab::canClose() const { return false; }

/*
 * Update
 */

// Row update function for BondsTable
void ForcefieldTab::updateBondsTableRow(int row, MasterIntra *masterBond, bool createItems)
{
    QTableWidgetItem *item;

    // Name
    if (createItems)
    {
        item = new QTableWidgetItem;
        item->setData(Qt::UserRole, QVariant::fromValue(masterBond));
        ui_.MasterBondsTable->setItem(row, 0, item);
    }
    else
        item = ui_.MasterBondsTable->item(row, 0);
    item->setText(QString::fromStdString(std::string(masterBond->name())));

    // Functional Form
    if (createItems)
    {
        item = new QTableWidgetItem;
        ui_.MasterBondsTable->setItem(row, 1, item);
    }
    else
        item = ui_.MasterBondsTable->item(row, 1);
    QString text = QString::fromStdString(std::string(SpeciesBond::bondFunctions().keywordFromInt(masterBond->form())));
    item->setText(text);

    // Parameters
    for (int n = 0; n < masterBond->parameters().size(); ++n)
    {
        if (createItems)
        {
            item = new QTableWidgetItem;
            ui_.MasterBondsTable->setItem(row, n + 2, item);
        }
        else
            item = ui_.MasterBondsTable->item(row, n + 2);
        item->setText(QString::number(masterBond->parameter(n)));
    }
}

// Row update function for AnglesTable
void ForcefieldTab::updateAnglesTableRow(int row, MasterIntra *masterAngle, bool createItems)
{
    QTableWidgetItem *item;

    // Name
    if (createItems)
    {
        item = new QTableWidgetItem;
        item->setData(Qt::UserRole, QVariant::fromValue(masterAngle));
        ui_.MasterAnglesTable->setItem(row, 0, item);
    }
    else
        item = ui_.MasterAnglesTable->item(row, 0);
    item->setText(QString::fromStdString(std::string(masterAngle->name())));

    // Functional Form
    if (createItems)
    {
        item = new QTableWidgetItem;
        ui_.MasterAnglesTable->setItem(row, 1, item);
    }
    else
        item = ui_.MasterAnglesTable->item(row, 1);
    item->setText(QString::fromStdString(std::string(SpeciesAngle::angleFunctions().keywordFromInt(masterAngle->form()))));

    // Parameters
    for (int n = 0; n < masterAngle->parameters().size(); ++n)
    {
        if (createItems)
        {
            item = new QTableWidgetItem;
            ui_.MasterAnglesTable->setItem(row, n + 2, item);
        }
        else
            item = ui_.MasterAnglesTable->item(row, n + 2);
        item->setText(QString::number(masterAngle->parameter(n)));
    }
}

// Row update function for TorsionsTable
void ForcefieldTab::updateTorsionsTableRow(int row, MasterIntra *masterTorsion, bool createItems)
{
    QTableWidgetItem *item;

    // Name
    if (createItems)
    {
        item = new QTableWidgetItem;
        item->setData(Qt::UserRole, QVariant::fromValue(masterTorsion));
        ui_.MasterTorsionsTable->setItem(row, 0, item);
    }
    else
        item = ui_.MasterTorsionsTable->item(row, 0);
    item->setText(QString::fromStdString(std::string(masterTorsion->name())));

    // Functional Form
    if (createItems)
    {
        item = new QTableWidgetItem;
        ui_.MasterTorsionsTable->setItem(row, 1, item);
    }
    else
        item = ui_.MasterTorsionsTable->item(row, 1);
    item->setText(
        QString::fromStdString(std::string(SpeciesTorsion::torsionFunctions().keywordFromInt(masterTorsion->form()))));

    // Parameters
    for (int n = 0; n < masterTorsion->parameters().size(); ++n)
    {
        if (createItems)
        {
            item = new QTableWidgetItem;
            ui_.MasterTorsionsTable->setItem(row, n + 2, item);
        }
        else
            item = ui_.MasterTorsionsTable->item(row, n + 2);
        item->setText(QString::number(masterTorsion->parameter(n)));
    }
}

// Row update function for ImpropersTable
void ForcefieldTab::updateImpropersTableRow(int row, MasterIntra *masterImproper, bool createItems)
{
    QTableWidgetItem *item;

    // Name
    if (createItems)
    {
        item = new QTableWidgetItem;
        item->setData(Qt::UserRole, QVariant::fromValue(masterImproper));
        ui_.MasterImpropersTable->setItem(row, 0, item);
    }
    else
        item = ui_.MasterImpropersTable->item(row, 0);
    item->setText(QString::fromStdString(std::string(masterImproper->name())));

    // Functional Form
    if (createItems)
    {
        item = new QTableWidgetItem;
        ui_.MasterImpropersTable->setItem(row, 1, item);
    }
    else
        item = ui_.MasterImpropersTable->item(row, 1);
    item->setText(
        QString::fromStdString(std::string(SpeciesTorsion::torsionFunctions().keywordFromInt(masterImproper->form()))));

    // Parameters
    for (int n = 0; n < masterImproper->parameters().size(); ++n)
    {
        if (createItems)
        {
            item = new QTableWidgetItem;
            ui_.MasterImpropersTable->setItem(row, n + 2, item);
        }
        else
            item = ui_.MasterImpropersTable->item(row, n + 2);
        item->setText(QString::number(masterImproper->parameter(n)));
    }
}

// Row update function for AtomTypesTable
void ForcefieldTab::updateAtomTypesTableRow(int row, std::shared_ptr<AtomType> atomType, bool createItems)
{
    QTableWidgetItem *item;

    // Name
    if (createItems)
    {
        item = new QTableWidgetItem;
        item->setData(Qt::UserRole, QVariant::fromValue(atomType));
        ui_.AtomTypesTable->setItem(row, 0, item);
    }
    else
        item = ui_.AtomTypesTable->item(row, 0);
    item->setText(QString::fromStdString(std::string(atomType->name())));

    // Target element
    if (createItems)
    {
        item = new QTableWidgetItem;
        item->setData(Qt::UserRole, QVariant::fromValue(atomType));
        item->setFlags(Qt::NoItemFlags);
        ui_.AtomTypesTable->setItem(row, 1, item);
    }
    else
        item = ui_.AtomTypesTable->item(row, 1);
    item->setText(QString::fromStdString(std::string(Elements::symbol(atomType->Z()))));

    // Charge
    if (createItems)
    {
        item = new QTableWidgetItem;
        item->setData(Qt::UserRole, QVariant::fromValue(atomType));
        ui_.AtomTypesTable->setItem(row, 2, item);
    }
    else
        item = ui_.AtomTypesTable->item(row, 2);
    item->setText(QString::number(atomType->charge()));

    // Short-Range Form
    if (createItems)
    {
        item = new QTableWidgetItem;
        item->setData(Qt::UserRole, QVariant::fromValue(atomType));
        ui_.AtomTypesTable->setItem(row, 3, item);
    }
    else
        item = ui_.AtomTypesTable->item(row, 3);
    item->setText(QString::fromStdString(std::string(Forcefield::shortRangeTypes().keyword(atomType->shortRangeType()))));

    // Parameters
    auto col = 4;
    for (auto x : atomType->shortRangeParameters())
    {
        if (createItems)
        {
            item = new QTableWidgetItem;
            item->setData(Qt::UserRole, QVariant::fromValue(atomType));
            ui_.AtomTypesTable->setItem(row, col++, item);
        }
        else
            item = ui_.AtomTypesTable->item(row, col++);
        item->setText(QString::number(x));
    }
}

// Row update function for PairPotentialsTable
void ForcefieldTab::updatePairPotentialsTableRow(int row, PairPotential *pairPotential, bool createItems)
{
    QTableWidgetItem *item;

    // Type I
    if (createItems)
    {
        item = new QTableWidgetItem;
        item->setData(Qt::UserRole, QVariant::fromValue(pairPotential));
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui_.PairPotentialsTable->setItem(row, 0, item);
    }
    else
        item = ui_.PairPotentialsTable->item(row, 0);
    item->setText(QString::fromStdString(std::string(pairPotential->atomTypeNameI())));

    // Type J
    if (createItems)
    {
        item = new QTableWidgetItem;
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui_.PairPotentialsTable->setItem(row, 1, item);
    }
    else
        item = ui_.PairPotentialsTable->item(row, 1);
    item->setText(QString::fromStdString(std::string(pairPotential->atomTypeNameJ())));

    // Short-Range Form
    if (createItems)
    {
        item = new QTableWidgetItem;
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui_.PairPotentialsTable->setItem(row, 2, item);
    }
    else
        item = ui_.PairPotentialsTable->item(row, 2);
    item->setText(QString::fromStdString(std::string(Forcefield::shortRangeTypes().keyword(pairPotential->shortRangeType()))));

    // Charge I
    if (createItems)
    {
        item = new QTableWidgetItem;
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui_.PairPotentialsTable->setItem(row, 3, item);
    }
    else
        item = ui_.PairPotentialsTable->item(row, 3);
    item->setText(QString::number(pairPotential->chargeI()));

    // Charge I
    if (createItems)
    {
        item = new QTableWidgetItem;
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui_.PairPotentialsTable->setItem(row, 4, item);
    }
    else
        item = ui_.PairPotentialsTable->item(row, 4);
    item->setText(QString::number(pairPotential->chargeJ()));

    // Parameters
    auto col = 5;
    for (auto x : pairPotential->parameters())
    {
        if (createItems)
        {
            item = new QTableWidgetItem;
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            ui_.PairPotentialsTable->setItem(row, col++, item);
        }
        else
            item = ui_.PairPotentialsTable->item(row, col++);
        item->setText(QString::number(x));
    }
}

// Update controls in tab
void ForcefieldTab::updateControls()
{
    Locker refreshLocker(refreshLock_);

    // Master Bonds Table
    TableWidgetUpdater<ForcefieldTab, MasterIntra> bondsUpdater(ui_.MasterBondsTable, dissolve_.coreData().masterBonds(), this,
                                                                &ForcefieldTab::updateBondsTableRow);
    ui_.MasterBondsTable->resizeColumnsToContents();

    // Master Angles Table
    TableWidgetUpdater<ForcefieldTab, MasterIntra> anglesUpdater(ui_.MasterAnglesTable, dissolve_.coreData().masterAngles(),
                                                                 this, &ForcefieldTab::updateAnglesTableRow);
    ui_.MasterAnglesTable->resizeColumnsToContents();

    // Torsions Table
    TableWidgetUpdater<ForcefieldTab, MasterIntra> torsionsUpdater(
        ui_.MasterTorsionsTable, dissolve_.coreData().masterTorsions(), this, &ForcefieldTab::updateTorsionsTableRow);
    ui_.MasterTorsionsTable->resizeColumnsToContents();

    // Impropers Table
    TableWidgetUpdater<ForcefieldTab, MasterIntra> impropersUpdater(
        ui_.MasterImpropersTable, dissolve_.coreData().masterImpropers(), this, &ForcefieldTab::updateImpropersTableRow);
    ui_.MasterImpropersTable->resizeColumnsToContents();

    // AtomTypes Table
    TableWidgetUpdater<ForcefieldTab, AtomType, std::shared_ptr<AtomType>> atomTypesUpdater(
        ui_.AtomTypesTable, dissolve_.atomTypes(), this, &ForcefieldTab::updateAtomTypesTableRow);
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
    auto ppRowIndex = ui_.PairPotentialsTable->currentRow();
    TableWidgetUpdater<ForcefieldTab, PairPotential> ppUpdater(ui_.PairPotentialsTable, dissolve_.pairPotentials(), this,
                                                               &ForcefieldTab::updatePairPotentialsTableRow);
    ui_.PairPotentialsTable->resizeColumnsToContents();

    refreshLocker.unlock();

    // Re-set active row in pair potentials table
    ui_.PairPotentialsTable->setCurrentCell(ppRowIndex == -1 ? 0 : ppRowIndex, 0);
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

    TableWidgetUpdater<ForcefieldTab, AtomType, std::shared_ptr<AtomType>> atomTypesUpdater(
        ui_.AtomTypesTable, dissolve_.atomTypes(), this, &ForcefieldTab::updateAtomTypesTableRow);
    ui_.AtomTypesTable->resizeColumnsToContents();

    dissolveWindow_->setModified();
}

void ForcefieldTab::on_AtomTypeRemoveButton_clicked(bool checked) { Messenger::error("NOT IMPLEMENTED YET.\n"); }

void ForcefieldTab::on_AtomTypesTable_itemChanged(QTableWidgetItem *w)
{
    if (refreshLock_.isLocked())
        return;

    // Get target AtomType from the passed widget
    if (w == nullptr)
        return;

    std::shared_ptr<AtomType> atomType = w->data(Qt::UserRole).value<std::shared_ptr<AtomType>>();
    if (!atomType)
        return;

    // Column of passed item tells us the type of data we need to change
    switch (w->column())
    {
        // Name
        case (0):
            atomType->setName(qPrintable(w->text()));
            dissolveWindow_->setModified();
            break;
        // Charge
        case (2):
            atomType->setCharge(w->text().toDouble());
            atomTypeDataModified();
            dissolveWindow_->setModified();
            break;
        // Short-range form
        case (3):
            atomType->setShortRangeType(Forcefield::shortRangeTypes().enumeration(qPrintable(w->text())));
            atomTypeDataModified();
            break;
        // Parameters
        case (4):
        case (5):
        case (6):
        case (7):
            atomType->setShortRangeParameter(w->column() - 4, w->text().toDouble());
            atomTypeDataModified();
            dissolveWindow_->setModified();
            break;
        default:
            Messenger::error("Don't know what to do with data from column {} of AtomTypes table.\n", w->column());
            break;
    }
}

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

void ForcefieldTab::on_PairPotentialsTable_currentItemChanged(QTableWidgetItem *currentItem, QTableWidgetItem *previousItem)
{
    if (refreshLock_.isLocked())
        return;

    // Clear all data in the graph
    DataViewer *graph = ui_.PairPotentialsPlotWidget->dataViewer();
    graph->clearRenderables();

    if (!currentItem)
        return;

    // Get the selected pair potential
    PairPotential *pp = VariantPointer<PairPotential>(currentItem->data(Qt::UserRole));
    if (pp)
    {
        printf("IN here?\n");
        auto fullPotential = graph->createRenderable<RenderableData1D, Data1D>(pp->uFull(), "Full");
        fullPotential->setColour(StockColours::BlackStockColour);

        auto originalPotential = graph->createRenderable<RenderableData1D, Data1D>(pp->uOriginal(), "Original");
        originalPotential->setColour(StockColours::RedStockColour);
        originalPotential->lineStyle().set(1.0, LineStipple::HalfDashStipple);

        auto additionalPotential = graph->createRenderable<RenderableData1D, Data1D>(pp->uAdditional(), "Additional");
        additionalPotential->setColour(StockColours::BlueStockColour);
        additionalPotential->lineStyle().set(1.0, LineStipple::DotStipple);

        auto dUFull = graph->createRenderable<RenderableData1D, Data1D>(pp->dUFull(), "Force");
        dUFull->setColour(StockColours::GreenStockColour);
    }
}

void ForcefieldTab::on_PairPotentialsTable_itemChanged(QTableWidgetItem *w)
{
    if (refreshLock_.isLocked())
        return;

    // Get target PairPotential from the passed widget
    PairPotential *pairPotential =
        w ? ui_.PairPotentialsTable->item(w->row(), 0)->data(Qt::UserRole).value<PairPotential *>() : nullptr;
    if (!pairPotential)
        return;

    // Column of passed item tells us the type of data we need to change
    switch (w->column())
    {
        // Functional form
        case (2):
            dissolveWindow_->setModified();
            break;
        // Charge I
        case (3):
            pairPotential->setChargeI(w->text().toDouble());
            dissolveWindow_->setModified();
            break;
        // Charge J
        case (4):
            pairPotential->setChargeJ(w->text().toDouble());
            dissolveWindow_->setModified();
            break;
        // Parameters
        case (5):
        case (6):
        case (7):
        case (8):
            pairPotential->setParameter(w->column() - 5, w->text().toDouble());
            dissolveWindow_->setModified();
            break;
        default:
            Messenger::error("Don't know what to do with data from column {} of PairPotentials table.\n", w->column());
            break;
    }
}

void ForcefieldTab::on_MasterTermAddBondButton_clicked(bool checked) { Messenger::error("NOT IMPLEMENTED YET.\n"); }

void ForcefieldTab::on_MasterTermRemoveBondButton_clicked(bool checked) { Messenger::error("NOT IMPLEMENTED YET.\n"); }

void ForcefieldTab::on_MasterBondsTable_itemChanged(QTableWidgetItem *w)
{
    if (refreshLock_.isLocked())
        return;

    // Get target MasterIntra from the passed widget
    MasterIntra *masterIntra = w ? ui_.MasterBondsTable->item(w->row(), 0)->data(Qt::UserRole).value<MasterIntra *>() : nullptr;
    if (!masterIntra)
        return;

    // Column of passed item tells us the type of data we need to change
    switch (w->column())
    {
        // Name
        case (0):
            masterIntra->setName(qPrintable(w->text()));
            dissolveWindow_->setModified();
            break;
        // Functional Form
        case (1):
            masterIntra->setForm(SpeciesBond::bondFunctions().enumeration(qPrintable(w->text())));
            dissolveWindow_->setModified();
            break;
        // Parameters
        case (2):
        case (3):
        case (4):
        case (5):
            masterIntra->setParameter(w->column() - 2, w->text().toDouble());
            dissolveWindow_->setModified();
            break;
        default:
            Messenger::error("Don't know what to do with data from column {} of MasterIntra table.\n", w->column());
            break;
    }
}

void ForcefieldTab::on_MasterTermAddAngleButton_clicked(bool checked) { Messenger::error("NOT IMPLEMENTED YET.\n"); }

void ForcefieldTab::on_MasterTermRemoveAngleButton_clicked(bool checked) { Messenger::error("NOT IMPLEMENTED YET.\n"); }

void ForcefieldTab::on_MasterAnglesTable_itemChanged(QTableWidgetItem *w)
{
    if (refreshLock_.isLocked())
        return;

    // Get target MasterIntra from the passed widget
    MasterIntra *masterIntra =
        w ? ui_.MasterAnglesTable->item(w->row(), 0)->data(Qt::UserRole).value<MasterIntra *>() : nullptr;
    if (!masterIntra)
        return;

    // Column of passed item tells us the type of data we need to change
    switch (w->column())
    {
        // Name
        case (0):
            masterIntra->setName(qPrintable(w->text()));
            dissolveWindow_->setModified();
            break;
        // Functional Form
        case (1):
            masterIntra->setForm(SpeciesAngle::angleFunctions().enumeration(qPrintable(w->text())));
            dissolveWindow_->setModified();
            break;
        // Parameters
        case (2):
        case (3):
        case (4):
        case (5):
            masterIntra->setParameter(w->column() - 2, w->text().toDouble());
            dissolveWindow_->setModified();
            break;
        default:
            Messenger::error("Don't know what to do with data from column {} of MasterIntra table.\n", w->column());
            break;
    }
}

void ForcefieldTab::on_MasterTermAddTorsionButton_clicked(bool checked) { Messenger::error("NOT IMPLEMENTED YET.\n"); }

void ForcefieldTab::on_MasterTermRemoveTorsionButton_clicked(bool checked) { Messenger::error("NOT IMPLEMENTED YET.\n"); }

void ForcefieldTab::on_MasterTorsionsTable_itemChanged(QTableWidgetItem *w)
{
    if (refreshLock_.isLocked())
        return;

    // Get target MasterIntra from the passed widgetmasterIntra->setForm(SpeciesBond::bondFunction(qPrintable(w->text())));
    MasterIntra *masterIntra =
        w ? ui_.MasterTorsionsTable->item(w->row(), 0)->data(Qt::UserRole).value<MasterIntra *>() : nullptr;
    if (!masterIntra)
        return;

    // Column of passed item tells us the type of data we need to change
    switch (w->column())
    {
        // Name
        case (0):
            masterIntra->setName(qPrintable(w->text()));
            dissolveWindow_->setModified();
            break;
        // Functional Form
        case (1):
            masterIntra->setForm(SpeciesTorsion::torsionFunctions().enumeration(qPrintable(w->text())));
            dissolveWindow_->setModified();
            break;
        // Parameters
        case (2):
        case (3):
        case (4):
        case (5):
            masterIntra->setParameter(w->column() - 2, w->text().toDouble());
            dissolveWindow_->setModified();
            break;
        default:
            Messenger::error("Don't know what to do with data from column {} of MasterIntra table.\n", w->column());
            break;
    }
}

void ForcefieldTab::on_MasterTermAddImproperButton_clicked(bool checked) { Messenger::error("NOT IMPLEMENTED YET.\n"); }

void ForcefieldTab::on_MasterTermRemoveImproperButton_clicked(bool checked) { Messenger::error("NOT IMPLEMENTED YET.\n"); }

void ForcefieldTab::on_MasterImpropersTable_itemChanged(QTableWidgetItem *w)
{
    if (refreshLock_.isLocked())
        return;

    // Get target MasterIntra from the passed widgetmasterIntra->setForm(SpeciesBond::bondFunction(qPrintable(w->text())));
    MasterIntra *masterIntra =
        w ? ui_.MasterImpropersTable->item(w->row(), 0)->data(Qt::UserRole).value<MasterIntra *>() : nullptr;
    if (!masterIntra)
        return;

    // Column of passed item tells us the type of data we need to change
    switch (w->column())
    {
        // Name
        case (0):
            masterIntra->setName(qPrintable(w->text()));
            dissolveWindow_->setModified();
            break;
        // Functional Form
        case (1):
            masterIntra->setForm(SpeciesTorsion::torsionFunctions().enumeration(qPrintable(w->text())));
            dissolveWindow_->setModified();
            break;
        // Parameters
        case (2):
        case (3):
        case (4):
        case (5):
            masterIntra->setParameter(w->column() - 2, w->text().toDouble());
            dissolveWindow_->setModified();
            break;
        default:
            Messenger::error("Don't know what to do with data from column {} of MasterIntra table.\n", w->column());
            break;
    }
}
