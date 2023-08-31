// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "io/import/cif.h"
#include "CIFImportLexer.h"
#include "base/messenger.h"
#include "base/sysFunc.h"
#include "classes/coreData.h"
#include "classes/empiricalFormula.h"
#include "classes/species.h"
#include "io/import/CIFImportErrorListeners.h"
#include "io/import/CIFImportVisitor.h"
#include "io/import/cif.h"
#include "neta/neta.h"
#include "procedure/nodes/add.h"
#include "procedure/nodes/box.h"
#include "procedure/nodes/coordinateSets.h"
#include "templates/algorithms.h"

CIFHandler::CIFHandler()
{
    structuralConfiguration_ = coreData_.addConfiguration();
    structuralConfiguration_->setName("Structural");
    cleanedConfiguration_ = coreData_.addConfiguration();
    cleanedConfiguration_->setName("Cleaned");
    molecularConfiguration_ = coreData_.addConfiguration();
    molecularConfiguration_->setName("Molecular");
    supercellConfiguration_ = coreData_.addConfiguration();
    supercellConfiguration_->setName("Supercell");
    partitionedConfiguration_ = coreData_.addConfiguration();
    partitionedConfiguration_->setName("Partitioned");
}

// Parse supplied file into the destination objects
bool CIFHandler::parse(std::string filename, CIFHandler::CIFTags &tags) const
{
    // Set up ANTLR input stream
    std::ifstream cifFile(std::string(filename), std::ios::in | std::ios::binary);
    if (!cifFile.is_open())
        return Messenger::error("Failed to open CIF file '{}.\n", filename);

    antlr4::ANTLRInputStream input(cifFile);

    // Create ANTLR lexer and set-up error listener
    CIFImportLexer lexer(&input);
    CIFImportLexerErrorListener lexerErrorListener;
    lexer.removeErrorListeners();
    lexer.addErrorListener(&lexerErrorListener);

    // Generate tokens from input stream
    antlr4::CommonTokenStream tokens(&lexer);

    // Create ANTLR parser and set-up error listeners
    CIFImportParser parser(&tokens);
    CIFImportParserErrorListener parserErrorListener;
    parser.removeErrorListeners();
    parser.removeParseListeners();
    parser.addErrorListener(&lexerErrorListener);
    parser.addErrorListener(&parserErrorListener);

    // Generate the AST
    CIFImportParser::CifContext *tree = nullptr;
    try
    {
        tree = parser.cif();
    }
    catch (CIFImportExceptions::CIFImportSyntaxException &ex)
    {
        Messenger::error(ex.what());
        return false;
    }

    // Visit the nodes in the AST
    CIFImportVisitor visitor(tags);
    try
    {
        visitor.extract(tree);
    }
    catch (CIFImportExceptions::CIFImportSyntaxException &ex)
    {
        return Messenger::error(ex.what());
    }

    return true;
}

// Return whether the specified file parses correctly
bool CIFHandler::validFile(std::string filename) const
{
    CIFTags tags;
    return parse(filename, tags);
}

