// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "classes/atomchangetoken.h"
#include "classes/atomtype.h"
#include "classes/pair_iter.h"
#include "classes/species.h"
#include "gui/helpers/listwidgetupdater.h"
#include "gui/helpers/treewidgetupdater.h"
#include "gui/importcifdialog.h"
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
    registerPage(ImportCIFDialog::SelectSpacegroupPage, "Choose Space Group", ImportCIFDialog::CIFInfoPage);
    registerPage(ImportCIFDialog::CIFInfoPage, "CIF Information", ImportCIFDialog::StructurePage);
    registerPage(ImportCIFDialog::StructurePage, "Basic Structure", ImportCIFDialog::SupercellPage);
    registerPage(ImportCIFDialog::SupercellPage, "Create Supercell", ImportCIFDialog::SpeciesPartitioningPage);
    registerPage(ImportCIFDialog::SpeciesPartitioningPage, "Species Partitioning");

    // Add spacegroup list
    for (auto n = 1; n <= SpaceGroup::nSpaceGroups(); ++n)
        ui_.SpacegroupsList->addItem(
            QString("%1 %2").arg(QString::number(n), QString::fromStdString(std::string(SpaceGroup::name(n)))));

    // Set assembly view model
    ui_.AssemblyView->setModel(&cifAssemblyModel_);
    ui_.AssemblyView->update();
    ui_.AssemblyView->expandAll();
    connect(&cifAssemblyModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QList<int> &)), this,
            SLOT(createStructuralSpecies()));
    connect(&cifAssemblyModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QList<int> &)),
            ui_.AssemblyView, SLOT(expandAll()));

    // Update moiety NETA
    createMoietyRemovalNETA(ui_.MoietyRemovalEdit->text().toStdString());

    // Create preview configurations
    structureConfiguration_ = temporaryCoreData_.addConfiguration();
    structureConfiguration_->setName("Crystal");
    supercellConfiguration_ = temporaryCoreData_.addConfiguration();
    supercellConfiguration_->setName("Supercell");
    partitioningConfiguration_ = temporaryCoreData_.addConfiguration();
    partitioningConfiguration_->setName("Partitioning");

    // Init the wizard
    initialise(this, ui_.MainStack, ImportCIFDialog::SelectCIFFilePage);
}

