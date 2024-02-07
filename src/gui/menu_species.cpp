// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/messenger.h"
#include "classes/species.h"
#include "gui/addForcefieldTermsDialog.h"
#include "gui/copySpeciesTermsDialog.h"
#include "gui/editSpeciesDialog.h"
#include "gui/gui.h"
#include "gui/importCIFDialog.h"
#include "gui/importLigParGenDialog.h"
#include "gui/importSpeciesDialog.h"
#include "gui/models/modifyChargesModel.h"
#include "gui/modifyChargesDialog.h"
#include "gui/selectAtomTypeDialog.h"
#include "gui/selectElementDialog.h"
#include "gui/selectSpeciesDialog.h"
#include "gui/speciesTab.h"
#include "io/import/species.h"
#include "math/sampledDouble.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QQuickView>
#include <qdialog.h>
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <valarray>

void DissolveWindow::on_SpeciesCreateAtomicAction_triggered(bool checked)
{
    // Raise an element selection dialog
    static SelectElementDialog selectElementDialog(this);
    auto Z = selectElementDialog.selectElement(Elements::Unknown);
    if (Z == Elements::Unknown)
        return;

    // Create the new Species, and add a single atom at {0,0,0}
    auto *newSpecies = dissolve_.coreData().addSpecies();
    newSpecies->addAtom(Z, Vec3<double>());
    newSpecies->setName(DissolveSys::uniqueName(Elements::symbol(Z), dissolve().coreData().species(),
                                                [&](const auto &sp) { return newSpecies == sp.get() ? "" : sp->name(); }));

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newSpecies);
}

void DissolveWindow::on_SpeciesCreateDrawAction_triggered(bool checked)
{
    auto *newSpecies = dissolve_.coreData().addSpecies();

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
        dissolve_.coreData().removeSpecies(newSpecies);
}

void DissolveWindow::on_SpeciesCreateFromExistingAction_triggered(bool checked)
{
    // Create a SpeciesSelectDialog and use it to get the Species to add to the mix
    SelectSpeciesDialog speciesSelectDialog(this, dissolve_.coreData(), "Select species to use as basis");

    auto baseSpecies = speciesSelectDialog.selectSpecies({}, 1, 1);
    if (baseSpecies.empty())
        return;

    // Copy the base species
    auto *newSpecies = dissolve_.coreData().copySpecies(baseSpecies.front());

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
        dissolve_.coreData().removeSpecies(newSpecies);
}

