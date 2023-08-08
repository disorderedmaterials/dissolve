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
    : WizardDialog(parent), cifHandler_(temporaryCoreData_),cifAssemblyModel_(cifHandler_.assemblies()), dissolve_(dissolve)
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
            SLOT(update()));
    connect(&cifAssemblyModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QList<int> &)),
            ui_.AssemblyView, SLOT(expandAll()));

    // Update moiety NETA
    createMoietyRemovalNETA(ui_.MoietyNETARemovalEdit->text().toStdString());

    // Init the wizard
    initialise(this, ui_.MainStack, ImportCIFDialog::SelectCIFFilePage);

    // Configuration
    partitioningConfiguration_ = temporaryCoreData_.addConfiguration();
    partitioningConfiguration_->setName("Partitioning");
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
            if (!cifHandler_.read(ui_.InputFileEdit->text().toStdString()))
                return Messenger::error("Failed to load CIF file '{}'.\n", ui_.InputFileEdit->text().toStdString());
            updateInfoPage();
            updateSpaceGroupPage();
            break;
        case (ImportCIFDialog::SelectSpaceGroupPage):
            cifHandler_.setSpaceGroup((SpaceGroups::SpaceGroupId)(ui_.SpaceGroupsList->currentRow() + 1));
            updateInfoPage();
            break;
        case (ImportCIFDialog::CIFInfoPage):
            /*if (!cifHandler_.hasBondDistances())
            else
                bondingFlags_.removeFlag(CIFHandler::BondingFlags::CalculateBonding);
            bondingFlags_.setFlag(CIFHandler::BondingFlags::CalculateBonding);
            ui_.BondFromCIFRadio->setEnabled(cifHandler_.hasBondDistances());
            //ui_.BondFromCIFRadio->setChecked(cifHandler_.hasBondDistances());
            if (ui_.BondingPreventMetallicCheck->isChecked())
                bondingFlags_.setFlag(CIFHandler::BondingFlags::PreventMetallicBonding);
            else
                bondingFlags_.removeFlag(CIFHandler::BondingFlags::PreventMetallicBonding);
            */
            bondingFlags_.setFlag(CIFHandler::BondingFlags::CalculateBonding);
            update();
            if (!cifHandler_.structuralSpecies())
                return false;
            ui_.AssemblyView->expandAll();
            break;
        case (ImportCIFDialog::StructurePage):
            update();
            if (!cifHandler_.cleanedSpecies())
                return false;
            break;
        case (ImportCIFDialog::CleanedPage):
            update();
            if (!cifHandler_.supercellSpecies())
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
        return cifHandler_.spaceGroup() != SpaceGroups::NoSpaceGroup ? ImportCIFDialog::CIFInfoPage
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
            sp->setName(cifHandler_.chemicalFormula());
        }
        else if (ui_.OutputSupermoleculeRadio->isChecked())
        {
            // Copy the species
            supercell->updateIntramolecularTerms();
            auto *sp = dissolve_.coreData().copySpecies(supercell);
            sp->setName(cifHandler_.chemicalFormula());

            // Remove the unit cell and any cell-crossing bonds
            sp->removePeriodicBonds();
            sp->updateIntramolecularTerms();
            sp->removeBox();
        }
    }
    else
    {
        //cifHandler_.createMolecularConfiguration(dissolve_.coreData());
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
    ui_.InfoDataLabel->setText(QString::fromStdString(cifHandler_.getTagString("DATA_").value_or("<Unknown>")));

    // Chemical Formula
    ui_.InfoChemicalFormulaLabel->setText(QString::fromStdString(cifHandler_.chemicalFormula()));

    // Publication Data
    ui_.InfoPublicationTitleLabel->setText(
        QString::fromStdString(cifHandler_.getTagString("_publ_section_title").value_or("<Unknown>")));
    ui_.InfoPublicationReferenceLabel->setText(QString::fromStdString(fmt::format(
        "{}, {}, <b>{}</b>, {}", cifHandler_.getTagString("_journal_name_full").value_or("???"),
        cifHandler_.getTagString("_journal_year").value_or("???"), cifHandler_.getTagString("_journal_volume").value_or("???"),
        cifHandler_.getTagString("_journal_page_first").value_or("???"))));
    ui_.InfoAuthorsList->clear();
    auto authors = cifHandler_.getTagStrings("_publ_author_name");
    for (auto &author : authors)
        ui_.InfoAuthorsList->addItem(QString::fromStdString(author));

    // Spacegroup
    if (cifHandler_.spaceGroup() != SpaceGroups::NoSpaceGroup)
        ui_.InfoSpacegroupLabel->setText(
            QString::fromStdString(std::string(SpaceGroups::formattedInformation(cifHandler_.spaceGroup()))));
    else
        ui_.InfoSpacegroupLabel->setText("<Unknown Space Group>");
}

/*
 * Structure Page
 */

void ImportCIFDialog::on_NormalOverlapToleranceRadio_clicked(bool checked)
{
    if (checked)
        update();
}

void ImportCIFDialog::on_LooseOverlapToleranceRadio_clicked(bool checked)
{
    if (checked)
        update();
}

void ImportCIFDialog::on_CalculateBondingRadio_clicked(bool checked)
{
    if (checked)
        bondingFlags_.setFlag(CIFHandler::CalculateBonding);
    else
        bondingFlags_.removeFlag(CIFHandler::CalculateBonding);

    if (ui_.MainStack->currentIndex() == ImportCIFDialog::StructurePage)
    {
        update();
    }
}

