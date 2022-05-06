// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "classes/species.h"
#include "classes/atomtype.h"
#include "data/ff/ff.h"
#include "data/isotopes.h"

Species::Species() : Species(std::string("Natural")) {}
Species::Species(std::string name) : attachedAtomListsGenerated_(false), forcefield_(nullptr), name_(name)
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
                             bond.masterTerm() ? '@' : ' ', BondFunctions::forms().keyword(bond.interactionForm()),
                             bond.interactionPotential().parametersAsString());
    }

    if (nAngles() > 0)
    {
        Messenger::print("\n  Angles:\n");
        Messenger::print("      I     J     K    Form             Parameters\n");
        Messenger::print("    ---------------------------------------------------------------------------------------\n");
        for (const auto &angle : angles_)
            Messenger::print("   {:4d}  {:4d}  {:4d}    {}{:<12}  {}\n", angle.indexI() + 1, angle.indexJ() + 1,
                             angle.indexK() + 1, angle.masterTerm() ? '@' : ' ',
                             AngleFunctions::forms().keyword(angle.interactionForm()),
                             angle.interactionPotential().parametersAsString());
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
                             TorsionFunctions::forms().keyword(torsion.interactionForm()),
                             torsion.interactionPotential().parametersAsString());
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
                             TorsionFunctions::forms().keyword(improper.interactionForm()),
                             improper.interactionPotential().parametersAsString());
    }
}

// Return version
int Species::version() const { return version_; }

toml::basic_value<toml::discard_comments, std::map, std::vector> Species::serialize()
{
    toml::basic_value<toml::discard_comments, std::map, std::vector> species;
    if (forcefield_ != nullptr)
        species["forcefield"] = forcefield_->name().data();

    if (!atoms_.empty())
    {
        toml::array atoms;
        for (auto &atom : atoms_)
            atoms.push_back(atom.serialize());
        species["atom"] = atoms;
    }

    if (!bonds_.empty())
    {
        toml::array bonds;
        for (auto &bond : bonds_)
            bonds.push_back(bond.serialize());
        species["bond"] = bonds;
    }

    if (!angles_.empty())
    {
        toml::array angles;
        for (auto &angle : angles_)
            angles.push_back(angle.serialize());
        species["angle"] = angles;
    }

    if (!impropers_.empty())
    {
        toml::array impropers;
        for (auto &improper : impropers_)
            impropers.push_back(improper.serialize());
        species["improper"] = impropers;
    }

    if (!torsions_.empty())
    {
        toml::array torsions;
        for (auto &torsion : torsions_)
            torsions.push_back(torsion.serialize());
        species["torsion"] = torsions;
    }

    if (!isotopologues_.empty())
    {
        toml::basic_value<toml::discard_comments, std::map, std::vector> isotopologues;
        for (auto &isotopologue : isotopologues_)
            isotopologues[isotopologue->name().data()] = isotopologue->serialize();
        species["isotopologues"] = isotopologues;
    }

    if (!sites_.empty())
    {
        toml::basic_value<toml::discard_comments, std::map, std::vector> sites;
        for (auto &site : sites_)
            sites[site->name().data()] = site->serialize();
        species["sites"] = sites;
    }

    return species;
}
// This method populates the object's members with values read from a 'species.name' TOML node
void Species::deserialize(toml::value node, CoreData &coreData)
{
    std::vector tomlAtoms = toml::find(node, "atom").as_array();
    for (auto tomlAtom : tomlAtoms)
        atoms_.emplace_back().deserialize(tomlAtom);

    if (node.contains("bond"))
    {
        std::vector tomlBonds = toml::find(node, "bond").as_array();
        for (auto tomlBond : tomlBonds)
            if (!tomlBond["i"].is_uninitialized() && !tomlBond["j"].is_uninitialized())
                bonds_.emplace_back(&atoms_[tomlBond["i"].as_integer() - 1], &atoms_[tomlBond["j"].as_integer() - 1])
                    .deserialize(tomlBond, coreData);
    }

    if (node.contains("angle"))
    {
        std::vector tomlAngles = toml::find(node, "angle").as_array();
        for (auto tomlAngle : tomlAngles)
            if (!tomlAngle["i"].is_uninitialized() && !tomlAngle["j"].is_uninitialized() && !tomlAngle["k"].is_uninitialized())
                angles_
                    .emplace_back(&atoms_[tomlAngle["i"].as_integer() - 1], &atoms_[tomlAngle["j"].as_integer() - 1],
                                  &atoms_[tomlAngle["k"].as_integer() - 1])
                    .deserialize(tomlAngle, coreData);
    }

    if (node.contains("improper"))
    {
        std::vector tomlImpropers = toml::find(node, "improper").as_array();
        for (auto tomlImproper : tomlImpropers)
            if (!tomlImproper["i"].is_uninitialized() && !tomlImproper["j"].is_uninitialized() &&
                !tomlImproper["k"].is_uninitialized() && !tomlImproper["l"].is_uninitialized())
                impropers_
                    .emplace_back(&atoms_[tomlImproper["i"].as_integer() - 1], &atoms_[tomlImproper["j"].as_integer() - 1],
                                  &atoms_[tomlImproper["k"].as_integer() - 1], &atoms_[tomlImproper["l"].as_integer() - 1])
                    .deserialize(tomlImproper, coreData);
    }

    if (node.contains("torsion"))
    {
        std::vector tomlTorsions = toml::find(node, "torsion").as_array();
        for (auto tomlTorsion : tomlTorsions)
            if (!tomlTorsion["i"].is_uninitialized() && !tomlTorsion["j"].is_uninitialized() &&
                !tomlTorsion["k"].is_uninitialized() && !tomlTorsion["l"].is_uninitialized())
                torsions_
                    .emplace_back(&atoms_[tomlTorsion["i"].as_integer() - 1], &atoms_[tomlTorsion["j"].as_integer() - 1],
                                  &atoms_[tomlTorsion["k"].as_integer() - 1], &atoms_[tomlTorsion["l"].as_integer() - 1])
                    .deserialize(tomlTorsion, coreData);
    }

    if (node.contains("isotopologues"))
    {
        toml::value tomlIsotopologues = toml::find(node, "isotopologues");
        for (auto &[name, data] : tomlIsotopologues.as_table())
            isotopologues_.emplace_back(std::make_unique<Isotopologue>(name))->deserialize(data, coreData);
    }

    if (node.contains("sites"))
    {
        toml::value tomlSites = toml::find(node, "sites");
        for (auto &[name, data] : tomlSites.as_table())
            sites_.emplace_back(std::make_unique<SpeciesSite>(this, name))->deserialize(data);
    }
}
