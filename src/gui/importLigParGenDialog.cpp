// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/importLigParGenDialog.h"
#include "data/ff/library.h"
#include "templates/algorithms.h"
#include <QFileDialog>
#include <QFileInfo>
#include <pugixml.hpp>

ImportLigParGenDialog::ImportLigParGenDialog(QWidget *parent, Dissolve &dissolve)
    : WizardDialog(parent), xmlFFModel_(dissolve.coreData()), dissolve_(dissolve), importedSpecies_(nullptr),
      importedForcefield_(nullptr)
{
    ui_.setupUi(this);

    // Create a temporary species to use when importing
    importedSpecies_ = temporaryCoreData_.addSpecies();

    // Set model for ff terms tree
    ui_.xmlTree->setModel(&xmlFFModel_);

    // Set model for the atom type list
    ui_.AtomTypeList->setModel(&atomTypeModel_);

    // Hide site actions in the SpeciesViewer
    ui_.SpeciesView->setSiteActionsVisible(false);

    // Register pages with the wizard
    registerPage(ImportLigParGenDialog::SelectFilesPage, "Choose XML and/or XYZ Files");
    registerPage(ImportLigParGenDialog::PreviewSpeciesPage, "Imported Coordinates");
    registerPage(ImportLigParGenDialog::PreviewTermsPage, "Imported Terms", ImportLigParGenDialog::AtomTypesMasterTermsPage);
    registerPage(ImportLigParGenDialog::AtomTypesMasterTermsPage, "Atom Types / Master Terms",
                 ImportLigParGenDialog::ApplyForcefieldPage);
    registerPage(ImportLigParGenDialog::ApplyForcefieldPage, "Apply Forcefield?");

    // Init the wizard
    initialise(this, ui_.MainStack, ImportLigParGenDialog::SelectFilesPage);
}

/*
 * Wizard
 */

// Return whether progression to the next page from the current page is allowed
bool ImportLigParGenDialog::progressionAllowed(int index) const
{
    // Check widget validity in the specified page, returning if progression (i.e. pushing 'Next' or 'Finish') is allowed
    switch (index)
    {
        case (ImportLigParGenDialog::SelectFilesPage):
            return (((!ui_.InputXMLEdit->text().isEmpty()) && QFile::exists(ui_.InputXMLEdit->text())) ||
                    ((!ui_.InputXYZEdit->text().isEmpty()) && QFile::exists(ui_.InputXYZEdit->text())));
        default:
            break;
    }

    return true;
}

// Perform any necessary actions before moving to the next page
bool ImportLigParGenDialog::prepareForNextPage(int currentIndex)
{
    pugi::xml_document doc;

    switch (currentIndex)
    {
        case (ImportLigParGenDialog::SelectFilesPage):
            importedSpecies_->clear();
            temporaryCoreData_.clearAtomTypes();
            temporaryCoreData_.clearMasterTerms();
            ui_.SpeciesView->setSpecies(nullptr);

            // Check that any specified XML exists and and can be read in successfully
            if (!ui_.InputXMLEdit->text().isEmpty())
            {
                if (doc.load_file(ui_.InputXMLEdit->text().toStdString().c_str()))
                {
                    xmlFFModel_.resetXml();
                    xmlFFModel_.readFile(doc.root());
                    ui_.xmlTree->expandAll();
                    for (int i = 0; i < xmlFFModel_.columnCount(QModelIndex()); ++i)
                        ui_.xmlTree->resizeColumnToContents(i);
                    ui_.xmlTree->collapseAll();

                    if (!xmlFFModel_.isValid())
                        return Messenger::error("Failed to parse XML file '{}'.\n",
                                                ui_.InputXMLEdit->text().toStdString().c_str());
                }
            }

            // Check that the XYZ file can be loaded
            if (!ui_.InputXYZEdit->text().isEmpty())
            {
                try
                {
                    if (!importLigParGenXYZ(ui_.InputXYZEdit->text().toStdString()))
                        return Messenger::error("Failed to load XYZ file '{}'.\n", ui_.InputXYZEdit->text().toStdString());
                }
                catch (...)
                {
                    return Messenger::error("Failed to load XYZ file '{}'.\n", ui_.InputXYZEdit->text().toStdString());
                }

                ui_.SpeciesView->setSpecies(importedSpecies_);
            }

            return true;
        case (ImportLigParGenDialog::PreviewTermsPage):
            // Get the data as a Dissolve forcefield
            importedForcefield_ = xmlFFModel_.toForcefield();

            // Apply forcefield terms to the model
            if (!applyForcefield(temporaryCoreData_, importedSpecies_))
                return false;

            atomTypeModel_.setData(temporaryCoreData_.atomTypes());
            break;
        default:
            break;
    }

    return true;
}

// Determine next page for the current page, based on current data
std::optional<int> ImportLigParGenDialog::determineNextPage(int currentIndex)
{
    switch (currentIndex)
    {
        case (ImportLigParGenDialog::SelectFilesPage):
            return importedSpecies_ && importedSpecies_->nAtoms() > 0 ? ImportLigParGenDialog::PreviewSpeciesPage
                                                                      : ImportLigParGenDialog::PreviewTermsPage;
        case (ImportLigParGenDialog::PreviewSpeciesPage):
            if (xmlFFModel_.isValid())
                return ImportLigParGenDialog::PreviewTermsPage;
            else
                return std::nullopt;
        default:
            return std::nullopt;
    }
}

// Perform any necessary actions before moving to the previous page
bool ImportLigParGenDialog::prepareForPreviousPage(int currentIndex)
{
    switch (currentIndex)
    {
        case (ImportLigParGenDialog::PreviewSpeciesPage):
            importedSpecies_->clear();
            ui_.SpeciesView->setSpecies(nullptr);
        case (ImportLigParGenDialog::PreviewTermsPage):
            xmlFFModel_.resetXml();
            importedForcefield_ = nullptr;
        default:
            break;
    }

    return true;
}

