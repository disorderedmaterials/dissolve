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
    unitCellConfiguration_.setName("Structural");
    unitCellSpecies_.setName("Crystal");
    cleanedUnitCellConfiguration_.setName("Cleaned");
    cleanedUnitCellSpecies_.setName("Crystal (Cleaned)");
    supercellConfiguration_.setName("Supercell");
    supercellSpecies_.setName("Crystal (Supercell)");
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
    generate();
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
    unitCellConfiguration_.empty();
    unitCellSpecies_.clear();
    atomLabelTypes_.clear();

    // Create temporary atom types corresponding to the unique atom labels
    for (auto &a : assemblies_)
    {
        for (auto &g : a.groups())
        {
            if (!g.active())
                continue;

            for (auto &i : g.atoms())
            {
                if (std::find_if(atomLabelTypes_.begin(), atomLabelTypes_.end(),
                                 [i](const auto &at) { return i.label() == at->name(); }) == atomLabelTypes_.end())
                {
                    atomLabelTypes_.emplace_back(std::make_shared<AtomType>(i.Z(), i.label()));
                }
            }
        }
    }

    // Configure the unit cell "species"
    auto cellLengths = getCellLengths();
    if (!cellLengths)
        return false;
    auto cellAngles = getCellAngles();
    if (!cellAngles)
        return false;
    unitCellSpecies_.createBox(cellLengths.value(), cellAngles.value());
    auto *box = unitCellSpecies_.box();

    // Configuration
    Messenger::setQuiet(true);
    unitCellConfiguration_.createBoxAndCells(cellLengths.value(), cellAngles.value(), false, 1.0);
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
                        if (std::any_of(unitCellSpecies_.atoms().begin(), unitCellSpecies_.atoms().end(),
                                        [&, r, box](const auto &j)
                                        { return box->minimumDistance(r, j.r()) < overlapTolerance_; }))
                            continue;

                        // Create the new atom
                        auto atIt = std::find_if(atomLabelTypes_.begin(), atomLabelTypes_.end(),
                                                 [&unique](const auto &at) { return unique.label() == at->name(); });
                        unitCellSpecies_.addAtom(unique.Z(), r, 0.0, atIt != atomLabelTypes_.end() ? *atIt : nullptr);
                    }

    // Check that we actually generated some atoms...
    if (unitCellSpecies_.nAtoms() == 0)
        return false;

    // Bonding
    if (useCIFBondingDefinitions_)
        applyCIFBonding(&unitCellSpecies_, preventMetallicBonds_);
    else
        unitCellSpecies_.addMissingBonds(bondingTolerance_, preventMetallicBonds_);

    unitCellConfiguration_.addMolecule(&unitCellSpecies_);
    unitCellConfiguration_.updateObjectRelationships();

    Messenger::print("Created basic crystal unit cell - {} non-overlapping atoms.\n", unitCellSpecies_.nAtoms());

    return true;
}

