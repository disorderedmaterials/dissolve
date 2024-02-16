// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

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
    unitCellConfiguration_ = coreData_.addConfiguration();
    unitCellConfiguration_->setName("Structural");
    cleanedUnitCellConfiguration_ = coreData_.addConfiguration();
    cleanedUnitCellConfiguration_->setName("Cleaned");
    molecularUnitCellConfiguration_ = coreData_.addConfiguration();
    molecularUnitCellConfiguration_->setName("Molecular");
    supercellConfiguration_ = coreData_.addConfiguration();
    supercellConfiguration_->setName("Supercell");
}

/*
 * Raw Data
 */

// Parse supplied file into the destination objects
bool CIFHandler::parse(std::string_view filename, CIFHandler::CIFTags &tags) const
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
bool CIFHandler::validFile(std::string_view filename) const
{
    CIFTags tags;
    return parse(filename, tags);
}

// Read CIF data from specified file
bool CIFHandler::read(std::string_view filename)
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
void CIFHandler::setSpaceGroup(SpaceGroups::SpaceGroupId sgid)
{
    if (spaceGroup_ == sgid)
        return;

    spaceGroup_ = sgid;
    generate(); // TODO From start
}

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

// Create basic unit cell
bool CIFHandler::createBasicUnitCell()
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
    unitCellSpecies_ = coreData_.addSpecies();
    unitCellSpecies_->setName("Crystal");

    // -- Set unit cell
    auto cellLengths = getCellLengths();
    if (!cellLengths)
        return false;
    auto cellAngles = getCellAngles();
    if (!cellAngles)
        return false;
    unitCellSpecies_->createBox(cellLengths.value(), cellAngles.value());
    auto *box = unitCellSpecies_->box();

    // Configuration
    Messenger::setQuiet(true);
    unitCellConfiguration_->createBoxAndCells(cellLengths.value(), cellAngles.value(), false, 1.0);
    Messenger::setQuiet(false);

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
                        if (std::any_of(unitCellSpecies_->atoms().begin(), unitCellSpecies_->atoms().end(),
                                        [&, r, box](const auto &j)
                                        { return box->minimumDistance(r, j.r()) < overlapTolerance_; }))
                            continue;

                        // Create the new atom
                        auto i = unitCellSpecies_->addAtom(unique.Z(), r);
                        unitCellSpecies_->atom(i).setAtomType(coreData_.findAtomType(unique.label()));
                    }

    // Check that we actually generated some atoms...
    if (unitCellSpecies_->nAtoms() == 0)
        return false;

    // Bonding
    if (useCIFBondingDefinitions_)
        applyCIFBonding(unitCellSpecies_, preventMetallicBonds_);
    else
        unitCellSpecies_->addMissingBonds(bondingTolerance_, preventMetallicBonds_);

    unitCellConfiguration_->addMolecule(unitCellSpecies_);
    unitCellConfiguration_->updateObjectRelationships();

    Messenger::print("Created basic crystal unit cell - {} non-overlapping atoms.\n", unitCellSpecies_->nAtoms());

    return true;
}

