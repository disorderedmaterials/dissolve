/*
    *** Species Input/Output
    *** src/classes/species_io.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/atomtype.h"
#include "classes/coredata.h"
#include "classes/species.h"
#include "data/elements.h"
#include "data/fflibrary.h"
#include "data/isotopes.h"
#include <string>

// Load Species from file
bool Species::load(std::string_view filename)
{
    // Grab extension from filename
    std::string_view ext = DissolveSys::afterLastChar(filename, '.');

    if (DissolveSys::sameString(ext, "xyz"))
        return loadFromXYZ(filename);
    else
        Messenger::print("Can't load species - unknown extension for file '{}'.\n", filename);

    return false;
}

// Load Species information from XYZ file
bool Species::loadFromXYZ(std::string_view filename)
{
    Messenger::print("Loading XYZ data from file '{}'\n", filename);

    // Open the specified file...
    LineParser parser;
    parser.openInput(filename);
    if (!parser.isFileGoodForReading())
    {
        Messenger::error("Couldn't open XYZ file '{}'.\n", filename);
        return false;
    }

    // Clear any existing data
    clear();

    // Simple format - first line is number of atoms, next line is title, then follow atoms/coordinates, one atom per line
    parser.getArgsDelim(LineParser::Defaults);
    auto nAtoms = parser.argi(0);
    parser.readNextLine(LineParser::Defaults);
    name_ = parser.line();
    int success;
    Element *el;
    for (auto n = 0; n < nAtoms; ++n)
    {
        success = parser.getArgsDelim(LineParser::Defaults);
        if (success != 0)
        {
            parser.closeFiles();
            Messenger::error("Couldn't read Atom {} from file '{}'\n", n + 1, filename);
            return false;
        }
        el = Elements::elementPointer(parser.argsv(0));
        SpeciesAtom *i = addAtom(el, parser.arg3d(1));
        if (parser.hasArg(4))
            i->setCharge(parser.argd(4));
    }

    Messenger::print("Succesfully loaded XYZ data from file '{}'.\n", filename);
    parser.closeFiles();
    return true;
}

/*
 * Read / Write
 */

// Return enum option info for SpeciesKeyword
EnumOptions<Species::SpeciesKeyword> Species::keywords()
{
    static EnumOptionsList SpeciesKeywords =
        EnumOptionsList() << EnumOption(Species::AngleKeyword, "Angle", 3, 6) << EnumOption(Species::AtomKeyword, "Atom", 6, 7)
                          << EnumOption(Species::BondKeyword, "Bond", 2, 5)
                          << EnumOption(Species::BondTypeKeyword, "BondType", 3)
                          << EnumOption(Species::ChargeKeyword, "Charge", 2)
                          << EnumOption(Species::CoordinateSetsKeyword, "CoordinateSets", 2, 99)
                          << EnumOption(Species::EndSpeciesKeyword, "EndSpecies")
                          << EnumOption(Species::ForcefieldKeyword, "Forcefield", 1)
                          << EnumOption(Species::ImproperKeyword, "Improper", 5, 9)
                          << EnumOption(Species::IsotopologueKeyword, "Isotopologue", EnumOption::OneOrMoreArguments)
                          << EnumOption(Species::SiteKeyword, "Site", 1)
                          << EnumOption(Species::TorsionKeyword, "Torsion", 4, 9);

    static EnumOptions<Species::SpeciesKeyword> options("SpeciesKeyword", SpeciesKeywords);

    return options;
}