// Create the cleaned unit cell
bool CIFHandler::createCleanedUnitCell()
{
    cleanedUnitCellConfiguration_.empty();
    cleanedUnitCellSpecies_.clear();

    // Configure the species
    cleanedUnitCellSpecies_.copyBasic(&unitCellSpecies_, true);
    auto cellLengths = getCellLengths();
    if (!cellLengths)
        return false;
    auto cellAngles = getCellAngles();
    if (!cellAngles)
        return false;
    cleanedUnitCellSpecies_.createBox(cellLengths.value(), cellAngles.value());

    // Configuration
    Messenger::setQuiet(true);
    cleanedUnitCellConfiguration_.createBoxAndCells(cellLengths.value(), cellAngles.value(), false, 1.0);
    Messenger::setQuiet(false);

    if (removeAtomics_)
    {
        std::vector<int> indicesToRemove;
        for (const auto &i : cleanedUnitCellSpecies_.atoms())
            if (i.nBonds() == 0)
                indicesToRemove.push_back(i.index());
        Messenger::print("Atomic removal deleted {} atoms.\n", indicesToRemove.size());

        // Remove selected atoms
        cleanedUnitCellSpecies_.removeAtoms(indicesToRemove);
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
        for (const auto &i : cleanedUnitCellSpecies_.atoms())
            if (waterVacuum.matches(&i))
                indicesToRemove.push_back(i.index());
        Messenger::print("Water removal deleted {} atoms.\n", indicesToRemove.size());

        // Remove selected atoms
        cleanedUnitCellSpecies_.removeAtoms(indicesToRemove);
    }

    if (removeNETA_ && moietyRemovalNETA_.isValid())
    {
        // Select all atoms that are in moieties where one of its atoms matches our NETA definition
        std::vector<int> indicesToRemove;
        for (auto &i : cleanedUnitCellSpecies_.atoms())
            if (moietyRemovalNETA_.matches(&i))
            {
                // Select all atoms that are part of the same moiety?
                if (removeNETAByFragment_)
                {
                    cleanedUnitCellSpecies_.clearAtomSelection();
                    auto selection = cleanedUnitCellSpecies_.fragment(i.index());
                    std::copy(selection.begin(), selection.end(), std::back_inserter(indicesToRemove));
                }
                else
                    indicesToRemove.push_back(i.index());
            }
        Messenger::print("Moiety removal deleted {} atoms.\n", indicesToRemove.size());

        // Remove selected atoms
        cleanedUnitCellSpecies_.removeAtoms(indicesToRemove);
    }

    cleanedUnitCellConfiguration_.addMolecule(&cleanedUnitCellSpecies_);
    cleanedUnitCellConfiguration_.updateObjectRelationships();

    Messenger::print("Created cleaned crystal unit cell - {} atoms after removal(s).\n", cleanedUnitCellSpecies_.nAtoms());

    return true;
}

// Try to detect molecules in the cell contents
bool CIFHandler::detectMolecules()
{
    molecularSpecies_.clear();

    // Try selecting within the species from the first atom - if this captures all atoms we have a bound framework...
    if (cleanedUnitCellSpecies_.fragment(0).size() == cleanedUnitCellSpecies_.nAtoms())
    {
        Messenger::print(
            "Can't create molecular definitions since this unit cell appears to be a continuous framework/network. Consider "
            "adjusting the bonding options in order to generate molecular fragments.\n");
        return false;
    }

    std::vector<bool> atomMask(cleanedUnitCellSpecies_.nAtoms(), false);

    // Find all molecular species, and their instances
    auto indexIterator = atomMask.begin();
    while (indexIterator != atomMask.end())
    {
        // Select a fragment from the next available index
        auto atomIndex = indexIterator - atomMask.begin();
        auto fragmentIndices = cleanedUnitCellSpecies_.fragment(atomIndex);

        // Create a new CIF molecular species from the fragment
        auto &cifSp = molecularSpecies_.emplace_back();
        auto *sp = cifSp.species().get();
        // -- Copy selected atoms
        for (auto fragAtomIndex : fragmentIndices)
        {
            const auto &unitCellAtom = cleanedUnitCellSpecies_.atom(fragAtomIndex);
            sp->addAtom(unitCellAtom.Z(), unitCellAtom.r(), 0.0, unitCellAtom.atomType());
        }

        // Give the species a temporary unit cell so we can calculate / apply bonding
        sp->createBox(cleanedUnitCellSpecies_.box()->axisLengths(), cleanedUnitCellSpecies_.box()->axisAngles());
        if (useCIFBondingDefinitions_)
            applyCIFBonding(sp, preventMetallicBonds_);
        else
            sp->addMissingBonds(bondingTolerance_, preventMetallicBonds_);
        sp->removeBox();

        // Set up a temporary molecule to unfold the species
        LocalMolecule tempMol;
        tempMol.setSpecies(sp);
        for (auto i = 0; i < sp->nAtoms(); ++i)
            tempMol.localAtom(i).setCoordinates(sp->atom(i).r());
        tempMol.unFold(cleanedUnitCellSpecies_.box());
        for (auto &&[molAtom, spAtom] : zip(tempMol.localAtoms(), sp->atoms()))
            spAtom.setCoordinates(molAtom.r());

        // Give the species a name
        sp->setName(EmpiricalFormula::formula(sp->atoms(), [&](const auto &at) { return at.Z(); }));

        // Find instances of this fragment. For large fragments that represent > 50% of the remaining atoms we don't even
        // attempt to create a NETA definition etc. For cases such as framework species this will speed up detection no end.
        std::vector<LocalMolecule> instances;
        if (fragmentIndices.size() * 2 > cleanedUnitCellSpecies_.nAtoms())
        {
            // Create an instance of the current fragment
            auto &mol = instances.emplace_back();
            mol.setSpecies(sp);
            for (auto i = 0; i < sp->nAtoms(); ++i)
            {
                mol.localAtom(i).setCoordinates(sp->atom(i).r());
                atomMask[fragmentIndices[i]] = true;
            }
        }
        else
        {
            // Determine the best NETA definition describing the fragment
            auto &&[bestNETA, rootAtoms] = bestNETADefinition(sp);
            if (rootAtoms.empty())
                return Messenger::error(
                    "Couldn't generate molecular partitioning for CIF - no suitable NETA definition for the "
                    "fragment {} could be determined.\n",
                    sp->name());

            // Find instances of this fragment
            instances = getSpeciesInstances(sp, atomMask, bestNETA, rootAtoms);
            if (instances.empty())
            {
                molecularSpecies_.clear();
                return Messenger::error("Failed to find species instances for fragment '{}'.\n", sp->name());
            }
        }

        // Store the instances
        cifSp.instances() = instances;

        // Search for the next valid starting index
        indexIterator = std::find(std::next(indexIterator), atomMask.end(), false);
    }

    Messenger::print("Partitioned unit cell into {} distinct molecular species:\n\n", molecularSpecies_.size());
    Messenger::print("   ID     N  Species Formula\n");
    auto count = 1;
    for (const auto &cifMol : molecularSpecies_)
        Messenger::print("  {:3d}  {:4d}  {}\n", count++, cifMol.instances().size(),
                         EmpiricalFormula::formula(cifMol.species()->atoms(), [](const auto &i) { return i.Z(); }));
    Messenger::print("");

    return true;
}

