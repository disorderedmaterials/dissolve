// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/species.h"
#include "classes/atomType.h"
#include "data/ff/ff.h"
#include "data/ff/library.h"
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

// Copy basic information (atoms and intramolecular terms)
void Species::copyBasic(const Species *source, bool copyAtomTypes)
{
    clear();

    name_ = source->name_;

    for (auto &i : source->atoms_)
        addAtom(i.Z(), i.r(), i.q(), copyAtomTypes ? i.atomType() : nullptr);

    for (auto &bond : source->bonds_)
        addBond(bond.indexI(), bond.indexJ());
    for (auto &angle : source->angles_)
        addAngle(angle.indexI(), angle.indexJ(), angle.indexK());
    for (auto &torsion : source->torsions_)
        addTorsion(torsion.indexI(), torsion.indexJ(), torsion.indexK(), torsion.indexL());
    for (auto &improper : source->impropers_)
        addImproper(improper.indexI(), improper.indexJ(), improper.indexK(), improper.indexL());
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
                         (i.atomType() ? i.atomType()->index() : -1), i.r().x, i.r().y, i.r().z, i.q());
    }

    if (nBonds() > 0)
    {
        Messenger::print("\n  Bonds:\n");
        Messenger::print("      I     J    Form             Parameters\n");
        Messenger::print("    ---------------------------------------------------------------------------------\n");
        for (const auto &bond : bonds_)
            Messenger::print("   {:4d}  {:4d}    {}{:<12}  {}\n", bond.indexI() + 1, bond.indexJ() + 1,
                             bond.commonTerm() ? '@' : ' ', BondFunctions::forms().keyword(bond.interactionForm()),
                             bond.interactionPotential().parametersAsString());
    }

    if (nAngles() > 0)
    {
        Messenger::print("\n  Angles:\n");
        Messenger::print("      I     J     K    Form             Parameters\n");
        Messenger::print("    ---------------------------------------------------------------------------------------\n");
        for (const auto &angle : angles_)
            Messenger::print("   {:4d}  {:4d}  {:4d}    {}{:<12}  {}\n", angle.indexI() + 1, angle.indexJ() + 1,
                             angle.indexK() + 1, angle.commonTerm() ? '@' : ' ',
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
                             torsion.indexK() + 1, torsion.indexL() + 1, torsion.commonTerm() ? '@' : ' ',
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
                             improper.indexK() + 1, improper.indexL() + 1, improper.commonTerm() ? '@' : ' ',
                             TorsionFunctions::forms().keyword(improper.interactionForm()),
                             improper.interactionPotential().parametersAsString());
    }
}

/*
 * Forcefield
 */

// Apply terms from specified Forcefield
bool Species::applyForcefieldTerms(std::shared_ptr<Forcefield> ff)
{
    if (!ff)
        return Messenger::error("No forcefield supplied!\n");

    // Assign atom types to the species
    if (!ff->assignAtomTypes(this, Forcefield::TypeAll, false).empty())
        return false;

    // Assign intramolecular terms
    if (!ff->assignIntramolecular(this))
        return false;

    return true;
}

// Clear forcefield terms
void Species::clearForcefieldTerms(bool nullifyAtomTypes)
{
    if (nullifyAtomTypes)
        clearAtomTypes();

    for (auto &b : bonds_)
        b.setInteractionFormAndParameters(BondFunctions::Form::None, std::vector<double>());

    for (auto &a : angles_)
        a.setInteractionFormAndParameters(AngleFunctions::Form::None, std::vector<double>());

    for (auto &t : torsions_)
        t.setInteractionFormAndParameters(TorsionFunctions::Form::None, std::vector<double>());

    impropers_.clear();
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
    Serialisable::toMap(node, "atomTypes", [this](const std::string &name, const auto &data)
                        { atomTypes_.emplace_back(std::make_shared<AtomType>(name))->deserialise(data); });

    Serialisable::toVector(node, "atoms", [this](const SerialisedValue &atom) { atoms_.emplace_back(this).deserialise(atom); });

    Serialisable::toMap(node, "commonBonds", [this](const std::string &name, const SerialisedValue &bond)
                        { commonBonds_.emplace_back(std::make_unique<CommonBond>(name))->deserialise(bond); });
    Serialisable::toVector(
        node, "bonds",
        [this](const SerialisedValue &bond)
        {
            bonds_.emplace_back(this, &atoms_.at(toml::find<int>(bond, "i") - 1), &atoms_.at(toml::find<int>(bond, "j") - 1))
                .deserialise(bond);
        });

    Serialisable::toMap(node, "commonAngles", [this](const std::string &name, const SerialisedValue &bond)
                        { commonAngles_.emplace_back(std::make_unique<CommonAngle>(name))->deserialise(bond); });
    Serialisable::toVector(node, "angles",
                           [this](const SerialisedValue &angle)
                           {
                               angles_
                                   .emplace_back(this, &atoms_.at(toml::find<int>(angle, "i") - 1),
                                                 &atoms_.at(toml::find<int>(angle, "j") - 1),
                                                 &atoms_.at(toml::find<int>(angle, "k") - 1))
                                   .deserialise(angle);
                           });

    Serialisable::toMap(node, "commonImpropers", [this](const std::string &name, const SerialisedValue &bond)
                        { commonImpropers_.emplace_back(std::make_unique<CommonImproper>(name))->deserialise(bond); });
    Serialisable::toVector(node, "impropers",
                           [this](const SerialisedValue &improper)
                           {
                               impropers_
                                   .emplace_back(this, &atoms_.at(toml::find<int>(improper, "i") - 1),
                                                 &atoms_.at(toml::find<int>(improper, "j") - 1),
                                                 &atoms_.at(toml::find<int>(improper, "k") - 1),
                                                 &atoms_.at(toml::find<int>(improper, "l") - 1))
                                   .deserialise(improper);
                           });

    Serialisable::toMap(node, "commonTorsions", [this](const std::string &name, const SerialisedValue &bond)
                        { commonTorsions_.emplace_back(std::make_unique<CommonTorsion>(name))->deserialise(bond); });
    Serialisable::toVector(node, "torsions",
                           [this](const SerialisedValue &torsion)
                           {
                               torsions_
                                   .emplace_back(this, &atoms_.at(toml::find<int>(torsion, "i") - 1),
                                                 &atoms_.at(toml::find<int>(torsion, "j") - 1),
                                                 &atoms_.at(toml::find<int>(torsion, "k") - 1),
                                                 &atoms_.at(toml::find<int>(torsion, "l") - 1))
                                   .deserialise(torsion);
                           });

    Serialisable::toMap(node, "isotopologues", [this](const std::string &name, const SerialisedValue &iso)
                        { isotopologues_.emplace_back(std::make_unique<Isotopologue>(this, name))->deserialise(iso); });

    Serialisable::toMap(node, "sites", [this](const std::string &name, const SerialisedValue &site)
                        { sites_.emplace_back(std::make_unique<SpeciesSite>(this, name))->deserialise(site); });
}
