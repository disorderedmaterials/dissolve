// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/atomType.h"
#include "classes/coreData.h"
#include "classes/species.h"
#include "data/elements.h"
#include "data/ff/library.h"
#include "data/isotopes.h"
#include <string>

/*
 * Read / Write
 */

// Return enum option info for SpeciesKeyword
EnumOptions<Species::SpeciesKeyword> Species::keywords()
{
    return EnumOptions<Species::SpeciesKeyword>(
        "SpeciesKeyword", {{Species::SpeciesKeyword::Angle, "Angle", 3, OptionArguments::AnyNumber},
                           {Species::SpeciesKeyword::Atom, "Atom", 6, 7},
                           {Species::SpeciesKeyword::Bond, "Bond", 2, OptionArguments::AnyNumber},
                           {Species::SpeciesKeyword::BondType, "BondType", 3},
                           {Species::SpeciesKeyword::BoxAngles, "BoxAngles", 3},
                           {Species::SpeciesKeyword::BoxLengths, "BoxLengths", 3},
                           {Species::SpeciesKeyword::Charge, "Charge", 2},
                           {Species::SpeciesKeyword::EndSpecies, "EndSpecies"},
                           {Species::SpeciesKeyword::Forcefield, "Forcefield", 1},
                           {Species::SpeciesKeyword::Improper, "Improper", 5, OptionArguments::AnyNumber},
                           {Species::SpeciesKeyword::Isotopologue, "Isotopologue", OptionArguments::OneOrMore},
                           {Species::SpeciesKeyword::NAngles, "NAngles", 1},
                           {Species::SpeciesKeyword::NAtoms, "NAtoms", 1},
                           {Species::SpeciesKeyword::NBonds, "NBonds", 1},
                           {Species::SpeciesKeyword::NImpropers, "NImpropers", 1},
                           {Species::SpeciesKeyword::NTorsions, "NTorsions", 1},
                           {Species::SpeciesKeyword::Scaling14, "Scaling14", 2, 2},
                           {Species::SpeciesKeyword::Site, "Site", 1},
                           {Species::SpeciesKeyword::Torsion, "Torsion", 4, OptionArguments::AnyNumber}});
}

