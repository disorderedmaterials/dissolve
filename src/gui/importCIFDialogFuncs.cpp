// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

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
    : WizardDialog(parent), cifAssemblyModel_(cifHandler_.assemblies()), dissolve_(dissolve)
{
    ui_.setupUi(this);

    // Register pages with the wizard
    registerPage(ImportCIFDialog::SelectCIFFilePage, "Choose CIF File");
    registerPage(ImportCIFDialog::SelectSpaceGroupPage, "Choose Space Group", ImportCIFDialog::CIFInfoPage);
    registerPage(ImportCIFDialog::CIFInfoPage, "CIF Information", ImportCIFDialog::StructurePage);
    registerPage(ImportCIFDialog::StructurePage, "Basic Structure", ImportCIFDialog::CleanedPage);
    registerPage(ImportCIFDialog::CleanedPage, "Clean Structure", ImportCIFDialog::OutputSpeciesPage);
    registerPage(ImportCIFDialog::OutputSpeciesPage, "Output");

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

    // Set default flags.
    outputFlags_.setFlag(CIFHandler::OutputFlags::OutputFramework);
    outputFlags_.setFlag(CIFHandler::OutputFlags::OutputConfiguration);

    // Init the wizard
    initialise(this, ui_.MainStack, ImportCIFDialog::SelectCIFFilePage);
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
            return ui_.OutputIndicator->state() == CheckIndicator::OKState;
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
            ui_.BondFromCIFRadio->setEnabled(cifHandler_.hasBondDistances());
            updateFlags_.setFlag(CIFHandler::UpdateFlags::CalculateBonding);
            update();
            if (!cifHandler_.structuralUnitCellSpecies())
                return false;
            ui_.AssemblyView->expandAll();
            break;
        case (ImportCIFDialog::StructurePage):
            update();
            if (!cifHandler_.cleanedUnitCellSpecies())
                return false;
            break;
        case (ImportCIFDialog::CleanedPage):
            ui_.OutputMolecularRadio->setChecked(!cifHandler_.molecularSpecies().empty());
            outputFlags_.setFlag(CIFHandler::OutputFlags::OutputMolecularSpecies);
            update();
            if (!cifHandler_.supercellConfiguration())
                return false;
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
            cifHandler_.resetSpeciesAndConfigurations();
            break;
        default:
            break;
    }
    return true;
}

// Perform any final actions before the wizard is closed
void ImportCIFDialog::finalise() { cifHandler_.finalise(dissolve_.coreData(), outputFlags_); }

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
        updateFlags_.setFlag(CIFHandler::CalculateBonding);
    else
        updateFlags_.removeFlag(CIFHandler::CalculateBonding);

    if (ui_.MainStack->currentIndex() == ImportCIFDialog::StructurePage)
    {
        update();
    }
}

void ImportCIFDialog::on_BondingPreventMetallicCheck_clicked(bool checked)
{
    if (checked)
        updateFlags_.setFlag(CIFHandler::PreventMetallicBonding);
    else
        updateFlags_.removeFlag(CIFHandler::PreventMetallicBonding);

    if (ui_.MainStack->currentIndex() == ImportCIFDialog::StructurePage)
        update();
}

void ImportCIFDialog::on_BondFromCIFRadio_clicked(bool checked)
{
    if (!checked)
        updateFlags_.setFlag(CIFHandler::CalculateBonding);
    else
        updateFlags_.removeFlag(CIFHandler::CalculateBonding);

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
        updateFlags_.setFlag(CIFHandler::UpdateFlags::CleanMoietyRemoveAtomics);
    else
        updateFlags_.removeFlag(CIFHandler::UpdateFlags::CleanMoietyRemoveAtomics);

    update();
}

void ImportCIFDialog::on_MoietyRemoveWaterCheck_clicked(bool checked)
{
    if (checked)
        updateFlags_.setFlag(CIFHandler::UpdateFlags::CleanMoietyRemoveWater);
    else
        updateFlags_.removeFlag(CIFHandler::UpdateFlags::CleanMoietyRemoveWater);

    update();
}