// Read CIF data from specified file
bool CIFHandler::read(std::string filename)
{
    assemblies_.clear();
    bondingPairs_.clear();
    tags_.clear();

    if (!parse(filename, tags_))
        return Messenger::error("Failed to parse CIF file '{}'.\n", filename);

    /*
     * Determine space group - the search order for tags is:
     *
     * 1. Hall symbol
     * 2. Hermann-Mauginn name
     * 3. Space group index
     *
     * In the case of 2 or 3 we also try to search for the origin choice.
     *
     * If a space group has already been set, don't try to overwrite it (it was probably forcibly set because the detection
     * below fails).
     */

    // Check for Hall symbol
    if (spaceGroup_ == SpaceGroups::NoSpaceGroup && hasTag("_space_group_name_Hall"))
        spaceGroup_ = SpaceGroups::findByHallSymbol(*getTagString("_space_group_name_Hall"));
    if (spaceGroup_ == SpaceGroups::NoSpaceGroup && hasTag("_symmetry_space_group_name_Hall"))
        spaceGroup_ = SpaceGroups::findByHallSymbol(*getTagString("_symmetry_space_group_name_Hall"));

    if (spaceGroup_ == SpaceGroups::NoSpaceGroup)
    {
        // Might need the coordinate system code...
        auto sgCode = getTagString("_space_group.IT_coordinate_system_code");

        // Find a HM name
        if (hasTag("_space_group_name_H-M_alt"))
            spaceGroup_ =
                SpaceGroups::findByHermannMauginnSymbol(*getTagString("_space_group_name_H-M_alt"), sgCode.value_or(""));
        if (spaceGroup_ == SpaceGroups::NoSpaceGroup && hasTag("_symmetry_space_group_name_H-M"))
            spaceGroup_ =
                SpaceGroups::findByHermannMauginnSymbol(*getTagString("_symmetry_space_group_name_H-M"), sgCode.value_or(""));

        // Find a space group index?
        if (spaceGroup_ == SpaceGroups::NoSpaceGroup && hasTag("_space_group_IT_number"))
            spaceGroup_ =
                SpaceGroups::findByInternationalTablesIndex(*getTagInt("_space_group_IT_number"), sgCode.value_or(""));
        if (spaceGroup_ == SpaceGroups::NoSpaceGroup && hasTag("_space_group.IT_number"))
            spaceGroup_ =
                SpaceGroups::findByInternationalTablesIndex(*getTagInt("_space_group.IT_number"), sgCode.value_or(""));
        if (spaceGroup_ == SpaceGroups::NoSpaceGroup && hasTag("_symmetry_Int_Tables_number"))
            spaceGroup_ =
                SpaceGroups::findByInternationalTablesIndex(*getTagInt("_symmetry_Int_Tables_number"), sgCode.value_or(""));
    }

    // Create symmetry-unique atoms list
    auto atomSiteLabel = getTagStrings("_atom_site_label");
    auto atomSiteTypeSymbol = getTagStrings("_atom_site_type_symbol");
    auto atomSiteFractX = getTagDoubles("_atom_site_fract_x");
    auto atomSiteFractY = getTagDoubles("_atom_site_fract_y");
    auto atomSiteFractZ = getTagDoubles("_atom_site_fract_z");
    auto atomSiteOccupancy = getTagDoubles("_atom_site_occupancy");
    auto atomDisorderAssembly = getTagStrings("_atom_site_disorder_assembly");
    auto atomDisorderGroup = getTagStrings("_atom_site_disorder_group");
    if (atomSiteLabel.empty() && atomSiteTypeSymbol.empty())
        return Messenger::error(
            "No suitable atom site names found (no '_atom_site_label' or '_atom_site_type_symbol' tags present in CIF).\n");
    if (atomSiteFractX.empty() || atomSiteFractY.empty() || atomSiteFractZ.empty())
        return Messenger::error("Atom site fractional positions are incomplete (vector sizes are {}, {}, and {}).\n",
                                atomSiteFractX.size(), atomSiteFractY.size(), atomSiteFractZ.size());
    if (!((atomSiteFractX.size() == atomSiteFractY.size()) && (atomSiteFractX.size() == atomSiteFractZ.size())))
        return Messenger::error("Atom site fractional positions have mismatched sizes (vector sizes are {}, {}, and {}).\n",
                                atomSiteFractX.size(), atomSiteFractY.size(), atomSiteFractZ.size());
    for (auto n = 0; n < atomSiteFractX.size(); ++n)
    {
        // Get standard information
        auto label = n < atomSiteLabel.size() ? atomSiteLabel[n] : fmt::format("{}{}", atomSiteTypeSymbol[n], n);
        auto Z = n < atomSiteTypeSymbol.size()
                     ? Elements::element(atomSiteTypeSymbol[n])
                     : (n < atomSiteLabel.size() ? Elements::element(atomSiteLabel[n]) : Elements::Unknown);
        auto occ = n < atomSiteOccupancy.size() ? atomSiteOccupancy[n] : 1.0;
        Vec3<double> rFrac(atomSiteFractX[n], atomSiteFractY[n], atomSiteFractZ[n]);

        // Add the atom to an assembly - there are three possibilities regarding (disorder) grouping:
        //   1) A group is defined, but no assembly - add the atom to the 'Disorder' assembly
        //   2) An assembly and a group are defined - add it to that
        //   3) No group or assembly are defined - add the atom to the 'Global' assembly under a 'Default' group
        auto assemblyName = atomDisorderAssembly.empty() ? "." : atomDisorderAssembly[n];
        auto groupName = atomDisorderGroup.empty() ? "." : atomDisorderGroup[n];
        if (assemblyName == "." && groupName != ".")
            assemblyName = "Disorder";
        else if (assemblyName == "." && groupName == ".")
            assemblyName = "Global";

        if (groupName == ".")
            groupName = "Default";

        // Get the assembly and group that we're adding the atom to
        auto &assembly = getAssembly(assemblyName);
        auto &group = assembly.getGroup(groupName);
        group.setActive(groupName == "Default" || groupName == "1");
        group.addAtom({label, Z, rFrac, occ});
    }

    // Construct bonding pairs list
    auto bondLabelsI = getTagStrings("_geom_bond_atom_site_label_1");
    auto bondLabelsJ = getTagStrings("_geom_bond_atom_site_label_2");
    auto bondDistances = getTagDoubles("_geom_bond_distance");
    if (bondLabelsI.size() == bondLabelsJ.size() && (bondLabelsI.size() == bondDistances.size()))
    {
        for (auto &&[i, j, r] : zip(bondLabelsI, bondLabelsJ, bondDistances))
            bondingPairs_.emplace_back(i, j, r);
    }
    else
        Messenger::warn("Bonding pairs array sizes are mismatched, so no bonding information will be available.");

    return true;
}

