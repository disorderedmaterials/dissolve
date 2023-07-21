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

    // Create preview configurations
    structureConfiguration_ = temporaryCoreData_.addConfiguration();
    structureConfiguration_->setName("Crystal");
    cleanedConfiguration_ = temporaryCoreData_.addConfiguration();
    cleanedConfiguration_->setName("Crystal (Cleaned)");
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
        // Grab indices and atom references
        auto [indexI, indexJ] = pair;
        if (indexI == indexJ)
            continue;
        auto &i = sp->atom(indexI);
        auto &j = sp->atom(indexJ);

        // Prevent metallic bonding?
        if (ui_.BondingPreventMetallicCheck->isChecked() && Elements::isMetallic(i.Z()) && Elements::isMetallic(j.Z()))
            continue;

        // Retrieve distance
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
        case (ImportCIFDialog::SelectSpaceGroupPage):
            return ui_.SpaceGroupsList->currentRow() != -1;
        case (ImportCIFDialog::OutputSpeciesPage):
            // If the "Framework" or "Supermolecule" options are chosen, the "Crystal" species must be a single moiety
            if (species_.empty() && (ui_.OutputFrameworkRadio->isChecked() || ui_.OutputSupermoleculeRadio->isChecked()))
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
            if (cleanedSpecies_->fragment(0).size() != cleanedSpecies_->nAtoms())
                distinctSpecies();
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
    if (species_.empty())
    {
        auto *supercell = temporaryCoreData_.findSpecies("Supercell");
        assert(supercell);

        if (ui_.OutputFrameworkRadio->isChecked())
        {
            // Just copy the species
            supercell->updateIntramolecularTerms();
            auto *sp = dissolve_.copySpecies(supercell);
            sp->setName(cifImporter_.chemicalFormula());
        }
        else if (ui_.OutputSupermoleculeRadio->isChecked())
        {
            // Copy the species
            supercell->updateIntramolecularTerms();
            auto *sp = dissolve_.copySpecies(supercell);
            sp->setName(cifImporter_.chemicalFormula());

            // Remove the unit cell and any cell-crossing bonds
            sp->removePeriodicBonds();
            sp->updateIntramolecularTerms();
            sp->removeBox();
        }
    }
    else
    {
        // Create a Configuration
        auto *cfg = dissolve_.addConfiguration();
        cfg->setName(cifImporter_.chemicalFormula());
        auto cellLengths = cifImporter_.getCellLengths();
        auto cellAngles = cifImporter_.getCellAngles();
        cfg->createBoxAndCells(cellLengths.value(), cellAngles.value(), false, 1.0);

        auto &generator = cfg->generator();
        for (auto i = 0; i < species_.size(); ++i)
        {
            auto *sp = dissolve_.copySpecies(species_.at(i));

            // CoordinateSets
            auto coordsNode = generator.createRootNode<CoordinateSetsProcedureNode>(fmt::format("CoordinateSets {}", i), sp);
            coordsNode->keywords().setEnumeration("Source", CoordinateSetsProcedureNode::CoordinateSetSource::File);
            coordsNode->setSets(coordinates_.at(i));

            // Add
            auto addNode = generator.createRootNode<AddProcedureNode>(fmt::format("Add {}", i), coordsNode);
            addNode->keywords().set("Population", NodeValue(int(coordinates_.at(i).size())));
            addNode->keywords().setEnumeration("Positioning", AddProcedureNode::PositioningType::Current);
            addNode->keywords().set("Rotate", false);
            addNode->keywords().setEnumeration("BoxAction", AddProcedureNode::BoxActionStyle::None);
        }
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
    partitioningConfiguration_->empty();
    supercellConfiguration_->empty();
    structureConfiguration_->empty();

    temporaryCoreData_.species().clear();
    temporaryCoreData_.atomTypes().clear();
    crystalSpecies_ = nullptr;
    cleanedSpecies_ = nullptr;

    // Create temporary atom types corresponding to the unique atom labels
    for (auto &a : cifImporter_.assemblies())
        for (auto &g : a.groups())
            if (g.active())
                for (auto &i : g.atoms())
                    if (!temporaryCoreData_.findAtomType(i.label()))
                    {
                        auto at = temporaryCoreData_.addAtomType(i.Z());
                        at->setName(i.label());
                    }

    // Generate a single species containing the entire crystal
    crystalSpecies_ = temporaryCoreData_.addSpecies();
    crystalSpecies_->setName("Crystal");
    // -- Set unit cell
    auto cellLengths = cifImporter_.getCellLengths();
    if (!cellLengths)
        return false;
    auto cellAngles = cifImporter_.getCellAngles();
    if (!cellAngles)
        return false;
    crystalSpecies_->createBox(cellLengths.value(), cellAngles.value());
    auto *box = crystalSpecies_->box();
    structureConfiguration_->createBoxAndCells(cellLengths.value(), cellAngles.value(), false, 1.0);
    // -- Generate atoms
    auto symmetryGenerators = SpaceGroups::symmetryOperators(cifImporter_.spaceGroup());
    auto tolerance = ui_.NormalOverlapToleranceRadio->isChecked() ? 0.1 : 0.5;
    for (const auto &generator : symmetryGenerators)
        for (auto &a : cifImporter_.assemblies())
            for (auto &g : a.groups())
                if (g.active())
                    for (auto &unique : g.atoms())
                    {
                        // Generate folded atomic position in real space
                        auto r = generator * unique.rFrac();
                        box->toReal(r);
                        r = box->fold(r);

                        // If this atom overlaps with another in the box, don't add it as it's a symmetry-related copy
                        if (std::any_of(crystalSpecies_->atoms().begin(), crystalSpecies_->atoms().end(),
                                        [&r, box, tolerance](const auto &j)
                                        { return box->minimumDistance(r, j.r()) < tolerance; }))
                            continue;

                        // Create the new atom
                        auto i = crystalSpecies_->addAtom(unique.Z(), r);
                        crystalSpecies_->atom(i).setAtomType(temporaryCoreData_.findAtomType(unique.label()));
                    }

    // Bonding
    if (ui_.CalculateBondingRadio->isChecked())
        crystalSpecies_->addMissingBonds(1.1, ui_.BondingPreventMetallicCheck->isChecked());
    else
        applyCIFBonding(crystalSpecies_);

    // Add the structural species to the configuration
    structureConfiguration_->addMolecule(crystalSpecies_);
    structureConfiguration_->updateObjectRelationships();

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
    cleanedConfiguration_->empty();

    // Remove the cleaned crystal species
    if (cleanedSpecies_)
        temporaryCoreData_.removeSpecies(cleanedSpecies_);

    // Copy the crystal species
    cleanedSpecies_ = temporaryCoreData_.addSpecies();
    cleanedSpecies_->setName("Crystal (Cleaned)");
    if (!crystalSpecies_)
        return Messenger::error("Crystal reference species doesn't exist.\n");
    cleanedSpecies_->copyBasic(crystalSpecies_, true);
    // -- Set unit cell
    auto cellLengths = cifImporter_.getCellLengths();
    if (!cellLengths)
        return false;
    auto cellAngles = cifImporter_.getCellAngles();
    if (!cellAngles)
        return false;
    cleanedSpecies_->createBox(cellLengths.value(), cellAngles.value());
    cleanedConfiguration_->createBoxAndCells(cellLengths.value(), cellAngles.value(), false, 1.0);

    // Atomics
    if (ui_.MoietyRemoveAtomicsCheck->isChecked())
    {
        std::vector<int> indicesToRemove;
        for (const auto &i : cleanedSpecies_->atoms())
            if (i.nBonds() == 0)
                indicesToRemove.push_back(i.index());
        Messenger::print("Atomic removal deleted {} atoms.\n", indicesToRemove.size());

        // Remove selected atoms
        cleanedSpecies_->removeAtoms(indicesToRemove);
    }

    // Water or coordinated oxygens
    if (ui_.MoietyRemoveWaterCheck->isChecked())
    {
        NETADefinition waterVacuum("?O,nbonds=1,nh<=1|?O,nbonds>=2,-H(nbonds=1,-O)");
        if (!waterVacuum.isValid())
            return Messenger::error("NETA definition for water removal is invalid.\n");

        std::vector<int> indicesToRemove;
        for (const auto &i : cleanedSpecies_->atoms())
            if (waterVacuum.matches(&i))
                indicesToRemove.push_back(i.index());
        Messenger::print("Water removal deleted {} atoms.\n", indicesToRemove.size());

        // Remove selected atoms
        cleanedSpecies_->removeAtoms(indicesToRemove);
    }

    // Custom NETA Removal
    if (ui_.MoietyRemoveByNETAGroup->isChecked() && moietyNETA_.isValid())
    {
        // Select all atoms that are in moieties where one of its atoms matches our NETA definition
        std::vector<int> indicesToRemove;
        for (auto &i : cleanedSpecies_->atoms())
            if (moietyNETA_.matches(&i))
            {
                // Select all atoms that are part of the same moiety?
                if (ui_.MoietyNETARemoveFragmentsCheck->isChecked())
                {
                    cleanedSpecies_->clearAtomSelection();
                    auto selection = cleanedSpecies_->fragment(i.index());
                    std::copy(selection.begin(), selection.end(), std::back_inserter(indicesToRemove));
                }
                else
                    indicesToRemove.push_back(i.index());
            }
        Messenger::print("Moiety removal deleted {} atoms.\n", indicesToRemove.size());

        // Remove selected atoms
        cleanedSpecies_->removeAtoms(indicesToRemove);
    }

    // Add the structural species to the configuration
    cleanedConfiguration_->addMolecule(cleanedSpecies_);
    cleanedConfiguration_->updateObjectRelationships();

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

bool ImportCIFDialog::distinctSpecies()
{
    // Temporary configuration
    auto cfg = temporaryCoreData_.addConfiguration();
    cfg->createBoxAndCells(cifImporter_.getCellLengths().value(), cifImporter_.getCellAngles().value(), false, 1.0);

    std::vector<std::vector<int>> fragments;
    auto idx = 0;
    std::vector<int> indices(cleanedSpecies_->nAtoms());
    std::iota(indices.begin(), indices.end(), 0);

    // Find all of the fragments
    while (!indices.empty())
    {
        // Choose a fragment
        auto fragment = cleanedSpecies_->fragment(idx);
        std::sort(fragment.begin(), fragment.end());
        fragments.push_back(fragment);
        // Remove all of the indices associated with the fragment
        indices.erase(std::remove_if(indices.begin(), indices.end(),
                                     [&](int value)
                                     { return std::find(fragment.begin(), fragment.end(), value) != fragment.end(); }),
                      indices.end());

        // Choose starting index of the next fragment
        idx = *std::min_element(indices.begin(), indices.end());
    }

    // Construct sp NETA definitions from the fragments
    // By sp, we mean that the definition produces a single match,
    // Therefore, the 'reference atom' is unique within the fragment.
    std::vector<NETADefinition> definitions;
    for (auto &fragment : fragments)
    {
        auto *tempSpecies = temporaryCoreData_.addSpecies();
        tempSpecies->copyBasic(cleanedSpecies_);

        // Remove all atoms from the temporary species, except those in the fragment.
        std::vector<int> allIndices(tempSpecies->nAtoms());
        std::iota(allIndices.begin(), allIndices.end(), 0);
        std::vector<int> indicesToRemove;
        std::set_difference(allIndices.begin(), allIndices.end(), fragment.begin(), fragment.end(),
                            std::back_inserter(indicesToRemove));
        tempSpecies->removeAtoms(indicesToRemove);

        NETADefinition neta;
        neta.flags() += NETADefinition::NETAFlags::MatchHydrogens;
        neta.flags() += NETADefinition::NETAFlags::IncludeRootElement;

        // Find a sp definition, if one exists.
        auto nDistinctMatches = 0;
        idx = 0;
        while (nDistinctMatches != 1)
        {
            if (idx >= tempSpecies->nAtoms())
                return false;
            neta.create(&tempSpecies->atom(idx++), 128);
            nDistinctMatches = std::count_if(tempSpecies->atoms().begin(), tempSpecies->atoms().end(),
                                             [&](const auto &i) { return neta.matches(&i); });
        }
        definitions.push_back(neta);
    }

    // Remove duplicate definitions
    definitions.erase(std::unique(definitions.begin(), definitions.end(),
                                  [](const auto &a, const auto &b)
                                  { return DissolveSys::sameString(a.definitionString(), b.definitionString()); }),
                      definitions.end());

    for (auto &neta : definitions)
    {
        std::vector<std::vector<int>> matchedIndices;
        std::vector<std::vector<const SpeciesAtom *>> matches;
        auto *sp = temporaryCoreData_.addSpecies();
        sp->copyBasic(cleanedSpecies_);

        for (auto &i : sp->atoms())
        {
            if (neta.matches(&i))
            {
                auto matchedGroup = neta.matchedPath(&i);
                auto matchedAtomsSet = matchedGroup.set();
                std::vector<const SpeciesAtom *> matchedAtoms(matchedAtomsSet.size());
                std::copy(matchedAtomsSet.begin(), matchedAtomsSet.end(), matchedAtoms.begin());
                std::vector<int> indices(matchedAtoms.size());
                std::transform(matchedAtomsSet.begin(), matchedAtomsSet.end(), indices.begin(),
                               [](const auto &atom) { return atom->index(); });
                matches.push_back(matchedAtoms);
                matchedIndices.push_back(std::move(indices));
            }
        }

        // Remove all atoms from the species, except those in the first match
        // The remaning atoms will constitute the Species
        std::vector<int> allIndices(sp->nAtoms());
        std::iota(allIndices.begin(), allIndices.end(), 0);
        std::vector<int> indicesToRemove;
        std::set_difference(allIndices.begin(), allIndices.end(), matchedIndices.front().begin(), matchedIndices.front().end(),
                            std::back_inserter(indicesToRemove));
        sp->removeAtoms(indicesToRemove);

        // Name the Species
        sp->setName(EmpiricalFormula::formula(sp->atoms(), [&](const auto &at) { return at.Z(); }));

        // 'Fix' the geometry of the species
        // Construct a temporary molecule, which is just the species.
        Molecule mol;
        mol->setSpecies(sp);
        std::vector<Atom> molAtoms(sp->nAtoms());
        for (auto &&[spAtom, atom] : zip(sp->atoms(), molAtoms))
        {
            atom.setSpeciesAtom(&spAtom);
            atom.setCoordinates(spAtom.r());
            mol->addAtom(&atom);
        }

        // Unfold the molecule
        mol->unFold(cleanedSpecies_->box());

        // Update the coordinates of atoms in the species
        for (auto n = 0; n < sp->nAtoms(); ++n)
            sp->atom(n).setCoordinates(mol->atom(n)->r());

        // Set the centre of geometry of the species to be at the origin.
        sp->setCentre(sp->box(), {0., 0., 0.});

        species_.push_back(sp);

        // Extract coordinates, to be used in the Configuration later on.
        // These coordinates correspond to the molecules of the Species within the crystal structure.
        std::vector<std::vector<Vec3<double>>> coordinates;
        for (auto &match : matches)
        {
            std::vector<Vec3<double>> coords;
            for (auto &i : match)
                coords.push_back(i->r());
            coordinates.push_back(std::move(coords));
        }
        coordinates_.push_back(std::move(coordinates));
    }

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
    supercell = temporaryCoreData_.addSpecies();
    supercell->setName("Supercell");

    // Set the repeat vector
    Vec3<int> repeat(ui_.RepeatASpin->value(), ui_.RepeatBSpin->value(), ui_.RepeatCSpin->value());

    // Add the cleaned crystal species and set a new Supercell box
    if (!cleanedSpecies_)
        return Messenger::error("Cleaned crystal reference species doesn't exist.\n");
    auto supercellLengths = cleanedSpecies_->box()->axisLengths();
    supercellLengths.multiply(repeat.x, repeat.y, repeat.z);
    supercell->createBox(supercellLengths, cleanedSpecies_->box()->axisAngles(), false);
    supercellConfiguration_->createBoxAndCells(supercellLengths, cleanedSpecies_->box()->axisAngles(), false, 1.0);

    // Copy atoms from the Crystal species - we'll do the bonding afterwards
    supercell->atoms().reserve(repeat.x * repeat.y * repeat.z * cleanedSpecies_->nAtoms());
    for (auto ix = 0; ix < repeat.x; ++ix)
        for (auto iy = 0; iy < repeat.y; ++iy)
            for (auto iz = 0; iz < repeat.z; ++iz)
            {
                Vec3<double> deltaR = cleanedSpecies_->box()->axes() * Vec3<double>(ix, iy, iz);
                for (const auto &i : cleanedSpecies_->atoms())
                    supercell->addAtom(i.Z(), i.r() + deltaR, 0.0, i.atomType());
            }

    // Bonding
    if (ui_.CalculateBondingRadio->isChecked())
        supercell->addMissingBonds();
    else
        applyCIFBonding(supercell);

    // Add the structural species to the configuration
    supercellConfiguration_->addMolecule(supercell);
    supercellConfiguration_->updateObjectRelationships();
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

    if (species_.empty())
    {
        // Set up the basic configuration
        auto *sp = temporaryCoreData_.findSpecies("Supercell");
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