// Create supercell species
bool CIFHandler::createSupercell()
{
    supercellConfiguration_.empty();
    supercellSpecies_.clear();

    // Configure the species
    auto supercellLengths = cleanedUnitCellSpecies_.box()->axisLengths();
    supercellLengths.multiply(supercellRepeat_.x, supercellRepeat_.y, supercellRepeat_.z);
    supercellSpecies_.createBox(supercellLengths, cleanedUnitCellSpecies_.box()->axisAngles(), false);

    // Set up configuration
    Messenger::setQuiet(true);
    supercellConfiguration_.createBoxAndCells(supercellLengths, cleanedUnitCellSpecies_.box()->axisAngles(), false, 1.0);
    Messenger::setQuiet(false);

    // Copy atoms from the Crystal species - we'll do the bonding afterwards
    if (molecularSpecies_.empty())
    {
        supercellSpecies_.atoms().reserve(supercellRepeat_.x * supercellRepeat_.y * supercellRepeat_.z *
                                          cleanedUnitCellSpecies_.nAtoms());
        for (auto ix = 0; ix < supercellRepeat_.x; ++ix)
            for (auto iy = 0; iy < supercellRepeat_.y; ++iy)
                for (auto iz = 0; iz < supercellRepeat_.z; ++iz)
                {
                    Vec3<double> deltaR = cleanedUnitCellSpecies_.box()->axes() * Vec3<double>(ix, iy, iz);
                    for (const auto &i : cleanedUnitCellSpecies_.atoms())
                        supercellSpecies_.addAtom(i.Z(), i.r() + deltaR, 0.0, i.atomType());
                }
        if (useCIFBondingDefinitions_)
            applyCIFBonding(&supercellSpecies_, preventMetallicBonds_);
        else
            supercellSpecies_.addMissingBonds(bondingTolerance_, preventMetallicBonds_);

        // Add the structural species to the configuration
        supercellConfiguration_.addMolecule(&supercellSpecies_);
        supercellConfiguration_.updateObjectRelationships();
    }
    else
    {
        supercellSpecies_.atoms().reserve(supercellRepeat_.x * supercellRepeat_.y * supercellRepeat_.z *
                                          cleanedUnitCellSpecies_.nAtoms());

        // Create images of all molecular unit cell species
        for (auto &molecularSpecies : molecularSpecies_)
        {
            const auto *sp = molecularSpecies.species().get();
            const auto &coreInstances = molecularSpecies.instances();
            std::vector<LocalMolecule> supercellInstances;
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
                        auto tVec = cleanedUnitCellSpecies_.box()->axes() * Vec3<double>(ix, iy, iz);

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
                auto mol = supercellConfiguration_.addMolecule(sp);
                for (auto &&[molAtom, instanceAtom] : zip(mol->atoms(), instance.localAtoms()))
                    molAtom->setCoordinates(instanceAtom.r());
            }
        }

        supercellConfiguration_.updateObjectRelationships();
    }

    Messenger::print("Created ({}, {}, {}) supercell - {} atoms total.\n", supercellRepeat_.x, supercellRepeat_.y,
                     supercellRepeat_.z, supercellConfiguration_.nAtoms());

    return true;
}