void DissolveWindow::on_SpeciesImportFromDissolveAction_triggered(bool checked)
{
    ImportSpeciesDialog importSpeciesDialog(this, dissolve_);

    if (importSpeciesDialog.exec() == QDialog::Accepted)
    {
        // Fully update GUI
        setModified();
        fullUpdate();

        ui_.MainTabs->setCurrentTab(dissolve_.coreData().species().back().get());
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
    auto *sp = dissolve_.coreData().addSpecies();
    SpeciesImportFileFormat importer(qPrintable(xyzFile));
    if (!importer.importData(sp))
    {
        dissolve_.coreData().removeSpecies(sp);
        return;
    }
    sp->addMissingBonds();

    // Offer the species up for editing before finalising
    EditSpeciesDialog editSpeciesDialog(this, sp);
    if (!editSpeciesDialog.editSpecies())
    {
        dissolve_.coreData().removeSpecies(sp);
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

        // Select the new species
        ui_.MainTabs->setCurrentTab(dissolve_.coreData().species().back().get());
    }
}

void DissolveWindow::on_SpeciesImportFromCIFAction_triggered(bool checked)
{
    ImportCIFDialog importCIFDialog(this, dissolve_);

    if (importCIFDialog.exec() == QDialog::Accepted)
    {
        // Fully update GUI
        setModified();
        fullUpdate();

        // Select the new species
        ui_.MainTabs->setCurrentTab(dissolve_.coreData().species().back().get());
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

    ui_.MainTabs->removeByPage(spTab->page());
    dissolve_.coreData().removeSpecies(spTab->species());

    // May now have unused atomtypes...
    dissolve_.coreData().removeUnusedAtomTypes();

    setModified({DissolveSignals::DataMutations::SpeciesMutated, DissolveSignals::DataMutations::IsotopologuesMutated});
    fullUpdate();
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
        // Atom types will likely have changed, so make sure the
        // Isotopologues in the species are up-to-date
        species->updateIsotopologues();

        // May now have unused atomtypes...
        dissolve_.coreData().removeUnusedAtomTypes();

        // Fully update GUI
        setModified();
        fullUpdate();
    }
}

void DissolveWindow::on_SpeciesCopyTermsAction_triggered(bool checked)
{
    // Get the current Species (if a SpeciesTab is selected)
    auto species = ui_.MainTabs->currentSpecies();
    if (!species)
        return;

    CopySpeciesTermsDialog copySpeciesTermsDialog(this, dissolve_, species);

    if (copySpeciesTermsDialog.exec() == QDialog::Accepted)
    {
        // Fully update GUI
        setModified();
        fullUpdate();
    }
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

        // May now have unused atomtypes...
        dissolve_.coreData().removeUnusedAtomTypes();

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

    species->removeHigherOrderIntramolecularTerms();
    species->updateIntramolecularTerms();

    setModified();
    ui_.MainTabs->currentTab()->updateControls();
}

void DissolveWindow::on_SpeciesSetAtomTypesInSelectionAction_triggered(bool checked)
{
    // Get the current Species (if a SpeciesTab is selected)
    auto species = ui_.MainTabs->currentSpecies();
    if (!species)
        return;

    // Check current selection
    if (species->selectedAtoms().empty() || !species->isSelectionSingleElement())
        return;

    SelectAtomTypeDialog atomTypeDialog(this, dissolve_.coreData(), "Select Atom Type");
    auto at = atomTypeDialog.selectAtomType(species->selectedAtoms().front()->Z());
    if (!at)
        return;

    // Set atom types in selection - we also need to update Isotopologues afterwards
    for (auto *i : species->modifiableSelectedAtoms())
        i->setAtomType(at);
    species->updateIsotopologues();

    setModified();

    fullUpdate();
}

void DissolveWindow::on_SpeciesSetChargesInSelectionAction_triggered(bool checked)
{
    // Get the current Species (if a SpeciesTab is selected)
    auto species = ui_.MainTabs->currentSpecies();
    if (!species)
        return;

    // Check current selection
    if (species->selectedAtoms().empty())
        return;

    auto ok = false;
    auto q = QInputDialog::getDouble(this, "Set atom charges", "Enter the charge (per atom) to apply to the selection", 0.0,
                                     -100.0, 100.0, 5, &ok);
    if (!ok)
        return;

    for (auto *i : species->modifiableSelectedAtoms())
        i->setCharge(q);

    setModified();

    fullUpdate();
}

void DissolveWindow::on_SpeciesCopyChargesFromAtomTypesAction_triggered(bool checked)
{
    // Get the current Species (if a SpeciesTab is selected)
    auto species = ui_.MainTabs->currentSpecies();
    if (!species)
        return;

    if (QMessageBox::warning(this, "Copy Charges from Atom Types",
                             "This will replace the species atom charges "
                             "with those of the assigned atom types.\n\n"
                             "This cannot be undone! Proceed?",
                             QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No,
                             QMessageBox::StandardButton::No) == QMessageBox::StandardButton::Yes)
    {
        for (auto &atom : species->atoms())
            if (atom.atomType())
                atom.setCharge(atom.atomType()->charge());

        setModified();

        fullUpdate();
    }
}

void DissolveWindow::on_SpeciesSetAtomTypeChargesFromSpeciesAction_triggered(bool checked)
{
    // Get the current Species (if a SpeciesTab is selected)
    auto species = ui_.MainTabs->currentSpecies();
    if (!species)
        return;

    std::map<std::shared_ptr<AtomType>, SampledDouble> charges;
    for (auto &atom : species->atoms())
        charges[atom.atomType()] += atom.charge();

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Set Atom Type Charges from Species Atoms");
    msgBox.setText("This will replace the current atom type charges "
                   "with the average charge of relevant species atoms.\n\n"
                   "This cannot be undone! Proceed?");
    msgBox.addButton(QMessageBox::Yes);
    msgBox.addButton(QMessageBox::No);
    msgBox.addButton("Test", QMessageBox::ActionRole);

    auto result = msgBox.exec();

    if (result == QMessageBox::No)
        return;

    Messenger::banner("Proposed atom type charges, averaged from {}", species->name());
    auto atomTypes = dissolve().coreData().atomTypes();
    for (auto &atomType : atomTypes)
    {
        if (charges[atomType].count() > 0)
        {
            Messenger::print("{}: {} -> {} \u00b1 {} [Averaged from {} atoms]", atomType->name(), atomType->charge(),
                             charges[atomType].value(), charges[atomType].stDev(), charges[atomType].count());
            if (result == QMessageBox::Yes)
                atomType->setCharge(charges[atomType].value());
        }
    }

    if (result == QMessageBox::Yes)
        setModified();
    fullUpdate();
}

void DissolveWindow::on_SpeciesScaleChargesAction_triggered(bool checked)
{
    // Get the current Species (if a SpeciesTab is selected)
    auto species = ui_.MainTabs->currentSpecies();
    if (!species)
        return;

    ModifyChargesDialog dialog(this, &modifyChargesModel_, ModifyChargesModel::Scaling);
    if (dialog.exec() == QDialog::Accepted)
    {
        auto success = modifyChargesModel.scale(species, true);
        setModified();
        fullUpdate();
    }
}

void DissolveWindow::on_SpeciesSmoothChargesAction_triggered(bool checked)
{
    // Get the current Species (if a SpeciesTab is selected)
    auto species = ui_.MainTabs->currentSpecies();
    if (!species)
        return;

    ModifyChargesDialog dialog(this, &modifyChargesModel_, ModifyChargesModel::Smoothing);
    if (dialog.exec() == QDialog::Accepted)
    {
        modifyChargesModel.smooth(species);
        setModified();
        fullUpdate();
    }
}

void DissolveWindow::on_SpeciesReduceChargesSigFigsAction_triggered(bool checked)
{
    // Get the current Species (if a SpeciesTab is selected)
    auto species = ui_.MainTabs->currentSpecies();
    if (!species)
        return;

    ModifyChargesDialog dialog(this, &modifyChargesModel_, ModifyChargesModel::ReduceSigFig);
    if (dialog.exec() == QDialog::Accepted)
    {
        modifyChargesModel.reduceSignificantFigures(species);
        setModified();
        fullUpdate();
    }
}