// Return if the specified tag exists
bool CIFHandler::hasTag(std::string tag) const { return tags_.find(tag) != tags_.end(); }

// Return tag data string (if it exists) assuming a single datum (first in the vector)
std::optional<std::string> CIFHandler::getTagString(std::string tag) const
{
    auto it = tags_.find(tag);
    if (it == tags_.end())
        return std::nullopt;

    // Check data vector size
    if (it->second.size() != 1)
        Messenger::warn("Returning first datum for tag '{}', but {} are available.\n", tag, it->second.size());

    return it->second.front();
}

// Return tag data strings (if it exists)
std::vector<std::string> CIFHandler::getTagStrings(std::string tag) const
{
    auto it = tags_.find(tag);
    if (it == tags_.end())
        return {};

    return it->second;
}

// Return tag data as double (if it exists) assuming a single datum (first in the vector)
std::optional<double> CIFHandler::getTagDouble(std::string tag) const
{
    auto it = tags_.find(tag);
    if (it == tags_.end())
        return std::nullopt;

    // Check data vector size
    if (it->second.size() != 1)
        Messenger::warn("Returning first datum for tag '{}', but {} are available.\n", tag, it->second.size());

    double result;
    try
    {
        result = std::stod(it->second.front());
    }
    catch (...)
    {
        Messenger::error("Data tag '{}' contains a value that can't be converted to a double ('{}').\n", tag,
                         it->second.front());
        return std::nullopt;
    }

    return result;
}

// Return tag data doubles (if it exists)
std::vector<double> CIFHandler::getTagDoubles(std::string tag) const
{
    auto it = tags_.find(tag);
    if (it == tags_.end())
        return {};

    std::vector<double> v;
    for (const auto &s : it->second)
    {
        auto d = 0.0;
        try
        {
            d = std::stod(s);
        }
        catch (...)
        {
            Messenger::warn("Data tag '{}' contains a value that can't be converted to a double ('{}').\n", tag, s);
        }
        v.push_back(d);
    }

    return v;
}

// Return tag data as integer (if it exists) assuming a single datum (first in the vector)
std::optional<int> CIFHandler::getTagInt(std::string tag) const
{
    auto it = tags_.find(tag);
    if (it == tags_.end())
        return std::nullopt;

    // Check data vector size
    if (it->second.size() != 1)
        Messenger::warn("Returning first datum for tag '{}', but {} are available.\n", tag, it->second.size());

    int result;
    try
    {
        result = std::stoi(it->second.front());
    }
    catch (...)
    {
        Messenger::error("Data tag '{}' contains a value that can't be converted to an integer ('{}').\n", tag,
                         it->second.front());
        return std::nullopt;
    }

    return result;
}

/*
 * Processed Data
 */

// Set space group from index
void CIFHandler::setSpaceGroup(SpaceGroups::SpaceGroupId sgid) { spaceGroup_ = sgid; }

// Return space group information
SpaceGroups::SpaceGroupId CIFHandler::spaceGroup() const { return spaceGroup_; }

