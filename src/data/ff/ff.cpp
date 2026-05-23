// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "data/ff/ff.h"
#include "classes/atomType.h"
#include "classes/box.h"
#include "classes/coreData.h"
#include "classes/species.h"
#include "classes/speciesAtom.h"
#include "classes/speciesBond.h"
#include "data/ff/angleTerm.h"
#include "data/ff/atomType.h"
#include "data/ff/bondTerm.h"
#include "data/ff/improperTerm.h"
#include "data/ff/torsionTerm.h"

/*
 * Set Up
 */

Forcefield::Forcefield(std::vector<ForcefieldAtomType> &atoms, std::vector<ForcefieldBondTerm> &bonds,
                       std::vector<ForcefieldAngleTerm> &angles, std::vector<ForcefieldTorsionTerm> &torsions,
                       std::vector<ForcefieldImproperTerm> &impropers)
    : atomTypes_(atoms), bondTerms_(bonds), angleTerms_(angles), torsionTerms_(torsions), improperTerms_(impropers)
{
}

// Prepare forcefield for use
bool Forcefield::prepare()
{
    if (!setUp())
        return false;

    // Create reference vectors of atom types by element
    for (auto &atomType : atomTypes_)
        atomTypesByElement_[atomType.Z()].push_back(atomType);

    return true;
}

/*
 * Atom Type Data
 */

// Add new atom type with its own parameters
void Forcefield::addAtomType(Elements::Element Z, int index, std::string_view name, std::string_view netaDefinition,
                             std::string_view description, double q, std::string_view parameterString)
{
    // The passed parameterString might be a name referencing some predefined named set, or be a set of name=value pairs
    auto refParams = shortRangeParameters(parameterString);
    if (refParams)
        atomTypes_.emplace_back(Z, index, name, netaDefinition, description, q, shortRangeForm(), *refParams, parameterString);
    else
        atomTypes_.emplace_back(Z, index, name, netaDefinition, description, q, shortRangeForm(), parameterString);
}
void Forcefield::addAtomType(Elements::Element Z, int index, std::string_view name, std::string_view netaDefinition,
                             std::string_view description, double q, const std::vector<double> &parameters)
{
    atomTypes_.emplace_back(Z, index, name, netaDefinition, description, q, parameters);
}

// Copy existing atom type
bool Forcefield::copyAtomType(OptionalReferenceWrapper<const ForcefieldAtomType> sourceTypeRef, std::string_view newTypeName,
                              std::string_view netaDefinition, std::string_view equivalentName)
{
    // Check for valid reference
    if (!sourceTypeRef)
        return Messenger::error("Can't copy atom type with new name '{}' into forcefield '{}' as no sourceType was provided.\n",
                                newTypeName, name());

    atomTypes_.emplace_back(*sourceTypeRef, newTypeName, netaDefinition, equivalentName);

    return true;
}

// Determine and return atom type for specified SpeciesAtom
OptionalReferenceWrapper<const ForcefieldAtomType> Forcefield::determineAtomType(const SpeciesAtom &i) const
{
    Messenger::printVerbose("Determining atom type for atom {} ({})\n", i.index(), Elements::symbol(i.Z()));

    // Go through AtomTypes defined for the target's element, and check NETA scores
    auto bestScore = -1;
    OptionalReferenceWrapper<const ForcefieldAtomType> bestType;
    if (atomTypesByElement_.contains(i.Z()))
        for (const auto &typeRef : atomTypesByElement_.at(i.Z()))
        {
            // Get the scoring for this type
            auto &type = typeRef.get();
            auto score = type.neta().score(&i);
            Messenger::printVerbose("  -- score for type index {} ({}) is {}.\n", type.index(), type.name(), score);
            if (score > bestScore)
            {
                bestScore = score;
                bestType = type;
            }
        }

    if (bestScore == -1)
        Messenger::printVerbose("  -- no suitable type found.");
    else
        Messenger::printVerbose("  Best type for atom {} is {} ({}) with a score of {}.\n", i.index(), bestType->get().index(),
                                bestType->get().name(), bestScore);

    return bestType;
}

