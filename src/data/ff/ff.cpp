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
 * Intramolecular Term Data
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
std::optional<ForcefieldImproperTerm> Forcefield::getImproperTerm(const ForcefieldAtomType &i, const ForcefieldAtomType &j,
                                                                  const ForcefieldAtomType &k,
                                                                  const ForcefieldAtomType &l) const
{
    return termMatch_(improperTerms_, i, j, k, l);
}

// Assign / generate bond term parameters
bool Forcefield::assignBondTermParameters(
    SpeciesBond &bond, const std::vector<std::reference_wrapper<const ForcefieldAtomType>> &ffAtomTypes) const
{
    // Default implementation - search term lists in the forcefield
    OptionalReferenceWrapper<const ForcefieldAtomType> ffi =
        ffAtomTypes.size() == 2 ? ffAtomTypes[0] : determineAtomType(*bond.i());
    OptionalReferenceWrapper<const ForcefieldAtomType> ffj =
        ffAtomTypes.size() == 2 ? ffAtomTypes[1] : determineAtomType(*bond.j());
    if (!(ffi && !ffj))
        return false;

    auto optTerm = getBondTerm(*ffi, *ffj);
    if (!optTerm)
        return Messenger::error("Failed to locate parameters for bond {}-{} ({}-{}).\n", bond.i()->index(), bond.j()->index(),
                                ffi->get().equivalentName(), ffj->get().equivalentName());

    const ForcefieldBondTerm &term = *optTerm;
    bond.setInteractionFormAndParameters(term.form(), term.parameters());

    return true;
}

// Assign / generate angle term parameters
bool Forcefield::assignAngleTermParameters(
    SpeciesAngle &angle, const std::vector<std::reference_wrapper<const ForcefieldAtomType>> &ffAtomTypes) const
{
    // Default implementation - search term lists in the forcefield
    OptionalReferenceWrapper<const ForcefieldAtomType> ffi =
        ffAtomTypes.size() == 3 ? ffAtomTypes[0] : determineAtomType(*angle.i());
    OptionalReferenceWrapper<const ForcefieldAtomType> ffj =
        ffAtomTypes.size() == 3 ? ffAtomTypes[1] : determineAtomType(*angle.j());
    OptionalReferenceWrapper<const ForcefieldAtomType> ffk =
        ffAtomTypes.size() == 3 ? ffAtomTypes[2] : determineAtomType(*angle.k());
    if (!(ffi && ffj && ffk))
        return false;

    auto optTerm = getAngleTerm(*ffi, *ffj, *ffk);
    if (!optTerm)
        return Messenger::error("Failed to locate parameters for angle {}-{}-{} ({}-{}-{}).\n", angle.i()->index(),
                                angle.j()->index(), angle.k()->index(), ffi->get().equivalentName(),
                                ffj->get().equivalentName(), ffk->get().equivalentName());

    const ForcefieldAngleTerm &term = *optTerm;
    angle.setInteractionFormAndParameters(term.form(), term.parameters());

    return true;
}

// Assign / generate torsion term parameters
bool Forcefield::assignTorsionTermParameters(
    SpeciesTorsion &torsion, const std::vector<std::reference_wrapper<const ForcefieldAtomType>> &ffAtomTypes) const
{
    // Default implementation - search term lists in the forcefield
    OptionalReferenceWrapper<const ForcefieldAtomType> ffi =
        ffAtomTypes.size() == 4 ? ffAtomTypes[0] : determineAtomType(*torsion.i());
    OptionalReferenceWrapper<const ForcefieldAtomType> ffj =
        ffAtomTypes.size() == 4 ? ffAtomTypes[1] : determineAtomType(*torsion.j());
    OptionalReferenceWrapper<const ForcefieldAtomType> ffk =
        ffAtomTypes.size() == 4 ? ffAtomTypes[2] : determineAtomType(*torsion.k());
    OptionalReferenceWrapper<const ForcefieldAtomType> ffl =
        ffAtomTypes.size() == 4 ? ffAtomTypes[2] : determineAtomType(*torsion.k());
    if (!(ffi && ffj && ffk && ffl))
        return false;

    auto optTerm = getTorsionTerm(*ffi, *ffj, *ffk, *ffl);
    if (!optTerm)
        return Messenger::error("Failed to locate parameters for torsion {}-{}-{}-{} ({}-{}-{}-{}).\n", torsion.i()->index(),
                                torsion.j()->index(), torsion.k()->index(), torsion.l()->index(), ffi->get().equivalentName(),
                                ffj->get().equivalentName(), ffk->get().equivalentName(), ffl->get().equivalentName());

    const ForcefieldTorsionTerm &term = *optTerm;
    torsion.setInteractionFormAndParameters(term.form(), term.parameters());

    return true;
}

// Return available pair potential overrides
std::vector<PairPotentialOverride> Forcefield::pairPotentialOverrides() { return {}; }