// Return cell lengths
std::optional<Vec3<double>> CIFHandler::getCellLengths() const
{
    auto a = getTagDouble("_cell_length_a");
    if (!a)
        Messenger::error("Cell length A not defined in CIF.\n");
    auto b = getTagDouble("_cell_length_b");
    if (!b)
        Messenger::error("Cell length B not defined in CIF.\n");
    auto c = getTagDouble("_cell_length_c");
    if (!c)
        Messenger::error("Cell length C not defined in CIF.\n");

    if (a && b && c)
        return Vec3<double>(a.value(), b.value(), c.value());
    else
        return std::nullopt;
}

// Return cell angles
std::optional<Vec3<double>> CIFHandler::getCellAngles() const
{
    auto alpha = getTagDouble("_cell_angle_alpha");
    if (!alpha)
        Messenger::error("Cell angle alpha not defined in CIF.\n");
    auto beta = getTagDouble("_cell_angle_beta");
    if (!beta)
        Messenger::error("Cell angle beta not defined in CIF.\n");
    auto gamma = getTagDouble("_cell_angle_gamma");
    if (!gamma)
        Messenger::error("Cell angle gamma not defined in CIF.\n");

    if (alpha && beta && gamma)
        return Vec3<double>(alpha.value(), beta.value(), gamma.value());
    else
        return std::nullopt;
}

// Return chemical formula
std::string CIFHandler::chemicalFormula() const
{
    auto it = tags_.find("_chemical_formula_sum");
    return (it != tags_.end() ? it->second.front() : "Unknown");
}

// Get (add or retrieve) named assembly
CIFAssembly &CIFHandler::getAssembly(std::string_view name)
{
    auto it = std::find_if(assemblies_.begin(), assemblies_.end(), [name](const auto &a) { return a.name() == name; });
    if (it != assemblies_.end())
        return *it;

    return assemblies_.emplace_back(name);
}

// Return atom assemblies
std::vector<CIFAssembly> &CIFHandler::assemblies() { return assemblies_; }

const std::vector<CIFAssembly> &CIFHandler::assemblies() const { return assemblies_; }

// Return whether any bond distances are defined
bool CIFHandler::hasBondDistances() const { return !bondingPairs_.empty(); }

// Return whether a bond distance is defined for the specified label pair
std::optional<double> CIFHandler::bondDistance(std::string_view labelI, std::string_view labelJ) const
{
    auto it = std::find_if(bondingPairs_.begin(), bondingPairs_.end(),
                           [labelI, labelJ](const auto &bp) {
                               return (bp.labelI() == labelI && bp.labelJ() == labelJ) ||
                                      (bp.labelI() == labelJ && bp.labelJ() == labelI);
                           });
    if (it != bondingPairs_.end())
        return it->r();
    return std::nullopt;
}

/*
 * Creation
 */

bool CIFHandler::update(double tolerance, Vec3<int> supercellRepeat, std::optional<NETADefinition> moietyNETA,
                        Flags<CIFHandler::UpdateFlags> flags)
{
    // Reset all species and configurations
    reset();

    if (!createStructuralSpecies(tolerance, flags))
        return false;
    Messenger::print("Created structural species");
    if (!createCleanedSpecies(flags, moietyNETA))
        return false;
    Messenger::print("Created cleaned species");
    if (cleanedSpecies_->fragment(0).size() != cleanedSpecies_->nAtoms())
    {
        if (!createMolecularSpecies())
            return false;
        Messenger::print("couldn't Created molecular species");
    }
    if (!createSupercellSpecies(supercellRepeat, flags))
        return false;
    Messenger::print("Created supercell species");
    if (molecularSpecies_.empty())
    {

        if (!createPartitionedSpecies(flags))
            return false;
        Messenger::print("Created sueprcell species");
    }
    return true;
}

void CIFHandler::reset()
{
    structuralConfiguration_->empty();
    structuralSpecies_ = nullptr;
    cleanedConfiguration_->empty();
    cleanedSpecies_ = nullptr;
    molecularSpecies_.clear();
    molecularConfiguration_->clear();
    supercellSpecies_ = nullptr;
    supercellConfiguration_->empty();
    partitionedSpecies_ = nullptr;
    partitionedConfiguration_->empty();
    coreData_.species().clear();
    coreData_.atomTypes().clear();
}

bool CIFHandler::isValid() const
{
    return !molecularSpecies_.empty() || partitionedSpecies_->fragment(0).size() != partitionedSpecies_->nAtoms();
}