// Set overlap tolerance
void CIFHandler::setOverlapTolerance(double tol)
{
    overlapTolerance_ = tol;

    generate(CIFGenerationStage::CreateBasicUnitCell);
}

// Set whether to use CIF bonding definitions
void CIFHandler::setUseCIFBondingDefinitions(bool b)
{
    if (useCIFBondingDefinitions_ == b)
        return;

    useCIFBondingDefinitions_ = b;

    generate();
}

// Set bonding tolerance
void CIFHandler::setBondingTolerance(double tol)
{
    bondingTolerance_ = tol;

    if (!useCIFBondingDefinitions_)
        generate();
}

// Set whether to prevent metallic bonding
void CIFHandler::setPreventMetallicBonds(bool b)
{
    if (preventMetallicBonds_ == b)
        return;

    preventMetallicBonds_ = b;

    generate();
}

// Set whether to remove free atomic moieties in clean-up
void CIFHandler::setRemoveAtomics(bool b)
{
    if (removeAtomics_ == b)
        return;

    removeAtomics_ = b;

    generate(CIFGenerationStage::CreateCleanedUnitCell);
}

// Set whether to remove water and coordinated oxygen atoms in clean-up
void CIFHandler::setRemoveWaterAndCoordinateOxygens(bool b)
{
    if (removeWaterAndCoordinateOxygens_ == b)
        return;

    removeWaterAndCoordinateOxygens_ = b;

    generate(CIFGenerationStage::CreateCleanedUnitCell);
}

// Set whether to remove by NETA definition in clean-up
void CIFHandler::setRemoveNETA(bool b, bool byFragment)
{
    if (removeNETA_ == b && removeNETAByFragment_ == byFragment)
        return;

    removeNETA_ = b;
    removeNETAByFragment_ = byFragment;

    if (moietyRemovalNETA_.isValid())
        generate(CIFGenerationStage::CreateCleanedUnitCell);
}

// Set NETA for moiety removal
bool CIFHandler::setMoietyRemovalNETA(std::string_view netaDefinition) { return moietyRemovalNETA_.create(netaDefinition); }

// Set supercell repeat
void CIFHandler::setSupercellRepeat(const Vec3<int> &repeat)
{
    supercellRepeat_ = repeat;

    generate(CIFGenerationStage::CreateSupercell);
}

// Recreate the data
bool CIFHandler::generate(CIFGenerationStage fromStage)
{
    // Generate data starting from the specified stage, falling through to subsequent stages in the switch

    switch (fromStage)
    {
        case (CIFGenerationStage::CreateBasicUnitCell):
            if (!createBasicUnitCell())
                return false;
        case (CIFGenerationStage::CreateCleanedUnitCell):
            if (!createCleanedUnitCell())
                return false;
        case (CIFGenerationStage::DetectMolecules):
            detectMolecules();
        case (CIFGenerationStage::CreateSupercell):
            if (!createSupercell())
                return false;
    }

    return true;
}

// Return whether the generated data is valid
bool CIFHandler::isValid() const
{
    return !molecularSpecies_.empty() || supercellSpecies_.fragment(0).size() != supercellSpecies_.nAtoms();
}

// Return the detected molecular species
const std::vector<CIFMolecularSpecies> &CIFHandler::molecularSpecies() const { return molecularSpecies_; }

// Return the generated configuration
Configuration *CIFHandler::generatedConfiguration() { return &supercellConfiguration_; }

