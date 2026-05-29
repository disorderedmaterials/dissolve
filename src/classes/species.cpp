// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/species.h"
#include "classes/atomType.h"
#include "data/ff/ff.h"
#include "data/isotopes.h"

Species::Species(std::string name) : name_(name), naturalIsotopologue_(this, "Natural"), attachedAtomListsGenerated_(false)
{
    box_ = std::make_unique<SingleImageBox>();
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

    atomTypes_.clear();
    commonBonds_.clear();
    commonAngles_.clear();
    commonTorsions_.clear();
    commonImpropers_.clear();
}

/*
 * Basic Information
 */

// Set name of the Species
void Species::setName(std::string_view name) { name_ = DissolveSys::niceName(name); }

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
            Messenger::error("Atom {} ({}) of species '{}' has no associated atom type.\n", i.index(), Elements::symbol(i.Z()),
                             name_);
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
        if ((i.bonds().size() == 0) && (atoms_.size() > 1))
        {
            Messenger::error("SpeciesAtom {} ({}) participates in no Bonds, but is part of a multi-atom Species.\n", i.index(),
                             Elements::symbol(i.Z()));
            ++nErrors;
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
                         (i.atomType() ? i.atomTypeIndex() : -1), i.r().x, i.r().y, i.r().z, i.q());
    }

    if (!bonds_.empty())
    {
        Messenger::print("\n  Bonds:\n");
        Messenger::print("      I     J    Form             Parameters\n");
        Messenger::print("    ---------------------------------------------------------------------------------\n");
        for (const auto &bond : bonds_)
            Messenger::print("   {:4d}  {:4d}    {}{:<12}  {}\n", bond.i()->index() + 1, bond.j()->index() + 1,
                             bond.commonTerm() ? '@' : ' ', BondFunctions::forms().keyword(bond.interactionForm()),
                             bond.interactionPotential().parametersAsString());
    }

    if (!angles_.empty())
    {
        Messenger::print("\n  Angles:\n");
        Messenger::print("      I     J     K    Form             Parameters\n");
        Messenger::print("    ---------------------------------------------------------------------------------------\n");
        for (const auto &angle : angles_)
            Messenger::print("   {:4d}  {:4d}  {:4d}    {}{:<12}  {}\n", angle.i()->index() + 1, angle.j()->index() + 1,
                             angle.k()->index() + 1, angle.commonTerm() ? '@' : ' ',
                             AngleFunctions::forms().keyword(angle.interactionForm()),
                             angle.interactionPotential().parametersAsString());
    }

    if (!torsions_.empty())
    {
        Messenger::print("\n  Torsions:\n");
        Messenger::print("      I     J     K     L    Form             Parameters\n");
        Messenger::print("    ---------------------------------------------------------------------------------------------\n");
        // Loop over Torsions
        for (const auto &torsion : torsions())
            Messenger::print("   {:4d}  {:4d}  {:4d}  {:4d}    {}{:<12}  {}\n", torsion.i()->index() + 1,
                             torsion.j()->index() + 1, torsion.k()->index() + 1, torsion.l()->index() + 1,
                             torsion.commonTerm() ? '@' : ' ', TorsionFunctions::forms().keyword(torsion.interactionForm()),
                             torsion.interactionPotential().parametersAsString());
    }

    if (!impropers_.empty())
    {
        Messenger::print("\n  Impropers:\n");
        Messenger::print("      I     J     K     L    Form             Parameters\n");
        Messenger::print("    ---------------------------------------------------------------------------------------------\n");
        // Loop over Impropers
        for (auto &improper : impropers())
            Messenger::print("   {:4d}  {:4d}  {:4d}  {:4d}    {}{:<12}  {}\n", improper.i()->index() + 1,
                             improper.j()->index() + 1, improper.k()->index() + 1, improper.l()->index() + 1,
                             improper.commonTerm() ? '@' : ' ', TorsionFunctions::forms().keyword(improper.interactionForm()),
                             improper.interactionPotential().parametersAsString());
    }
}

/*
 * Serialisation
 */