// Create a structural species
bool CIFHandler::createStructuralSpecies(double tolerance, Flags<UpdateFlags> flags)
{
    // Create temporary atom types corresponding to the unique atom labels
    for (auto &a : assemblies_)
        for (auto &g : a.groups())
            if (g.active())
                for (auto &i : g.atoms())
                    if (!coreData_.findAtomType(i.label()))
                    {
                        auto at = coreData_.addAtomType(i.Z());
                        at->setName(i.label());
                    }

    // Generate a single species containing the entire crystal
    structuralSpecies_ = coreData_.addSpecies();
    structuralSpecies_->setName("Crystal");

    // -- Set unit cell
    auto cellLengths = getCellLengths();
    if (!cellLengths)
        return false;
    auto cellAngles = getCellAngles();
    if (!cellAngles)
        return false;
    structuralSpecies_->createBox(cellLengths.value(), cellAngles.value());
    auto *box = structuralSpecies_->box();

    // Configuration
    structuralConfiguration_->createBoxAndCells(cellLengths.value(), cellAngles.value(), false, 1.0);

    // -- Generate atoms
    auto symmetryGenerators = SpaceGroups::symmetryOperators(spaceGroup_);
    for (const auto &generator : symmetryGenerators)
        for (auto &a : assemblies_)
            for (auto &g : a.groups())
                if (g.active())
                    for (auto &unique : g.atoms())
                    {
                        // Generate folded atomic position in real space
                        auto r = generator * unique.rFrac();
                        box->toReal(r);
                        r = box->fold(r);

                        // If this atom overlaps with another in the box, don't add it as it's a symmetry-related copy
                        if (std::any_of(structuralSpecies_->atoms().begin(), structuralSpecies_->atoms().end(),
                                        [&r, box, tolerance](const auto &j)
                                        { return box->minimumDistance(r, j.r()) < tolerance; }))
                            continue;

                        // Create the new atom
                        auto i = structuralSpecies_->addAtom(unique.Z(), r);
                        structuralSpecies_->atom(i).setAtomType(coreData_.findAtomType(unique.label()));
                    }

    // Bonding
    if (flags.isSet(UpdateFlags::CalculateBonding))
        structuralSpecies_->addMissingBonds(1.1, flags.isSet(UpdateFlags::PreventMetallicBonding));
    else
        applyCIFBonding(structuralSpecies_, flags.isSet(UpdateFlags::PreventMetallicBonding));

    structuralConfiguration_->addMolecule(structuralSpecies_);
    structuralConfiguration_->updateObjectRelationships();

    return true;
}

