// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "base/messenger.h"
#include "classes/species.h"
#include "gui/addforcefieldtermsdialog.h"
#include "gui/copyspeciestermsdialog.h"
#include "gui/editspeciesdialog.h"
#include "gui/gui.h"
#include "gui/importcifdialog.h"
#include "gui/importligpargendialog.h"
#include "gui/importspeciesdialog.h"
#include "gui/selectatomtypedialog.h"
#include "gui/selectelementdialog.h"
#include "gui/selectspeciesdialog.h"
#include "gui/speciestab.h"
#include "io/import/species.h"
#include "math/sampleddouble.h"
#include "algorithm"
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <algorithm>
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <iostream>
#include <sstream>

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
    newSpecies->setName(DissolveSys::uniqueName(Elements::symbol(Z), dissolve().coreData().species(),
                                                [&](const auto &sp)
                                                { return newSpecies == sp.get() ? std::string() : sp->name(); }));

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

void DissolveWindow::on_SpeciesCreateFromExistingAction_triggered(bool checked)
{
    // Create a SpeciesSelectDialog and use it to get the Species to add to the mix
    SelectSpeciesDialog speciesSelectDialog(this, dissolve_.coreData(), "Select species to use as basis");

    auto baseSpecies = speciesSelectDialog.selectSpecies({}, 1, 1);
    if (baseSpecies.empty())
        return;

    // Copy the base species
    auto *newSpecies = dissolve_.copySpecies(baseSpecies.front());

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
    SpeciesImportFileFormat importer(qPrintable(xyzFile));
    if (!importer.importData(sp))
    {
        dissolve_.removeSpecies(sp);
        return;
    }
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

        // Select the new species
        ui_.MainTabs->setCurrentTab(dissolve_.species().back().get());
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
        ui_.MainTabs->setCurrentTab(dissolve_.species().back().get());
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
    dissolve_.removeSpecies(spTab->species());

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
        // Atom types will likely have changed, so make sure the Isotopologues in the species are up-to-date
        species->updateIsotopologues();

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
                             "This will replace the species charges "
                             "with those of the corresponding Atom Types.\n\n"
                             "This cannot be undone! Proceed?",
                             QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No, QMessageBox::StandardButton::No) == QMessageBox::StandardButton::Yes)
    {
        auto atomTypes = dissolve().coreData().atomTypes();
        for (auto& atom : species->atoms())
        {
            bool found = false;
            for (auto& at : atomTypes)
            {
                if (at == atom.atomType())
                {
                    found = true;
                    atom.setCharge(at->charge());
                }
            }
        }

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
    auto atomTypes = dissolve().coreData().atomTypes();
    for (auto& atom : species->atoms())
    {
        for (auto& at : atomTypes)
        {
            if (at == atom.atomType())
            {
                charges[at]+=atom.charge();
            }
        }
    }
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Copy Charges from Atom Types");
    msgBox.setText("This will replace the species charges "
                   "with those of the corresponding Atom Types.\n\n"
                   "This cannot be undone! Proceed?");
    msgBox.addButton(QMessageBox::Yes);
    msgBox.addButton(QMessageBox::No);
    QPushButton *testButton = msgBox.addButton("Test", QMessageBox::ActionRole);

    auto result = msgBox.exec();

    if (result == QMessageBox::Yes)
    {
        for (auto const&x : charges)
            x.first->setCharge(x.second.value());
    }
    else if (result == QMessageBox::ActionRole) {
        for (auto const&x : charges)
        {    
            Messenger::print("{}: {} -> {}", x.first->name(), x.first->charge(), x.second.value());
        }
    }
}

void DissolveWindow::on_SpeciesScaleChargesAction_triggered(bool checked)
{
    // Get the current Species (if a SpeciesTab is selected)
    auto species = ui_.MainTabs->currentSpecies();
    if (!species)
        return;

    auto ok = false;
    static auto scaleFactor = 1.0;
    auto newScaleFactor = QInputDialog::getDouble(this, "Scale atom charges", "Enter the scale factor to apply to all atoms",
                                                  scaleFactor, -100.0, 100.0, 5, &ok);
    if (!ok)
        return;

    scaleFactor = newScaleFactor;
    for (auto &i : species->atoms())
        i.setCharge(scaleFactor * i.charge());

    setModified();

    fullUpdate();
}

void DissolveWindow::on_SpeciesReduceChargesSigFigsAction_triggered(bool checked)
{
    // Get the current Species (if a SpeciesTab is selected)
    auto species = ui_.MainTabs->currentSpecies();
    if (!species)
        return;

    auto ok = false;
    auto significantFigures = QInputDialog::getInt(this, "Reduce Signficant Figures in Charges", "Enter the number of significant figures to use for all atoms",
                                                   1, 1, 100, 1, &ok);
    if (!ok)
        return;

    for (auto &atom : species->atoms())
    {
        double rounded = std::round(atom.charge() * std::pow(10, significantFigures)) / std::pow(10, significantFigures); 
        atom.setCharge(rounded);
    }
}
