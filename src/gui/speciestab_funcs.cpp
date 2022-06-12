// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "classes/atomtype.h"
#include "gui/delegates/combolist.hui"
#include "gui/delegates/customcombodelegate.h"
#include "gui/delegates/exponentialspin.hui"
#include "gui/delegates/intraformcombo.h"
#include "gui/delegates/isotopecombo.hui"
#include "gui/delegates/null.h"
#include "gui/getspeciesnamedialog.h"
#include "gui/gui.h"
#include "gui/speciestab.h"
#include "main/dissolve.h"
#include <QMessageBox>

SpeciesTab::SpeciesTab(DissolveWindow *dissolveWindow, Dissolve &dissolve, MainTabsWidget *parent, const QString title,
                       Species *species)
    : MainTab(dissolveWindow, dissolve, parent, QString("Species: %1").arg(title), this), atoms_(*species, dissolve),
      angles_(species->angles(), dissolve.coreData()), bonds_(species->bonds(), dissolve.coreData()),
      torsions_(species->torsions(), dissolve.coreData()), impropers_(species->impropers(), dissolve.coreData()),
      isos_(*species), sites_(species->sites())
{
    ui_.setupUi(this);

    Locker refreshLocker(refreshLock_);

    species_ = species;

    // Icons for charge warnings
    ui_.TotalAtomChargeIndicator->setWarning();
    ui_.TotalAtomTypeChargeIndicator->setWarning();

    // Populate density units combo
    ComboEnumOptionsPopulator(ui_.DensityUnitsCombo, Units::densityUnits());

    // Set item delegates
    // -- SpeciesAtomTable
    ui_.AtomTable->setItemDelegateForColumn(1, new CustomComboDelegate<SpeciesTab>(this, &SpeciesTab::validAtomTypeNames));
    for (auto n = 2; n < 6; ++n)
        ui_.AtomTable->setItemDelegateForColumn(n, new ExponentialSpinDelegate(this));
    // -- Geometry tables
    ui_.BondTable->setItemDelegateForColumn(
        2, new IntraFormComboDelegate(this, new ComboEnumOptionsItems<BondFunctions::Form>(BondFunctions::forms()),
                                      dissolve.coreData().masterBonds()));
    ui_.AngleTable->setItemDelegateForColumn(
        3, new IntraFormComboDelegate(this, new ComboEnumOptionsItems<AngleFunctions::Form>(AngleFunctions::forms()),
                                      dissolve.coreData().masterAngles()));
    ui_.TorsionTable->setItemDelegateForColumn(
        4, new IntraFormComboDelegate(this, new ComboEnumOptionsItems<TorsionFunctions::Form>(TorsionFunctions::forms()),
                                      dissolve.coreData().masterTorsions()));
    ui_.ImproperTable->setItemDelegateForColumn(
        4, new IntraFormComboDelegate(this, new ComboEnumOptionsItems<TorsionFunctions::Form>(TorsionFunctions::forms()),
                                      dissolve.coreData().masterImpropers()));
    // -- Isotopologues Tree
    ui_.IsotopologuesTree->setModel(&isos_);
    connect(&isos_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)), this,
            SLOT(isotopologuesChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)));
    connect(ui_.IsotopologuesTree->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(isotopologuesSelectionChanged(const QItemSelection &, const QItemSelection &)));
    ui_.IsotopologuesTree->setItemDelegateForColumn(1, new NullDelegate(this));
    ui_.IsotopologuesTree->setItemDelegateForColumn(2, new IsotopeComboDelegate(this));

    // Set models and ensure fonts for table headers are set correctly and the headers themselves are visible
    ui_.AtomTable->setModel(&atoms_);
    ui_.AtomTable->horizontalHeader()->setFont(font());
    ui_.AtomTable->horizontalHeader()->setVisible(true);
    ui_.AtomTable->verticalHeader()->setFont(font());
    ui_.AtomTable->verticalHeader()->setVisible(true);
    ui_.BondTable->setModel(&bonds_);
    ui_.BondTable->horizontalHeader()->setFont(font());
    ui_.BondTable->horizontalHeader()->setVisible(true);
    ui_.AngleTable->setModel(&angles_);
    ui_.AngleTable->horizontalHeader()->setFont(font());
    ui_.AngleTable->horizontalHeader()->setVisible(true);
    ui_.TorsionTable->setModel(&torsions_);
    ui_.TorsionTable->horizontalHeader()->setFont(font());
    ui_.TorsionTable->horizontalHeader()->setVisible(true);
    ui_.ImproperTable->setModel(&impropers_);
    ui_.ImproperTable->horizontalHeader()->setFont(font());
    ui_.ImproperTable->horizontalHeader()->setVisible(true);

    // Set sites model and connect signals
    ui_.SiteList->setModel(&sites_);
    connect(&sites_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)), dissolveWindow,
            SLOT(setModified()));
    connect(ui_.SiteList->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this,
            SLOT(siteSelectionChanged(const QItemSelection &, const QItemSelection &)));

    // Set up SpeciesViewer
    ui_.ViewerWidget->speciesViewer()->setDissolveWindow(dissolveWindow_);
    ui_.ViewerWidget->setDissolve(&dissolve);
    ui_.ViewerWidget->setSpecies(species_);

    // Set up SiteViewer
    ui_.SiteViewerWidget->setCoreData(&dissolve.coreData());
    ui_.SiteViewerWidget->setSpecies(species_);

    // Connect signals / slots
    connect(ui_.ViewerWidget, SIGNAL(dataModified()), this, SLOT(updateControls()));
    connect(ui_.ViewerWidget, SIGNAL(dataModified()), dissolveWindow_, SLOT(setModified()));
    connect(ui_.ViewerWidget->speciesViewer(), SIGNAL(atomsChanged()), dissolveWindow_, SLOT(updateMenus()));
    connect(ui_.ViewerWidget->speciesViewer(), SIGNAL(atomsChanged()), this, SLOT(updateAtomTableSelection()));
    connect(ui_.SiteViewerWidget, SIGNAL(dataModified()), this, SLOT(updateSitesTab()));
    connect(ui_.SiteViewerWidget, SIGNAL(siteCreatedAndShown()), this, SLOT(setCurrentSiteFromViewer()));
    connect(ui_.SiteViewerWidget, SIGNAL(dataModified()), dissolveWindow_, SLOT(setModified()));

    connect(&atoms_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), this,
            SLOT(atomTableDataChanged(const QModelIndex &, const QModelIndex &)));
    connect(&atoms_, SIGNAL(atomTypeChanged()), this, SLOT(updateIsotopologuesTab()));
    connect(ui_.AtomTable->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this,
            SLOT(updateUnderlyingAtomSelection()));
    connect(&bonds_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), dissolveWindow_, SLOT(setModified()));
    connect(&angles_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), dissolveWindow_, SLOT(setModified()));
    connect(&torsions_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), dissolveWindow_, SLOT(setModified()));
    connect(&impropers_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), dissolveWindow_, SLOT(setModified()));
    connect(&isos_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), dissolveWindow_, SLOT(setModified()));

    connect(ui_.IsotopologuesTree->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(updateIsotopologuesTab()));
}