// Read Species definition from specified LineParser
bool Species::read(LineParser &parser, CoreData &coreData)
{
    Messenger::print("\nParsing Species '{}'\n", name());

    Element *el;
    std::shared_ptr<AtomType> at;
    Isotopologue *iso;
    OptionalReferenceWrapper<SpeciesAngle> a;
    SpeciesAtom *i;
    OptionalReferenceWrapper<SpeciesBond> b;
    OptionalReferenceWrapper<SpeciesImproper> imp;
    OptionalReferenceWrapper<SpeciesTorsion> torsion;
    SpeciesSite *site;
    SpeciesBond::BondFunction bf;
    SpeciesAngle::AngleFunction af;
    SpeciesImproper::ImproperFunction impf;
    SpeciesTorsion::TorsionFunction tf;
    SpeciesBond::BondType bt;
    Isotope *tope;
    auto blockDone = false, error = false;

    // Turn off intramolecular term autogeneration while we're reading
    autoUpdateIntramolecularTerms_ = false;

    while (!parser.eofOrBlank())
    {
        // Read in a line, which should contain a keyword and a minimum number of arguments
        if (parser.getArgsDelim() != LineParser::Success)
            return false;

        // Do we recognise this keyword and, if so, do we have an appropriate number of arguments?
        if (!keywords().isValid(parser.argsv(0)))
            return keywords().errorAndPrintValid(parser.argsv(0));
        auto kwd = keywords().enumeration(parser.argsv(0));
        if (!keywords().validNArgs(kwd, parser.nArgs() - 1))
            return false;

        // All OK, so process the keyword
        switch (kwd)
        {
            case (Species::AngleKeyword):
                // Create a new angle definition between the specified atoms
                a = addAngle(parser.argi(1) - 1, parser.argi(2) - 1, parser.argi(3) - 1);
                if (!a)
                {
                    error = true;
                    break;
                }

                /*
                 * If only the indices were given, create an angle without a specified functional form (a
                 * Forcefield is presumably going to be specified). Otherwise, check the functional form
                 * specified - if it starts with
                 * '@' it is a reference to master parameters
                 */
                if (parser.nArgs() == 4)
                    a->get().setForm(SpeciesAngle::NoForm);
                else if (parser.argsv(4)[0] == '@')
                {
                    // Search through master Angle parameters to see if this name exists
                    MasterIntra *master = coreData.hasMasterAngle(parser.argsv(4));
                    if (!master)
                    {
                        Messenger::error("No master Angle parameters named '{}' exist.\n", &parser.argsv(4)[1]);
                        error = true;
                        break;
                    }

                    a->get().setMasterParameters(master);
                }
                else
                {
                    if (!SpeciesAngle::angleFunctions().isValid(parser.argsv(4)))
                    {
                        Messenger::error("Functional form of Angle ({}) not recognised.\n", parser.argsv(4));
                        error = true;
                        break;
                    }
                    af = SpeciesAngle::angleFunctions().enumeration(parser.argsv(4));
                    a->get().setForm(af);

                    // Check number of args provided
                    if (!SpeciesAngle::angleFunctions().validNArgs(af, parser.nArgs() - 5))
                    {
                        error = true;
                        break;
                    }

                    // Set parameters
                    for (auto n = 5; n < parser.nArgs(); ++n)
                        a->get().addParameter(parser.argd(n));
                }

                // Perform any final setup on the Angle
                a->get().setUp();
                break;
            case (Species::AtomKeyword):
                el = Elements::elementPointer(parser.argsv(2));
                if (el->Z() == 0)
                {
                    Messenger::error("Unrecognised element symbol '{}' found in {} keyword.\n", parser.argsv(2),
                                     Species::keywords().keyword(Species::AtomKeyword));
                    el = nullptr;
                    error = true;
                    break;
                }
                i = addAtom(el, parser.arg3d(3));
                if (parser.hasArg(7))
                    i->setCharge(parser.argd(7));

                // Locate the AtomType assigned to the Atom
                if (DissolveSys::sameString("None", parser.argsv(6)))
                    at = nullptr;
                else
                {
                    at = coreData.findAtomType(parser.argsv(6));
                    if (!at)
                    {
                        Messenger::printVerbose("Creating AtomType '{}'...\n", parser.argsv(6));
                        at = coreData.addAtomType(el);
                        at->setName(parser.argsv(6));
                    }
                }

                // Finally, set AtomType for the Atom
                i->setAtomType(at);
                break;
            case (Species::BondKeyword):
                // Create a new bond definition between the specified atoms
                b = addBond(parser.argi(1) - 1, parser.argi(2) - 1);
                if (!b)
                {
                    error = true;
                    break;
                }

                /*
                 * If only the indices were given, create a bond without a specified functional form (a
                 * Forcefield is presumably going to be specified). Otherwise, check the functional form
                 * specified - if it starts with
                 * '@' it is a reference to master parameters
                 */
                if (parser.nArgs() == 3)
                    b->get().setForm(SpeciesBond::NoForm);
                else if (parser.argsv(3)[0] == '@')
                {
                    // Search through master Bond parameters to see if this name exists
                    MasterIntra *master = coreData.hasMasterBond(parser.argsv(3));
                    if (!master)
                    {
                        Messenger::error("No master Bond parameters named '{}' exist.\n", &parser.argsv(3)[1]);
                        error = true;
                        break;
                    }

                    b->get().setMasterParameters(master);
                }
                else
                {
                    // Check the functional form specified
                    if (!SpeciesBond::bondFunctions().isValid(parser.argsv(3)))
                    {
                        Messenger::error("Functional form of Bond ({}) not recognised.\n", parser.argsv(3));
                        error = true;
                        break;
                    }
                    bf = SpeciesBond::bondFunctions().enumeration(parser.argsv(3));
                    b->get().setForm(bf);

                    // Check number of args provided
                    if (!SpeciesBond::bondFunctions().validNArgs(bf, parser.nArgs() - 4))
                    {
                        error = true;
                        break;
                    }

                    // Set parameters
                    for (auto n = 4; n < parser.nArgs(); ++n)
                        b->get().addParameter(parser.argd(n));
                }

                // Perform any final setup on the Bond
                b->get().setUp();
                break;
            case (Species::BondTypeKeyword):
                // Find the specified bond
                b = getBond(parser.argi(1) - 1, parser.argi(2) - 1);
                if (!b)
                {
                    Messenger::error("Tried to set the bond type of bond between atoms {} and {}, but this bond "
                                     "does not exist.\n",
                                     parser.argi(1), parser.argi(2));
                    error = true;
                    break;
                }

                // Get the bond type
                bt = SpeciesBond::bondType(parser.argsv(3));
                if (bt == SpeciesBond::nBondTypes)
                {
                    Messenger::error("Unrecognised bond type '{}'.\n", parser.argsv(3));
                    error = true;
                    break;
                }
                b->get().setBondType(bt);
                break;
            case (Species::ChargeKeyword):
                i = atom(parser.argi(1) - 1);
                if (i)
                    i->setCharge(parser.argd(2));
                else
                {
                    Messenger::error("Specified Atom index ({}) for Charge keyword is out of range.\n", parser.argi(1));
                    error = true;
                }
                break;
            case (Species::CoordinateSetsKeyword):
                if (!coordinateSetInputCoordinates_.read(
                        parser, 1, fmt::format("End{}", keywords().keyword(Species::CoordinateSetsKeyword)), coreData))
                {
                    Messenger::error("Failed to set coordinate sets file / format.\n");
                    error = true;
                }
                else
                {
                    // Open the specified file
                    LineParser coordinateSetParser(parser.processPool());
                    if ((!coordinateSetParser.openInput(coordinateSetInputCoordinates_.filename())) ||
                        (!coordinateSetParser.isFileGoodForReading()))
                    {
                        Messenger::error("Couldn't open coordinate sets file '{}'.\n",
                                         coordinateSetInputCoordinates_.filename());
                        error = true;
                        break;
                    }

                    while (!coordinateSetParser.eofOrBlank())
                    {
                        CoordinateSet *coordSet = addCoordinateSet();
                        if (!coordinateSetInputCoordinates_.importData(coordinateSetParser, coordSet->coordinates()))
                        {
                            Messenger::error("Failed to read coordinate set {} from file.\n", nCoordinateSets());
                            error = true;
                            coordinateSets_.remove(coordSet);
                            break;
                        }
                    }

                    Messenger::print("{} coordinate sets read in for Species '{}'.\n", nCoordinateSets(), name());
                }
                break;
            case (Species::EndSpeciesKeyword):
                if (forcefield_)
                    applyForcefieldTerms(coreData);
                Messenger::print("Found end of Species '{}'.\n", name());
                blockDone = true;
                break;
            case (Species::ForcefieldKeyword):
                forcefield_ = ForcefieldLibrary::forcefield(parser.argsv(1));
                if (!forcefield_)
                {
                    Messenger::error("No forcefield named '{}' exists.\n", parser.argsv(1));
                    error = true;
                    break;
                }
                break;
            case (Species::ImproperKeyword):
                // Check the functional form specified - if it starts with '@' it is a reference to master
                // parameters
                if (parser.argsv(5)[0] == '@')
                {
                    // Search through master Improper parameters to see if this name exists
                    MasterIntra *master = coreData.hasMasterImproper(parser.argsv(5));
                    if (!master)
                    {
                        Messenger::error("No master Improper parameters named '{}' exist.\n", &parser.argsv(5)[1]);
                        error = true;
                        break;
                    }

                    // Create a new improper definition
                    imp = addImproper(parser.argi(1) - 1, parser.argi(2) - 1, parser.argi(3) - 1, parser.argi(4) - 1);
                    if (!imp)
                    {
                        error = true;
                        break;
                    }
                    imp->get().setMasterParameters(master);
                }
                else
                {
                    // Check the functional form specified
                    if (!SpeciesImproper::improperFunctions().isValid(parser.argsv(5)))
                    {
                        Messenger::error("Functional form of Improper ({}) not recognised.\n", parser.argsv(5));
                        error = true;
                        break;
                    }
                    impf = SpeciesImproper::improperFunctions().enumeration(parser.argsv(5));

                    // Create a new improper definition
                    imp = addImproper(parser.argi(1) - 1, parser.argi(2) - 1, parser.argi(3) - 1, parser.argi(4) - 1);
                    if (!imp)
                    {
                        error = true;
                        break;
                    }
                    imp->get().setForm(impf);

                    // Check number of args provided
                    if (!SpeciesImproper::improperFunctions().validNArgs(impf, parser.nArgs() - 6))
                    {
                        error = true;
                        break;
                    }

                    // Set parameters
                    for (auto n = 6; n < parser.nArgs(); ++n)
                        imp->get().addParameter(parser.argd(n));
                }

                // Perform any final setup on the Improper
                imp->get().setUp();
                break;
            case (Species::IsotopologueKeyword):
                iso = addIsotopologue(uniqueIsotopologueName(parser.argsv(1)));
                Messenger::printVerbose("Added Isotopologue '{}' to Species '{}'\n", iso->name(), name());
                // Each parser argument is a string of the form ATOMTYPE=ISO
                for (auto n = 2; n < parser.nArgs(); ++n)
                {
                    // Split argument into parts before and after '='
                    std::string_view arg1 = DissolveSys::beforeChar(parser.argsv(n), '=');
                    std::string_view arg2 = DissolveSys::afterChar(parser.argsv(n), '=');

                    at = coreData.findAtomType(arg1);
                    if (!at)
                    {
                        Messenger::error("Failed to find AtomType '{}', referred to in Isotopologue '{}', "
                                         "Species '{}'\n",
                                         arg1, iso->name(), name());
                        error = true;
                        break;
                    }

                    // Is the supplied isotope valid for the AtomType's element?
                    el = at->element();
                    auto A = std::stoi(std::string(arg2));
                    tope = Isotopes::isotope(el, A);
                    if (tope == nullptr)
                    {
                        Messenger::error("No such Isotope ({}) for element {} (AtomType '{}') in Isotopologue "
                                         "'{}', Species '{}'\n",
                                         A, el->symbol(), at->name(), iso->name(), name());
                        error = true;
                        break;
                    }

                    // Assign isotope to AtomType
                    if (!iso->setAtomTypeIsotope(at, tope))
                    {
                        error = true;
                        break;
                    }
                }
                break;
            case (Species::SiteKeyword):
                // First argument is the name of the site to create - make sure it doesn't exist already
                site = findSite(parser.argsv(1));
                if (site)
                {
                    Messenger::error("The site '{}' already exists on Species '{}', and cannot be redefined.\n",
                                     parser.argsv(1), name());
                    error = true;
                    break;
                }

                site = addSite(parser.argsv(1));
                if (!site->read(parser))
                    error = true;
                break;
            case (Species::TorsionKeyword):
                // Create a new angle definition between the specified atoms
                torsion = addTorsion(parser.argi(1) - 1, parser.argi(2) - 1, parser.argi(3) - 1, parser.argi(4) - 1);
                if (!torsion)
                {
                    error = true;
                    break;
                }

                /*
                 * If only the indices were given, create an angle without a specified functional form (a
                 * Forcefield is presumably going to be specified). Otherwise, check the functional form
                 * specified - if it starts with
                 * '@' it is a reference to master parameters
                 */
                if (parser.nArgs() == 5)
                    torsion->get().setForm(SpeciesTorsion::NoForm);
                else if (parser.argsv(5)[0] == '@')
                {
                    // Search through master Torsion parameters to see if this name exists
                    MasterIntra *master = coreData.hasMasterTorsion(parser.argsv(5));
                    if (!master)
                    {
                        Messenger::error("No master Torsion parameters named '{}' exist.\n", &parser.argsv(5)[1]);
                        error = true;
                        break;
                    }

                    torsion->get().setMasterParameters(master);
                }
                else
                {
                    // Check the functional form specified
                    if (!SpeciesTorsion::torsionFunctions().isValid(parser.argsv(5)))
                    {
                        Messenger::error("Functional form of Torsion ({}) not recognised.\n", parser.argsv(5));
                        error = true;
                        break;
                    }
                    tf = SpeciesTorsion::torsionFunctions().enumeration(parser.argsv(5));
                    torsion->get().setForm(tf);

                    // Check number of args provided
                    if (!SpeciesTorsion::torsionFunctions().validNArgs(tf, parser.nArgs() - 6))
                    {
                        error = true;
                        break;
                    }

                    // Set parameters
                    for (auto n = 6; n < parser.nArgs(); ++n)
                        torsion->get().addParameter(parser.argd(n));
                }

                // Perform any final setup on the Torsion
                torsion->get().setUp();
                break;
            default:
                Messenger::error("Species block keyword '{}' not accounted for.\n", keywords().keyword(kwd));
                error = true;
                break;
        }

        // Error encountered?
        if (error)
            break;

        // End of block?
        if (blockDone)
            break;
    }

    // Turn intramolecular term autogeneration back on
    autoUpdateIntramolecularTerms_ = true;

    // If there's no error and the blockDone flag isn't set, return an error
    if (!error && !blockDone)
    {
        Messenger::error("Unterminated Species block found.\n");
        error = true;
    }

    return (!error);
}