// Create the cleaned unit cell
bool CIFHandler::createCleanedUnitCell()
{
    if (!unitCellSpecies_)
        return false;

    cleanedUnitCellSpecies_ = coreData_.addSpecies();
    cleanedUnitCellSpecies_->setName("Crystal (Cleaned)");
    cleanedUnitCellSpecies_->copyBasic(unitCellSpecies_, true);

    // -- Set unit cell
    auto cellLengths = getCellLengths();
    if (!cellLengths)
        return false;
    auto cellAngles = getCellAngles();
    if (!cellAngles)
        return false;
    cleanedUnitCellSpecies_->createBox(cellLengths.value(), cellAngles.value());

    // Configuration
    Messenger::setQuiet(true);
    cleanedUnitCellConfiguration_->createBoxAndCells(cellLengths.value(), cellAngles.value(), false, 1.0);
    Messenger::setQuiet(false);

    if (removeAtomics_)
    {
        std::vector<int> indicesToRemove;
        for (const auto &i : cleanedUnitCellSpecies_->atoms())
            if (i.nBonds() == 0)
                indicesToRemove.push_back(i.index());
        Messenger::print("Atomic removal deleted {} atoms.\n", indicesToRemove.size());

        // Remove selected atoms
        cleanedUnitCellSpecies_->removeAtoms(indicesToRemove);
    }

    if (removeWaterAndCoordinateOxygens_)
    {
        NETADefinition waterVacuum("?O,nbonds=1,nh<=1|?O,nbonds>=2,-H(nbonds=1,-O)");
        if (!waterVacuum.isValid())
        {
            Messenger::error("NETA definition for water removal is invalid.\n");
            return false;
        }

        std::vector<int> indicesToRemove;
        for (const auto &i : cleanedUnitCellSpecies_->atoms())
            if (waterVacuum.matches(&i))
                indicesToRemove.push_back(i.index());
        Messenger::print("Water removal deleted {} atoms.\n", indicesToRemove.size());

        // Remove selected atoms
        cleanedUnitCellSpecies_->removeAtoms(indicesToRemove);
    }

    if (removeNETA_ && moietyRemovalNETA_.isValid())
    {
        // Select all atoms that are in moieties where one of its atoms matches our NETA definition
        std::vector<int> indicesToRemove;
        for (auto &i : cleanedUnitCellSpecies_->atoms())
            if (moietyRemovalNETA_.matches(&i))
            {
                // Select all atoms that are part of the same moiety?
                if (removeNETAByFragment_)
                {
                    cleanedUnitCellSpecies_->clearAtomSelection();
                    auto selection = cleanedUnitCellSpecies_->fragment(i.index());
                    std::copy(selection.begin(), selection.end(), std::back_inserter(indicesToRemove));
                }
                else
                    indicesToRemove.push_back(i.index());
            }
        Messenger::print("Moiety removal deleted {} atoms.\n", indicesToRemove.size());

        // Remove selected atoms
        cleanedUnitCellSpecies_->removeAtoms(indicesToRemove);
    }

    cleanedUnitCellConfiguration_->addMolecule(cleanedUnitCellSpecies_);
    cleanedUnitCellConfiguration_->updateObjectRelationships();

    Messenger::print("Created cleaned crystal unit cell - {} atoms after removal(s).\n", cleanedUnitCellSpecies_->nAtoms());

    return true;
}

// Try to detect molecules in the cell contents
bool CIFHandler::detectMolecules()
{
    if (!cleanedUnitCellSpecies_)
        return false;

    // Try selecting within the species from the first atom - if this captures all atoms we have a bound framework...
    if (cleanedUnitCellSpecies_->fragment(0).size() == cleanedUnitCellSpecies_->nAtoms())
    {
        Messenger::print(
            "Can't create molecular definitions since this unit cell appears to be a continuous framework/network. Consider "
            "adjusting the bonding options in order to generate molecular fragments.\n");
        return false;
    }

    std::vector<int> allAtomIndices(cleanedUnitCellSpecies_->nAtoms());
    std::iota(allAtomIndices.begin(), allAtomIndices.end(), 0);

    // Find all molecular species, and their instances
    auto idx = 0;
    while (!allAtomIndices.empty())
    {
        // Select a fragment
        auto fragmentIndices = cleanedUnitCellSpecies_->fragment(idx);
        std::sort(fragmentIndices.begin(), fragmentIndices.end());

        // Set up the CIF species
        auto *sp = coreData_.addSpecies();
        sp->copyBasic(cleanedUnitCellSpecies_);

        // Empty the species of all atoms, except those in the reference instance
        std::vector<int> speciesAtomIndices(sp->nAtoms());
        std::iota(speciesAtomIndices.begin(), speciesAtomIndices.end(), 0);
        std::vector<int> indicesToRemove;
        std::set_difference(speciesAtomIndices.begin(), speciesAtomIndices.end(), fragmentIndices.begin(),
                            fragmentIndices.end(), std::back_inserter(indicesToRemove));
        sp->removeAtoms(indicesToRemove);

        // Give the species a name
        sp->setName(EmpiricalFormula::formula(sp->atoms(), [&](const auto &at) { return at.Z(); }));

        // Fix geometry of the extracted species
        fixGeometry(sp, cleanedUnitCellSpecies_->box());

        // Find instances of this fragment. For large fragments that represent > 50% of the remaining atoms we don't even
        // attempt to create a NETA definition etc. For cases such as framework species this will speed up detection no end.
        std::vector<CIFLocalMolecule> instances;
        if (fragmentIndices.size() * 2 > allAtomIndices.size())
        {
            // Create an instance of the current fragment
            auto &mol = instances.emplace_back();
            mol.setSpecies(sp);
            for (auto i = 0; i < sp->nAtoms(); ++i)
            {
                mol.setAtom(i, sp->atom(i).r(), sp->atom(i).index());
            }

            allAtomIndices.erase(std::remove_if(allAtomIndices.begin(), allAtomIndices.end(),
                                                [&](int value) {
                                                    return std::find(fragmentIndices.begin(), fragmentIndices.end(), value) !=
                                                           fragmentIndices.end();
                                                }),
                                 allAtomIndices.end());
        }
        else
        {
            // Determine a unique NETA definition describing the fragment
            auto neta = uniqueNETADefinition(sp);
            if (!neta.has_value())
                return Messenger::error("Couldn't generate molecular partitioning for CIF - no unique NETA definition for the "
                                        "fragment {} could be determined.\n",
                                        sp->name());

            // Find instances of this fragmentIndices
            instances = getSpeciesInstances(sp, *neta);

            // Remove the current fragment and all copies
            for (const auto &instance : instances)
            {
                allAtomIndices.erase(std::remove_if(allAtomIndices.begin(), allAtomIndices.end(),
                                                    [&](int value)
                                                    {
                                                        return std::find(instance.unitCellIndices().begin(),
                                                                         instance.unitCellIndices().end(),
                                                                         value) != instance.unitCellIndices().end();
                                                    }),
                                     allAtomIndices.end());
            }
        }

        // Push a new definition
        molecularUnitCellSpecies_.emplace_back(sp, instances);

        // Search for the next valid starting index
        idx = *std::min_element(allAtomIndices.begin(), allAtomIndices.end());
    }

    Messenger::print("Partitioned unit cell into {} distinct molecular species:\n\n", molecularUnitCellSpecies_.size());
    Messenger::print("   ID     N  Species Formula\n");
    auto count = 1;
    for (const auto &cifMol : molecularUnitCellSpecies_)
        Messenger::print("  {:3d}  {:4d}  {}\n", count++, cifMol.instances().size(),
                         EmpiricalFormula::formula(cifMol.species()->atoms(), [](const auto &i) { return i.Z(); }));
    Messenger::print("");

    return true;
}