// Create a cleaned structural species
bool CIFHandler::createCleanedSpecies(Flags<UpdateFlags> flags, std::optional<NETADefinition> moietyNETA)
{
    if (!structuralSpecies_)
        return false;

    cleanedSpecies_ = coreData_.addSpecies();
    cleanedSpecies_->setName("Crystal (Cleaned)");
    cleanedSpecies_->copyBasic(structuralSpecies_);

    // -- Set unit cell
    auto cellLengths = getCellLengths();
    if (!cellLengths)
        return false;
    auto cellAngles = getCellAngles();
    if (!cellAngles)
        return false;
    cleanedSpecies_->createBox(cellLengths.value(), cellAngles.value());
    auto *box = cleanedSpecies_->box();

    // Configuration
    cleanedConfiguration_->createBoxAndCells(cellLengths.value(), cellAngles.value(), false, 1.0);

    if (flags.isSet(UpdateFlags::CleanMoietyRemoveAtomics))
    {
        std::vector<int> indicesToRemove;
        for (const auto &i : cleanedSpecies_->atoms())
            if (i.nBonds() == 0)
                indicesToRemove.push_back(i.index());
        Messenger::print("Atomic removal deleted {} atoms.\n", indicesToRemove.size());

        // Remove selected atoms
        cleanedSpecies_->removeAtoms(indicesToRemove);
    }

    if (flags.isSet(UpdateFlags::CleanMoietyRemoveWater))
    {
        NETADefinition waterVacuum("?O,nbonds=1,nh<=1|?O,nbonds>=2,-H(nbonds=1,-O)");
        if (!waterVacuum.isValid())
        {
            Messenger::error("NETA definition for water removal is invalid.\n");
            return false;
        }

        std::vector<int> indicesToRemove;
        for (const auto &i : cleanedSpecies_->atoms())
            if (waterVacuum.matches(&i))
                indicesToRemove.push_back(i.index());
        Messenger::print("Water removal deleted {} atoms.\n", indicesToRemove.size());

        // Remove selected atoms
        cleanedSpecies_->removeAtoms(indicesToRemove);
    }

    if (flags.isSet(UpdateFlags::CleanMoietyRemoveNETA) && moietyNETA.has_value() && moietyNETA.value().isValid())
    {
        // Select all atoms that are in moieties where one of its atoms matches our NETA definition
        std::vector<int> indicesToRemove;
        for (auto &i : cleanedSpecies_->atoms())
            if (moietyNETA.value().matches(&i))
            {
                // Select all atoms that are part of the same moiety?
                if (flags.isSet(UpdateFlags::CleanRemoveBoundFragments))
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

    cleanedConfiguration_->addMolecule(cleanedSpecies_);
    cleanedConfiguration_->updateObjectRelationships();

    return true;
}

// Create molecular species
bool CIFHandler::createMolecularSpecies()
{
    if (!cleanedSpecies_)
        return false;

    std::vector<int> indices(cleanedSpecies_->nAtoms());
    std::iota(indices.begin(), indices.end(), 0);

    // Find all molecular species, and their instances
    auto idx = 0;
    while (!indices.empty())
    {
        auto *species = new CIFMolecularSpecies;
        // Choose a fragment
        auto fragment = cleanedSpecies_->fragment(idx);
        std::sort(fragment.begin(), fragment.end());

        // Set up the CIF species
        auto *sp = coreData_.addSpecies();
        sp->copyBasic(cleanedSpecies_);

        // Empty the species of all atoms, except those in the reference instance
        std::vector<int> allIndices(sp->nAtoms());
        std::iota(allIndices.begin(), allIndices.end(), 0);
        std::vector<int> indicesToRemove;
        std::set_difference(allIndices.begin(), allIndices.end(), fragment.begin(), fragment.end(),
                            std::back_inserter(indicesToRemove));
        sp->removeAtoms(indicesToRemove);

        // Determine a unique NETA definition describing the fragment
        auto neta = uniqueNETADefinition(sp);
        if (!neta.has_value())
            return false;

        // Find copies
        auto copies = speciesCopies(cleanedSpecies_, neta.value());

        // Remove the current fragment and all copies
        for (auto &copy : copies)
        {
            indices.erase(std::remove_if(indices.begin(), indices.end(),
                                         [&](int value) { return std::find(copy.begin(), copy.end(), value) != copy.end(); }),
                          indices.end());
        }

        // Determine coordinates
        auto coords = speciesCopiesCoordinates(cleanedSpecies_, copies);

        // Fix geometry
        fixGeometry(sp, cleanedSpecies_->box());

        // Give the species a name
        sp->setName(EmpiricalFormula::formula(sp->atoms(), [&](const auto &at) { return at.Z(); }));

        species->species = sp;
        species->netaString = neta->definitionString();
        species->instances = std::move(copies);
        species->coordinates = std::move(coords);
        molecularSpecies_.push_back(species);
        idx = *std::min_element(indices.begin(), indices.end());
    }

    return true;
}

// Create supercell species
bool CIFHandler::createSupercellSpecies(Vec3<int> repeat, Flags<CIFHandler::UpdateFlags> flags)
{
    supercellSpecies_ = coreData_.addSpecies();
    supercellSpecies_ = coreData_.addSpecies();
    supercellSpecies_->setName("Supercell");
    auto supercellLengths = cleanedSpecies_->box()->axisLengths();
    supercellLengths.multiply(repeat.x, repeat.y, repeat.z);
    supercellSpecies_->createBox(supercellLengths, cleanedSpecies_->box()->axisAngles(), false);

    // Configuration
    supercellConfiguration_->createBoxAndCells(supercellLengths, cleanedSpecies_->box()->axisAngles(), false, 1.0);

    // Copy atoms from the Crystal species - we'll do the bonding afterwards
    supercellSpecies_->atoms().reserve(repeat.x * repeat.y * repeat.z * cleanedSpecies_->nAtoms());
    for (auto ix = 0; ix < repeat.x; ++ix)
        for (auto iy = 0; iy < repeat.y; ++iy)
            for (auto iz = 0; iz < repeat.z; ++iz)
            {
                Vec3<double> deltaR = cleanedSpecies_->box()->axes() * Vec3<double>(ix, iy, iz);
                for (const auto &i : cleanedSpecies_->atoms())
                    supercellSpecies_->addAtom(i.Z(), i.r() + deltaR, 0.0, i.atomType());
            }

    if (flags.isSet(UpdateFlags::CalculateBonding))
        supercellSpecies_->addMissingBonds();
    else
        applyCIFBonding(supercellSpecies_, flags.isSet(UpdateFlags::PreventMetallicBonding));

    // Add the structural species to the configuration
    supercellConfiguration_->addMolecule(supercellSpecies_);
    supercellConfiguration_->updateObjectRelationships();

    return true;
}

bool CIFHandler::createPartitionedSpecies(Flags<UpdateFlags> flags)
{
    partitionedSpecies_ = coreData_.addSpecies();
    partitionedSpecies_->copyBasic(supercellSpecies_);
    partitionedConfiguration_->createBoxAndCells(supercellSpecies_->box()->axisLengths(),
                                                 supercellSpecies_->box()->axisAngles(), false, 1.0);
    if (flags.isSet(UpdateFlags::CreateSupermolecule))
    {
        partitionedSpecies_->removePeriodicBonds();
        partitionedSpecies_->updateIntramolecularTerms();
        partitionedSpecies_->removeBox();
    }
    // Add the partitioned species to the configuration
    partitionedConfiguration_->addMolecule(partitionedSpecies_);
    partitionedConfiguration_->updateObjectRelationships();
    return true;
}

/*
 * Helpers
 */

// Apply CIF bonding to a given species
void CIFHandler::applyCIFBonding(Species *sp, bool preventMetallicBonding)
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
        if (preventMetallicBonding && Elements::isMetallic(i.Z()) && Elements::isMetallic(j.Z()))
            continue;

        // Retrieve distance
        auto r = bondDistance(i.atomType()->name(), j.atomType()->name());
        if (!r)
            continue;
        else if (fabs(box->minimumDistance(i.r(), j.r()) - r.value()) < 1.0e-2)
            sp->addBond(&i, &j);
    }
}

