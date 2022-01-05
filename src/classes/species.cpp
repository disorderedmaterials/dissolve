// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "classes/species.h"
#include "classes/atomtype.h"
#include "data/isotopes.h"

Species::Species() : attachedAtomListsGenerated_(false), forcefield_(nullptr)
{
    box_ = std::make_unique<SingleImageBox>();

    // Set up natural Isotopologue
    naturalIsotopologue_.setName("Natural");
    naturalIsotopologue_.setParent(this);
}

// Clear Data
void Species::clear()
{
    isotopologues_.clear();
    impropers_.clear();
    torsions_.clear();
    angles_.clear();
    bonds_.clear();
    atoms_.clear();

    ++version_;
}

// Copy basic information (atoms and intramolecular terms)
void Species::copyBasic(const Species *source)
{
    clear();

    name_ = source->name_;

    for (auto &i : source->atoms_)
        addAtom(i.Z(), i.r(), i.charge());

    for (auto &bond : source->bonds_)
        addBond(bond.indexI(), bond.indexJ());
    for (auto &angle : source->angles_)
        addAngle(angle.indexI(), angle.indexJ(), angle.indexK());
    for (auto &torsion : source->torsions_)
        addTorsion(torsion.indexI(), torsion.indexJ(), torsion.indexK(), torsion.indexL());
    for (auto &improper : source->impropers_)
        addTorsion(improper.indexI(), improper.indexJ(), improper.indexK(), improper.indexL());
}

/*
 * Basic Information
 */

// Set name of the Species
void Species::setName(std::string_view name) { name_ = name; }

// Return the name of the Species
std::string_view Species::name() const { return name_; }

// Check set-up of Species
bool Species::checkSetUp() const
{
    auto nErrors = 0;

    // Must have at least one atom...
    if (atoms_.size() == 0)
    {
        Messenger::error("Species contains no Atoms.\n");
        return false;
    }

    /*
     * AtomTypes
     */
    for (auto &i : atoms_)
    {
        if (i.atomType() == nullptr)
        {
            Messenger::error("Atom {} ({}) of species '{}' has no associated atom type.\n", i.userIndex(),
                             Elements::symbol(i.Z()), name_);
            ++nErrors;
        }
    }
    if (nErrors > 0)
        return false;

    /*
     * IntraMolecular Data
     */
    for (auto &i : atoms_)
    {
        if ((i.nBonds() == 0) && (atoms_.size() > 1))
        {
            Messenger::error("SpeciesAtom {} ({}) participates in no Bonds, but is part of a multi-atom Species.\n",
                             i.userIndex(), Elements::symbol(i.Z()));
            ++nErrors;
        }

        // Check each Bond for two-way consistency
        for (const SpeciesBond &bond : i.bonds())
        {
            auto *partner = bond.partner(&i);
            if (!partner->getBond(&i))
            {
                Messenger::error("SpeciesAtom {} references a Bond to SpeciesAtom {}, but SpeciesAtom {} does not.\n",
                                 i.userIndex(), partner->userIndex(), partner->userIndex());
                ++nErrors;
            }
        }
    }
    if (nErrors > 0)
        return false;

    /*
     * Check Isotopologues
     */
    for (auto &iso : isotopologues_)
    {
        for (auto [atomType, isotope] : iso->isotopes())
        {
            if (isotope == Sears91::Isotope::Unknown)
            {
                Messenger::error("Isotopologue '{}' does not refer to an elemental Isotope for AtomType '{}'.\n", iso->name(),
                                 atomType->name());
                ++nErrors;
            }
            else if (Sears91::Z(isotope) != atomType->Z())
            {
                Messenger::error("Isotopologue '{}' does not refer to a suitable Isotope for AtomType '{}'.\n", iso->name(),
                                 atomType->name());
                ++nErrors;
            }
        }
    }

    return (nErrors == 0);
}