// Create supercell species
bool CIFHandler::createSupercell()
{
    auto supercellLengths = cleanedUnitCellSpecies_->box()->axisLengths();
    supercellLengths.multiply(supercellRepeat_.x, supercellRepeat_.y, supercellRepeat_.z);

    supercellSpecies_ = coreData_.addSpecies();
    supercellSpecies_->setName("Supercell");
    supercellSpecies_->createBox(supercellLengths, cleanedUnitCellSpecies_->box()->axisAngles(), false);

    // Configuration
    Messenger::setQuiet(true);
    supercellConfiguration_->createBoxAndCells(supercellLengths, cleanedUnitCellSpecies_->box()->axisAngles(), false, 1.0);
    Messenger::setQuiet(false);

    // Copy atoms from the Crystal species - we'll do the bonding afterwards
    if (molecularUnitCellSpecies_.empty())
    {
        supercellSpecies_->atoms().reserve(supercellRepeat_.x * supercellRepeat_.y * supercellRepeat_.z *
                                           cleanedUnitCellSpecies_->nAtoms());
        for (auto ix = 0; ix < supercellRepeat_.x; ++ix)
            for (auto iy = 0; iy < supercellRepeat_.y; ++iy)
                for (auto iz = 0; iz < supercellRepeat_.z; ++iz)
                {
                    Vec3<double> deltaR = cleanedUnitCellSpecies_->box()->axes() * Vec3<double>(ix, iy, iz);
                    for (const auto &i : cleanedUnitCellSpecies_->atoms())
                        supercellSpecies_->addAtom(i.Z(), i.r() + deltaR, 0.0, i.atomType());
                }
        if (useCIFBondingDefinitions_)
            applyCIFBonding(supercellSpecies_, preventMetallicBonds_);
        else
            supercellSpecies_->addMissingBonds(bondingTolerance_, preventMetallicBonds_);

        // Add the structural species to the configuration
        supercellConfiguration_->addMolecule(supercellSpecies_);
        supercellConfiguration_->updateObjectRelationships();
    }
    else
    {
        supercellSpecies_->atoms().reserve(supercellRepeat_.x * supercellRepeat_.y * supercellRepeat_.z *
                                           cleanedUnitCellSpecies_->nAtoms());

        // Create images of all molecular unit cell species
        for (auto &molecularSpecies : molecularUnitCellSpecies_)
        {
            const auto *sp = molecularSpecies.species();
            const auto &coreInstances = molecularSpecies.instances();
            std::vector<CIFLocalMolecule> supercellInstances;
            supercellInstances.reserve(supercellRepeat_.x * supercellRepeat_.y * supercellRepeat_.z * coreInstances.size());

            // Loop over cell images
            for (auto ix = 0; ix < supercellRepeat_.x; ++ix)
            {
                for (auto iy = 0; iy < supercellRepeat_.y; ++iy)
                {
                    for (auto iz = 0; iz < supercellRepeat_.z; ++iz)
                    {
                        // Skip origin cell
                        if (ix == 0 && iy == 0 && iz == 0)
                            continue;

                        // Set translation vector
                        auto tVec = cleanedUnitCellSpecies_->box()->axes() * Vec3<double>(ix, iy, iz);

                        // Create images of core molecule instances
                        for (auto &instance : coreInstances)
                        {
                            auto &mol = supercellInstances.emplace_back();
                            mol.setSpecies(sp);

                            for (auto &&[coreAtom, instanceAtom] : zip(instance.localAtoms(), mol.localAtoms()))
                                instanceAtom.setCoordinates(coreAtom.r() + tVec);
                        }
                    }
                }
            }

            // Append the new instances to our existing ones for the unit cell
            molecularSpecies.appendInstances(supercellInstances);

            // Add the molecules to our configuration
            for (const auto &instance : molecularSpecies.instances())
            {
                auto mol = supercellConfiguration_->addMolecule(sp);
                for (auto &&[molAtom, instanceAtom] : zip(mol->atoms(), instance.localAtoms()))
                    molAtom->setCoordinates(instanceAtom.r());
            }
        }

        supercellConfiguration_->updateObjectRelationships();
    }

    Messenger::print("Created ({}, {}, {}) supercell - {} atoms total.\n", supercellRepeat_.x, supercellRepeat_.y,
                     supercellRepeat_.z, supercellConfiguration_->nAtoms());

    return true;
}