void ImportCIFDialog::on_MoietyRemoveByNETAGroup_clicked(bool checked)
{
    if (checked)
        updateFlags_.setFlag(CIFHandler::UpdateFlags::CleanMoietyRemoveNETA);
    else
        updateFlags_.removeFlag(CIFHandler::UpdateFlags::CleanMoietyRemoveNETA);

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
        updateFlags_.setFlag(CIFHandler::UpdateFlags::CleanRemoveBoundFragments);
    else
        updateFlags_.removeFlag(CIFHandler::UpdateFlags::CleanRemoveBoundFragments);

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

void ImportCIFDialog::on_OutputMolecularRadio_clicked(bool checked)
{
    if (checked)
        outputFlags_.setFlag(CIFHandler::OutputFlags::OutputMolecularSpecies);
    else
        outputFlags_.removeFlag(CIFHandler::OutputFlags::OutputMolecularSpecies);
    update();
}

void ImportCIFDialog::on_OutputFrameworkRadio_clicked(bool checked)
{
    if (checked)
        outputFlags_.setFlag(CIFHandler::OutputFlags::OutputFramework);
    else
        outputFlags_.removeFlag(CIFHandler::OutputFlags::OutputFramework);
    update();
}

void ImportCIFDialog::on_OutputSupermoleculeRadio_clicked(bool checked)
{
    if (checked)
    {
        outputFlags_.setFlag(CIFHandler::OutputFlags::OutputSupermolecule);
        ui_.OutputConfigurationCheck->setChecked(false);
        outputFlags_.removeFlag(CIFHandler::OutputFlags::OutputConfiguration);
    }
    else
    {
        outputFlags_.removeFlag(CIFHandler::OutputFlags::OutputSupermolecule);
    }
    update();
}

void ImportCIFDialog::on_OutputConfigurationCheck_clicked(bool checked)
{
    if (checked)
        outputFlags_.setFlag(CIFHandler::OutputFlags::OutputConfiguration);
    else
        outputFlags_.removeFlag(CIFHandler::OutputFlags::OutputConfiguration);
    update();
}

bool ImportCIFDialog::update()
{
    // Set up the CIF handler
    cifHandler_.setSupercellRepeat({ui_.RepeatASpin->value(), ui_.RepeatBSpin->value(), ui_.RepeatCSpin->value()});
    cifHandler_.setBondingTolerance(ui_.NormalOverlapToleranceRadio->isChecked() ? 0.1 : 0.5);
    cifHandler_.setMoietyRemovalNETA(moietyNETA_.definitionString());

    auto result = cifHandler_.generate(updateFlags_);

    ui_.StructureViewer->setConfiguration(cifHandler_.structuralUnitCellConfiguration());
    ui_.CleanedViewer->setConfiguration(cifHandler_.cleanedUnitCellConfiguration());
    ui_.OutputViewer->setConfiguration(cifHandler_.supercellConfiguration());

    auto *supercellConfiguration = cifHandler_.supercellConfiguration();

    // Update the information panel
    ui_.SupercellBoxALabel->setText(QString::number(supercellConfiguration->box()->axisLengths().x) + " &#8491;");
    ui_.SupercellBoxBLabel->setText(QString::number(supercellConfiguration->box()->axisLengths().y) + " &#8491;");
    ui_.SupercellBoxCLabel->setText(QString::number(supercellConfiguration->box()->axisLengths().z) + " &#8491;");
    ui_.SupercellBoxAlphaLabel->setText(QString::number(supercellConfiguration->box()->axisAngles().x) + "&deg;");
    ui_.SupercellBoxBetaLabel->setText(QString::number(supercellConfiguration->box()->axisAngles().y) + "&deg;");
    ui_.SupercellBoxGammaLabel->setText(QString::number(supercellConfiguration->box()->axisAngles().z) + "&deg;");
    auto chemicalDensity = cifHandler_.supercellConfiguration()->chemicalDensity();
    ui_.SupercellDensityLabel->setText(chemicalDensity ? QString::number(*chemicalDensity) + " g cm<sup>3</sup>"
                                                       : "-- g cm<sup>3</sup>");
    ui_.SupercellVolumeLabel->setText(QString::number(supercellConfiguration->box()->volume()) + " &#8491;<sup>3</sup>");
    ui_.SupercellNAtomsLabel->setText(QString::number(supercellConfiguration->nAtoms()));

    if (ui_.OutputMolecularRadio->isChecked())
    {
        ui_.OutputMolecularSpeciesList->setVisible(true);
        ui_.OutputMolecularSpeciesList->clear();
        for (auto &molecularSp : cifHandler_.molecularSpecies())
        {
            ui_.OutputMolecularSpeciesList->addItem(QString::fromStdString(std::string(molecularSp.species()->name())));
        }
    }
    else
    {
        ui_.OutputMolecularSpeciesList->setVisible(false);
    }

    ui_.OutputConfigurationCheck->setEnabled(ui_.OutputMolecularRadio->isChecked() || ui_.OutputFrameworkRadio->isChecked());

    if (ui_.OutputSupermoleculeRadio->isChecked())
    {
        ui_.OutputConfigurationCheck->setChecked(false);
        outputFlags_.removeFlag(CIFHandler::OutputFlags::OutputConfiguration);
    }

    auto validSpecies = true;
    QString indicatorText("Species are valid.");

    if (ui_.OutputMolecularRadio->isChecked())
    {
        if (cifHandler_.molecularSpecies().empty())
        {
            validSpecies = false;
            indicatorText =
                QString("Unable to generate molecular partitioning from Species, so cannot be used in a simulation. "
                        "Choose a different partitioning.");
        }
    }
    else
    {
        auto *supercell = cifHandler_.supercellSpecies();
        if (!supercell || supercell->nAtoms() != supercellConfiguration->molecule(0)->nAtoms())
        {
            validSpecies = false;
            indicatorText = QString("Species contains more than one molecule/fragment, and cannot be used in a "
                                    "simulation. Choose a different partitioning.");
        }
    }

    ui_.OutputLabel->setText(indicatorText);
    ui_.OutputIndicator->setOK(validSpecies);

    updateProgressionControls();

    return true;
}