/*
 * Widget Functions - Structure
 */

// Current Box
void SpeciesTab::on_DensityUnitsCombo_currentIndexChanged(int index) { updateDensityLabel(); }

/*
 * UI
 */

// Update density label
void SpeciesTab::updateDensityLabel()
{
    if (!species_ || species_->box()->type() == Box::BoxType::NonPeriodic)
        ui_.DensityUnitsLabel->setText("N/A");
    else
        ui_.DensityUnitsLabel->setText(QString::number(
            ui_.DensityUnitsCombo->currentIndex() == 0 ? species_->nAtoms() / species_->box()->volume()
                                                       : (species_->mass() / AVOGADRO) / (species_->box()->volume() / 1.0E24)));
}

// Update controls in tab
void SpeciesTab::updateControls()
{
    Locker refreshLocker(refreshLock_);

    // Structure Tab
    ui_.BoxWidget->setVisible(species_->box()->type() != Box::BoxType::NonPeriodic);
    if (species_->box()->type() != Box::BoxType::NonPeriodic)
    {
        const auto *box = species_->box();
        ui_.CurrentBoxTypeLabel->setText(QString::fromStdString(std::string(Box::boxTypes().keyword(box->type()))));
        ui_.CurrentBoxALabel->setText(QString::number(box->axisLengths().x));
        ui_.CurrentBoxBLabel->setText(QString::number(box->axisLengths().y));
        ui_.CurrentBoxCLabel->setText(QString::number(box->axisLengths().z));
        ui_.CurrentBoxAlphaLabel->setText(QString::number(box->axisAngles().x));
        ui_.CurrentBoxBetaLabel->setText(QString::number(box->axisAngles().y));
        ui_.CurrentBoxGammaLabel->setText(QString::number(box->axisAngles().z));
    }
    ui_.ViewerWidget->postRedisplay();

    // Contents / Forcefield Tab
    updateTotalCharges();
    updateGeometryTables();

    // Isotopologues Tab
    updateIsotopologuesTab();

    // Sites Tab
    updateSitesTab();

    speciesVersion_ = species_->version();
}

// Prevent editing within tab
void SpeciesTab::preventEditing() { setEnabled(false); }

// Allow editing within tab
void SpeciesTab::allowEditing() { setEnabled(true); }

/*
 * MainTab Reimplementations
 */

// Return tab type
MainTab::TabType SpeciesTab::type() const { return MainTab::TabType::Species; }

// Raise suitable dialog for entering / checking new tab name
QString SpeciesTab::getNewTitle(bool &ok)
{
    // Get a new, valid name for the Species
    GetSpeciesNameDialog nameDialog(this, dissolve_.coreData());
    ok = nameDialog.get(species_, QString::fromStdString(std::string(species_->name())));

    if (ok)
    {
        // Rename our Species, and flag that our data has been modified
        species_->setName(qPrintable(nameDialog.newName()));

        dissolveWindow_->setModified();
    }

    return nameDialog.newName();
}

// Return whether the title of the tab can be changed
bool SpeciesTab::canChangeTitle() const { return true; }

// Return whether the tab can be closed (after any necessary user querying, etc.)
bool SpeciesTab::canClose() const
{
    // Check that we really want to delete this tab
    QMessageBox queryBox;
    queryBox.setText(QString("Really delete the species '%1'?\nThis cannot be undone!")
                         .arg(QString::fromStdString(std::string(species_->name()))));
    queryBox.setInformativeText("Proceed?");
    queryBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    queryBox.setDefaultButton(QMessageBox::No);
    auto ret = queryBox.exec();

    if (ret != QMessageBox::Yes)
        return false;

    return true;
}

/*
 * Species Target
 */

// Return displayed Species
Species *SpeciesTab::species() const { return species_; }