// Reset all objects
void CIFHandler::resetSpeciesAndConfigurations()
{
    unitCellConfiguration_->empty();
    unitCellSpecies_ = nullptr;
    cleanedUnitCellConfiguration_->empty();
    cleanedUnitCellSpecies_ = nullptr;
    molecularUnitCellSpecies_.clear();
    molecularUnitCellConfiguration_->empty();
    supercellSpecies_ = nullptr;
    supercellConfiguration_->empty();
    coreData_.species().clear();
    coreData_.atomTypes().clear();
}

// Set overlap tolerance
void CIFHandler::setOverlapTolerance(double tol)
{
    overlapTolerance_ = tol;

    generate(); // TODO From Start
}

// Set whether to use CIF bonding definitions
void CIFHandler::setUseCIFBondingDefinitions(bool b)
{
    if (useCIFBondingDefinitions_ == b)
        return;

    useCIFBondingDefinitions_ = b;

    generate(); // TODO From Start
}

// Set bonding tolerance
void CIFHandler::setBondingTolerance(double tol)
{
    bondingTolerance_ = tol;

    if (!useCIFBondingDefinitions_)
        generate(); // TODO From Start
}

// Set whether to prevent metallic bonding
void CIFHandler::setPreventMetallicBonds(bool b)
{
    if (preventMetallicBonds_ == b)
        return;

    preventMetallicBonds_ = b;

    generate(); // TODO FRom start
}

// Set whether to remove free atomic moieties in clean-up
void CIFHandler::setRemoveAtomics(bool b)
{
    if (removeAtomics_ == b)
        return;

    removeAtomics_ = b;

    generate(); // TODO After Unit Cell Generation
}

// Set whether to remove water and coordinated oxygen atoms in clean-up
void CIFHandler::setRemoveWaterAndCoordinateOxygens(bool b)
{
    if (removeWaterAndCoordinateOxygens_ == b)
        return;

    removeWaterAndCoordinateOxygens_ = b;

    generate(); // TODO After Unit Cell Generation
}