// Read Species definition from specified LineParser
bool Species::read(LineParser &parser, CoreData &coreData)
{
    Messenger::print("\nParsing Species '{}'\n", name());

    Elements::Element Z;
    std::shared_ptr<AtomType> at;
    Isotopologue *iso;
    OptionalReferenceWrapper<SpeciesAngle> a;
    OptionalReferenceWrapper<SpeciesBond> b;
    OptionalReferenceWrapper<SpeciesImproper> imp;
    OptionalReferenceWrapper<SpeciesTorsion> torsion;
    SpeciesSite *site;
    BondFunctions::Form bf;
    AngleFunctions::Form af;
    TorsionFunctions::Form tf;
    SpeciesBond::BondType bt;
    Vec3<double> boxAngles(90.0, 90.0, 90.0);
    std::optional<Vec3<double>> boxLengths;
    auto elec14Scaling = 0.5, vdw14Scaling = 0.5;
    auto blockDone = false, errorsEncountered = false;
    auto atomVectorFixed = false, bondVectorFixed = false, angleVectorFixed = false, torsionVectorFixed = false,
         improperVectorFixed = false;
    auto atomIndex = 0, bondIndex = 0, angleIndex = 0, torsionIndex = 0, improperIndex = 0;

    while (!parser.eofOrBlank())
    {
        // Read in a line, which should contain a keyword and a minimum number of arguments
        if (parser.getArgsDelim() != LineParser::Success)
            return false;

        // Do we recognise this keyword and, if so, do we have an appropriate number of arguments?
        if (!keywords().isValid(parser.argsv(0)))
        {
            keywords().errorAndPrintValid(parser.argsv(0));
            errorsEncountered = true;
            continue;
        }
        auto kwd = keywords().enumeration(parser.argsv(0));
        if (!keywords().validNArgs(kwd, parser.nArgs() - 1))
        {
            errorsEncountered = true;
            continue;
        }

        // All OK, so process the keyword
        switch (kwd)
        {
            case (Species::SpeciesKeyword::Angle):
                // Create a new angle definition between the specified atoms
                if (angleVectorFixed && angleIndex < angles_.size())
                {
                    if (hasAngle(&atoms_[parser.argi(1) - 1], &atoms_[parser.argi(2) - 1], &atoms_[parser.argi(3) - 1]))
                        return false;
                    angles_[angleIndex].assign(&atoms_[parser.argi(1) - 1], &atoms_[parser.argi(2) - 1],
                                               &atoms_[parser.argi(3) - 1]);
                    a = angles_[angleIndex++];
                }
                else
                    a = addAngle(parser.argi(1) - 1, parser.argi(2) - 1, parser.argi(3) - 1);

                /*
                 * If only the indices were given, create an angle without a specified functional form (a
                 * Forcefield is presumably going to be specified). Otherwise, check the functional form
                 * specified - if it starts with
                 * '@' it is a reference to master parameters
                 */
                if (parser.nArgs() == 4)
                    a->get().setInteractionForm(AngleFunctions::Form::None);
                else if (parser.argsv(4)[0] == '@')
                {
                    // Search through master Angle parameters to see if this name exists
                    auto master = coreData.getMasterAngle(parser.argsv(4));
                    if (!master)
                    {
                        Messenger::error("No master Angle parameters named '{}' exist.\n", &parser.argsv(4)[1]);
                        errorsEncountered = true;
                        break;
                    }

                    a->get().setMasterTerm(&master->get());
                }
                else
                {
                    if (!AngleFunctions::forms().isValid(parser.argsv(4)))
                    {
                        Messenger::error("Functional form of Angle ({}) not recognised.\n", parser.argsv(4));
                        errorsEncountered = true;
                        break;
                    }
                    af = AngleFunctions::forms().enumeration(parser.argsv(4));
                    a->get().setInteractionForm(af);

                    // Check number of args provided
                    if (!AngleFunctions::forms().validNArgs(af, parser.nArgs() - 5))
                    {
                        errorsEncountered = true;
                        break;
                    }

                    // Set parameters
                    if (!a->get().setInteractionParameters(parser, 5))
                    {
                        errorsEncountered = true;
                        break;
                    }
                }
                break;
            case (Species::SpeciesKeyword::Atom):
                Z = Elements::element(parser.argsv(2));
                if (Z == Elements::Unknown)
                {
                    Messenger::error("Unrecognised element symbol '{}' found in {} keyword.\n", parser.argsv(2),
                                     Species::keywords().keyword(Species::SpeciesKeyword::Atom));
                    errorsEncountered = true;
                    break;
                }
                if (atomVectorFixed && atomIndex < atoms_.size())
                    atoms_[atomIndex].set(Z, parser.arg3d(3), parser.hasArg(7) ? parser.argd(7) : 0.0);
                else
                    atomIndex = addAtom(Z, parser.arg3d(3), parser.hasArg(7) ? parser.argd(7) : 0.0);

                // Locate the AtomType assigned to the Atom
                if (DissolveSys::sameString("None", parser.argsv(6)))
                    at = nullptr;
                else
                {
                    at = coreData.findAtomType(parser.argsv(6));
                    if (!at)
                    {
                        Messenger::printVerbose("Creating AtomType '{}'...\n", parser.argsv(6));
                        at = coreData.addAtomType(Z);
                        at->setName(parser.argsv(6));
                    }
                }

                // Finally, set AtomType for the Atom
                atoms_[atomIndex++].setAtomType(at);
                break;
            case (Species::SpeciesKeyword::Bond):
                // Create a new bond definition between the specified atoms
                if (bondVectorFixed && bondIndex < bonds_.size())
                {
                    if (hasBond(&atoms_[parser.argi(1) - 1], &atoms_[parser.argi(2) - 1]))
                        return false;
                    bonds_[bondIndex].assign(&atoms_[parser.argi(1) - 1], &atoms_[parser.argi(2) - 1]);
                    b = bonds_[bondIndex++];
                }
                else
                    b = addBond(parser.argi(1) - 1, parser.argi(2) - 1);

                /*
                 * If only the indices were given, create a bond without a specified functional form (a
                 * Forcefield is presumably going to be specified). Otherwise, check the functional form
                 * specified - if it starts with
                 * '@' it is a reference to master parameters
                 */
                if (parser.nArgs() == 3)
                    b->get().setInteractionForm(BondFunctions::Form::None);
                else if (parser.argsv(3)[0] == '@')
                {
                    // Search through master Bond parameters to see if this name exists
                    auto master = coreData.getMasterBond(parser.argsv(3));
                    if (!master)
                    {
                        Messenger::error("No master Bond parameters named '{}' exist.\n", &parser.argsv(3)[1]);
                        errorsEncountered = true;
                        break;
                    }

                    b->get().setMasterTerm(&master->get());
                }
                else
                {
                    // Check the functional form specified
                    if (!BondFunctions::forms().isValid(parser.argsv(3)))
                    {
                        Messenger::error("Functional form of Bond ({}) not recognised.\n", parser.argsv(3));
                        errorsEncountered = true;
                        break;
                    }
                    bf = BondFunctions::forms().enumeration(parser.argsv(3));
                    b->get().setInteractionForm(bf);

                    // Check number of args provided
                    if (!BondFunctions::forms().validNArgs(bf, parser.nArgs() - 4))
                    {
                        errorsEncountered = true;
                        break;
                    }

                    // Set parameters
                    if (!b->get().setInteractionParameters(parser, 4))
                    {
                        errorsEncountered = true;
                        break;
                    }
                }
                break;
            case (Species::SpeciesKeyword::BondType):
                // Find the specified bond
                b = getBond(parser.argi(1) - 1, parser.argi(2) - 1);
                if (!b)
                {
                    Messenger::error("Tried to set the bond type of bond between atoms {} and {}, but this bond "
                                     "does not exist.\n",
                                     parser.argi(1), parser.argi(2));
                    errorsEncountered = true;
                    break;
                }

                // Get the bond type
                bt = SpeciesBond::bondType(parser.argsv(3));
                if (bt == SpeciesBond::nBondTypes)
                {
                    Messenger::error("Unrecognised bond type '{}'.\n", parser.argsv(3));
                    errorsEncountered = true;
                    break;
                }
                b->get().setBondType(bt);
                break;
            case (Species::SpeciesKeyword::BoxAngles):
                boxAngles = parser.arg3d(1);
                break;
            case (Species::SpeciesKeyword::BoxLengths):
                boxLengths = parser.arg3d(1);
                break;
            case (Species::SpeciesKeyword::Charge):
            {
                auto index = parser.argi(1) - 1;
                if (index >= nAtoms())
                {
                    Messenger::error("Specified Atom index ({}) for Charge keyword is out of range.\n", parser.argi(1));
                    errorsEncountered = true;
                }
                auto &i = atom(index);
                i.setCharge(parser.argd(2));
                break;
            }
            case (Species::SpeciesKeyword::EndSpecies):
                // Set periodic box?
                if (boxLengths)
                {
                    // Generate Box
                    box_ = Box::generate(*boxLengths, boxAngles);

                    // Fold atoms
                    for (auto &i : atoms_)
                        setAtomCoordinates(&i, box_->fold(i.r()));
                }
                // Apply Forcefield?
                if (forcefield_ && !applyForcefieldTerms(coreData))
                {
                    errorsEncountered = true;
                    break;
                }
                updateIsotopologues();
                Messenger::print("Found end of Species '{}'.\n", name());
                blockDone = true;
                break;
            case (Species::SpeciesKeyword::Forcefield):
                forcefield_ = ForcefieldLibrary::forcefield(parser.argsv(1));
                if (!forcefield_)
                {
                    Messenger::error("No forcefield named '{}' exists.\n", parser.argsv(1));
                    errorsEncountered = true;
                    break;
                }
                break;
            case (Species::SpeciesKeyword::Improper):
                // Create a new improper definition
                if (improperVectorFixed && improperIndex < impropers_.size())
                {
                    if (hasImproper(&atoms_[parser.argi(1) - 1], &atoms_[parser.argi(2) - 1], &atoms_[parser.argi(3) - 1],
                                    &atoms_[parser.argi(4) - 1]))
                        return false;
                    impropers_[improperIndex].assign(&atoms_[parser.argi(1) - 1], &atoms_[parser.argi(2) - 1],
                                                     &atoms_[parser.argi(3) - 1], &atoms_[parser.argi(4) - 1]);
                    imp = impropers_[improperIndex++];
                }
                else
                    imp = addImproper(parser.argi(1) - 1, parser.argi(2) - 1, parser.argi(3) - 1, parser.argi(4) - 1);

                // Check the functional form specified - if it starts with '@' it is a reference to master
                // parameters
                if (parser.argsv(5)[0] == '@')
                {
                    // Search through master Improper parameters to see if this name exists
                    auto master = coreData.getMasterImproper(parser.argsv(5));
                    if (!master)
                    {
                        Messenger::error("No master Improper parameters named '{}' exist.\n", &parser.argsv(5)[1]);
                        errorsEncountered = true;
                        break;
                    }

                    imp->get().setMasterTerm(&master->get());
                }
                else
                {
                    // Check the functional form specified
                    if (!TorsionFunctions::forms().isValid(parser.argsv(5)))
                    {
                        Messenger::error("Functional form of Improper ({}) not recognised.\n", parser.argsv(5));
                        errorsEncountered = true;
                        break;
                    }
                    tf = TorsionFunctions::forms().enumeration(parser.argsv(5));
                    imp->get().setInteractionForm(tf);

                    // Check number of args provided
                    if (!TorsionFunctions::forms().validNArgs(tf, parser.nArgs() - 6))
                    {
                        errorsEncountered = true;
                        break;
                    }

                    // Set parameters
                    if (!imp->get().setInteractionParameters(parser, 6))
                    {
                        errorsEncountered = true;
                        break;
                    }
                }
                break;
            case (Species::SpeciesKeyword::Isotopologue):
                iso = addIsotopologue(DissolveSys::uniqueName(DissolveSys::niceName(parser.argsv(1)), isotopologues_,
                                                              [](const auto &i) { return i->name(); }));
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
                        errorsEncountered = true;
                        break;
                    }

                    // Is the supplied isotope valid for the AtomType's element?
                    auto A = std::stoi(std::string(arg2));
                    if (!Sears91::hasIsotope(at->Z(), A))
                    {
                        Messenger::error("No such Isotope ({}) for element {} (AtomType '{}') in Isotopologue "
                                         "'{}', Species '{}'\n",
                                         A, Elements::symbol(at->Z()), at->name(), iso->name(), name());
                        errorsEncountered = true;
                        break;
                    }

                    // Assign isotope to AtomType
                    iso->setAtomTypeIsotope(at, Sears91::isotope(at->Z(), A));
                }
                break;
            case (Species::SpeciesKeyword::NAngles):
                if (angleVectorFixed)
                    return Messenger::error("{} keyword can't be specified more than once.\n",
                                            keywords().keyword(Species::SpeciesKeyword::NAngles));
                if (angleIndex != 0)
                    return Messenger::error("{} keyword must specified before the first angle definition.\n",
                                            keywords().keyword(Species::SpeciesKeyword::NAngles));
                angleVectorFixed = true;
                angles_.resize(parser.argi(1));
                break;
            case (Species::SpeciesKeyword::NAtoms):
                if (atomVectorFixed)
                    return Messenger::error("{} keyword can't be specified more than once.\n",
                                            keywords().keyword(Species::SpeciesKeyword::NAtoms));
                if (atomIndex != 0)
                    return Messenger::error("{} keyword must specified before the first atom definition.\n",
                                            keywords().keyword(Species::SpeciesKeyword::NAtoms));
                atomVectorFixed = true;
                atoms_.resize(parser.argi(1));
                for (auto i = 0; i < atoms_.size(); ++i)
                    atoms_[i].setIndex(i);
                break;
            case (Species::SpeciesKeyword::NBonds):
                if (bondVectorFixed)
                    return Messenger::error("{} keyword can't be specified more than once.\n",
                                            keywords().keyword(Species::SpeciesKeyword::NBonds));
                if (bondIndex != 0)
                    return Messenger::error("{} keyword must specified before the first bond definition.\n",
                                            keywords().keyword(Species::SpeciesKeyword::NBonds));
                bondVectorFixed = true;
                bonds_.resize(parser.argi(1));
                break;
            case (Species::SpeciesKeyword::NImpropers):
                if (improperVectorFixed)
                    return Messenger::error("{} keyword can't be specified more than once.\n",
                                            keywords().keyword(Species::SpeciesKeyword::NImpropers));
                if (improperIndex != 0)
                    return Messenger::error("{} keyword must specified before the first improper definition.\n",
                                            keywords().keyword(Species::SpeciesKeyword::NImpropers));
                improperVectorFixed = true;
                impropers_.resize(parser.argi(1));
                break;
            case (Species::SpeciesKeyword::NTorsions):
                if (torsionVectorFixed)
                    return Messenger::error("{} keyword can't be specified more than once.\n",
                                            keywords().keyword(Species::SpeciesKeyword::NTorsions));
                if (torsionIndex != 0)
                    return Messenger::error("{} keyword must specified before the first torsion definition.\n",
                                            keywords().keyword(Species::SpeciesKeyword::NTorsions));
                torsionVectorFixed = true;
                torsions_.resize(parser.argi(1));
                break;
            case (Species::SpeciesKeyword::Scaling14):
                elec14Scaling = parser.argd(1);
                vdw14Scaling = parser.argd(2);
                break;
            case (Species::SpeciesKeyword::Site):
                // First argument is the name of the site to create - make sure it doesn't exist already
                site = findSite(parser.argsv(1));
                if (site)
                {
                    Messenger::error("The site '{}' already exists on Species '{}', and cannot be redefined.\n",
                                     parser.argsv(1), name());
                    errorsEncountered = true;
                    break;
                }

                site = addSite(parser.argsv(1));
                if (!site->read(parser, coreData))
                    errorsEncountered = true;
                break;
            case (Species::SpeciesKeyword::Torsion):
                // Create a new angle definition between the specified atoms
                if (torsionVectorFixed && torsionIndex < torsions_.size())
                {
                    if (hasTorsion(&atoms_[parser.argi(1) - 1], &atoms_[parser.argi(2) - 1], &atoms_[parser.argi(3) - 1],
                                   &atoms_[parser.argi(4) - 1]))
                        return false;
                    torsions_[torsionIndex].assign(&atoms_[parser.argi(1) - 1], &atoms_[parser.argi(2) - 1],
                                                   &atoms_[parser.argi(3) - 1], &atoms_[parser.argi(4) - 1]);
                    torsion = torsions_[torsionIndex++];
                }
                else
                    torsion = addTorsion(parser.argi(1) - 1, parser.argi(2) - 1, parser.argi(3) - 1, parser.argi(4) - 1);

                /*
                 * If only the indices were given, create an angle without a specified functional form (a
                 * Forcefield is presumably going to be specified). Otherwise, check the functional form
                 * specified - if it starts with
                 * '@' it is a reference to master parameters
                 */
                if (parser.nArgs() == 5)
                    torsion->get().setInteractionForm(TorsionFunctions::Form::None);
                else if (parser.argsv(5)[0] == '@')
                {
                    // Search through master Torsion parameters to see if this name exists
                    auto master = coreData.getMasterTorsion(parser.argsv(5));
                    if (!master)
                    {
                        Messenger::error("No master Torsion parameters named '{}' exist.\n", &parser.argsv(5)[1]);
                        errorsEncountered = true;
                        break;
                    }

                    torsion->get().setMasterTerm(&master->get());
                }
                else
                {
                    // Check the functional form specified
                    if (!TorsionFunctions::forms().isValid(parser.argsv(5)))
                    {
                        Messenger::error("Functional form of Torsion ({}) not recognised.\n", parser.argsv(5));
                        errorsEncountered = true;
                        break;
                    }
                    tf = TorsionFunctions::forms().enumeration(parser.argsv(5));
                    torsion->get().setInteractionForm(tf);

                    // Check number of args provided
                    if (!TorsionFunctions::forms().validNArgs(tf, parser.nArgs() - 6))
                    {
                        errorsEncountered = true;
                        break;
                    }

                    // Set parameters
                    if (!torsion->get().setInteractionParameters(parser, 6))
                    {
                        errorsEncountered = true;
                        break;
                    }

                    // Set 1-4 scale factors
                    torsion->get().set14ScalingFactors(elec14Scaling, vdw14Scaling);
                }
                break;
            default:
                Messenger::error("Species block keyword '{}' not accounted for.\n", keywords().keyword(kwd));
                errorsEncountered = true;
                break;
        }

        // End of block?
        if (blockDone)
            break;
    }

    // If there's no errorsEncountered and the blockDone flag isn't set, return an errorsEncountered
    if (!errorsEncountered && !blockDone)
    {
        Messenger::error("Unterminated Species block found.\n");
        errorsEncountered = true;
    }

    return (!errorsEncountered);
}

