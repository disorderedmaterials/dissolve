// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "data/ff/ff.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "classes/coredata.h"
#include "classes/species.h"
#include "classes/speciesatom.h"
#include "classes/speciesbond.h"
#include "data/ff/angleterm.h"
#include "data/ff/atomtype.h"
#include "data/ff/bondterm.h"
#include "data/ff/improperterm.h"
#include "data/ff/torsionterm.h"

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
    atomTypesByElementPrivate_.resize(Elements::nElements);

    for (auto &atomType : atomTypes_)
        atomTypesByElementPrivate_[atomType.Z()].push_back(atomType);

    return true;
}

/*
 * Definition
 */

// Return enum options for ShortRangeType
EnumOptions<Forcefield::ShortRangeType> Forcefield::shortRangeTypes()
{
    return EnumOptions<Forcefield::ShortRangeType>("ShortRangeType", {{Forcefield::UndefinedType, "Undefined"},
                                                                      {Forcefield::NoInteractionType, "None"},
                                                                      {Forcefield::LennardJonesType, "LJ"},
                                                                      {Forcefield::LennardJonesGeometricType, "LJGeometric"}});
}

/*
 * Atom Type Data
 */

// Add new atom type with its own parameters
void Forcefield::addAtomType(Elements::Element Z, int index, std::string_view name, std::string_view netaDefinition,
                             std::string_view description, double q, const std::vector<double> &parameters)
{
    atomTypes_.emplace_back(Z, index, name, netaDefinition, description, q, parameters);
}

