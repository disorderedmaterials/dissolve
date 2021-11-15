// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/species.h"
#include "gui/addforcefieldtermsdialog.h"
#include "gui/editspeciesdialog.h"
#include "gui/gui.h"
#include "gui/importligpargendialog.h"
#include "gui/importspeciesdialog.h"
#include "gui/selectelementdialog.h"
#include "gui/speciestab.h"
#include <QFileDialog>
#include <QMessageBox>

void DissolveWindow::on_SpeciesCreateAtomicAction_triggered(bool checked)
{
    // Raise an element selection dialog
    static SelectElementDialog selectElementDialog(this);
    auto Z = selectElementDialog.selectElement(Elements::Unknown);
    if (Z == Elements::Unknown)
        return;

    // Create the new Species, and add a single atom at {0,0,0}
    auto *newSpecies = dissolve_.addSpecies();
    newSpecies->addAtom(Z, Vec3<double>());
    newSpecies->setName(dissolve_.coreData().uniqueSpeciesName(Elements::symbol(Z)));

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newSpecies);
}

void DissolveWindow::on_SpeciesCreateDrawAction_triggered(bool checked)
{
    auto *newSpecies = dissolve_.addSpecies();

    EditSpeciesDialog editSpeciesDialog(this, newSpecies);
    if (editSpeciesDialog.editSpecies())
    {
        // Renumber the atoms so they are sequential
        newSpecies->renumberAtoms();
        newSpecies->updateIntramolecularTerms();

        setModified();
        fullUpdate();
        ui_.MainTabs->setCurrentTab(newSpecies);
    }
    else
        dissolve_.removeSpecies(newSpecies);
}

void DissolveWindow::on_SpeciesImportFromDissolveAction_triggered(bool checked)
{
    ImportSpeciesDialog importSpeciesDialog(this, dissolve_);

    if (importSpeciesDialog.exec() == QDialog::Accepted)
    {
        // Fully update GUI
        setModified();
        fullUpdate();

        ui_.MainTabs->setCurrentTab(dissolve_.species().back().get());
    }
}

void DissolveWindow::on_SpeciesImportFromXYZAction_triggered(bool checked)
{
    // Request a new file to open
    QString xyzFile =
        QFileDialog::getOpenFileName(this, "Choose XYZ file to open", QDir().absolutePath(), "XYZ Coordinates (*.xyz)");
    if (xyzFile.isEmpty())
        return;

    // Add new species, load from the xyz, and create intramolecular terms
    auto *sp = dissolve_.addSpecies();
    sp->loadFromXYZ(qPrintable(xyzFile));
    sp->addMissingBonds();

    // Offer the species up for editing before finalising
    EditSpeciesDialog editSpeciesDialog(this, sp);
    if (!editSpeciesDialog.editSpecies())
    {
        dissolve_.removeSpecies(sp);
        return;
    }

    // Add higher-order connectivity
    sp->updateIntramolecularTerms();

    // Fully update GUI
    setModified();
    fullUpdate();

    ui_.MainTabs->setCurrentTab(sp);
}

void DissolveWindow::on_SpeciesImportLigParGenAction_triggered(bool checked)
{
    ImportLigParGenDialog importLigParGenDialog(this, dissolve_);

    if (importLigParGenDialog.exec() == QDialog::Accepted)
    {
        // Fully update GUI
        setModified();
        fullUpdate();
    }
}

void DissolveWindow::on_SpeciesRenameAction_triggered(bool checked)
{
    // Get the current tab - make sure it is a SpeciesTab, then call its rename() function
    auto tab = ui_.MainTabs->currentTab();
    if ((!tab) || (tab->type() != MainTab::TabType::Species))
        return;
    tab->rename();
}

void DissolveWindow::on_SpeciesAddForcefieldTermsAction_triggered(bool checked)
{
    // Get the current Species (if a SpeciesTab is selected)
    auto species = ui_.MainTabs->currentSpecies();
    if (!species)
        return;

    AddForcefieldTermsDialog addForcefieldTermsDialog(this, dissolve_, species);

    if (addForcefieldTermsDialog.exec() == QDialog::Accepted)
    {
        // Fully update GUI
        setModified();
        fullUpdate();
    }
}

void DissolveWindow::on_SpeciesRegenerateIntraFromConnectivityAction_triggered(bool checked)
{
    // Get the current Species (if a SpeciesTab is selected)
    auto species = ui_.MainTabs->currentSpecies();
    if (!species)
        return;

    // Confirm with the user
    QMessageBox queryBox;
    queryBox.setText(
        "This will delete and regenerate all angle and torsion terms, and any defined improper terms.\nThis cannot be undone!");
    queryBox.setInformativeText("Proceed?");
    queryBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    queryBox.setDefaultButton(QMessageBox::No);
    auto ret = queryBox.exec();

    if (ret != QMessageBox::Yes)
        return;

    species->updateIntramolecularTerms();

    setModified();
    updateWindowTitle();
    ui_.MainTabs->currentTab()->updateControls();
}

void DissolveWindow::on_SpeciesSimplifyAtomTypesAction_triggered(bool checked)
{
    // Get the current Species (if a SpeciesTab is selected)
    auto *species = ui_.MainTabs->currentSpecies();
    if (!species)
        return;

    auto nModified = species->simplifyAtomTypes();
    if (nModified > 0)
    {
        Messenger::print("{} atom types were modified.\n", nModified);
        setModified();
        fullUpdate();
    }
    else
        Messenger::print("No atom types changed.\n");
}

void DissolveWindow::on_SpeciesReduceToMasterTermsAction_triggered(bool checked)
{
    // Get the current Species (if a SpeciesTab is selected)
    auto *species = ui_.MainTabs->currentSpecies();
    if (!species)
        return;

    species->reduceToMasterTerms(dissolve_.coreData());

    setModified();
    fullUpdate();
}

void DissolveWindow::on_SpeciesDeleteAction_triggered(bool checked)
{
    // Get the current tab - make sure it is a SpeciesTab
    auto tab = ui_.MainTabs->currentTab();
    if ((!tab) || (tab->type() != MainTab::TabType::Species))
        return;

    // Cast up the tab to a SpeciesTab
    auto spTab = dynamic_cast<SpeciesTab *>(tab);
    if (!spTab)
        return;

    // Check that we really want to delete the Species
    if (!spTab->canClose())
        return;

    // Update the GUI
    ui_.MainTabs->removeByPage(spTab->page());
    setModified();
    fullUpdate();
}