// Add short-range parameters
void Forcefield::addParameters(std::string_view name, const std::string_view parameterString)
{
    shortRangeParameters_.emplace_back(name, parameterString);
}

// Create NETA definitions for all atom types from stored defs
bool Forcefield::createNETADefinitions()
{
    auto nFailed = std::count_if(atomTypes_.begin(), atomTypes_.end(),
                                 [this](auto &atomType)
                                 {
                                     auto success = atomType.createNETA(this);
                                     if (!success)
                                         Messenger::print("Failed to parse NETA definition '{}' for atom type '{}'.",
                                                          atomType.neta().definitionString(), atomType.name());
                                     return !success;
                                 });

    if (nFailed > 0)
        Messenger::error("Failed to create {} NETA {} for the forcefield '{}'.\n", nFailed,
                         nFailed == 1 ? "definition" : "definitions", name());

    return (nFailed == 0);
}

// Return named short-range parameters (if they exist)
std::optional<std::string> Forcefield::shortRangeParameters(std::string_view name) const
{
    auto it = std::find_if(shortRangeParameters_.begin(), shortRangeParameters_.end(),
                           [&name](const auto &params) { return DissolveSys::sameString(name, params.first); });
    if (it != shortRangeParameters_.end())
        return it->second;

    return std::nullopt;
}

// Return the named ForcefieldAtomType (if it exists)
OptionalReferenceWrapper<const ForcefieldAtomType> Forcefield::atomTypeByName(std::string_view name) const
{
    // Go through types associated to the Element
    auto it =
        std::ranges::find_if(atomTypes_, [&name](const auto &type) { return DissolveSys::sameString(type.name(), name); });
    if (it != atomTypes_.end())
        return OptionalReferenceWrapper<const ForcefieldAtomType>(*it);

    return {};
}

// Return the ForcefieldAtomType with specified id (if it exists)
OptionalReferenceWrapper<const ForcefieldAtomType> Forcefield::atomTypeById(int id) const
{
    // Go through types associated to the Element
    auto it = std::ranges::find_if(atomTypes_, [&id](const auto &type) { return type.index() == id; });
    if (it != atomTypes_.end())
        return OptionalReferenceWrapper<const ForcefieldAtomType>(*it);

    return {};
}

/*
 * Term Data
 */

// Add bond term
void Forcefield::addBondTerm(std::string_view typeI, std::string_view typeJ, BondFunctions::Form form,
                             std::string_view parameterString)
{
    bondTerms_.emplace_back(typeI, typeJ, form, parameterString);
}

// Add angle term
void Forcefield::addAngleTerm(std::string_view typeI, std::string_view typeJ, std::string_view typeK, AngleFunctions::Form form,
                              std::string_view parameterString)
{
    angleTerms_.emplace_back(typeI, typeJ, typeK, form, parameterString);
}

// Add torsion term
void Forcefield::addTorsionTerm(std::string_view typeI, std::string_view typeJ, std::string_view typeK, std::string_view typeL,
                                TorsionFunctions::Form form, std::string_view parameterString)
{
    torsionTerms_.emplace_back(typeI, typeJ, typeK, typeL, form, parameterString);
}

// Add improper term
void Forcefield::addImproperTerm(std::string_view typeI, std::string_view typeJ, std::string_view typeK, std::string_view typeL,
                                 TorsionFunctions::Form form, std::string_view parameterString)
{
    improperTerms_.emplace_back(typeI, typeJ, typeK, typeL, form, parameterString);
}

// Return bond term for the supplied atom type pair (if it exists)
OptionalReferenceWrapper<const ForcefieldBondTerm> Forcefield::getBondTerm(const ForcefieldAtomType &i,
                                                                           const ForcefieldAtomType &j) const
{
    return termMatch_(bondTerms_, i, j);
}