// Express as a serialisable value
void Species::serialise(std::string tag, SerialisedValue &target) const
{
    auto &result = target[tag];
    result["name"] = name_;

    Serialisable::fromVectorToTable(atomTypes_, "atomTypes", result);
    Serialisable::fromVector<>(atoms_, "atoms", result);
    Serialisable::fromVectorToTable<>(commonBonds_, "commonBonds", result);
    Serialisable::fromVector<>(bonds_, "bonds", result);
    Serialisable::fromVectorToTable<>(commonAngles_, "commonAngles", result);
    Serialisable::fromVector<>(angles_, "angles", result);
    Serialisable::fromVectorToTable<>(commonTorsions_, "commonTorsions", result);
    Serialisable::fromVector<>(torsions_, "torsions", result);
    Serialisable::fromVectorToTable<>(commonImpropers_, "commonImpropers", result);
    Serialisable::fromVector<>(impropers_, "impropers", result);
    Serialisable::fromVectorToTable<>(isotopologues_, "isotopologues", result);
    Serialisable::fromVectorToTable<>(sites_, "sites", result);
}

// Read values from a serialisable value
void Species::deserialise(const SerialisedValue &node)
{
    setName(toml::find<std::string>(node, "name"));

    Serialisable::toMap(node, "atomTypes",
                        [this](const std::string &name, const auto &data)
                        {
                            atomTypes_.emplace_back(std::make_shared<AtomType>(name))->deserialise(data);
                        });

    Serialisable::toVector(node, "atoms", [this](const SerialisedValue &atom) { atoms_.emplace_back(this).deserialise(atom); });

    Serialisable::toMap(node, "commonBonds", [this](const std::string &name, const SerialisedValue &bond)
                        { commonBonds_.emplace_back(std::make_unique<CommonBond>(name))->deserialise(bond); });
    Serialisable::toVector(
        node, "bonds",
        [this](const SerialisedValue &bond)
        {
            bonds_.emplace_back(this, &atoms_.at(toml::find<int>(bond, "i")), &atoms_.at(toml::find<int>(bond, "j")))
                .deserialise(bond);
        });

    Serialisable::toMap(node, "commonAngles", [this](const std::string &name, const SerialisedValue &bond)
                        { commonAngles_.emplace_back(std::make_unique<CommonAngle>(name))->deserialise(bond); });
    Serialisable::toVector(node, "angles",
                           [this](const SerialisedValue &angle)
                           {
                               angles_
                                   .emplace_back(this, &atoms_.at(toml::find<int>(angle, "i")),
                                                 &atoms_.at(toml::find<int>(angle, "j")),
                                                 &atoms_.at(toml::find<int>(angle, "k")))
                                   .deserialise(angle);
                           });

    Serialisable::toMap(node, "commonImpropers", [this](const std::string &name, const SerialisedValue &bond)
                        { commonImpropers_.emplace_back(std::make_unique<CommonImproper>(name))->deserialise(bond); });
    Serialisable::toVector(
        node, "impropers",
        [this](const SerialisedValue &improper)
        {
            impropers_
                .emplace_back(this, &atoms_.at(toml::find<int>(improper, "i")), &atoms_.at(toml::find<int>(improper, "j")),
                              &atoms_.at(toml::find<int>(improper, "k")), &atoms_.at(toml::find<int>(improper, "l")))
                .deserialise(improper);
        });

    Serialisable::toMap(node, "commonTorsions", [this](const std::string &name, const SerialisedValue &bond)
                        { commonTorsions_.emplace_back(std::make_unique<CommonTorsion>(name))->deserialise(bond); });
    Serialisable::toVector(
        node, "torsions",
        [this](const SerialisedValue &torsion)
        {
            torsions_
                .emplace_back(this, &atoms_.at(toml::find<int>(torsion, "i")), &atoms_.at(toml::find<int>(torsion, "j")),
                              &atoms_.at(toml::find<int>(torsion, "k")), &atoms_.at(toml::find<int>(torsion, "l")))
                .deserialise(torsion);
        });

    Serialisable::toMap(node, "isotopologues", [this](const std::string &name, const SerialisedValue &iso)
                        { isotopologues_.emplace_back(std::make_unique<Isotopologue>(this, name))->deserialise(iso); });

    // We must finalise the intramolecular data before we attempt to add sites as Fragment sites need the bond connectivity
    finaliseIntramolecularData(false);

    Serialisable::toMap(node, "sites", [this](const std::string &name, const SerialisedValue &site)
                        { sites_.emplace_back(std::make_unique<SpeciesSite>(this, name))->deserialise(site); });
}