// Set whether to remove by NETA definition in clean-up
void CIFHandler::setRemoveNETA(bool b, bool byFragment)
{
    if (removeNETA_ == b && removeNETAByFragment_ == byFragment)
        return;

    removeNETA_ = b;
    removeNETAByFragment_ = byFragment;

    if (moietyRemovalNETA_.isValid())
        generate(); // TODO After Unit Cell Generation
}

// Set NETA for moiety removal
bool CIFHandler::setMoietyRemovalNETA(std::string_view netaDefinition) { return moietyRemovalNETA_.create(netaDefinition); }

// Set supercell repeat
void CIFHandler::setSupercellRepeat(const Vec3<int> &repeat)
{
    supercellRepeat_ = repeat;

    generate(); // TODO After detectMolecules
}

// Recreate the data
bool CIFHandler::generate()
{
    // Reset all species and configurations
    resetSpeciesAndConfigurations();

    if (!createBasicUnitCell())
        return false;

    if (!createCleanedUnitCell())
        return false;

    detectMolecules();

    if (!createSupercell())
        return false;

    return true;
}

// Return whether the generated data is valid
bool CIFHandler::isValid() const
{
    return !molecularUnitCellSpecies_.empty() || supercellSpecies_->fragment(0).size() != supercellSpecies_->nAtoms();
}

// Finalise, returning the required species and resulting configuration
std::pair<std::vector<const Species *>, Configuration *> CIFHandler::finalise(CoreData &coreData,
                                                                              const Flags<OutputFlags> &flags) const
{
    std::vector<const Species *> species;
    Configuration *configuration;

    if (flags.isSet(OutputFlags::OutputMolecularSpecies))
    {
        if (flags.isSet(OutputFlags::OutputConfiguration))
        {
            configuration = coreData.addConfiguration();
            configuration->setName(chemicalFormula());

            // Grab the generator
            auto &generator = configuration->generator();

            // Add Box
            auto boxNode = generator.createRootNode<BoxProcedureNode>({});
            auto cellLengths = supercellConfiguration_->box()->axisLengths();
            auto cellAngles = supercellConfiguration_->box()->axisAngles();
            boxNode->keywords().set("Lengths", Vec3<NodeValue>(cellLengths.get(0), cellLengths.get(1), cellLengths.get(2)));
            boxNode->keywords().set("Angles", Vec3<NodeValue>(cellAngles.get(0), cellAngles.get(1), cellAngles.get(2)));

            for (auto &cifMolecularSp : molecularUnitCellSpecies_)
            {
                // Add the species
                auto *sp = coreData.copySpecies(cifMolecularSp.species());
                species.push_back(sp);

                // Determine a unique suffix
                auto base = sp->name();
                std::string uniqueSuffix{base};
                if (!generator.nodes().empty())
                {
                    // Start from the last root node
                    auto root = generator.nodes().back();
                    auto suffix = 0;

                    while (generator.rootSequence().nodeInScope(root, fmt::format("SymmetryCopies_{}", uniqueSuffix)) !=
                           nullptr)
                        uniqueSuffix = fmt::format("{}_{:02d}", base, ++suffix);
                }

                // We use 'CoordinateSets' here, because in this instance we are working with (CoordinateSet, Add) pairs

                // CoordinateSets
                auto coordsNode =
                    generator.createRootNode<CoordinateSetsProcedureNode>(fmt::format("SymmetryCopies_{}", uniqueSuffix), sp);
                coordsNode->keywords().setEnumeration("Source", CoordinateSetsProcedureNode::CoordinateSetSource::File);
                coordsNode->setSets(cifMolecularSp.allInstanceCoordinates());

                // Add
                auto addNode = generator.createRootNode<AddProcedureNode>(fmt::format("Add_{}", uniqueSuffix), coordsNode);
                addNode->keywords().set("Population", NodeValueProxy(int(cifMolecularSp.instances().size())));
                addNode->keywords().setEnumeration("Positioning", AddProcedureNode::PositioningType::Current);
                addNode->keywords().set("Rotate", false);
                addNode->keywords().setEnumeration("BoxAction", AddProcedureNode::BoxActionStyle::None);
            }
        }
        else
        {
            for (auto &cifMolecularSp : molecularUnitCellSpecies_)
            {
                auto *sp = cifMolecularSp.species();
                // Add the species
                sp = coreData.copySpecies(cifMolecularSp.species());
                species.push_back(sp);
            }
        }
    }
    else
    {
        auto *sp = coreData.addSpecies();
        sp->copyBasic(supercellSpecies_);
        species.push_back(sp);
        if (flags.isSet(OutputFlags::OutputFramework))
        {
            sp->removePeriodicBonds();
            sp->updateIntramolecularTerms();
            sp->removeBox();
        }

        if (flags.isSet(OutputFlags::OutputConfiguration))
        {
            configuration = coreData.addConfiguration();
            configuration->setName(chemicalFormula());

            // Grab the generator
            auto &generator = configuration->generator();

            // Add Box
            auto boxNode = generator.createRootNode<BoxProcedureNode>({});
            auto cellLengths = supercellConfiguration_->box()->axisLengths();
            auto cellAngles = supercellConfiguration_->box()->axisAngles();
            boxNode->keywords().set("Lengths", Vec3<NodeValue>(cellLengths.get(0), cellLengths.get(1), cellLengths.get(2)));
            boxNode->keywords().set("Angles", Vec3<NodeValue>(cellAngles.get(0), cellAngles.get(1), cellAngles.get(2)));

            // Add
            auto addNode = generator.createRootNode<AddProcedureNode>(fmt::format("Add_{}", sp->name()), sp);
            addNode->keywords().set("Population", NodeValueProxy(1));
            addNode->keywords().setEnumeration("Positioning", AddProcedureNode::PositioningType::Current);
            addNode->keywords().set("Rotate", false);
            addNode->keywords().setEnumeration("BoxAction", AddProcedureNode::BoxActionStyle::None);
        }
    }

    return {species, configuration};
}

