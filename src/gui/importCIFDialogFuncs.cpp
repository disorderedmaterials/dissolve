// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/empiricalFormula.h"
#include "classes/molecule.h"
#include "classes/pairIterator.h"
#include "classes/species.h"
#include "gui/importCIFDialog.h"
#include "neta/node.h"
#include "procedure/nodes/add.h"
#include "procedure/nodes/box.h"
#include "procedure/nodes/coordinateSets.h"
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QInputDialog>
#include <unordered_set>

ImportCIFDialog::ImportCIFDialog(QWidget *parent, Dissolve &dissolve)
    : WizardDialog(parent), cifAssemblyModel_(cifImporter_.assemblies()), dissolve_(dissolve)
{
    ui_.setupUi(this);

    // Register pages with the wizard
    registerPage(ImportCIFDialog::SelectCIFFilePage, "Choose CIF File");
    registerPage(ImportCIFDialog::SelectSpaceGroupPage, "Choose Space Group", ImportCIFDialog::CIFInfoPage);
    registerPage(ImportCIFDialog::CIFInfoPage, "CIF Information", ImportCIFDialog::StructurePage);
    registerPage(ImportCIFDialog::StructurePage, "Basic Structure", ImportCIFDialog::CleanedPage);
    registerPage(ImportCIFDialog::CleanedPage, "Clean Structure", ImportCIFDialog::SupercellPage);
    registerPage(ImportCIFDialog::SupercellPage, "Create Supercell", ImportCIFDialog::OutputSpeciesPage);
    registerPage(ImportCIFDialog::OutputSpeciesPage, "Species Partitioning");

    // Add spacegroup list
    for (auto n = 1; n < SpaceGroups::nSpaceGroupIds; ++n)
        ui_.SpaceGroupsList->addItem(
            QString::fromStdString(std::string(SpaceGroups::formattedInformation((SpaceGroups::SpaceGroupId)n))));

    // Set assembly view model
    ui_.AssemblyView->setModel(&cifAssemblyModel_);
    ui_.AssemblyView->update();
    ui_.AssemblyView->expandAll();
    connect(&cifAssemblyModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QList<int> &)), this,
            SLOT(createStructuralSpecies()));
    connect(&cifAssemblyModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QList<int> &)),
            ui_.AssemblyView, SLOT(expandAll()));

    // Update moiety NETA
    createMoietyRemovalNETA(ui_.MoietyNETARemovalEdit->text().toStdString());

    // Init the wizard
    initialise(this, ui_.MainStack, ImportCIFDialog::SelectCIFFilePage);

    cifSpecies_ = new CIFSpecies(cifImporter_, temporaryCoreData_);
}

/*
 * Wizard
 */

// Return whether progression to the next page from the current page is allowed
bool ImportCIFDialog::progressionAllowed(int index) const
{
    // Check widget validity in the specified page, returning if progression (i.e. pushing 'Next' or 'Finish') is allowed
    switch (index)
    {
        case (ImportCIFDialog::SelectCIFFilePage):
            if (ui_.InputFileEdit->text().isEmpty() || !QFile::exists(ui_.InputFileEdit->text()))
                return false;
            break;
        case (ImportCIFDialog::SelectSpaceGroupPage):
            return ui_.SpaceGroupsList->currentRow() != -1;
        case (ImportCIFDialog::OutputSpeciesPage):
            // If the "Framework" or "Supermolecule" options are chosen, the "Crystal" species must be a single moiety
            if (molecularSpecies_.empty() &&
                (ui_.OutputFrameworkRadio->isChecked() || ui_.OutputSupermoleculeRadio->isChecked()))
                return ui_.PartitioningIndicator->state() == CheckIndicator::OKState;
            break;
        default:
            break;
    }

    return true;
}

// Perform any necessary actions before moving to the next page
bool ImportCIFDialog::prepareForNextPage(int currentIndex)
{
    switch (currentIndex)
    {
        case (ImportCIFDialog::SelectCIFFilePage):
            // Load the CIF file
            if (!cifImporter_.read(ui_.InputFileEdit->text().toStdString()))
                return Messenger::error("Failed to load CIF file '{}'.\n", ui_.InputFileEdit->text().toStdString());
            updateInfoPage();
            updateSpaceGroupPage();
            break;
        case (ImportCIFDialog::SelectSpaceGroupPage):
            cifImporter_.setSpaceGroup((SpaceGroups::SpaceGroupId)(ui_.SpaceGroupsList->currentRow() + 1));
            updateInfoPage();
            break;
        case (ImportCIFDialog::CIFInfoPage):
            ui_.BondFromCIFRadio->setEnabled(cifImporter_.hasBondDistances());
            ui_.BondFromCIFRadio->setChecked(cifImporter_.hasBondDistances());
            if (!createStructuralSpecies())
                return false;
            ui_.AssemblyView->expandAll();
            break;
        case (ImportCIFDialog::StructurePage):
            if (!createCleanedSpecies())
                return false;
            break;
        case (ImportCIFDialog::CleanedPage):
            detectUniqueSpecies();
            if (!createSupercellSpecies())
                return false;
            break;
        case (ImportCIFDialog::SupercellPage):
            createPartitionedSpecies();
            break;
        default:
            break;
    }

    return true;
}