// Finalise, copying the required species and resulting configuration to the target CoreData
void CIFHandler::finalise(CoreData &coreData, const Flags<OutputFlags> &flags) const
{
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
            auto cellLengths = supercellConfiguration_.box()->axisLengths();
            auto cellAngles = supercellConfiguration_.box()->axisAngles();
            boxNode->keywords().set("Lengths", Vec3<NodeValue>(cellLengths.get(0), cellLengths.get(1), cellLengths.get(2)));
            boxNode->keywords().set("Angles", Vec3<NodeValue>(cellAngles.get(0), cellAngles.get(1), cellAngles.get(2)));

            for (auto &cifMolecularSp : molecularSpecies_)
            {
                // Add the species
                auto *sp = coreData.copySpecies(cifMolecularSp.species().get());

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
            for (auto &cifMolecularSp : molecularSpecies_)
            {
                coreData.copySpecies(cifMolecularSp.species().get());
            }
        }
    }
    else
    {
        auto *sp = coreData.addSpecies();
        sp->copyBasic(&supercellSpecies_);
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
            auto cellLengths = supercellConfiguration_.box()->axisLengths();
            auto cellAngles = supercellConfiguration_.box()->axisAngles();
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
}

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

// Determine the best NETA definition for the supplied species
std::tuple<NETADefinition, std::vector<SpeciesAtom *>> CIFHandler::bestNETADefinition(Species *sp)
{
    // Set up the return value and bind its contents
    std::tuple<NETADefinition, std::vector<SpeciesAtom *>> result{NETADefinition(), {}};
    auto &&[bestNETA, rootAtoms] = result;

    // Maintain a set of atoms matched by any NETA description we generate
    std::set<SpeciesAtom *> alreadyMatched;

    // Loop over species atoms
    for (auto &i : sp->atoms())
    {
        // Skip this atom?
        if (alreadyMatched.find(&i) != alreadyMatched.end())
            continue;

        // Create a NETA definition with this atom as the root
        NETADefinition neta;
        neta.create(&i, std::nullopt,
                    Flags<NETADefinition::NETACreationFlags>(NETADefinition::NETACreationFlags::ExplicitHydrogens,
                                                             NETADefinition::NETACreationFlags::IncludeRootElement));

        // Apply this match over the whole species
        std::vector<SpeciesAtom *> currentRootAtoms;
        for (auto &j : sp->atoms())
        {
            if (neta.matches(&j))
            {
                currentRootAtoms.push_back(&j);
                alreadyMatched.insert(&j);
            }
        }

        // Is this a better description?
        auto better = false;
        if (rootAtoms.empty() || currentRootAtoms.size() < rootAtoms.size())
            better = true;
        else if (currentRootAtoms.size() == rootAtoms.size())
        {
            // Replace the current match if there are more bonds on the current atom.
            if (i.nBonds() > rootAtoms.front()->nBonds())
                better = true;
        }

        if (better)
        {
            bestNETA = neta;
            rootAtoms = currentRootAtoms;
        }
    }

    return result;
}