void ImportCIFDialog::on_BondingPreventMetallicCheck_clicked(bool checked)
{
    if (checked)
        bondingFlags_.setFlag(CIFHandler::PreventMetallicBonding);
    else
        bondingFlags_.removeFlag(CIFHandler::PreventMetallicBonding);

    if (ui_.MainStack->currentIndex() == ImportCIFDialog::StructurePage)
        update();
}

void ImportCIFDialog::on_BondFromCIFRadio_clicked(bool checked)
{
    if (!checked)
        bondingFlags_.setFlag(CIFHandler::CalculateBonding);
    else
        bondingFlags_.removeFlag(CIFHandler::CalculateBonding);

    if (ui_.MainStack->currentIndex() == ImportCIFDialog::StructurePage)
        update();
}

/*
 * Cleaned Page
 */


// Create / check NETA definition for moiety removal
bool ImportCIFDialog::createMoietyRemovalNETA(std::string definition)
{
    auto result = moietyNETA_.create(definition);

    ui_.MoietyNETARemovalIndicator->setOK(result);

    return result;
}

void ImportCIFDialog::on_MoietyRemoveAtomicsCheck_clicked(bool checked)
{
    if (checked)
        cleaningFlags_.setFlag(CIFHandler::CleaningFlags::MoietyRemoveAtomics);
    else
        cleaningFlags_.removeFlag(CIFHandler::CleaningFlags::MoietyRemoveAtomics);

    update();
}

void ImportCIFDialog::on_MoietyRemoveWaterCheck_clicked(bool checked)
{
    if (checked)
        cleaningFlags_.setFlag(CIFHandler::CleaningFlags::MoietyRemoveWater);
    else
        cleaningFlags_.removeFlag(CIFHandler::CleaningFlags::MoietyRemoveWater);

    update();
}

void ImportCIFDialog::on_MoietyRemoveByNETAGroup_clicked(bool checked)
{
    if (checked)
        cleaningFlags_.setFlag(CIFHandler::CleaningFlags::MoietyRemoveNETA);
    else
        cleaningFlags_.removeFlag(CIFHandler::CleaningFlags::MoietyRemoveNETA);

    update();
}

void ImportCIFDialog::on_MoietyNETARemovalEdit_textEdited(const QString &text)
{
    createMoietyRemovalNETA(text.toStdString());
    if (moietyNETA_.isValid())
        update();
}

void ImportCIFDialog::on_MoietyNETARemoveFragmentsCheck_clicked(bool checked)
{
    if (checked)
        cleaningFlags_.setFlag(CIFHandler::CleaningFlags::RemoveBoundFragments);
    else
        cleaningFlags_.removeFlag(CIFHandler::CleaningFlags::RemoveBoundFragments);

    if (moietyNETA_.isValid())
        update();
}

/*
 * Supercell Page
 */

void ImportCIFDialog::on_RepeatASpin_valueChanged(int value) { update(); }

void ImportCIFDialog::on_RepeatBSpin_valueChanged(int value) { update(); }

void ImportCIFDialog::on_RepeatCSpin_valueChanged(int value) { update(); }

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
        auto *sp = cifHandler_.supercellSpecies();
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

bool ImportCIFDialog::update()
{
    // Set the repeat vector
    Vec3<int> repeat(ui_.RepeatASpin->value(), ui_.RepeatBSpin->value(), ui_.RepeatCSpin->value());
    auto result = cifHandler_.update(ui_.NormalOverlapToleranceRadio->isChecked() ? 0.1 : 0.5, repeat, moietyNETA_, bondingFlags_, cleaningFlags_);
    ui_.StructureViewer->setConfiguration(cifHandler_.structuralConfiguration());
    ui_.CleanedViewer->setConfiguration(cifHandler_.cleanedConfiguration());
    ui_.SupercellViewer->setConfiguration(cifHandler_.supercellConfiguration());

    auto* supercell = cifHandler_.supercellSpecies();

    if (supercell)
    {
        // Update the information panel
        ui_.SupercellBoxALabel->setText(QString::number(supercell->box()->axisLengths().x) + " &#8491;");
        ui_.SupercellBoxBLabel->setText(QString::number(supercell->box()->axisLengths().y) + " &#8491;");
        ui_.SupercellBoxCLabel->setText(QString::number(supercell->box()->axisLengths().z) + " &#8491;");
        ui_.SupercellBoxAlphaLabel->setText(QString::number(supercell->box()->axisAngles().x) + "&deg;");
        ui_.SupercellBoxBetaLabel->setText(QString::number(supercell->box()->axisAngles().y) + "&deg;");
        ui_.SupercellBoxGammaLabel->setText(QString::number(supercell->box()->axisAngles().z) + "&deg;");
        auto chemicalDensity = cifHandler_.supercellConfiguration()->chemicalDensity();
        ui_.SupercellDensityLabel->setText(chemicalDensity ? QString::number(*chemicalDensity) + " g cm<sup>3</sup>"
                                                           : "-- g cm<sup>3</sup>");
        ui_.SupercellVolumeLabel->setText(QString::number(cifHandler_.supercellConfiguration()->box()->volume()) + " &#8491;<sup>3</sup>");
        ui_.SupercellNAtomsLabel->setText(QString::number(cifHandler_.supercellConfiguration()->nAtoms()));
        //createPartitionedSpecies();
    }

    return true;
}