// Return angle term for the supplied atom type trio (if it exists)
OptionalReferenceWrapper<const ForcefieldAngleTerm>
Forcefield::getAngleTerm(const ForcefieldAtomType &i, const ForcefieldAtomType &j, const ForcefieldAtomType &k) const
{
    return termMatch_(angleTerms_, i, j, k);
}

// Return torsion term for the supplied atom type quartet (if it exists)
OptionalReferenceWrapper<const ForcefieldTorsionTerm> Forcefield::getTorsionTerm(const ForcefieldAtomType &i,
                                                                                 const ForcefieldAtomType &j,
                                                                                 const ForcefieldAtomType &k,
                                                                                 const ForcefieldAtomType &l) const
{
    return termMatch_(torsionTerms_, i, j, k, l);
}

// Return improper term for the supplied atom type quartet (if it exists)
OptionalReferenceWrapper<const ForcefieldImproperTerm> Forcefield::getImproperTerm(const ForcefieldAtomType &i,
                                                                                   const ForcefieldAtomType &j,
                                                                                   const ForcefieldAtomType &k,
                                                                                   const ForcefieldAtomType &l) const
{
    return termMatch_(improperTerms_, i, j, k, l);
}

/*
 * Term Assignment
 */

// Find / determine atom type(s) for the specified atom(s)
std::vector<std::reference_wrapper<const ForcefieldAtomType>>
Forcefield::getAtomTypes(const std::vector<const SpeciesAtom *> &atoms) const
{
    std::vector<std::reference_wrapper<const ForcefieldAtomType>> types;
    for (const auto *i : atoms)
    {
        auto optType = determineAtomType(*i);
        if (!optType)
        {
            Messenger::error("Couldn't find or assign type for atom {}.\n", i->index());
            return {};
        }
        types.emplace_back(*optType);
    }

    return types;
}

// Assign suitable AtomType to the supplied atom
bool Forcefield::assignAtomType(SpeciesAtom &i) const
{
    auto optRef = determineAtomType(i);
    if (!optRef)
        return false;
    const ForcefieldAtomType &assignedType = *optRef;

    assignAtomType(assignedType, i);

    return true;
}

// Assign suitable atom types to the supplied Species, returning the number of failures
std::vector<int> Forcefield::assignAtomTypes(Species *sp, AtomTypeAssignmentStrategy strategy) const
{
    Messenger::print("Assigning atomtypes to species '{}' from forcefield '{}'...\n", sp->name(), name());

    // Loop over Species atoms
    std::vector<int> failedElements;
    for (auto &i : sp->atoms())
    {
        // Obey the supplied strategy:
        // -- Don't reassign a type to this atom if one already exists (strategy == Forcefield::TypeMissing)
        if ((strategy == Forcefield::TypeMissing) && i.atomType())
            continue;

        // -- Don't assign a type unless the atom is selected (strategy == Forcefield::TypeSelection)
        // if ((strategy == Forcefield::TypeSelection) && (!i.isSelected()))
        // continue;
        // TODO DISSOLVE2

        if (!assignAtomType(i))
        {
            Messenger::error("No matching forcefield type for atom {} ({}).\n", i.index(), Elements::symbol(i.Z()));
            failedElements.push_back(i.index());
        }
    }

    if (!failedElements.empty())
        Messenger::error("Failed to assign atom {} to {} {}.\n", (failedElements.size() == 1 ? "type" : "types"),
                         failedElements.size(), (failedElements.size() == 1 ? "atom" : "atoms"));

    return failedElements;
}