// Get instances for the supplied species from the cleaned unit cell
std::vector<LocalMolecule> CIFHandler::getSpeciesInstances(const Species *referenceSpecies, std::vector<bool> &atomMask,
                                                           const NETADefinition &neta,
                                                           const std::vector<SpeciesAtom *> &referenceRootAtoms)
{
    if (referenceRootAtoms.empty() || !neta.isValid())
        return {};
    referenceSpecies->print();
    for (auto *i : referenceRootAtoms)
        Messenger::print("Detecting instances for fragment '{}' - there are {} unique root atoms.\n", referenceSpecies->name(),
                         referenceRootAtoms.size());
    // Form basis sites on the molecularSpecies for each root atom that we will try to match in our fragment molecule
    std::map<const SpeciesAtom *, Matrix3> referenceBases;
    for (auto *rootAtom : referenceRootAtoms)
    {
        referenceBases[rootAtom] = Matrix3();

        // Get the available angle pairs for this root atom
        auto anglePairs = rootAtom->getAnglePairs(170.0, false);
        if (anglePairs.empty())
            continue;

        auto &&[i, k] = anglePairs.front();

        // Work relative to the root atom coordinates
        auto x = i->r() - rootAtom->r();
        x.normalise();
        auto y = k->r() - rootAtom->r();
        y.orthogonalise(x);
        y.normalise();

        referenceBases[rootAtom].setColumn(0, x);
        referenceBases[rootAtom].setColumn(1, y);
        referenceBases[rootAtom].setColumn(2, x * y);
    }

    // Loop over atoms in the unit cell - we'll mark any that we select as an instance so we speed things up and avoid
    // duplicates
    const auto &unitCellAtoms = cleanedUnitCellSpecies_.atoms();
    std::vector<LocalMolecule> instances;
    auto atomIndexIterator = std::find(atomMask.begin(), atomMask.end(), false);
    while (atomIndexIterator != atomMask.end())
    {
        // Try to match this atom / fragment
        const auto atomIndex = atomIndexIterator - atomMask.begin();
        auto &atom = unitCellAtoms[atomIndex];
        auto matchedUnitCellAtoms = neta.matchedPath(&atom).set();
        if (matchedUnitCellAtoms.empty())
        {
            atomIndexIterator = std::find(std::next(atomIndexIterator), atomMask.end(), false);
            continue;
        }
        Messenger::print("Current index matching NETA is {}...\n", atomIndex);

        // Found a fragment that matches the NETA description - we now create a temporary instance Species which will contain
        // the selected fragment atoms, reassembled into a molecule (i.e. unfolded) and with bonding applied / calculated.
        // We need to copy the unit cell from the crystal so we detect bonds properly.
        Species instanceSpecies;
        instanceSpecies.createBox(unitCellSpecies_.box()->axisLengths(), unitCellSpecies_.box()->axisAngles());
        auto rootAtomLocalIndex = -1;
        // -- Create species atoms from those matched in the unit cell by the NETA description.
        for (auto &matchedAtom : matchedUnitCellAtoms)
        {
            auto idx = instanceSpecies.addAtom(matchedAtom->Z(), matchedAtom->r(), 0.0, matchedAtom->atomType());

            // Store the index of the root atom in match in our instance species when we find it
            if (matchedAtom == &atom)
                rootAtomLocalIndex = idx;
        }
        // -- Store the local root atom so we can access its coordinates for the origin translation
        auto &instanceSpeciesRootAtom = instanceSpecies.atom(rootAtomLocalIndex);
        // -- Calculate / apply bonding
        if (useCIFBondingDefinitions_)
            applyCIFBonding(&instanceSpecies, preventMetallicBonds_);
        else
            instanceSpecies.addMissingBonds(bondingTolerance_, preventMetallicBonds_);

        // Create a LocalMolecule as a working area for folding, translation, and rotation of the instance coordinates.
        LocalMolecule instanceMolecule;
        instanceMolecule.setSpecies(&instanceSpecies);
        // -- Copy the coordinates off the matched unit cell atoms to our molecule and flag them as complete
        auto count = 0;
        for (auto &&[matchedAtom, instanceMolAtom] : zip(matchedUnitCellAtoms, instanceMolecule.localAtoms()))
        {
            instanceMolAtom.setCoordinates(matchedAtom->r());
            atomMask[matchedAtom->index()] = true;
        }
        auto &instanceMoleculeRootAtom = instanceMolecule.localAtoms()[rootAtomLocalIndex];

        // Unfold the molecule and store the unfolded molecule coordinates back into the instance Species.
        // This represents our full instance coordinates we will be storing (but not their final order)
        instanceMolecule.unFold(unitCellSpecies_.box());
        for (auto &&[molAtom, spAtom] : zip(instanceMolecule.localAtoms(), instanceSpecies.atoms()))
            spAtom.setCoordinates(molAtom.r());
        instanceSpecies.print();

        /*
         * Now, we have a root match atom on the current instance and a vector of possible matching sites on the reference
         * species (in referenceRootAtoms). For each of the referenceRootAtoms, try to incremental select along bonds using
         * basic NETA connectivity.
         */

        // Generate basic NETA descriptions for each atom in the reference and candidate species
        std::map<const SpeciesAtom *, NETADefinition> referenceAtomNETA;
        for (auto &spAtom : referenceSpecies->atoms())
            referenceAtomNETA[&spAtom] = NETADefinition(&spAtom, 1, {NETADefinition::NETACreationFlags::IncludeRootElement});

        std::map<const SpeciesAtom *, const SpeciesAtom *> matchMap;
        for (const auto *referenceRootAtom : referenceRootAtoms)
        {
            // The root atom is the starting point
            matchMap = matchAtom(referenceRootAtom, &instanceSpeciesRootAtom, referenceAtomNETA, {});
            if (matchMap.empty())
                continue;
            else
            {
                printf("Got a match starting from reference root atom index %i.\n", referenceRootAtom->index());
                break;
            }
        }

        // Result?
        if (matchMap.empty())
        {
            Messenger::error("Failed to match connectivity of an instance to the reference molecule.\n");
            return {};
        }
        else if (matchMap.size() != referenceSpecies->nAtoms())
        {
            Messenger::error(
                "Internal error - failed to match connectivity of all atoms within an instance to the reference molecule.\n");
            return {};
        }

        // Create the final instance
        auto &instance = instances.emplace_back();
        instance.setSpecies(referenceSpecies);
        for (const auto &[refSpeciesAtom, instanceSpeciesAtom] : matchMap)
        //        for (auto refSpeciesI = 0; refSpeciesI < referenceSpecies->nAtoms(); ++refSpeciesI)
        {
            fmt::print("Reference index {} maps to instance index {}\n", refSpeciesAtom->index(), instanceSpeciesAtom->index());
            // Set the final instance coordinates from those of our local instance species
            instance.localAtom(refSpeciesAtom->index()).setCoordinates(instanceSpeciesAtom->r());
        }

        // Find the next available atom
        atomIndexIterator = std::find(std::next(atomIndexIterator), atomMask.end(), false);
    }

    return instances;
}