// Determine next page for the current page, based on current data
std::optional<int> ImportCIFDialog::determineNextPage(int currentIndex)
{
    // Only check for the first page
    if (currentIndex == ImportCIFDialog::SelectCIFFilePage)
        return cifImporter_.spaceGroup() != SpaceGroups::NoSpaceGroup ? ImportCIFDialog::CIFInfoPage
                                                                      : ImportCIFDialog::SelectSpaceGroupPage;
    else
        return std::get<3>(getPage(currentIndex));
}

// Perform any necessary actions before moving to the previous page
bool ImportCIFDialog::prepareForPreviousPage(int currentIndex)
{
    switch (currentIndex)
    {
        case (ImportCIFDialog::CIFInfoPage):
            structureConfiguration_->empty();
            cleanedConfiguration_->empty();
            supercellConfiguration_->empty();
            partitioningConfiguration_->empty();
            temporaryCoreData_.species().clear();
            temporaryCoreData_.atomTypes().clear();
            break;
        default:
            break;
    }

    return true;
}

// Perform any final actions before the wizard is closed
void ImportCIFDialog::finalise()
{
    if (molecularSpecies_.empty())
    {
        auto *supercell = temporaryCoreData_.findSpecies("Supercell");
        assert(supercell);

        if (ui_.OutputFrameworkRadio->isChecked())
        {
            // Just copy the species
            supercell->updateIntramolecularTerms();
            auto *sp = dissolve_.coreData().copySpecies(supercell);
            sp->setName(cifImporter_.chemicalFormula());
        }
        else if (ui_.OutputSupermoleculeRadio->isChecked())
        {
            // Copy the species
            supercell->updateIntramolecularTerms();
            auto *sp = dissolve_.coreData().copySpecies(supercell);
            sp->setName(cifImporter_.chemicalFormula());

            // Remove the unit cell and any cell-crossing bonds
            sp->removePeriodicBonds();
            sp->updateIntramolecularTerms();
            sp->removeBox();
        }
    }
    else
    {
        for (auto &sp : molecularSpecies_)
            dissolve_.coreData().copySpecies(sp);
        cifSpecies_->createMolecularConfiguration(dissolve_.coreData());
    }
}

/*
 * Select CIF File Page
 */

void ImportCIFDialog::on_InputFileEdit_textChanged(const QString text) { updateProgressionControls(); }

void ImportCIFDialog::on_InputFileSelectButton_clicked(bool checked)
{
    QString inputFile = QFileDialog::getOpenFileName(this, "Choose CIF file to open", QDir().absolutePath(),
                                                     "CIF files (*.cif);;All files (*)");
    if (inputFile.isEmpty())
        return;

    // Set input file in edit
    ui_.InputFileEdit->setText(inputFile);

    updateProgressionControls();
}

/*
 * Select Space Group Page
 */

void ImportCIFDialog::updateSpaceGroupPage() { ui_.SpaceGroupsList->setCurrentRow(-1); }

void ImportCIFDialog::on_SpaceGroupsList_currentRowChanged(int row)
{
    if (currentPage().has_value() && currentPage().value() == ImportCIFDialog::SelectSpaceGroupPage)
        updateProgressionControls();
}

void ImportCIFDialog::on_SpaceGroupsList_itemDoubleClicked(QListWidgetItem *item) { goToNextPage(); }

/*f
 * Basic CIF Info Page
 */

void ImportCIFDialog::updateInfoPage()
{
    // DATA_ ID
    ui_.InfoDataLabel->setText(QString::fromStdString(cifImporter_.getTagString("DATA_").value_or("<Unknown>")));

    // Chemical Formula
    ui_.InfoChemicalFormulaLabel->setText(QString::fromStdString(cifImporter_.chemicalFormula()));

    // Publication Data
    ui_.InfoPublicationTitleLabel->setText(
        QString::fromStdString(cifImporter_.getTagString("_publ_section_title").value_or("<Unknown>")));
    ui_.InfoPublicationReferenceLabel->setText(QString::fromStdString(
        fmt::format("{}, {}, <b>{}</b>, {}", cifImporter_.getTagString("_journal_name_full").value_or("???"),
                    cifImporter_.getTagString("_journal_year").value_or("???"),
                    cifImporter_.getTagString("_journal_volume").value_or("???"),
                    cifImporter_.getTagString("_journal_page_first").value_or("???"))));
    ui_.InfoAuthorsList->clear();
    auto authors = cifImporter_.getTagStrings("_publ_author_name");
    for (auto &author : authors)
        ui_.InfoAuthorsList->addItem(QString::fromStdString(author));

    // Spacegroup
    if (cifImporter_.spaceGroup() != SpaceGroups::NoSpaceGroup)
        ui_.InfoSpacegroupLabel->setText(
            QString::fromStdString(std::string(SpaceGroups::formattedInformation(cifImporter_.spaceGroup()))));
    else
        ui_.InfoSpacegroupLabel->setText("<Unknown Space Group>");
}