// Assign specific AtomType to the supplied atom
void Forcefield::assignAtomType(const ForcefieldAtomType &ffa, SpeciesAtom &i) const
{

    // Check if an AtomType of the same name already exists - if it does, just use that one
    auto at = i.parent()->findAtomType(ffa.name());
    if (!at)
    {
        at = i.parent()->addAtomType(i.Z(), ffa.name());
        Messenger::print("Adding AtomType '{}' for atom {} ({}).\n", at->name(), i.index(), Elements::symbol(i.Z()));
    }
    else
        Messenger::print("Re-using AtomType '{}' for atom {} ({}).\n", at->name(), i.index(), Elements::symbol(i.Z()));

    // Set type in the SpeciesAtom
    i.setAtomType(at);

    // Copy parameters from the assigned atom type - we take only the required number for the specified shortRangeType.
    // This is to avoid copying e.g. generator data (stored after the short range parameters) and causing issues elsewhere
    std::vector<double> params;
    params.insert(params.begin(), ffa.parameters().begin(),
                  ffa.parameters().begin() + ShortRangeFunctions::forms().minArgs(shortRangeForm()).value_or(0));
    at->interactionPotential().setFormAndParameters(shortRangeForm(), params);
    at->setCharge(ffa.charge());
}

// Assign / generate bond term parameters
bool Forcefield::assignBondTermParameters(const Species *parent, SpeciesBond &bond) const
{
    // Default implementation - search term lists in the forcefield
    auto *i = bond.i();
    auto *j = bond.j();

    auto atomTypes = getAtomTypes({i, j});
    if (atomTypes.size() != 2)
        return false;

    auto optTerm = getBondTerm(atomTypes[0], atomTypes[1]);
    if (!optTerm)
        return Messenger::error("Failed to locate parameters for bond {}-{} ({}-{}).\n", i->index(), j->index(),
                                atomTypes[0].get().equivalentName(), atomTypes[1].get().equivalentName());
    const ForcefieldBondTerm &term = *optTerm;

    bond.setInteractionFormAndParameters(term.form(), term.parameters());

    return true;
}

// Assign / generate angle term parameters
bool Forcefield::assignAngleTermParameters(const Species *parent, SpeciesAngle &angle) const
{
    // Default implementation - search term lists in the forcefield
    auto *i = angle.i();
    auto *j = angle.j();
    auto *k = angle.k();

    auto atomTypes = getAtomTypes({i, j, k});
    if (atomTypes.size() != 3)
        return false;

    auto optTerm = getAngleTerm(atomTypes[0], atomTypes[1], atomTypes[2]);
    if (!optTerm)
        return Messenger::error("Failed to locate parameters for angle {}-{}-{} ({}-{}-{}).\n", i->index(), j->index(),
                                k->index(), atomTypes[0].get().equivalentName(), atomTypes[1].get().equivalentName(),
                                atomTypes[2].get().equivalentName());
    const ForcefieldAngleTerm &term = *optTerm;

    angle.setInteractionFormAndParameters(term.form(), term.parameters());

    return true;
}

// Assign / generate torsion term parameters
bool Forcefield::assignTorsionTermParameters(const Species *parent, SpeciesTorsion &torsion) const
{
    // Default implementation - search term lists in the forcefield
    SpeciesAtom *i = torsion.i();
    SpeciesAtom *j = torsion.j();
    SpeciesAtom *k = torsion.k();
    SpeciesAtom *l = torsion.l();

    auto atomTypes = getAtomTypes({i, j, k, l});
    if (atomTypes.size() != 4)
        return false;

    auto optTerm = getTorsionTerm(atomTypes[0], atomTypes[1], atomTypes[2], atomTypes[3]);
    if (!optTerm)
        return Messenger::error("Failed to locate parameters for torsion {}-{}-{}-{} ({}-{}-{}-{}).\n", i->index(), j->index(),
                                k->index(), l->index(), atomTypes[0].get().equivalentName(),
                                atomTypes[1].get().equivalentName(), atomTypes[2].get().equivalentName(),
                                atomTypes[3].get().equivalentName());
    const ForcefieldTorsionTerm &term = *optTerm;

    torsion.setInteractionFormAndParameters(term.form(), term.parameters());

    return true;
}