// Determine a unique NETA definition corresponding to a given species
std::optional<NETADefinition> CIFHandler::uniqueNETADefinition(Species *sp)
{
    NETADefinition neta;
    auto nMatches = 0, idx = 0;
    while (nMatches != 1)
    {
        if (idx >= sp->nAtoms())
            return std::nullopt;
        neta.create(&sp->atom(idx++), std::nullopt,
                    Flags<NETADefinition::NETACreationFlags>(NETADefinition::NETACreationFlags::ExplicitHydrogens,
                                                             NETADefinition::NETACreationFlags::IncludeRootElement));
        nMatches = std::count_if(sp->atoms().begin(), sp->atoms().end(), [&](const auto &i) { return neta.matches(&i); });
    }
    return neta;
}

// Determine instances of a NETA definition in a given species
std::vector<std::vector<int>> CIFHandler::speciesCopies(Species *sp, NETADefinition neta)
{
    std::vector<std::vector<int>> copies;
    for (auto &i : sp->atoms())
    {
        if (neta.matches(&i))
        {
            auto matchedGroup = neta.matchedPath(&i);
            auto matchedAtoms = matchedGroup.set();
            std::vector<int> indices(matchedAtoms.size());
            std::transform(matchedAtoms.begin(), matchedAtoms.end(), indices.begin(),
                           [](const auto &atom) { return atom->index(); });
            std::sort(indices.begin(), indices.end());
            copies.push_back(std::move(indices));
        }
    }
    return copies;
}

// Determine coordinates of copies
std::vector<std::vector<Vec3<double>>> CIFHandler::speciesCopiesCoordinates(Species *sp, std::vector<std::vector<int>> copies)
{
    std::vector<std::vector<Vec3<double>>> coordinates;
    for (auto &copy : copies)
    {
        std::vector<Vec3<double>> coords(copy.size());
        std::transform(copy.begin(), copy.end(), coords.begin(), [&](auto i) { return sp->atom(i).r(); });
        coordinates.push_back(std::move(coords));
    }
    return coordinates;
}

// 'Fix' the geometry of a given species
void CIFHandler::fixGeometry(Species *sp, const Box *box)
{
    // 'Fix' the geometry of the species
    // Construct a temporary molecule, which is just the species.
    std::shared_ptr<Molecule> mol = std::make_shared<Molecule>();
    std::vector<Atom> molAtoms(sp->nAtoms());
    for (auto &&[spAtom, atom] : zip(sp->atoms(), molAtoms))
    {
        atom.setSpeciesAtom(&spAtom);
        atom.setCoordinates(spAtom.r());
        mol->addAtom(&atom);
    }

    // Unfold the molecule
    mol->unFold(box);

    // Update the coordinates of the species atoms
    for (auto &&[spAtom, atom] : zip(sp->atoms(), molAtoms))
        spAtom.setCoordinates(atom.r());

    // Set the centre of geometry of the species to be at the origin.
    sp->setCentre(box, {0., 0., 0.});
}