// Structural
Species *CIFHandler::structuralUnitCellSpecies() { return unitCellSpecies_; }
Configuration *CIFHandler::structuralUnitCellConfiguration() { return unitCellConfiguration_; }

// Cleaned
Species *CIFHandler::cleanedUnitCellSpecies() { return cleanedUnitCellSpecies_; }
Configuration *CIFHandler::cleanedUnitCellConfiguration() { return cleanedUnitCellConfiguration_; }

// Molecular
const std::vector<CIFMolecularSpecies> &CIFHandler::molecularSpecies() const { return molecularUnitCellSpecies_; }
Configuration *CIFHandler::molecularConfiguration() { return molecularUnitCellConfiguration_; }

// Supercell
Species *CIFHandler::supercellSpecies() { return supercellSpecies_; }
Configuration *CIFHandler::supercellConfiguration() { return supercellConfiguration_; }

/*
 * Helpers
 */

// Apply CIF bonding to a given species
void CIFHandler::applyCIFBonding(Species *sp, bool preventMetallicBonding)
{
    if (!hasBondDistances())
        return;

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

// Get instances for the supplied species from the cleaned unit cell
std::vector<CIFLocalMolecule> CIFHandler::getSpeciesInstances(Species *moleculeSpecies, const NETADefinition &neta)
{
    std::vector<CIFLocalMolecule> instances;

    // Loop over atoms in the unit cell - we'll mark any that we select as an instance so we speed things up and avoid
    // duplicates
    std::vector<bool> atomFlags(cleanedUnitCellSpecies_->nAtoms());
    std::fill(atomFlags.begin(), atomFlags.end(), false);
    const auto &unitCellAtoms = cleanedUnitCellSpecies_->atoms();
    for (auto i = 0; i < atomFlags.size(); ++i)
    {
        if (atomFlags[i])
            continue;

        auto &atom = unitCellAtoms[i];
        auto matchedAtoms = neta.matchedPath(&atom).set();
        if (matchedAtoms.empty())
            continue;

        // Get the (sorted) matched atom indices
        std::vector<int> indices(matchedAtoms.size());
        std::transform(matchedAtoms.begin(), matchedAtoms.end(), indices.begin(),
                       [](const auto matchedAtom) { return matchedAtom->index(); });

        // For each match create a CIFLocalMolecule instance
        auto &mol = instances.emplace_back();
        mol.setSpecies(moleculeSpecies);
        for (auto idx = 0; idx < indices.size(); ++idx)
        {
            mol.setAtom(idx, unitCellAtoms[indices[idx]].r(), indices[idx]);

            atomFlags[indices[idx]] = true;
        }

        // Unfold the molecule
        mol.unFold(cleanedUnitCellSpecies_->box());
    }

    return instances;
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