std::map<const SpeciesAtom *, const SpeciesAtom *>
CIFHandler::matchAtom(const SpeciesAtom *referenceAtom, const SpeciesAtom *instanceAtom,
                      const std::map<const SpeciesAtom *, NETADefinition> &refNETA,
                      const std::map<const SpeciesAtom *, const SpeciesAtom *> &map, const SpeciesAtom *fromThis)
{
    static int depth = 0;
    // Check that the reference atom NETA matches the instance atom
    if (!refNETA.at(referenceAtom).matches(instanceAtom))
    {
        fmt::print("[{}] Reference atom index {} ({},nbonds={}) doesn't match the instance atom index {} ({},nbonds={})\n",
                   depth, referenceAtom->index(), Elements::symbol(referenceAtom->Z()), referenceAtom->nBonds(),
                   instanceAtom->index(), Elements::symbol(instanceAtom->Z()), instanceAtom->nBonds());
        return {};
    }

    // Check the map to see if we have already associated the instance atom to a reference atom. If we have then it must be the
    // same atom as we were given here.
    auto mapIt = map.find(referenceAtom);
    if (mapIt != map.end())
    {
        if (mapIt->second != instanceAtom)
        {
            fmt::print("[{}] FAIL Reference atom index {} ({},nbonds={}) is already mapped but not to this instance atom "
                       "(index {} ({},nbonds={})\n",
                       depth, referenceAtom->index(), Elements::symbol(referenceAtom->Z()), referenceAtom->nBonds(),
                       instanceAtom->index(), Elements::symbol(instanceAtom->Z()), instanceAtom->nBonds());
            return {};
        }

        // Already matched and confirmed, so can return the original map
        fmt::print("[{}] RETURN Reference atom index {} ({},nbonds={}) is already mapped to this instance atom (index {} "
                   "({},nbonds={})\n",
                   depth, referenceAtom->index(), Elements::symbol(referenceAtom->Z()), referenceAtom->nBonds(),
                   instanceAtom->index(), Elements::symbol(instanceAtom->Z()), instanceAtom->nBonds());
        return map;
    }

    // Also check the reverse mapping to ensure that this instance atom isn't already associated to a reference atom match
    if (std::find_if(map.begin(), map.end(), [instanceAtom](const auto &kv) { return kv.second == instanceAtom; }) != map.end())
    {
        fmt::print("[{}] FAIL Reference atom index {} ({},nbonds={}) is not the one mapped to this instance atom (index {} "
                   "({},nbonds={})\n",
                   depth, referenceAtom->index(), Elements::symbol(referenceAtom->Z()), referenceAtom->nBonds(),
                   instanceAtom->index(), Elements::symbol(instanceAtom->Z()), instanceAtom->nBonds());
        return {};
    }

    // Copy the current map, associate our initial pair of atoms and try to extend it
    auto newMap = map;
    newMap[referenceAtom] = instanceAtom;
    fmt::print("[{}] GOOD Reference atom index {} ({},nbonds={}) is matches this instance atom (index {} ({},nbonds={})\n",
               depth, referenceAtom->index(), Elements::symbol(referenceAtom->Z()), referenceAtom->nBonds(),
               instanceAtom->index(), Elements::symbol(instanceAtom->Z()), instanceAtom->nBonds());

    // Cycle over bonds on the reference atom and find
    auto count1 = 0;
    for (const auto &referenceBond : referenceAtom->bonds())
    {
        // Get the reference bond partner
        auto *referenceBondPartner = referenceBond.get().partner(referenceAtom);

        fmt::print("[{}] Loop reference atom bomds ({} of {}) - to {}\n", depth, ++count1, referenceAtom->nBonds(),
                   Elements::symbol(referenceBondPartner->Z()));

        // Try to find a match over bonds on the instance atom
        std::map<const SpeciesAtom *, const SpeciesAtom *> bondResult;
        auto count2 = 0;
        for (const auto &instanceBond : instanceAtom->bonds())
        {
            fmt::print("[{}] Search instance atom bomds ({} of {})\n", depth, ++count2, instanceAtom->nBonds());

            // Get the instance bond partner
            auto *instanceBondPartner = instanceBond.get().partner(instanceAtom);

            //            // If this atom is where we just recursed from along a bond, no need to match it again...
            //            if (instanceBondPartner == fromThis)
            //            {
            //                fmt::print("[{}] Skipping just-matched atom index {} ({},nbonds={})\n", depth, fromThis->index(),
            //                           Elements::symbol(fromThis->Z()), fromThis->nBonds());
            //                bondResult = newMap;
            //                break;
            //            }

            // Try the match
            ++depth;
            fmt::print("[{}] ... trying to match reference bond partner {} ({},nbonds={}) with instance bond partner {} "
                       "({},nbonds={})...\n",
                       depth, referenceBondPartner->index(), Elements::symbol(referenceBondPartner->Z()),
                       referenceBondPartner->nBonds(), instanceBondPartner->index(), Elements::symbol(instanceBondPartner->Z()),
                       instanceBondPartner->nBonds());
            auto matchedMap = matchAtom(referenceBondPartner, instanceBondPartner, refNETA, newMap, instanceAtom);
            --depth;
            if (!matchedMap.empty())
            {
                fmt::print("[{}] Bond requirements satisfied for bond {}\n", depth, count2);
                bondResult = matchedMap;
                break;
            }
        }

        // If we found a suitable match recursing into the bond, store the result into newMap and continue to the next bond.
        // If we didn't find a good match, we return now.
        if (bondResult.empty())
        {
            fmt::print("[{}] FAIL Bond requirements not met.\n", depth);
            return {};
        }
        else
            newMap = bondResult;
    }

    fmt::print("[{}] All requirements met.\n", depth);
    // If we get to here then we succeeded, so return the new map
    return newMap;
}