// Write Species definition to specified LineParser
bool Species::write(LineParser &parser, std::string_view prefix)
{
    if (!parser.writeLineF("{}Species '{}'\n", prefix, name()))
        return false;

    // Create new prefix
    std::string newPrefix = fmt::format("{}  ", prefix);

    // Atoms
    if (!parser.writeLineF("{}# Atoms\n", newPrefix))
        return false;
    if (!parser.writeLineF("{}{}  {}\n", newPrefix, keywords().keyword(Species::SpeciesKeyword::NAtoms), atoms_.size()))
        return false;
    auto count = 0;
    for (const auto &i : atoms_)
    {
        if (!parser.writeLineF("{}{}  {:3d}  {:3}  {:12.6e}  {:12.6e}  {:12.6e}  '{}'  {:12.6e}\n", newPrefix,
                               keywords().keyword(Species::SpeciesKeyword::Atom), ++count, Elements::symbol(i.Z()), i.r().x,
                               i.r().y, i.r().z, i.atomType() == nullptr ? "None" : i.atomType()->name(), i.charge()))
            return false;
    }

    // Bonds
    std::vector<const SpeciesBond *> bondTypes[SpeciesBond::nBondTypes];
    if (!bonds_.empty())
    {
        if (!parser.writeLineF("\n{}# Bonds\n", newPrefix))
            return false;
        if (!parser.writeLineF("{}{}  {}\n", newPrefix, keywords().keyword(Species::SpeciesKeyword::NBonds), bonds_.size()))
            return false;
        for (const auto &bond : bonds_)
        {
            if (bond.masterTerm())
            {
                if (!parser.writeLineF("{}{}  {:3d}  {:3d}  @{}\n", newPrefix,
                                       keywords().keyword(Species::SpeciesKeyword::Bond), bond.indexI() + 1, bond.indexJ() + 1,
                                       bond.masterTerm()->name()))
                    return false;
            }
            else if (!parser.writeLineF("{}{}  {:3d}  {:3d}  {} {}\n", newPrefix,
                                        keywords().keyword(Species::SpeciesKeyword::Bond), bond.indexI() + 1, bond.indexJ() + 1,
                                        BondFunctions::forms().keyword(bond.interactionForm()),
                                        bond.interactionPotential().parametersAsString()))
                return false;

            // Add the bond to the reference vector based on its indicated bond type (unless it is a SingleBond,
            // which we will ignore as this is the default)
            if (bond.bondType() != SpeciesBond::SingleBond)
                bondTypes[bond.bondType()].push_back(&bond);
        }

        // Any bond type information to write?
        auto bondTypeHeaderWritten = false;
        for (auto bt = 1; bt < SpeciesBond::nBondTypes; ++bt)
            if (!bondTypes[bt].empty())
            {
                // Write header if it hasn't been written already
                if (!bondTypeHeaderWritten)
                {
                    if (!parser.writeLineF("\n{}# Bond Types\n", newPrefix))
                        return false;
                    bondTypeHeaderWritten = true;
                }
                for (const auto *bond : bondTypes[bt])
                    if (!parser.writeLineF("{}{}  {:3d}  {:3d}  {}\n", newPrefix,
                                           keywords().keyword(Species::SpeciesKeyword::BondType), bond->indexI() + 1,
                                           bond->indexJ() + 1, SpeciesBond::bondType((SpeciesBond::BondType)bt)))
                        return false;
            }
    }

    // Angles
    if (nAngles() > 0)
    {
        if (!parser.writeLineF("\n{}# Angles\n", newPrefix))
            return false;
        if (!parser.writeLineF("{}{}  {}\n", newPrefix, keywords().keyword(Species::SpeciesKeyword::NAngles), angles_.size()))
            return false;
        for (const auto &angle : angles())
        {
            if (angle.masterTerm())
            {
                if (!parser.writeLineF("{}{}  {:3d}  {:3d}  {:3d}  @{}\n", newPrefix,
                                       keywords().keyword(Species::SpeciesKeyword::Angle), angle.indexI() + 1,
                                       angle.indexJ() + 1, angle.indexK() + 1, angle.masterTerm()->name()))
                    return false;
            }
            else if (!parser.writeLineF("{}{}  {:3d}  {:3d}  {:3d}  {}  {}\n", newPrefix,
                                        keywords().keyword(Species::SpeciesKeyword::Angle), angle.indexI() + 1,
                                        angle.indexJ() + 1, angle.indexK() + 1,
                                        AngleFunctions::forms().keyword(angle.interactionForm()),
                                        angle.interactionPotential().parametersAsString()))
                return false;
        }
    }

    // Torsions
    if (nTorsions() > 0)
    {
        if (!parser.writeLineF("\n{}# Torsions\n", newPrefix))
            return false;
        if (!parser.writeLineF("{}{}  {}\n", newPrefix, keywords().keyword(Species::SpeciesKeyword::NTorsions),
                               torsions_.size()))
            return false;

        // Set default scaling factors
        auto elec14Scaling = 0.5, vdw14Scaling = 0.5;

        for (const auto &torsion : torsions())
        {
            if (torsion.masterTerm())
            {
                if (!parser.writeLineF("{}{}  {:3d}  {:3d}  {:3d}  {:3d}  @{}\n", newPrefix,
                                       keywords().keyword(Species::SpeciesKeyword::Torsion), torsion.indexI() + 1,
                                       torsion.indexJ() + 1, torsion.indexK() + 1, torsion.indexL() + 1,
                                       torsion.masterTerm()->name()))
                    return false;
            }
            else
            {
                // Write new 1-4 scale factor line if this torsion has different values
                if ((torsion.electrostatic14Scaling() != elec14Scaling || torsion.vanDerWaals14Scaling() != vdw14Scaling) &&
                    !parser.writeLineF(fmt::format("{}{}  {}  {}\n", newPrefix,
                                                   keywords().keyword(Species::SpeciesKeyword::Scaling14),
                                                   torsion.electrostatic14Scaling(), torsion.vanDerWaals14Scaling())))
                    return false;

                if (!parser.writeLineF(fmt::format("{}{}  {:3d}  {:3d}  {:3d}  {:3d}  {}  {}\n", newPrefix,
                                                   keywords().keyword(Species::SpeciesKeyword::Torsion), torsion.indexI() + 1,
                                                   torsion.indexJ() + 1, torsion.indexK() + 1, torsion.indexL() + 1,
                                                   TorsionFunctions::forms().keyword(torsion.interactionForm()),
                                                   torsion.interactionPotential().parametersAsString())))
                    return false;

                elec14Scaling = torsion.electrostatic14Scaling();
                vdw14Scaling = torsion.vanDerWaals14Scaling();
            }
        }
    }

    // Impropers
    if (nImpropers() > 0)
    {
        if (!parser.writeLineF("\n{}# Impropers\n", newPrefix))
            return false;
        if (!parser.writeLineF("{}{}  {}\n", newPrefix, keywords().keyword(Species::SpeciesKeyword::NImpropers),
                               impropers_.size()))
            return false;
        for (auto &imp : impropers())
        {
            if (imp.masterTerm())
            {
                if (!parser.writeLineF("{}{}  {:3d}  {:3d}  {:3d}  {:3d}  @{}\n", newPrefix,
                                       keywords().keyword(Species::SpeciesKeyword::Improper), imp.indexI() + 1,
                                       imp.indexJ() + 1, imp.indexK() + 1, imp.indexL() + 1, imp.masterTerm()->name()))
                    return false;
            }
            else if (!parser.writeLineF("{}{}  {:3d}  {:3d}  {:3d}  {:3d}  {}  {}\n", newPrefix,
                                        keywords().keyword(Species::SpeciesKeyword::Improper), imp.indexI() + 1,
                                        imp.indexJ() + 1, imp.indexK() + 1, imp.indexL() + 1,
                                        TorsionFunctions::forms().keyword(imp.interactionForm()),
                                        imp.interactionPotential().parametersAsString()))
                return false;
        }
    }

    // Box
    if (box_->type() != Box::BoxType::NonPeriodic)
    {
        if (!parser.writeLineF("{}{}  {}  {}  {}\n", newPrefix, keywords().keyword(Species::SpeciesKeyword::BoxAngles),
                               box_->axisAngles().x, box_->axisAngles().y, box_->axisAngles().z))
            return false;
        if (!parser.writeLineF("{}{}  {}  {}  {}\n", newPrefix, keywords().keyword(Species::SpeciesKeyword::BoxLengths),
                               box_->axisLengths().x, box_->axisLengths().y, box_->axisLengths().z))
            return false;
    }

    // Forcefield
    if (forcefield_)
    {
        if (!parser.writeLineF("\n{}# Forcefield\n", newPrefix))
            return false;
        if (!parser.writeLineF("{}{}  '{}'\n", newPrefix, keywords().keyword(Species::SpeciesKeyword::Forcefield),
                               forcefield_->name()))
            return false;
    }

    // Isotopologues
    if (nIsotopologues() > 0)
    {
        if (!parser.writeLineF("\n{}# Isotopologues\n", newPrefix))
            return false;

        for (auto &iso : isotopologues_)
        {
            if (!parser.writeLineF("{}{}  '{}'", newPrefix, keywords().keyword(Species::SpeciesKeyword::Isotopologue),
                                   iso->name()))
                return false;
            for (auto [atomType, isotope] : iso->isotopes())
            {
                // No need to write anything that's the natural isotope...
                if (Sears91::A(isotope) == 0)
                    continue;

                if (!parser.writeLineF("  {}={}", atomType->name(), Sears91::A(isotope)))
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

        for (auto &site : sites())
            if (!site->write(parser, newPrefix))
                return false;
    }

    // Done with this species
    if (!parser.writeLineF("{}{}\n", prefix, keywords().keyword(Species::SpeciesKeyword::EndSpecies)))
        return false;

    return true;
}