// Print Species information
void Species::print() const
{
    Messenger::print("  Atoms:\n");
    Messenger::print("      ID   El  Type (ID)        X             Y             Z             Q\n");
    Messenger::print("    ----------------------------------------------------------------------------\n");
    for (auto n = 0; n < nAtoms(); ++n)
    {
        auto &i = atom(n);
        Messenger::print("    {:4d}  {:3}  {:4} ({:2d})  {:12.4e}  {:12.4e}  {:12.4e}  {:12.4e}\n", n + 1,
                         Elements::symbol(i.Z()), (i.atomType() ? i.atomType()->name() : "??"),
                         (i.atomType() ? i.atomType()->index() : -1), i.r().x, i.r().y, i.r().z, i.charge());
    }

    if (nBonds() > 0)
    {
        Messenger::print("\n  Bonds:\n");
        Messenger::print("      I     J    Form             Parameters\n");
        Messenger::print("    ---------------------------------------------------------------------------------\n");
        for (const auto &bond : bonds_)
            Messenger::print("   {:4d}  {:4d}    {}{:<12}  {}\n", bond.indexI() + 1, bond.indexJ() + 1,
                             bond.masterTerm() ? '@' : ' ', BondFunctions::forms().keyword(bond.form()),
                             bond.parametersAsString());
    }

    if (nAngles() > 0)
    {
        Messenger::print("\n  Angles:\n");
        Messenger::print("      I     J     K    Form             Parameters\n");
        Messenger::print("    ---------------------------------------------------------------------------------------\n");
        for (const auto &angle : angles_)
            Messenger::print("   {:4d}  {:4d}  {:4d}    {}{:<12}  {}\n", angle.indexI() + 1, angle.indexJ() + 1,
                             angle.indexK() + 1, angle.masterTerm() ? '@' : ' ', AngleFunctions::forms().keyword(angle.form()),
                             angle.parametersAsString());
    }

    if (nTorsions() > 0)
    {
        Messenger::print("\n  Torsions:\n");
        Messenger::print("      I     J     K     L    Form             Parameters\n");
        Messenger::print("    ---------------------------------------------------------------------------------------------\n");
        // Loop over Torsions
        for (const auto &torsion : torsions())
            Messenger::print("   {:4d}  {:4d}  {:4d}  {:4d}    {}{:<12}  {}\n", torsion.indexI() + 1, torsion.indexJ() + 1,
                             torsion.indexK() + 1, torsion.indexL() + 1, torsion.masterTerm() ? '@' : ' ',
                             TorsionFunctions::forms().keyword(torsion.form()), torsion.parametersAsString());
    }

    if (nImpropers() > 0)
    {
        Messenger::print("\n  Impropers:\n");
        Messenger::print("      I     J     K     L    Form             Parameters\n");
        Messenger::print("    ---------------------------------------------------------------------------------------------\n");
        // Loop over Impropers
        for (auto &improper : impropers())
            Messenger::print("   {:4d}  {:4d}  {:4d}  {:4d}    {}{:<12}  {}\n", improper.indexI() + 1, improper.indexJ() + 1,
                             improper.indexK() + 1, improper.indexL() + 1, improper.masterTerm() ? '@' : ' ',
                             TorsionFunctions::forms().keyword(improper.form()), improper.parametersAsString());
    }
}

// Return version
int Species::version() const { return version_; }

/*
 * Coordinate Sets
 */

// Clear coordinate sets
void Species::clearCoordinateSets() { coordinateSets_.clear(); }

// Add new coordinate set
std::vector<Vec3<double>> &Species::addCoordinateSet()
{
    auto &newSet = coordinateSets_.emplace_back(atoms_.size(), Vec3<double>());

    return newSet;
}

// Return number of defined coordinate sets
int Species::nCoordinateSets() const { return coordinateSets_.size(); }

// Return coordinates sets
const std::vector<std::vector<Vec3<double>>> &Species::coordinateSets() const { return coordinateSets_; }