// Perform any final actions before the wizard is closed
void ImportLigParGenDialog::finalise()
{
    // Copy the species to the main Dissolve instance and set its new name
    if (importedSpecies_ && importedSpecies_->nAtoms() > 0)
    {
        auto *sp = dissolve_.coreData().copySpecies(importedSpecies_);
        sp->setName("LigParGen Species");
    }

    // Register forcefield?
    if (ui_.CreateForcefieldGroup->isChecked() && importedForcefield_)
        ForcefieldLibrary::registerForcefield(importedForcefield_);
}

/*
 * Input File Page
 */

// Import LigParGen-style XYZ data
bool ImportLigParGenDialog::importLigParGenXYZ(std::string filename)
{
    xyzClassIDs_.clear();
    std::vector<std::vector<int>> connectivity;

    LineParser parser;
    if (!parser.openInput(filename))
        return false;

    // First line in file is number of atoms followed by temporary filename / ff id stuff
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    auto nAtoms = parser.argi(0);
    xyzClassIDs_.reserve(nAtoms);
    connectivity.resize(nAtoms, std::vector<int>());
    for (auto n = 0; n < nAtoms; ++n)
    {
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return Messenger::error("LigParGen XYZ file ended abruptly at atom {}.\n", n + 1);
        auto Z = Elements::element(parser.argsv(1));
        importedSpecies_->addAtom(Z, parser.arg3d(2));
        xyzClassIDs_.push_back(parser.argi(5));
        for (auto i = 6; i < parser.nArgs(); ++i)
            connectivity[n].push_back(parser.argi(i) - 1);
    }

    // Bond atoms according to the connectivity integers we read in
    for (auto i = 0; i < nAtoms; ++i)
        for (auto j : connectivity[i])
            if (!importedSpecies_->hasBond(i, j))
                importedSpecies_->addBond(i, j);

    // Create higher-order connectivity terms
    importedSpecies_->updateIntramolecularTerms();

    return true;
}

void ImportLigParGenDialog::on_InputXMLEdit_textChanged(QString text) { updateProgressionControls(); }

void ImportLigParGenDialog::on_InputXMLButton_clicked(bool checked)
{
    auto fileName = QFileDialog::getOpenFileName(this, "Open XML file", QDir().absolutePath(), "OpenMM XML files (*.xml)");
    if (fileName.isEmpty())
        return;

    ui_.InputXMLEdit->setText(fileName);

    // Set the default forcefield name
    QFileInfo path(fileName);
    ui_.ForcefieldNameEdit->setText("OpenMM/" + path.baseName());

    // If a correspondingly-named xyz file exists, set that too
    auto xyzFile = QDir::toNativeSeparators(path.absolutePath() + "/") + path.completeBaseName() + ".xyz";
    if (QFile::exists(xyzFile))
        ui_.InputXYZEdit->setText(xyzFile);

    updateProgressionControls();
}

void ImportLigParGenDialog::on_InputXYZEdit_textChanged(QString text) { updateProgressionControls(); }

void ImportLigParGenDialog::on_InputXYZButton_clicked(bool checked)
{
    auto fileName = QFileDialog::getOpenFileName(this, "Open XYZ file", QDir().absolutePath(), "LigParGen XYZ files (*.xyz)");
    if (fileName.isEmpty())
        return;

    ui_.InputXYZEdit->setText(fileName);

    updateProgressionControls();
}

/*
 * Atom Types
 */

void ImportLigParGenDialog::on_SimplifyAtomTypesGroup_clicked(bool checked)
{
    // Apply forcefield terms to the model
    applyForcefield(temporaryCoreData_, importedSpecies_);

    atomTypeModel_.setData(temporaryCoreData_.atomTypes());
}

void ImportLigParGenDialog::on_ReduceToMasterTermsGroup_clicked(bool checked)
{
    // Apply forcefield terms to the model
    applyForcefield(temporaryCoreData_, importedSpecies_);

    atomTypeModel_.setData(temporaryCoreData_.atomTypes());
}

/*
 * Apply Forcefield
 */

// Attempt to manually apply our forcefield to the specified species
bool ImportLigParGenDialog::applyForcefield(CoreData &coreData, Species *sp) const
{
    sp->clearAtomTypes();
    sp->detachFromMasterTerms();
    coreData.clearAtomTypes();
    coreData.clearMasterTerms();

    // Assign atomic charges from the original forcefield, based on the element/class atom types
    for (auto &&[i, classID] : zip(sp->atoms(), xyzClassIDs_))
    {
        auto optType = importedForcefield_->atomTypeByName(fmt::format("{}{}", Elements::symbol(i.Z()), classID), i.Z());
        if (!optType)
            return Messenger::error("No associated type {}{} is present in the forcefield.\n", Elements::symbol(i.Z()),
                                    classID);
        importedForcefield_->assignAtomType(*optType, i, coreData, true);
    }

    // Assign intramolecular terms based on the assigned element/classId atom types
    if (!importedForcefield_->assignIntramolecular(sp))
        return false;

    // Simplify atom types?
    if (ui_.SimplifyAtomTypesGroup->isChecked())
    {
        sp->simplifyAtomTypes();
        coreData.removeUnusedAtomTypes();
    }

    // Reduce to master terms?
    if (ui_.ReduceToMasterTermsGroup->isChecked())
        sp->reduceToMasterTerms(coreData);

    return true;
}

void ImportLigParGenDialog::on_ForcefieldNameEdit_textChanged(QString text)
{
    xmlFFModel_.setName(text.toStdString());

    updateProgressionControls();
}