/*
 * Structure Page
 */

// Generate structural species from CIF data
bool ImportCIFDialog::createStructuralSpecies()
{
    ui_.StructureViewer->setConfiguration(nullptr);
    temporaryCoreData_.species().clear();
    temporaryCoreData_.atomTypes().clear();
    cifSpecies_->createStructuralSpecies(ui_.NormalOverlapToleranceRadio->isChecked() ? 0.1 : 0.5,
                                         ui_.CalculateBondingRadio->isChecked(), ui_.BondingPreventMetallicCheck->isChecked());
    crystalSpecies_ = cifSpecies_->structuralSpecies();
    if (crystalSpecies_ == nullptr)
        return false;

    structureConfiguration_ = cifSpecies_->structuralConfiguration();

    if (structureConfiguration_ == nullptr)
        return false;

    ui_.StructureViewer->setConfiguration(structureConfiguration_);

    return true;
}

void ImportCIFDialog::on_NormalOverlapToleranceRadio_clicked(bool checked)
{
    if (checked)
        createStructuralSpecies();
}

void ImportCIFDialog::on_LooseOverlapToleranceRadio_clicked(bool checked)
{
    if (checked)
        createStructuralSpecies();
}

void ImportCIFDialog::on_CalculateBondingRadio_clicked(bool checked)
{
    if (ui_.MainStack->currentIndex() == ImportCIFDialog::StructurePage)
        createStructuralSpecies();
}

void ImportCIFDialog::on_BondingPreventMetallicCheck_clicked(bool checked)
{
    if (ui_.MainStack->currentIndex() == ImportCIFDialog::StructurePage)
        createStructuralSpecies();
}

void ImportCIFDialog::on_BondFromCIFRadio_clicked(bool checked)
{
    if (ui_.MainStack->currentIndex() == ImportCIFDialog::StructurePage)
        createStructuralSpecies();
}

/*
 * Cleaned Page
 */

// Generate cleaned species from CIF data
bool ImportCIFDialog::createCleanedSpecies()
{
    ui_.CleanedViewer->setConfiguration(nullptr);
    partitioningConfiguration_->empty();
    supercellConfiguration_->empty();

    // Remove the cleaned crystal species
    if (cleanedSpecies_)
        temporaryCoreData_.removeSpecies(cleanedSpecies_);

    if (ui_.MoietyRemoveByNETAGroup->isChecked())
        cifSpecies_->createCleanedSpecies(ui_.MoietyRemoveAtomicsCheck->isChecked(), ui_.MoietyRemoveWaterCheck->isChecked(),
                                          moietyNETA_, ui_.MoietyNETARemoveFragmentsCheck->isChecked());
    else
        cifSpecies_->createCleanedSpecies(ui_.MoietyRemoveAtomicsCheck->isChecked(), ui_.MoietyRemoveWaterCheck->isChecked());

    cleanedSpecies_ = cifSpecies_->cleanedSpecies();

    if (cleanedSpecies_ == nullptr)
        return false;

    cleanedConfiguration_ = cifSpecies_->cleanedConfiguration();

    if (cleanedConfiguration_ == nullptr)
        return false;

    ui_.CleanedViewer->setConfiguration(cleanedConfiguration_);

    return true;
}

// Create / check NETA definition for moiety removal
bool ImportCIFDialog::createMoietyRemovalNETA(std::string definition)
{
    auto result = moietyNETA_.create(definition);

    ui_.MoietyNETARemovalIndicator->setOK(result);

    return result;
}

void ImportCIFDialog::on_MoietyRemoveAtomicsCheck_clicked(bool checked) { createCleanedSpecies(); }

void ImportCIFDialog::on_MoietyRemoveWaterCheck_clicked(bool checked) { createCleanedSpecies(); }

void ImportCIFDialog::on_MoietyRemoveByNETAGroup_clicked(bool checked) { createCleanedSpecies(); }

void ImportCIFDialog::on_MoietyNETARemovalEdit_textEdited(const QString &text)
{
    createMoietyRemovalNETA(text.toStdString());
    if (moietyNETA_.isValid())
        createCleanedSpecies();
}

