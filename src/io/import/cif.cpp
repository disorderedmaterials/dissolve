// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "io/import/cif.h"
#include "CIFImportLexer.h"
#include "base/messenger.h"
#include "base/sysfunc.h"
#include "io/import/CIFImportErrorListeners.h"
#include "io/import/CIFImportVisitor.h"
#include "templates/algorithms.h"

// Parse supplied file into the destination objects
bool CIFImport::parse(std::string filename, CIFImport::CIFTags &tags) const
{
    // Set up ANTLR input stream
    std::ifstream cifFile(std::string(filename), std::ios::in | std::ios::binary);
    if (!cifFile.is_open())
    {
        Messenger::error("Failed to open CIF file '{}.\n", filename);
        return false;
    }
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
bool CIFImport::validFile(std::string filename) const
{
    CIFTags tags;
    return parse(filename, tags);
}

// Read CIF data from specified file
bool CIFImport::read(std::string filename)
{
    assemblies_.clear();
    bondingPairs_.clear();
    tags_.clear();

    if (!parse(filename, tags_))
        return Messenger::error("Failed to parse CIF file '{}'.\n", filename);

    // Set up spacegroup
    std::optional<std::string> sgName = getTagString("_space_group_name_H-M_alt");
    if (!sgName)
        sgName = getTagString("_symmetry_space_group_name_H-M");
    if (!sgName)
    {
        auto sgID = getTagInt("_space_group_IT_number");
        if (sgID)
            sgName = Spacegroup::name(sgID.value());
    }

    if (!sgName)
        return Messenger::error("No suitable spacegroup information found in CIF.\n");
    spacegroup_.initialise(sgName.value());

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
        auto Z = n < atomSiteTypeSymbol.size() ? Elements::element(atomSiteTypeSymbol[n]) : Elements::Unknown;
        auto occ = n < atomSiteOccupancy.size() ? atomSiteOccupancy[n] : 1.0;
        Vec3<double> rFrac(atomSiteFractX[n], atomSiteFractY[n], atomSiteFractZ[n]);

        // Add the atom to an assembly - there are three possibilities regarding (disorder) grouping:
        //   1) A group is defined, but no assembly - add the atom to the 'Disorder' assembly
        //   2) An assembly and a group are defined - add it to that
        //   3) No group or assembly are defined - add the atom to the 'Global' assembly under the 'All' group
        auto assemblyName = atomDisorderAssembly.empty() ? "." : atomDisorderAssembly[n];
        auto groupName = atomDisorderGroup.empty() ? "." : atomDisorderGroup[n];
        if (groupName != "." && assemblyName == ".")
            assemblyName = "Disorder";
        else if (groupName == "." && assemblyName == ".")
        {
            assemblyName = "Global";
            groupName = "All";
        }

        // Get the assembly and group that we're adding the atom to
        auto &assembly = getAssembly(assemblyName);
        auto &group = assembly.getGroup(groupName);
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

// Return tag data string (if it exists) assuming a single datum (first in the vector)
std::optional<std::string> CIFImport::getTagString(std::string tag) const
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
std::vector<std::string> CIFImport::getTagStrings(std::string tag) const
{
    auto it = tags_.find(tag);
    if (it == tags_.end())
        return {};

    return it->second;
}

// Return tag data as double (if it exists) assuming a single datum (first in the vector)
std::optional<double> CIFImport::getTagDouble(std::string tag) const
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
std::vector<double> CIFImport::getTagDoubles(std::string tag) const
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
std::optional<int> CIFImport::getTagInt(std::string tag) const
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

// Get (add or retrieve) named assembly
CIFAssembly &getAssembly(std::string_view name);

// Return spacegroup information
const Spacegroup &CIFImport::spacegroup() const { return spacegroup_; }

// Return cell lengths
std::optional<Vec3<double>> CIFImport::getCellLengths() const
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
std::optional<Vec3<double>> CIFImport::getCellAngles() const
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
std::string CIFImport::chemicalFormula() const
{
    auto it = tags_.find("_chemical_formula_sum");
    return (it != tags_.end() ? it->second.front() : "Unknown");
}

// Get (add or retrieve) named assembly
CIFAssembly &CIFImport::getAssembly(std::string_view name)
{
    auto it = std::find_if(assemblies_.begin(), assemblies_.end(), [name](const auto &a) { return a.name() == name; });
    if (it != assemblies_.end())
        return *it;

    return assemblies_.emplace_back(name);
}

// Return atom assemblies
std::vector<CIFAssembly> &CIFImport::assemblies() { return assemblies_; }

const std::vector<CIFAssembly> &CIFImport::assemblies() const { return assemblies_; }

// Return whether any bond distances are defined
bool CIFImport::hasBondDistances() const { return !bondingPairs_.empty(); }

// Return whether a bond distance is defined for the specified label pair
std::optional<double> CIFImport::bondDistance(std::string_view labelI, std::string_view labelJ) const
{
    auto it = std::find_if(bondingPairs_.begin(), bondingPairs_.end(), [labelI, labelJ](const auto &bp) {
        return (bp.labelI() == labelI && bp.labelJ() == labelJ) || (bp.labelI() == labelJ && bp.labelJ() == labelI);
    });
    if (it != bondingPairs_.end())
        return it->r();
    return std::nullopt;
}