// Assign / generate improper term parameters
bool Forcefield::assignImproperTermParameters(ForcefieldImproperTerm &improper, SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k,
                                              SpeciesAtom *l) const
{
    auto atomTypes = getAtomTypes({i, j, k, l});
    if (atomTypes.size() != 4)
        return false;

    auto optTerm = getImproperTerm(atomTypes[0], atomTypes[1], atomTypes[2], atomTypes[3]);
    if (!optTerm)
        improper = {atomTypes[0].get().equivalentName(), atomTypes[1].get().equivalentName(),
                    atomTypes[2].get().equivalentName(), atomTypes[3].get().equivalentName(), TorsionFunctions::Form::None};
    else
        improper = *optTerm;

    return true;
}

// Assign intramolecular parameters to the supplied Species
bool Forcefield::assignIntramolecular(Species *sp, int flags) const
{
    Messenger::print("Assigning intramolecular terms to species '{}' from forcefield '{}'...\n", sp->name(), name());

    auto determineTypes = flags & Forcefield::DetermineTypesFlag;
    auto selectionOnly = flags & Forcefield::SelectionOnlyFlag;

    // Assign bond terms
    for (auto &bond : sp->bonds())
    {
        // if (selectionOnly && (!bond.isSelected()))
        // continue;

        if (!assignBondTermParameters(sp, bond))
            return false;
    }

    // Generate angle parameters
    for (auto &angle : sp->angles())
    {
        // if (selectionOnly && (!angle.isSelected()))
        // continue;

        if (!assignAngleTermParameters(sp, angle))
            return false;
    }

    // Generate torsion parameters
    for (auto &torsion : sp->torsions())
    {
        // if (selectionOnly && (!torsion.isSelected()))
        // continue;

        if (!assignTorsionTermParameters(sp, torsion))
            return false;
    }

    // Generate improper terms
    if (flags & Forcefield::GenerateImpropersFlag)
    {
        ForcefieldImproperTerm improperTerm;

        // Loop over potential improper sites in the Species and see if any match terms in the forcefield
        for (auto &i : sp->atoms())
        {
            // If we don't have exactly three bonds to the central atom 'i', move on
            if (i.bonds().size() != 3)
                continue;

            // if (selectionOnly && (!i.isSelected()))
            // continue;

            // Get SpeciesAtom 'j'
            auto *j = i.bonds().front()->partner(&i);

            // if (selectionOnly && (!j->isSelected()))
            // continue;

            for (auto indexK = 1; indexK < 2; ++indexK)
            {
                // Get SpeciesAtom 'k'
                auto *k = i.bonds()[indexK]->partner(&i);

                // if (selectionOnly && (!k->isSelected()))
                // continue;

                for (auto indexL = indexK + 1; indexL < 3; ++indexL)
                {
                    // Get SpeciesAtom 'l'
                    auto *l = i.bonds()[indexL]->partner(&i);

                    // if (selectionOnly && (!l->isSelected()))
                    // continue;

                    // Try to assign / generate an improper term (which may legitimately not exist)
                    if (!assignImproperTermParameters(improperTerm, &i, j, k, l))
                        return false;

                    if (improperTerm.form() == TorsionFunctions::Form::None)
                        continue;

                    // If an improper term already exists in the species, overwrite its parameters. Otherwise, create a new
                    // one.
                    // auto optImproper = sp->getImproper(i, j, k, l);
                    // if (!optImproper)
                    // optImproper = sp->addImproper(&i, j, k, l);
                    // SpeciesImproper &improper = *optImproper;

                    // improper.setInteractionFormAndParameters(improperTerm.form(), improperTerm.parameters());
                    // TODO DISSOLVE2
                }
            }
        }
    }

    return true;
}

// Return available pair potential overrides
std::vector<PairPotentialOverride> Forcefield::pairPotentialOverrides() { return {}; }