void ImportCIFDialog::on_MoietyNETARemoveFragmentsCheck_clicked(bool checked)
{
    if (moietyNETA_.isValid())
        createCleanedSpecies();
}

// Detect unique species in the structural species
bool ImportCIFDialog::detectUniqueSpecies()
{
    cifSpecies_->createMolecularSpecies();
    molecularSpecies_ = cifSpecies_->molecularSpecies();
    return true;
}

/*
 * Supercell Page
 */

// Create supercell species
bool ImportCIFDialog::createSupercellSpecies()
{
    ui_.SupercellViewer->setConfiguration(nullptr);
    supercellConfiguration_->empty();
    partitioningConfiguration_->empty();

    // Remove the old Supercell species if it exists, and create a new one
    auto *supercell = temporaryCoreData_.findSpecies("Supercell");
    if (supercell)
        temporaryCoreData_.removeSpecies(supercell);

    // Set the repeat vector
    Vec3<int> repeat(ui_.RepeatASpin->value(), ui_.RepeatBSpin->value(), ui_.RepeatCSpin->value());

    cifSpecies_->createSupercellSpecies(repeat, ui_.CalculateBondingRadio->isChecked(),
                                        ui_.BondingPreventMetallicCheck->isChecked());
    supercell = cifSpecies_->supercellSpecies();
    if (!supercell)
        return false;

    supercellConfiguration_ = cifSpecies_->supercellConfiguration();
    ui_.SupercellViewer->setConfiguration(supercellConfiguration_);

    // Update the information panel
    ui_.SupercellBoxALabel->setText(QString::number(supercell->box()->axisLengths().x) + " &#8491;");
    ui_.SupercellBoxBLabel->setText(QString::number(supercell->box()->axisLengths().y) + " &#8491;");
    ui_.SupercellBoxCLabel->setText(QString::number(supercell->box()->axisLengths().z) + " &#8491;");
    ui_.SupercellBoxAlphaLabel->setText(QString::number(supercell->box()->axisAngles().x) + "&deg;");
    ui_.SupercellBoxBetaLabel->setText(QString::number(supercell->box()->axisAngles().y) + "&deg;");
    ui_.SupercellBoxGammaLabel->setText(QString::number(supercell->box()->axisAngles().z) + "&deg;");
    auto chemicalDensity = supercellConfiguration_->chemicalDensity();
    ui_.SupercellDensityLabel->setText(chemicalDensity ? QString::number(*chemicalDensity) + " g cm<sup>3</sup>"
                                                       : "-- g cm<sup>3</sup>");
    ui_.SupercellVolumeLabel->setText(QString::number(supercellConfiguration_->box()->volume()) + " &#8491;<sup>3</sup>");
    ui_.SupercellNAtomsLabel->setText(QString::number(supercellConfiguration_->nAtoms()));

    return true;
}

void ImportCIFDialog::on_RepeatASpin_valueChanged(int value) { createSupercellSpecies(); }

void ImportCIFDialog::on_RepeatBSpin_valueChanged(int value) { createSupercellSpecies(); }

void ImportCIFDialog::on_RepeatCSpin_valueChanged(int value) { createSupercellSpecies(); }

/*
 * Species Partitioning Page
 */

// Create partitioned species from CIF data
bool ImportCIFDialog::createPartitionedSpecies()
{

    ui_.PartitioningViewer->setConfiguration(nullptr);
    partitioningConfiguration_->clear();

    if (molecularSpecies_.empty())
    {
        // Set up the basic configuration
        auto *sp = cifSpecies_->supercellSpecies();
        assert(sp);
        partitioningConfiguration_->createBoxAndCells(sp->box()->axisLengths(), sp->box()->axisAngles(), false, 1.0);

        auto validSpecies = true;

        if (ui_.OutputFrameworkRadio->isChecked() || ui_.OutputSupermoleculeRadio->isChecked())
        {
            // Add the supercell species to the configuration
            partitioningConfiguration_->addMolecule(sp);
            partitioningConfiguration_->updateObjectRelationships();

            // Update the indicator and label
            sp->clearAtomSelection();
            auto selection = sp->fragment(0);
            if (selection.size() != sp->nAtoms())
            {
                ui_.PartitioningIndicator->setOK(false);
                ui_.PartitioningLabel->setText("Species contains more than one molecule/fragment, and cannot be used in a "
                                               "simulation. Choose a different partitioning.");

                validSpecies = false;
            }
        }

        ui_.PartitioningViewer->setConfiguration(partitioningConfiguration_);

        if (validSpecies)
        {
            ui_.PartitioningIndicator->setOK(true);
            ui_.PartitioningLabel->setText("Species are valid.");
        }
    }
    else
    {
        ui_.PartitioningLayoutWidget->setEnabled(false);
        ui_.PartitioningViewFrame->setEnabled(false);
    }
    return true;
}
