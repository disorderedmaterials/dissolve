// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/species.h"
#include "gui/addforcefieldtermsdialog.h"
#include "gui/editspeciesdialog.h"
#include "gui/gui.h"
#include "gui/importforcefieldwizard.h"
#include "gui/importspeciesdialog.h"
#include "gui/selectelementdialog.h"
#include "gui/speciestab.h"
#include <QFileDialog>

void DissolveWindow::on_SpeciesCreateAtomicAction_triggered(bool checked)
{
    // Raise an element selection dialog
    static SelectElementDialog selectElementDialog(this);
    auto Z = selectElementDialog.selectElement(Elements::Unknown);
    if (Z == Elements::Unknown)
        return;

    // Create the new Species, and add a single atom at {0,0,0}
    Species *newSpecies = dissolve_.addSpecies();
    newSpecies->addAtom(Z, Vec3<double>());
    newSpecies->setName(dissolve_.coreData().uniqueSpeciesName(Elements::symbol(Z)));

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newSpecies);
}

void DissolveWindow::on_SpeciesCreateDrawAction_triggered(bool checked)
{
    Species *newSpecies = dissolve_.addSpecies();

    EditSpeciesDialog editSpeciesDialog(this, newSpecies);
    if (editSpeciesDialog.editSpecies())
    {
        // Renumber the atoms so they are sequential
        newSpecies->renumberAtoms();

        setModified();
        fullUpdate();
        ui_.MainTabs->setCurrentTab(newSpecies);
    }
    else
        dissolve_.removeSpecies(newSpecies);
}

void DissolveWindow::on_SpeciesImportFromDissolveAction_triggered(bool checked)
{
    static ImportSpeciesDialog importSpeciesDialog(this, dissolve_);

    importSpeciesDialog.reset();

    if (importSpeciesDialog.exec() == QDialog::Accepted)
    {
        Species *sp = importSpeciesDialog.importSpecies(dissolve_);

        // Fully update GUI
        setModified();
        fullUpdate();

        ui_.MainTabs->setCurrentTab(sp);
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
    Species *sp = dissolve_.addSpecies();
    sp->loadFromXYZ(qPrintable(xyzFile));
    sp->addMissingBonds();

    // Offer the species up for editing before finalising
    EditSpeciesDialog editSpeciesDialog(this, sp);
    if (!editSpeciesDialog.editSpecies())
    {
        dissolve_.removeSpecies(sp);
        return;
    }

    // Fully update GUI
    setModified();
    fullUpdate();

    ui_.MainTabs->setCurrentTab(sp);
}

void DissolveWindow::on_SpeciesRenameAction_triggered(bool checked)
{
    // Get the current tab - make sure it is a SpeciesTab, then call its rename() function
    MainTab *tab = ui_.MainTabs->currentTab();
    if ((!tab) || (tab->type() != MainTab::SpeciesTabType))
        return;
    tab->rename();
}

void DissolveWindow::on_SpeciesAddForcefieldTermsAction_triggered(bool checked)
{
    // Get the current Species (if a SpeciesTab is selected)
    Species *species = ui_.MainTabs->currentSpecies();
    if (!species)
        return;

    static AddForcefieldTermsDialog addForcefieldTermsDialog(this, dissolve_);

    addForcefieldTermsDialog.reset();
    addForcefieldTermsDialog.setTargetSpecies(species);

    if (addForcefieldTermsDialog.exec() == QDialog::Accepted)
    {
        addForcefieldTermsDialog.applyForcefieldTerms(dissolve_);

        // Fully update GUI
        setModified();
        fullUpdate();
    }
}

void DissolveWindow::on_ImportForcefieldAction_triggered(bool checked)
{
    // Get the current Species (if a SpeciesTab is selected)
    ImportForcefieldWizard importForcefieldWizardDialog(this, dissolve_);

    importForcefieldWizardDialog.reset();

    if (importForcefieldWizardDialog.exec() == QDialog::Accepted)
    {
        // Fully update GUI
        setModified();
        fullUpdate();
    }
}

void DissolveWindow::on_SpeciesDeleteAction_triggered(bool checked)
{
    // Get the current tab - make sure it is a SpeciesTab
    MainTab *tab = ui_.MainTabs->currentTab();
    if ((!tab) || (tab->type() != MainTab::SpeciesTabType))
        return;

    // Cast up the tab to a SpeciesTab
    SpeciesTab *spTab = dynamic_cast<SpeciesTab *>(tab);
    if (!spTab)
        return;

    // Check that we really want to delete the Species
    if (!spTab->close())
        return;

    // Update the GUI
    ui_.MainTabs->removeByPage(spTab->page());
    setModified();
    fullUpdate();
}