// Add new atom type referencing existing parameters by name
void Forcefield::addAtomType(Elements::Element Z, int index, std::string_view name, std::string_view netaDefinition,
                             std::string_view description, double q, std::string_view parameterReference)
{
    auto refParams = shortRangeParameters(parameterReference);
    if (!refParams)
        Messenger::error("Reference parameters named '{}' are not defined in the forcefield '{}'.\n", parameterReference,
                         this->name());
    atomTypes_.emplace_back(Z, index, name, netaDefinition, description, q, *refParams, parameterReference);
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

// Determine and return atom type for specified SpeciesAtom from supplied Array of types
OptionalReferenceWrapper<const ForcefieldAtomType>
Forcefield::determineAtomType(const SpeciesAtom &i,
                              const std::vector<std::vector<std::reference_wrapper<const ForcefieldAtomType>>> &atomTypes)
{
    Messenger::printVerbose("Determining atom type for atom {} ({})\n", i.userIndex(), Elements::symbol(i.Z()));

    // Go through AtomTypes defined for the target's element, and check NETA scores
    auto bestScore = -1;
    OptionalReferenceWrapper<const ForcefieldAtomType> bestType;
    for (const auto &typeRef : atomTypes[i.Z()])
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
        Messenger::printVerbose("  Best type for atom {} is {} ({}) with a score of {}.\n", i.userIndex(),
                                bestType->get().index(), bestType->get().name(), bestScore);

    return bestType;
}

// Determine and return atom type for specified SpeciesAtom
OptionalReferenceWrapper<const ForcefieldAtomType> Forcefield::determineAtomType(const SpeciesAtom &i) const
{
    return determineAtomType(i, atomTypesByElementPrivate_);
}

// Ass short-range parameters
void Forcefield::addParameters(std::string_view name, const std::vector<double> &parameters)
{
    shortRangeParameters_.emplace_back(name, parameters);
}

// Create NETA definitions for all atom types from stored defs
bool Forcefield::createNETADefinitions()
{
    auto nFailed = std::count_if(atomTypes_.begin(), atomTypes_.end(), [this](auto &atomType) {
        auto success = atomType.createNETA(this);
        if (!success)
            Messenger::print("Failed to parse NETA definition '{}' for atom type '{}'.", atomType.neta().definitionString(),
                             atomType.name());
        return !success;
    });

    if (nFailed > 0)
        Messenger::error("Failed to create {} NETA {} for the forcefield '{}'.\n", nFailed,
                         nFailed == 1 ? "definition" : "definitions", name());

    return (nFailed == 0);
}

// Return named short-range parameters (if they exist)
std::optional<std::vector<double>> Forcefield::shortRangeParameters(std::string_view name) const
{
    auto it = std::find_if(shortRangeParameters_.begin(), shortRangeParameters_.end(),
                           [&name](const auto &params) { return DissolveSys::sameString(name, params.first); });
    if (it != shortRangeParameters_.end())
        return it->second;

    return std::nullopt;
}

// Return the named ForcefieldAtomType (if it exists)
OptionalReferenceWrapper<const ForcefieldAtomType> Forcefield::atomTypeByName(std::string_view name,
                                                                              Elements::Element onlyZ) const
{
    auto endZ = (onlyZ != Elements::Unknown ? onlyZ : Elements::nElements - 1);
    for (int Z = onlyZ; Z <= endZ; ++Z)
    {
        // Go through types associated to the Element
        auto it = std::find_if(atomTypesByElementPrivate_[Z].cbegin(), atomTypesByElementPrivate_[Z].cend(),
                               [&name](const auto &type) { return DissolveSys::sameString(type.get().name(), name); });
        if (it != atomTypesByElementPrivate_[Z].end())
            return OptionalReferenceWrapper<const ForcefieldAtomType>(*it);
    }

    return {};
}

// Return the ForcefieldAtomType with specified id (if it exists)
OptionalReferenceWrapper<const ForcefieldAtomType> Forcefield::atomTypeById(int id, Elements::Element onlyZ) const
{
    auto endZ = (onlyZ != Elements::Unknown ? onlyZ : Elements::nElements - 1);
    for (int Z = onlyZ; Z <= endZ; ++Z)
    {
        // Go through types associated to the Element
        auto it = std::find_if(atomTypesByElementPrivate_[Z].cbegin(), atomTypesByElementPrivate_[Z].cend(),
                               [&id](const auto &type) { return type.get().index() == id; });
        if (it != atomTypesByElementPrivate_[Z].end())
            return OptionalReferenceWrapper<const ForcefieldAtomType>(*it);
    }

    return {};
}

/*
 * Term Data
 */

// Add bond term
void Forcefield::addBondTerm(std::string_view typeI, std::string_view typeJ, SpeciesBond::BondFunction form,
                             const std::vector<double> parameters)
{
    bondTerms_.emplace_back(typeI, typeJ, form, parameters);
}

// Add angle term
void Forcefield::addAngleTerm(std::string_view typeI, std::string_view typeJ, std::string_view typeK,
                              SpeciesAngle::AngleFunction form, const std::vector<double> parameters)
{
    angleTerms_.emplace_back(typeI, typeJ, typeK, form, parameters);
}

// Add torsion term
void Forcefield::addTorsionTerm(std::string_view typeI, std::string_view typeJ, std::string_view typeK, std::string_view typeL,
                                SpeciesTorsion::TorsionFunction form, const std::vector<double> parameters)
{
    torsionTerms_.emplace_back(typeI, typeJ, typeK, typeL, form, parameters);
}

// Add improper term
void Forcefield::addImproperTerm(std::string_view typeI, std::string_view typeJ, std::string_view typeK, std::string_view typeL,
                                 SpeciesTorsion::TorsionFunction form, const std::vector<double> parameters)
{
    improperTerms_.emplace_back(typeI, typeJ, typeK, typeL, form, parameters);
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
Forcefield::getAtomTypes(const std::vector<const SpeciesAtom *> &atoms, bool determineType) const
{
    std::vector<std::reference_wrapper<const ForcefieldAtomType>> types;
    for (const auto *i : atoms)
    {
        auto optType = determineType ? determineAtomType(*i) : atomTypeByName(i->atomType()->name(), i->Z());
        if (!optType)
        {
            Messenger::error("Couldn't find or assign type for atom {}.\n", i->userIndex());
            return {};
        }
        types.emplace_back(*optType);
    }

    return types;
}

// Assign suitable AtomType to the supplied atom
bool Forcefield::assignAtomType(SpeciesAtom &i, CoreData &coreData, bool setSpeciesAtomCharges) const
{
    auto optRef = determineAtomType(i);
    if (!optRef)
        return false;
    const ForcefieldAtomType &assignedType = *optRef;

    // Check if an AtomType of the same name already exists - if it does, just use that one
    auto at = coreData.findAtomType(assignedType.name());
    if (!at)
    {
        at = coreData.addAtomType(i.Z());
        at->setName(assignedType.name());
        Messenger::print("Adding AtomType '{}' for atom {} ({}).\n", at->name(), i.userIndex(), Elements::symbol(i.Z()));
    }
    else
        Messenger::print("Re-using AtomType '{}' for atom {} ({}).\n", at->name(), i.userIndex(), Elements::symbol(i.Z()));

    // Copy parameters from the Forcefield's atom type
    at->setShortRangeParameters(assignedType.parameters());
    at->setShortRangeType(shortRangeType());
    at->setCharge(assignedType.charge());

    // Set the charge on the SpeciesAtom if requested
    if (setSpeciesAtomCharges)
        i.setCharge(assignedType.charge());

    // Set type in the SpeciesAtom
    i.setAtomType(at);

    return true;
}

// Assign suitable atom types to the supplied Species, returning the number of failures
int Forcefield::assignAtomTypes(Species *sp, CoreData &coreData, AtomTypeAssignmentStrategy strategy,
                                bool setSpeciesAtomCharges) const
{
    Messenger::print("Assigning atomtypes to species '{}' from forcefield '{}'...\n", sp->name(), name());

    // Loop over Species atoms
    auto nFailed = 0;
    for (auto &i : sp->atoms())
    {
        // Obey the supplied strategy:
        // -- Don't reassign a type to this atom if one already exists (strategy == Forcefield::TypeMissing)
        if ((strategy == Forcefield::TypeMissing) && i.atomType())
            continue;

        // -- Don't assign a type unless the atom is selected (strategy == Forcefield::TypeSelection)
        if ((strategy == Forcefield::TypeSelection) && (!i.isSelected()))
            continue;

        if (!assignAtomType(i, coreData, setSpeciesAtomCharges))
        {
            Messenger::error("No matching forcefield type for atom {} ({}).\n", i.userIndex(), Elements::symbol(i.Z()));
            ++nFailed;
        }
    }

    if (nFailed)
        Messenger::error("Failed to assign atom {} to {} {}.\n", (nFailed == 1 ? "type" : "types"), nFailed,
                         (nFailed == 1 ? "atom" : "atoms"));

    return nFailed;
}

// Assign / generate bond term parameters
bool Forcefield::assignBondTermParameters(SpeciesBond &bond, bool determineTypes) const
{
    // Default implementation - search term lists in the forcefield
    auto *i = bond.i();
    auto *j = bond.j();

    auto atomTypes = getAtomTypes({i, j}, determineTypes);
    if (atomTypes.size() != 2)
        return false;

    auto optTerm = getBondTerm(atomTypes[0], atomTypes[1]);
    if (!optTerm)
        return Messenger::error("Failed to locate parameters for bond {}-{} ({}-{}).\n", i->userIndex(), j->userIndex(),
                                atomTypes[0].get().equivalentName(), atomTypes[1].get().equivalentName());
    const ForcefieldBondTerm &term = *optTerm;

    bond.setForm(term.form());
    bond.setParameters(term.parameters());

    return true;
}

// Assign / generate angle term parameters
bool Forcefield::assignAngleTermParameters(SpeciesAngle &angle, bool determineTypes) const
{
    // Default implementation - search term lists in the forcefield
    auto *i = angle.i();
    auto *j = angle.j();
    auto *k = angle.k();

    auto atomTypes = getAtomTypes({i, j, k}, determineTypes);
    if (atomTypes.size() != 3)
        return false;

    auto optTerm = getAngleTerm(atomTypes[0], atomTypes[1], atomTypes[2]);
    if (!optTerm)
        return Messenger::error("Failed to locate parameters for angle {}-{}-{} ({}-{}-{}).\n", i->userIndex(), j->userIndex(),
                                k->userIndex(), atomTypes[0].get().equivalentName(), atomTypes[1].get().equivalentName(),
                                atomTypes[2].get().equivalentName());
    const ForcefieldAngleTerm &term = *optTerm;

    angle.setForm(term.form());
    angle.setParameters(term.parameters());

    return true;
}

// Assign / generate torsion term parameters
bool Forcefield::assignTorsionTermParameters(SpeciesTorsion &torsion, bool determineTypes) const
{
    // Default implementation - search term lists in the forcefield
    SpeciesAtom *i = torsion.i();
    SpeciesAtom *j = torsion.j();
    SpeciesAtom *k = torsion.k();
    SpeciesAtom *l = torsion.l();

    auto atomTypes = getAtomTypes({i, j, k, l}, determineTypes);
    if (atomTypes.size() != 4)
        return false;

    auto optTerm = getTorsionTerm(atomTypes[0], atomTypes[1], atomTypes[2], atomTypes[3]);
    if (!optTerm)
        return Messenger::error("Failed to locate parameters for torsion {}-{}-{}-{} ({}-{}-{}-{}).\n", i->userIndex(),
                                j->userIndex(), k->userIndex(), l->userIndex(), atomTypes[0].get().equivalentName(),
                                atomTypes[1].get().equivalentName(), atomTypes[2].get().equivalentName(),
                                atomTypes[3].get().equivalentName());
    const ForcefieldTorsionTerm &term = *optTerm;

    torsion.setForm(term.form());
    torsion.setParameters(term.parameters());

    return true;
}

// Assign / generate improper term parameters
bool Forcefield::assignImproperTermParameters(ForcefieldImproperTerm &improper, SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k,
                                              SpeciesAtom *l, bool determineTypes) const
{
    auto atomTypes = getAtomTypes({i, j, k, l}, determineTypes);
    if (atomTypes.size() != 4)
        return false;

    auto optTerm = getImproperTerm(atomTypes[0], atomTypes[1], atomTypes[2], atomTypes[3]);
    if (!optTerm)
        improper = {atomTypes[0].get().equivalentName(), atomTypes[1].get().equivalentName(),
                    atomTypes[2].get().equivalentName(), atomTypes[3].get().equivalentName(), SpeciesTorsion::NoForm};
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
        if (selectionOnly && (!bond.isSelected()))
            continue;

        if (!assignBondTermParameters(bond, determineTypes))
            return false;
    }

    // Generate angle parameters
    for (auto &angle : sp->angles())
    {
        if (selectionOnly && (!angle.isSelected()))
            continue;

        if (!assignAngleTermParameters(angle, determineTypes))
            return false;
    }

    // Generate torsion parameters
    for (auto &torsion : sp->torsions())
    {
        if (selectionOnly && (!torsion.isSelected()))
            continue;

        if (!assignTorsionTermParameters(torsion, determineTypes))
            return false;
    }

    // Generate improper terms
    if (flags & Forcefield::GenerateImpropersFlag)
    {
        ForcefieldImproperTerm improperTerm;

        // Loop over potential improper sites in the Species and see if any match terms in the forcefield
        for (auto &i : sp->atoms())
        {
            // If we have less than three bonds to the central atom 'i', can continue now
            if (i.nBonds() < 3)
                continue;

            if (selectionOnly && (!i.isSelected()))
                continue;

            // Loop over combinations of bonds to the central atom
            for (auto indexJ = 0; indexJ < i.nBonds() - 2; ++indexJ)
            {
                // Get SpeciesAtom 'j'
                auto *j = i.bond(indexJ).partner(&i);

                if (selectionOnly && (!j->isSelected()))
                    continue;

                for (auto indexK = indexJ + 1; indexK < i.nBonds() - 1; ++indexK)
                {
                    // Get SpeciesAtom 'k'
                    auto *k = i.bond(indexK).partner(&i);

                    if (selectionOnly && (!k->isSelected()))
                        continue;

                    for (auto indexL = indexK + 1; indexL < i.nBonds(); ++indexL)
                    {
                        // Get SpeciesAtom 'l'
                        auto *l = i.bond(indexL).partner(&i);

                        if (selectionOnly && (!l->isSelected()))
                            continue;

                        // Try to assign / generate an improper term (which may legitimately not exist)
                        if (!assignImproperTermParameters(improperTerm, &i, j, k, l, determineTypes))
                            return false;

                        if (improperTerm.form() == SpeciesTorsion::NoForm)
                            continue;

                        // If an improper term already exists in the species, overwrite its parameters. Otherwise, create a new
                        // one.
                        auto optImproper = sp->getImproper(&i, j, k, l);
                        if (!optImproper)
                            optImproper = sp->addImproper(&i, j, k, l);
                        SpeciesImproper &improper = *optImproper;

                        improper.setForm(improperTerm.form());
                        improper.setParameters(improperTerm.parameters());
                    }
                }
            }
        }
    }

    return true;
}
