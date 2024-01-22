// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/atomType.h"
#include "classes/empiricalFormula.h"
#include "gui/delegates/comboList.h"
#include "gui/delegates/customComboDelegate.h"
#include "gui/delegates/exponentialSpin.h"
#include "gui/delegates/intraFormCombo.h"
#include "gui/delegates/isotopeCombo.h"
#include "gui/delegates/null.h"
#include "gui/getSpeciesNameDialog.h"
#include "gui/gui.h"
#include "gui/speciesTab.h"
#include "main/dissolve.h"
#include <QMessageBox>

SpeciesTab::SpeciesTab(DissolveWindow *dissolveWindow, Dissolve &dissolve, MainTabsWidget *parent, const QString title,
                       Species *species)
    : MainTab(dissolveWindow, dissolve, parent, QString("Species: %1").arg(title), this), atoms_(*species, dissolve.coreData()),
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
    ui_.AtomTable->resizeColumnsToContents();

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
    ui_.ViewerWidget->setSite(species_->sites().empty() ? nullptr : species_->sites().front().get());
    ui_.ViewerWidget->speciesViewer()->setSiteVisible(false);

    // Hide frames
    ui_.IsotopologuesFrame->setVisible(false);
    ui_.SitesFrame->setVisible(false);

    // Connect signals / slots
    connect(ui_.ViewerWidget, SIGNAL(dataModified()), this, SLOT(updateControls()));
    connect(ui_.ViewerWidget, SIGNAL(dataModified()), dissolveWindow_, SLOT(setModified()));
    connect(ui_.ViewerWidget->speciesViewer(), SIGNAL(atomsChanged()), dissolveWindow_, SLOT(updateMenus()));
    connect(ui_.ViewerWidget->speciesViewer(), SIGNAL(atomsChanged()), this, SLOT(updateAtomTableSelection()));
    connect(ui_.ViewerWidget->speciesViewer(), SIGNAL(sitesChanged()), this, SLOT(updateSitesTab()));
    connect(ui_.ViewerWidget->speciesViewer(), SIGNAL(siteCreatedAndShown()), this, SLOT(setCurrentSiteFromViewer()));
    connect(ui_.ViewerWidget->speciesViewer(), SIGNAL(sitesChanged()), dissolveWindow_, SLOT(setModified()));

    connect(&atoms_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), this,
            SLOT(atomTableDataChanged(const QModelIndex &, const QModelIndex &)));
    connect(&atoms_, SIGNAL(atomTypeChanged()), this, SLOT(updateIsotopologuesTab()));
    connect(ui_.AtomTable->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this,
            SLOT(updateUnderlyingAtomSelection()));
    connect(&isos_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), dissolveWindow_, SLOT(setModified()));
    connect(&isos_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(updateControls()));
    connect(ui_.IsotopologuesTree->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(updateIsotopologuesTab()));
    connect(ui_.SpeciesButtonGroup, SIGNAL(buttonToggled(QAbstractButton *, bool)), this,
            SLOT(buttonGroupToggled(QAbstractButton *, bool)));
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

    // Update headers
    ui_.AtomsPushButton->setText(QString("Atoms (%1)").arg(species_->nAtoms()));
    ui_.IsotopologuesPushButton->setText(QString("Isotopologues (%1)").arg(species_->nIsotopologues()));
    ui_.SitesPushButton->setText(QString("Sites (%1)").arg(species_->nSites()));

    // Current Box
    const auto *box = species_->box();
    ui_.CurrentBoxTypeLabel->setText(QString::fromStdString(std::string(Box::boxTypes().keyword(box->type()))));
    QString boxInfo = QString("<b>A:</b>  %1 &#8491;<br>").arg(box->axisLengths().x);
    boxInfo += QString("<b>B:</b>  %1 &#8491;<br>").arg(box->axisLengths().y);
    boxInfo += QString("<b>C:</b>  %1 &#8491;<br>").arg(box->axisLengths().z);
    boxInfo += QString("<b>&#x3B1;:</b>  %1&#xb0;<br>").arg(box->axisAngles().x);
    boxInfo += QString("<b>&#x3B2;:</b>  %1&#xb0;<br>").arg(box->axisAngles().y);
    boxInfo += QString("<b>&#x3B3;:</b>  %1&#xb0;").arg(box->axisAngles().z);
    ui_.CurrentBoxFrame->setToolTip(boxInfo);
    updateDensityLabel();

    ui_.EmpiricalFormulaLabel->setText(QString::fromStdString(EmpiricalFormula::formula(
        species_->atoms(), [](const auto &i) { return i.Z(); }, true)));

    // Charges
    updateTotalCharges();

    // Atoms Tab
    ui_.BondsCountLabel->setText(QString::number(species_->nBonds()));
    ui_.AnglesCountLabel->setText(QString::number(species_->nAngles()));
    ui_.TorsionsCountLabel->setText(QString::number(species_->nTorsions()));
    ui_.ImpropersCountLabel->setText(QString::number(species_->nImpropers()));

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

// Button group toggled
void SpeciesTab::buttonGroupToggled(QAbstractButton *button, bool checked)
{
    if (button == ui_.AtomsPushButton)
    {
        ui_.AtomsFrame->setVisible(checked);
        if (checked)
            ui_.AtomTable->resizeColumnsToContents();
        ui_.IsotopologuesFrame->setVisible(!checked);
        ui_.SitesFrame->setVisible(!checked);
    }
    else if (button == ui_.IsotopologuesPushButton)
    {
        ui_.IsotopologuesFrame->setVisible(checked);
        ui_.AtomsFrame->setVisible(!checked);
        ui_.SitesFrame->setVisible(!checked);
    }
    else if (button == ui_.SitesPushButton)
    {
        ui_.SitesFrame->setVisible(checked);
        ui_.ViewerWidget->speciesViewer()->setSiteVisible(checked);
        ui_.AtomsFrame->setVisible(!checked);
        ui_.IsotopologuesFrame->setVisible(!checked);
    }
}