// Calculate difference metric between the supplied species and local molecule
std::pair<double, std::vector<int>> CIFHandler::differenceMetric(const Species *species, const LocalMolecule &molecule)
{
    auto difference = 0.0;
    std::vector<int> atomIndexMap(species->nAtoms(), -1);
    auto nBadAtoms = 0;
    for (auto spI = 0; spI < species->nAtoms(); ++spI)
    {
        auto &spAtom = species->atom(spI);

        // For this species atom find the closest atom in the molecule
        auto distanceSq = 1.0e6;
        for (auto molI = 0; molI < molecule.nAtoms(); ++molI)
        {
            auto rABSq = (spAtom.r() - molecule.localAtoms()[molI].r()).magnitudeSq();
            if (rABSq < distanceSq)
            {
                distanceSq = rABSq;
                atomIndexMap[spI] = molI;
            }
        }

        if (distanceSq > 0.1)
            ++nBadAtoms;

        // Update the difference score
        const auto &closestMolSpAtom = molecule.species()->atom(atomIndexMap[spI]);
        difference += distanceSq;
        if (spAtom.Z() != closestMolSpAtom.Z())
            difference += std::max(spAtom.Z(), closestMolSpAtom.Z()) * 10.0;
    }

    Messenger::print("NBADATOMS = {} of {}\n", nBadAtoms, species->nAtoms());

    return {difference, atomIndexMap};
}