// Write Species definition to specified LineParser
bool Species::write(LineParser &parser, std::string_view prefix)
{
    if (!parser.writeLineF("{}Species '{}'\n", prefix, name()))
        return false;

    // Create new prefix
    std::string newPrefix = fmt::format("{}  ", prefix);

    // Atoms
    parser.writeLineF("{}# Atoms\n", newPrefix);
    auto count = 0;
    for (auto *i = atoms_.first(); i != nullptr; i = i->next())
    {
        if (!parser.writeLineF("{}{}  {:3d}  {:3}  {:12.6e}  {:12.6e}  {:12.6e}  '{}'  {:12.6e}\n", newPrefix,
                               keywords().keyword(Species::AtomKeyword), ++count, i->element()->symbol(), i->r().x, i->r().y,
                               i->r().z, i->atomType() == nullptr ? "None" : i->atomType()->name(), i->charge()))
            return false;
    }

    // Bonds
    RefList<const SpeciesBond> bondTypes[SpeciesBond::nBondTypes];
    if (!bonds_.empty())
    {
        if (!parser.writeLineF("\n{}# Bonds\n", newPrefix))
            return false;
        for (const auto &bond : bonds_)
        {
            if (bond.form() == SpeciesBond::NoForm)
            {
                if (!parser.writeLineF("{}{}  {:3d}  {:3d}\n", newPrefix, keywords().keyword(Species::BondKeyword),
                                       bond.indexI() + 1, bond.indexJ() + 1))
                    return false;
            }
            else if (bond.masterParameters())
            {
                if (!parser.writeLineF("{}{}  {:3d}  {:3d}  @{}\n", newPrefix, keywords().keyword(Species::BondKeyword),
                                       bond.indexI() + 1, bond.indexJ() + 1, bond.masterParameters()->name()))
                    return false;
            }
            else
            {
                std::string line =
                    fmt::format("{}{}  {:3d}  {:3d}  {}", newPrefix, keywords().keyword(Species::BondKeyword),
                                bond.indexI() + 1, bond.indexJ() + 1, SpeciesBond::bondFunctions().keywordFromInt(bond.form()));
                for (const auto param : bond.parameters())
                    line += fmt::format("  {:.4f}", param);
                if (!parser.writeLine(line))
                    return false;
            }

            // Add the bond to the reflist corresponding to its indicated bond type (unless it is a SingleBond,
            // which we will ignore as this is the default)
            if (bond.bondType() != SpeciesBond::SingleBond)
                bondTypes[bond.bondType()].append(&bond);
        }

        // Any bond type information to write?
        auto bondTypeHeaderWritten = false;
        for (auto bt = 1; bt < SpeciesBond::nBondTypes; ++bt)
            if (bondTypes[bt].nItems() > 0)
            {
                // Write header if it hasn't been written already
                if (!bondTypeHeaderWritten)
                {
                    if (!parser.writeLineF("\n{}# Bond Types\n", newPrefix))
                        return false;
                    bondTypeHeaderWritten = true;
                }
                for (const auto *bond : bondTypes[bt])
                    if (!parser.writeLineF("{}{}  {:3d}  {:3d}  {}\n", newPrefix, keywords().keyword(Species::BondTypeKeyword),
                                           bond->indexI() + 1, bond->indexJ() + 1,
                                           SpeciesBond::bondType((SpeciesBond::BondType)bt)))
                        return false;
            }
    }

    // Angles
    if (nAngles() > 0)
    {
        if (!parser.writeLineF("\n{}# Angles\n", newPrefix))
            return false;
        for (const auto &angle : angles())
        {
            if (angle.form() == SpeciesAngle::NoForm)
            {
                if (!parser.writeLineF("{}{}  {:3d}  {:3d}  {:3d}\n", newPrefix, keywords().keyword(Species::AngleKeyword),
                                       angle.indexI() + 1, angle.indexJ() + 1, angle.indexK() + 1))
                    return false;
            }
            else if (angle.masterParameters())
            {
                if (!parser.writeLineF("{}{}  {:3d}  {:3d}  {:3d}  @{}\n", newPrefix, keywords().keyword(Species::AngleKeyword),
                                       angle.indexI() + 1, angle.indexJ() + 1, angle.indexK() + 1,
                                       angle.masterParameters()->name()))
                    return false;
            }
            else
            {
                std::string line = fmt::format(
                    "{}{}  {:3d}  {:3d}  {:3d}  {}", newPrefix, keywords().keyword(Species::AngleKeyword), angle.indexI() + 1,
                    angle.indexJ() + 1, angle.indexK() + 1, SpeciesAngle::angleFunctions().keywordFromInt(angle.form()));
                for (const auto param : angle.parameters())
                    line += fmt::format("  {:.4f}", param);
                if (!parser.writeLine(line))
                    return false;
            }
        }
    }

    // Torsions
    if (nTorsions() > 0)
    {
        if (!parser.writeLineF("\n{}# Torsions\n", newPrefix))
            return false;
        for (const auto &torsion : torsions())
        {
            if (torsion.form() == SpeciesTorsion::NoForm)
            {
                if (!parser.writeLineF("{}{}  {:3d}  {:3d}  {:3d}  {:3d}\n", newPrefix,
                                       keywords().keyword(Species::TorsionKeyword), torsion.indexI() + 1, torsion.indexJ() + 1,
                                       torsion.indexK() + 1, torsion.indexL() + 1))
                    return false;
            }
            else if (torsion.masterParameters())
            {
                if (!parser.writeLineF("{}{}  {:3d}  {:3d}  {:3d}  {:3d}  @{}\n", newPrefix,
                                       keywords().keyword(Species::TorsionKeyword), torsion.indexI() + 1, torsion.indexJ() + 1,
                                       torsion.indexK() + 1, torsion.indexL() + 1, torsion.masterParameters()->name()))
                    return false;
            }
            else
            {
                std::string line =
                    fmt::format("{}{}  {:3d}  {:3d}  {:3d}  {:3d}  {}", newPrefix, keywords().keyword(Species::TorsionKeyword),
                                torsion.indexI() + 1, torsion.indexJ() + 1, torsion.indexK() + 1, torsion.indexL() + 1,
                                SpeciesTorsion::torsionFunctions().keywordFromInt(torsion.form()));
                for (const auto param : torsion.parameters())
                    line += fmt::format("  {:.4f}", param);
                if (!parser.writeLine(line))
                    return false;
            }
        }
    }

    // Impropers
    if (nImpropers() > 0)
    {
        if (!parser.writeLineF("\n{}# Impropers\n", newPrefix))
            return false;
        for (auto &imp : impropers())
        {
            if (imp.masterParameters())
            {
                if (!parser.writeLineF("{}{}  {:3d}  {:3d}  {:3d}  {:3d}  @{}\n", newPrefix,
                                       keywords().keyword(Species::ImproperKeyword), imp.indexI() + 1, imp.indexJ() + 1,
                                       imp.indexK() + 1, imp.indexL() + 1, imp.masterParameters()->name()))
                    return false;
            }
            else
            {
                std::string line =
                    fmt::format("{}{}  {:3d}  {:3d}  {:3d}  {:3d}  {}", newPrefix, keywords().keyword(Species::ImproperKeyword),
                                imp.indexI() + 1, imp.indexJ() + 1, imp.indexK() + 1, imp.indexL() + 1,
                                SpeciesImproper::improperFunctions().keywordFromInt(imp.form()));
                for (const auto param : imp.parameters())
                    line += fmt::format("  {:.4f}", param);
                if (!parser.writeLine(line))
                    return false;
            }
        }
    }

    // Forcefield
    if (forcefield_)
    {
        if (!parser.writeLineF("\n{}# Forcefield\n", newPrefix))
            return false;
        if (!parser.writeLineF("{}{}  '{}'\n", newPrefix, keywords().keyword(Species::ForcefieldKeyword), forcefield_->name()))
            return false;
    }

    // Isotopologues
    if (nIsotopologues() > 0)
    {
        if (!parser.writeLineF("\n{}# Isotopologues\n", newPrefix))
            return false;

        for (auto *iso = isotopologues_.first(); iso != nullptr; iso = iso->next())
        {
            if (!parser.writeLineF("{}{}  '{}'", newPrefix, keywords().keyword(Species::IsotopologueKeyword), iso->name()))
                return false;
            for (auto [atomType, isotope] : iso->isotopes())
            {
                // No need to write anything that's the natural isotope...
                if (isotope->A() == 0)
                    continue;

                if (!parser.writeLineF("  {}={}", atomType->name(), isotope->A()))
                    return false;
            }
            if (!parser.writeLineF("\n"))
                return false;
        }
    }

    // Sites
    if (nSites() > 0)
    {
        if (!parser.writeLineF("\n{}# Sites\n", newPrefix))
            return false;

        ListIterator<SpeciesSite> siteIterator(sites());
        while (SpeciesSite *site = siteIterator.iterate())
            if (!site->write(parser, newPrefix))
                return false;
    }

    // Input Coordinates
    if (coordinateSetInputCoordinates_.hasValidFileAndFormat())
    {
        if (!coordinateSetInputCoordinates_.writeFilenameAndFormat(
                parser, fmt::format("\n{}{}  ", newPrefix, keywords().keyword(Species::CoordinateSetsKeyword))))
            return false;
        if (!coordinateSetInputCoordinates_.writeBlock(parser, newPrefix))
            return false;
        if (!parser.writeLineF("{}End{}\n", newPrefix, keywords().keyword(Species::CoordinateSetsKeyword)))
            return false;
    }

    // Done with this species
    if (!parser.writeLineF("{}{}\n", prefix, keywords().keyword(Species::EndSpeciesKeyword)))
        return false;

    return true;
}