std::pair<std::vector<Species *>, Configuration *> CIFHandler::finalise(CoreData &coreData) const
{
    std::vector<Species *> species;
    Configuration *configuration;
    if (!molecularSpecies_.empty())
    {
        configuration = coreData.addConfiguration();
        configuration->setName(chemicalFormula());

        // Grab the generator
        auto &generator = configuration->generator();

        // Add Box
        auto boxNode = generator.createRootNode<BoxProcedureNode>({});
        auto cellLengths = getCellLengths().value();
        auto cellAngles = getCellAngles().value();
        boxNode->keywords().set("Lengths", Vec3<NodeValue>(cellLengths.get(0), cellLengths.get(1), cellLengths.get(2)));
        boxNode->keywords().set("Angles", Vec3<NodeValue>(cellAngles.get(0), cellAngles.get(1), cellAngles.get(2)));

        for (auto &cifMolecularSp : molecularSpecies_)
        {
            auto *sp = cifMolecularSp->species;
            // Add the species
            sp = coreData.copySpecies(cifMolecularSp->species);

            // Determine a unique suffix
            auto base = sp->name();
            std::string uniqueSuffix{base};
            if (!generator.nodes().empty())
            {
                // Start from the last root node
                auto root = generator.nodes().back();
                auto suffix = 0;

                // We use 'CoordinateSets' here, because in this instance we are working with (CoordinateSet, Add) pairs
                while (generator.rootSequence().nodeInScope(root, fmt::format("SymmetryCopies_{}", uniqueSuffix)) != nullptr)
                    uniqueSuffix = fmt::format("{}_{:02d}", base, ++suffix);
            }

            // CoordinateSets
            auto coordsNode =
                generator.createRootNode<CoordinateSetsProcedureNode>(fmt::format("SymmetryCopies_{}", uniqueSuffix), sp);
            coordsNode->keywords().setEnumeration("Source", CoordinateSetsProcedureNode::CoordinateSetSource::File);
            coordsNode->setSets(cifMolecularSp->coordinates);

            // Add
            auto addNode = generator.createRootNode<AddProcedureNode>(fmt::format("Add_{}", uniqueSuffix), coordsNode);
            addNode->keywords().set("Population", NodeValueProxy(int(cifMolecularSp->coordinates.size())));
            addNode->keywords().setEnumeration("Positioning", AddProcedureNode::PositioningType::Current);
            addNode->keywords().set("Rotate", false);
            addNode->keywords().setEnumeration("BoxAction", AddProcedureNode::BoxActionStyle::None);
        }
    }
    else if (supercellSpecies_)
    {
        auto *sp = coreData.addSpecies();
        sp->setName(chemicalFormula());
        sp->copyBasic(supercellSpecies_);
        species.push_back(sp);
    }
    return {species, configuration};
}

/*
 * Retrieval
 */

// Structural
Species *CIFHandler::structuralSpecies() { return structuralSpecies_; }
Configuration *CIFHandler::structuralConfiguration() { return structuralConfiguration_; }

// Cleaned
Species *CIFHandler::cleanedSpecies() { return cleanedSpecies_; }
Configuration *CIFHandler::cleanedConfiguration() { return cleanedConfiguration_; }

// Molecular
std::vector<Species *> CIFHandler::molecularSpecies()
{
    std::vector<Species *> molecularSpecies;
    for (auto &cifMolecularSpecies : molecularSpecies_)
        molecularSpecies.push_back(cifMolecularSpecies->species);
    return molecularSpecies;
}
Configuration *CIFHandler::molecularConfiguration() { return molecularConfiguration_; }

// Supercell
Species *CIFHandler::supercellSpecies() { return supercellSpecies_; }
Configuration *CIFHandler::supercellConfiguration() { return supercellConfiguration_; }

// Partitioned
Species *CIFHandler::partitionedSpecies() { return partitionedSpecies_; }

Configuration *CIFHandler::partitionedConfiguration() { return partitionedConfiguration_; }