// Apply CIF bond definitions within specified species
void ImportCIFDialog::applyCIFBonding(Species *sp)
{
    auto *box = sp->box();
    auto pairs = PairIterator(sp->nAtoms());
    for (auto pair : pairs)
    {
        auto [indexI, indexJ] = pair;
        if (indexI == indexJ)
            return;
        auto &i = sp->atom(indexI);
        auto &j = sp->atom(indexJ);
        auto r = cifImporter_.bondDistance(i.atomType()->name(), j.atomType()->name());
        if (!r)
            continue;
        else if (fabs(box->minimumDistance(i.r(), j.r()) - r.value()) < 1.0e-2)
            sp->addBond(&i, &j);
    }
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
        case (ImportCIFDialog::SelectSpacegroupPage):
            return ui_.SpacegroupsList->currentRow() != -1;
        case (ImportCIFDialog::SpeciesPartitioningPage):
            // If the "No Partitioning" option is chosen, the "Crystal" species must be a single moiety
            if (ui_.PartioningNoneRadio->isChecked())
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
        case (ImportCIFDialog::SelectSpacegroupPage):
            cifImporter_.setSpaceGroupFromIndex(ui_.SpacegroupsList->currentRow() + 1);
            updateInfoPage();
            break;
        case (ImportCIFDialog::CIFInfoPage):
            ui_.BondFromCIFRadio->setEnabled(cifImporter_.hasBondDistances());
            ui_.BondFromCIFRadio->setChecked(cifImporter_.hasBondDistances());
            if (!createStructuralSpecies())
                return false;
            break;
        case (ImportCIFDialog::StructurePage):
            createSupercellSpecies();
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
        return cifImporter_.spaceGroup().isValid() ? ImportCIFDialog::CIFInfoPage : ImportCIFDialog::SelectSpacegroupPage;
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
    if (ui_.PartioningNoneRadio->isChecked())
    {
        // Copy the species to the main Dissolve instance and set its new name
        auto *supercell = temporaryCoreData_.findSpecies("Supercell");
        assert(supercell);
        supercell->updateIntramolecularTerms();
        auto *sp = dissolve_.copySpecies(supercell);
        sp->setName(cifImporter_.chemicalFormula());
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

void ImportCIFDialog::updateSpaceGroupPage()
{
    ui_.SpacegroupsList->setCurrentRow(-1);

    // Add any relevant space group dictionary keys we might have
    ui_.SpacegroupKeysTable->clearContents();
    ui_.SpacegroupKeysTable->setRowCount(5);
    auto row = 0;
    for (auto key : {"_space_group_IT_number", "_space_group_name_Hall", "_space_group_name_H-M_alt",
                     "_symmetry_space_group_name_Hall", "_symmetry_space_group_name_H-M"})
    {
        ui_.SpacegroupKeysTable->setItem(row, 0, new QTableWidgetItem(key));
        auto value = cifImporter_.getTagString(key);
        ui_.SpacegroupKeysTable->setItem(
            row++, 1, new QTableWidgetItem(value.has_value() ? QString::fromStdString(value.value()) : "<Not Found>"));
    }
    ui_.SpacegroupKeysTable->resizeColumnsToContents();
}

void ImportCIFDialog::on_SpacegroupsList_currentRowChanged(int row)
{
    if (currentPage().has_value() && currentPage().value() == ImportCIFDialog::SelectSpacegroupPage)
        updateProgressionControls();
}

void ImportCIFDialog::on_SpacegroupsList_itemDoubleClicked(QListWidgetItem *item) { goToNextPage(); }

/*
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
    if (cifImporter_.spaceGroup().isValid())
        ui_.InfoSpacegroupLabel->setText(
            QString("%1 (%2)")
                .arg(QString::fromStdString(std::string(cifImporter_.spaceGroup().formattedName())))
                .arg(cifImporter_.spaceGroup().internationalTableIndex()));
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
    partitioningConfiguration_->empty();
    supercellConfiguration_->empty();
    structureConfiguration_->empty();

    temporaryCoreData_.species().clear();
    temporaryCoreData_.atomTypes().clear();

    // Create temporary atom types corresponding to the unique atom labels
    for (auto &a : cifImporter_.assemblies())
    {
        for (auto &i : a.activeGroup().atoms())
            if (!temporaryCoreData_.findAtomType(i.label()))
            {
                auto at = temporaryCoreData_.addAtomType(i.Z());
                at->setName(i.label());
            }
    }

    // Generate a single species containing the entire crystal
    auto *sp = temporaryCoreData_.addSpecies();
    sp->setName("Crystal");
    // -- Set unit cell
    auto cellLengths = cifImporter_.getCellLengths();
    if (!cellLengths)
        return false;
    auto cellAngles = cifImporter_.getCellAngles();
    if (!cellAngles)
        return false;
    sp->createBox(cellLengths.value(), cellAngles.value());
    auto *box = sp->box();
    structureConfiguration_->createBoxAndCells(cellLengths.value(), cellAngles.value(), false, 7.0, 1.0);
    // -- Generate atoms
    auto symmetryGenerators = cifImporter_.spaceGroup().symmetryOperators();
    auto tolerance = ui_.NormalOverlapToleranceRadio->isChecked() ? 0.1 : 0.5;
    for (auto &generator : symmetryGenerators)
        for (auto &a : cifImporter_.assemblies())
            for (auto &unique : a.activeGroup().atoms())
            {
                // Generate folded atomic position in real space
                auto r = generator.transform(unique.rFrac());
                box->toReal(r);
                r = box->fold(r);

                // If this atom overlaps with another in the box, don't add it as it's a symmetry-related copy
                if (std::any_of(sp->atoms().begin(), sp->atoms().end(),
                                [&r, box, tolerance](const auto &j) { return box->minimumDistance(r, j.r()) < tolerance; }))
                    continue;

                // Create the new atom
                auto i = sp->addAtom(unique.Z(), r);
                sp->atom(i).setAtomType(temporaryCoreData_.findAtomType(unique.label()));
            }

    // Bonding
    if (ui_.CalculateBondingRadio->isChecked())
        sp->addMissingBonds();
    else
        applyCIFBonding(sp);

    // Moiety Removal
    if (ui_.MoietyRemovalGroup->isChecked() && moietyNETA_.isValid())
    {
        // Select all atoms that are in moieties where one of its atoms matches our NETA definition
        std::vector<int> indicesToRemove;
        for (auto &i : sp->atoms())
            if (moietyNETA_.matches(&i))
            {
                // Select all atoms that are part of the same moiety?
                if (ui_.MoietyRemoveFragmentsCheck->isChecked())
                {
                    sp->clearAtomSelection();
                    auto selection = sp->fragment(i.index());
                    std::copy(selection.begin(), selection.end(), std::back_inserter(indicesToRemove));
                }
                else
                    indicesToRemove.push_back(i.index());
            }
        Messenger::print("Moiety removal deleted {} atoms.\n", indicesToRemove.size());

        // Remove selected atoms
        sp->removeAtoms(indicesToRemove);
    }

    // Add the structural species to the configuration
    AtomChangeToken lock(*structureConfiguration_);
    structureConfiguration_->addMolecule(lock, sp);
    structureConfiguration_->updateCellContents();

    ui_.StructureViewer->setConfiguration(structureConfiguration_);

    return true;
}

// Create / check NETA definition for moiety removal
bool ImportCIFDialog::createMoietyRemovalNETA(std::string definition)
{
    auto result = moietyNETA_.create(definition);

    ui_.MoietyRemovalIndicator->setOK(result);

    return result;
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

void ImportCIFDialog::on_BondFromCIFRadio_clicked(bool checked)
{
    if (ui_.MainStack->currentIndex() == ImportCIFDialog::StructurePage)
        createStructuralSpecies();
}

void ImportCIFDialog::on_MoietyRemovalGroup_clicked(bool checked) { createStructuralSpecies(); }

void ImportCIFDialog::on_MoietyRemovalEdit_textEdited(const QString &text)
{
    createMoietyRemovalNETA(text.toStdString());
    if (moietyNETA_.isValid())
        createStructuralSpecies();
}

void ImportCIFDialog::on_MoietyRemoveFragmentsCheck_clicked(bool checked)
{
    if (moietyNETA_.isValid())
        createStructuralSpecies();
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
    supercell = temporaryCoreData_.addSpecies();
    supercell->setName("Supercell");

    // Set the repeat vector
    Vec3<int> repeat(ui_.RepeatASpin->value(), ui_.RepeatBSpin->value(), ui_.RepeatCSpin->value());

    // Get the Crystal species and set a new Supercell box
    const auto *crystal = temporaryCoreData_.findSpecies("Crystal");
    if (!crystal)
        return Messenger::error("Crystal reference species doesn't exist.\n");
    auto supercellLengths = crystal->box()->axisLengths();
    supercellLengths.multiply(repeat.x, repeat.y, repeat.z);
    supercell->createBox(supercellLengths, crystal->box()->axisAngles(), false);
    supercellConfiguration_->createBoxAndCells(supercellLengths, crystal->box()->axisAngles(), false, 7.0, 1.0);

    // Copy atoms from the Crystal species - we'll do the bonding afterwards
    supercell->atoms().reserve(repeat.x * repeat.y * repeat.z * crystal->nAtoms());
    for (auto ix = 0; ix < repeat.x; ++ix)
        for (auto iy = 0; iy < repeat.y; ++iy)
            for (auto iz = 0; iz < repeat.z; ++iz)
            {
                Vec3<double> deltaR = crystal->box()->axes() * Vec3<double>(ix, iy, iz);
                for (const auto &i : crystal->atoms())
                    supercell->addAtom(i.Z(), i.r() + deltaR, 0.0, i.atomType());
            }

    // Bonding
    if (ui_.CalculateBondingRadio->isChecked())
        supercell->addMissingBonds();
    else
        applyCIFBonding(supercell);

    // Add the structural species to the configuration
    AtomChangeToken lock(*supercellConfiguration_);
    supercellConfiguration_->addMolecule(lock, supercell);
    supercellConfiguration_->updateCellContents();
    ui_.SupercellViewer->setConfiguration(supercellConfiguration_);

    // Update the information panel
    ui_.SupercellBoxALabel->setText(QString::number(supercell->box()->axisLengths().x) + " &#8491;");
    ui_.SupercellBoxBLabel->setText(QString::number(supercell->box()->axisLengths().y) + " &#8491;");
    ui_.SupercellBoxCLabel->setText(QString::number(supercell->box()->axisLengths().z) + " &#8491;");
    ui_.SupercellBoxAlphaLabel->setText(QString::number(supercell->box()->axisAngles().x) + "&deg;");
    ui_.SupercellBoxBetaLabel->setText(QString::number(supercell->box()->axisAngles().y) + "&deg;");
    ui_.SupercellBoxGammaLabel->setText(QString::number(supercell->box()->axisAngles().z) + "&deg;");
    ui_.SupercellDensityLabel->setText(QString::number(supercellConfiguration_->chemicalDensity()) + " g cm<sup>3</sup>");
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

    // Set up the basic configuration
    auto *sp = temporaryCoreData_.findSpecies("Supercell");
    assert(sp);
    partitioningConfiguration_->createBoxAndCells(sp->box()->axisLengths(), sp->box()->axisAngles(), false, 7.0, 1.0);

    auto validSpecies = true;

    if (ui_.PartioningNoneRadio->isChecked())
    {
        // Add the supercell species to the configuration
        AtomChangeToken lock(*partitioningConfiguration_);
        partitioningConfiguration_->addMolecule(lock, sp);
        partitioningConfiguration_->updateCellContents();

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

    return true;
}
