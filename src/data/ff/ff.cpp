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
    static EnumOptionsList ShortRangeTypeOptions =
        EnumOptionsList() << EnumOption(Forcefield::UndefinedType, "Undefined")
                          << EnumOption(Forcefield::NoInteractionType, "None") << EnumOption(Forcefield::LennardJonesType, "LJ")
                          << EnumOption(Forcefield::LennardJonesGeometricType, "LJGeometric");

    static EnumOptions<Forcefield::ShortRangeType> options("ShortRangeType", ShortRangeTypeOptions);

    return options;
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
Forcefield::determineAtomType(SpeciesAtom *i,
                              const std::vector<std::vector<std::reference_wrapper<const ForcefieldAtomType>>> &atomTypes)
{
    Messenger::printVerbose("Determining atom type for atom {} ({})\n", i->userIndex(), Elements::symbol(i->Z()));

    // Go through AtomTypes defined for the target's element, and check NETA scores
    auto bestScore = -1;
    OptionalReferenceWrapper<const ForcefieldAtomType> bestType;
    for (const auto &typeRef : atomTypes[i->Z()])
    {
        // Get the scoring for this type
        auto &type = typeRef.get();
        auto score = type.neta().score(i);
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
        Messenger::printVerbose("  Best type for atom {} is {} ({}) with a score of {}.\n", i->userIndex(),
                                bestType->get().index(), bestType->get().name(), bestScore);

    return bestType;
}

// Determine and return atom type for specified SpeciesAtom
OptionalReferenceWrapper<const ForcefieldAtomType> Forcefield::determineAtomType(SpeciesAtom *i) const
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
                                 SpeciesImproper::ImproperFunction form, const std::vector<double> parameters)
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

// Assign suitable AtomType to the supplied atom
bool Forcefield::assignAtomType(SpeciesAtom *i, CoreData &coreData) const
{
    auto optRef = determineAtomType(i);
    if (!optRef)
        return false;
    const ForcefieldAtomType &assignedType = *optRef;

    // Check if an AtomType of the same name already exists - if it does, just use that one
    auto at = coreData.findAtomType(assignedType.name());
    if (!at)
    {
        at = coreData.addAtomType(i->Z());
        at->setName(assignedType.name());
        Messenger::print("Adding AtomType '{}' for atom {} ({}).\n", at->name(), i->userIndex(), Elements::symbol(i->Z()));
    }
    else
        Messenger::print("Re-using AtomType '{}' for atom {} ({}).\n", at->name(), i->userIndex(), Elements::symbol(i->Z()));

    // Copy parameters from the Forcefield's atom type
    at->setShortRangeParameters(assignedType.parameters());
    at->setShortRangeType(shortRangeType());
    at->setCharge(assignedType.charge());

    // Set type in the SpeciesAtom
    i->setAtomType(at);

    return true;
}

// Assign suitable atom types to the supplied Species, returning the number of failures
int Forcefield::assignAtomTypes(Species *sp, CoreData &coreData, AtomTypeAssignmentStrategy strategy) const
{
    Messenger::print("Assigning atomtypes to species '{}' from forcefield '{}'...\n", sp->name(), name());

    // Loop over Species atoms
    auto nFailed = 0;
    for (auto *i = sp->atoms().first(); i != nullptr; i = i->next())
    {
        // Obey the supplied strategy:
        // -- Don't reassign a type to this atom if one already exists (strategy == Forcefield::TypeMissing)
        if ((strategy == Forcefield::TypeMissing) && i->atomType())
            continue;

        // -- Don't assign a type unless the atom is selected (strategy == Forcefield::TypeSelection)
        if ((strategy == Forcefield::TypeSelection) && (!i->isSelected()))
            continue;

        if (!assignAtomType(i, coreData))
        {
            Messenger::error("No matching forcefield type for atom {} ({}).\n", i->userIndex(), Elements::symbol(i->Z()));
            ++nFailed;
        }
    }

    if (nFailed)
        Messenger::error("Failed to assign atom {} to {} {}.\n", (nFailed == 1 ? "type" : "types"), nFailed,
                         (nFailed == 1 ? "atom" : "atoms"));

    return nFailed;
}

// Assign intramolecular parameters to the supplied Species
bool Forcefield::assignIntramolecular(Species *sp, int flags) const
{
    /*
     * Default implementation - search through term lists for suitable ones to apply, based on ForcefieldAtomType names.
     */

    Messenger::print("Assigning intramolecular terms to species '{}' from forcefield '{}'...\n", sp->name(), name());

    auto determineTypes = flags & Forcefield::DetermineTypesFlag;
    auto selectionOnly = flags & Forcefield::SelectionOnlyFlag;

    // Assign bond terms
    for (auto &bond : sp->bonds())
    {
        auto *i = bond.i();
        auto *j = bond.j();

        if (selectionOnly && (!bond.isSelected()))
            continue;

        auto optTypeI = determineTypes ? determineAtomType(i) : atomTypeByName(i->atomType()->name(), i->Z());
        if (!optTypeI)
            return Messenger::error("Couldn't locate object for atom type named '{}'.\n", i->atomType()->name());
        const ForcefieldAtomType &typeI = *optTypeI;
        auto optTypeJ = determineTypes ? determineAtomType(j) : atomTypeByName(j->atomType()->name(), j->Z());
        if (!optTypeJ)
            return Messenger::error("Couldn't locate object for atom type named '{}'.\n", j->atomType()->name());
        const ForcefieldAtomType &typeJ = *optTypeJ;

        auto optTerm = getBondTerm(typeI, typeJ);
        if (!optTerm)
            return Messenger::error("Failed to locate parameters for bond {}-{} ({}-{}).\n", i->userIndex(), j->userIndex(),
                                    typeI.equivalentName(), typeJ.equivalentName());

        const ForcefieldBondTerm &term = *optTerm;
        bond.setForm(term.form());
        bond.setParameters(term.parameters());
    }

    // Generate angle parameters
    for (auto &angle : sp->angles())
    {
        auto *i = angle.i();
        auto *j = angle.j();
        auto *k = angle.k();

        if (selectionOnly && (!angle.isSelected()))
            continue;

        auto optTypeI = determineTypes ? determineAtomType(i) : atomTypeByName(i->atomType()->name(), i->Z());
        if (!optTypeI)
            return Messenger::error("Couldn't locate object for atom type named '{}'.\n", i->atomType()->name());
        const ForcefieldAtomType &typeI = *optTypeI;
        auto optTypeJ = determineTypes ? determineAtomType(j) : atomTypeByName(j->atomType()->name(), j->Z());
        if (!optTypeJ)
            return Messenger::error("Couldn't locate object for atom type named '{}'.\n", j->atomType()->name());
        const ForcefieldAtomType &typeJ = *optTypeJ;
        auto optTypeK = determineTypes ? determineAtomType(k) : atomTypeByName(k->atomType()->name(), k->Z());
        if (!optTypeK)
            return Messenger::error("Couldn't locate object for atom type named '{}'.\n", k->atomType()->name());
        const ForcefieldAtomType &typeK = *optTypeK;

        auto optTerm = getAngleTerm(typeI, typeJ, typeK);
        if (!optTerm)
            return Messenger::error("Failed to locate parameters for angle {}-{}-{} ({}-{}-{}).\n", i->userIndex(),
                                    j->userIndex(), k->userIndex(), typeI.equivalentName(), typeJ.equivalentName(),
                                    typeK.equivalentName());

        const ForcefieldAngleTerm &term = *optTerm;
        angle.setForm(term.form());
        angle.setParameters(term.parameters());
    }

    // Generate torsion parameters
    for (auto &torsion : sp->torsions())
    {
        SpeciesAtom *i = torsion.i();
        SpeciesAtom *j = torsion.j();
        SpeciesAtom *k = torsion.k();
        SpeciesAtom *l = torsion.l();

        if (selectionOnly && (!torsion.isSelected()))
            continue;

        auto optTypeI = determineTypes ? determineAtomType(i) : atomTypeByName(i->atomType()->name(), i->Z());
        if (!optTypeI)
            return Messenger::error("Couldn't locate object for atom type named '{}'.\n", i->atomType()->name());
        const ForcefieldAtomType &typeI = *optTypeI;
        auto optTypeJ = determineTypes ? determineAtomType(j) : atomTypeByName(j->atomType()->name(), j->Z());
        if (!optTypeJ)
            return Messenger::error("Couldn't locate object for atom type named '{}'.\n", j->atomType()->name());
        const ForcefieldAtomType &typeJ = *optTypeJ;
        auto optTypeK = determineTypes ? determineAtomType(k) : atomTypeByName(k->atomType()->name(), k->Z());
        if (!optTypeK)
            return Messenger::error("Couldn't locate object for atom type named '{}'.\n", k->atomType()->name());
        const ForcefieldAtomType &typeK = *optTypeK;
        auto optTypeL = determineTypes ? determineAtomType(l) : atomTypeByName(l->atomType()->name(), l->Z());
        if (!optTypeL)
            return Messenger::error("Couldn't locate object for atom type named '{}'.\n", l->atomType()->name());
        const ForcefieldAtomType &typeL = *optTypeL;

        auto optTerm = getTorsionTerm(typeI, typeJ, typeK, typeL);
        if (!optTerm)
            return Messenger::error("Failed to locate parameters for torsion {}-{}-{}-{} ({}-{}-{}-{}).\n", i->userIndex(),
                                    j->userIndex(), k->userIndex(), l->userIndex(), typeI.equivalentName(),
                                    typeJ.equivalentName(), typeK.equivalentName(), typeL.equivalentName());

        const ForcefieldTorsionTerm &term = *optTerm;
        torsion.setForm(term.form());
        torsion.setParameters(term.parameters());
    }

    // Generate improper terms
    if ((flags & Forcefield::GenerateImpropersFlag) && (improperTerms_.size() > 0))
    {
        // Loop over potential improper sites in the Species and see if any match terms in the forcefield
        ListIterator<SpeciesAtom> atomIterator(sp->atoms());
        while (SpeciesAtom *i = atomIterator.iterate())
        {
            // If we have less than three bonds to the central atom 'i', can continue now
            if (i->nBonds() < 3)
                continue;

            auto optTypeI = determineTypes ? determineAtomType(i) : atomTypeByName(i->atomType()->name(), i->Z());
            if (!optTypeI)
                return Messenger::error("Couldn't locate object for atom type named '{}'.\n", i->atomType()->name());
            const ForcefieldAtomType &typeI = *optTypeI;
            if (selectionOnly && (!i->isSelected()))
                continue;

            // Loop over combinations of bonds to the central atom
            for (auto indexJ = 0; indexJ < i->nBonds() - 2; ++indexJ)
            {
                // Get SpeciesAtom 'j'
                auto *j = i->bond(indexJ).partner(i);
                auto optTypeJ = determineTypes ? determineAtomType(j) : atomTypeByName(j->atomType()->name(), j->Z());
                if (!optTypeJ)
                    return Messenger::error("Couldn't locate object for atom type named '{}'.\n", j->atomType()->name());
                const ForcefieldAtomType &typeJ = *optTypeJ;
                if (selectionOnly && (!j->isSelected()))
                    continue;

                for (auto indexK = indexJ + 1; indexK < i->nBonds() - 1; ++indexK)
                {
                    // Get SpeciesAtom 'k'
                    auto *k = i->bond(indexK).partner(i);
                    auto optTypeK = determineTypes ? determineAtomType(k) : atomTypeByName(k->atomType()->name(), k->Z());
                    if (!optTypeK)
                        return Messenger::error("Couldn't locate object for atom type named '{}'.\n", k->atomType()->name());
                    const ForcefieldAtomType &typeK = *optTypeK;
                    if (selectionOnly && (!k->isSelected()))
                        continue;

                    for (auto indexL = indexK + 1; indexL < i->nBonds(); ++indexL)
                    {
                        // Get SpeciesAtom 'l'
                        auto *l = i->bond(indexL).partner(i);
                        auto optTypeL = determineTypes ? determineAtomType(l) : atomTypeByName(l->atomType()->name(), l->Z());
                        if (!optTypeL)
                            return Messenger::error("Couldn't locate object for atom type named '{}'.\n",
                                                    l->atomType()->name());
                        const ForcefieldAtomType &typeL = *optTypeL;
                        if (selectionOnly && (!l->isSelected()))
                            continue;

                        auto optTerm = getImproperTerm(typeI, typeJ, typeK, typeL);
                        if (optTerm)
                        {
                            const ForcefieldImproperTerm &term = *optTerm;
                            // Check to see if the Species already has an improper definition - if
                            // not create one
                            auto improper = sp->getImproper(i, j, k, l);
                            if (!improper)
                                improper = sp->addImproper(i, j, k, l);

                            Messenger::print("Added improper between atoms {}-{}-{}-{} ({}-{}-{}-{}).\n", i->userIndex(),
                                             j->userIndex(), k->userIndex(), l->userIndex(), typeI.equivalentName(),
                                             typeJ.equivalentName(), typeK.equivalentName(), typeL.equivalentName());

                            improper->get().setForm(term.form());
                            improper->get().setParameters(term.parameters());
                        }
                    }
                }
            }
        }
    }

    return true;
}

/*
 * Atom Environment Helpers
 */

// Calculate and return the geometry of the specified SpeciesAtom
Forcefield::AtomGeometry Forcefield::geometryOfAtom(SpeciesAtom *i) const
{
    AtomGeometry result = nAtomGeometries;
    double angle, largest;
    SpeciesAtom *h, *j;

    // Work based on the number of bound atoms
    switch (i->nBonds())
    {
        // 'Simple' cases first
        case (0):
            result = Forcefield::UnboundGeometry;
            break;
        case (1):
            result = Forcefield::TerminalGeometry;
            break;
        case (5):
            result = Forcefield::TrigonalBipyramidalGeometry;
            break;
        case (6):
            result = Forcefield::OctahedralGeometry;
            break;
        // For the remaining types, take averages of bond angles about the atom
        case (2):
            h = i->bond(0).partner(i);
            j = i->bond(1).partner(i);
            angle = NonPeriodicBox::literalAngleInDegrees(h->r(), i->r(), j->r());
            if (angle > 150.0)
                result = Forcefield::LinearGeometry;
            else
                result = Forcefield::TetrahedralGeometry;
            break;
        case (3):
            h = i->bond(0).partner(i);
            j = i->bond(1).partner(i);
            angle = NonPeriodicBox::literalAngleInDegrees(h->r(), i->r(), j->r());
            largest = angle;
            j = i->bond(2).partner(i);
            angle = NonPeriodicBox::literalAngleInDegrees(h->r(), i->r(), j->r());
            if (angle > largest)
                largest = angle;
            h = i->bond(1).partner(i);
            angle = NonPeriodicBox::literalAngleInDegrees(h->r(), i->r(), j->r());
            if (angle > largest)
                largest = angle;
            if (largest > 150.0)
                result = Forcefield::TShapeGeometry;
            else if ((largest > 115.0) && (largest < 125.0))
                result = Forcefield::TrigonalPlanarGeometry;
            else
                result = Forcefield::TetrahedralGeometry;
            break;
        case (4):
            // Two possibilities - tetrahedral or square planar. Tetrahedral will have an
            // average of all angles of ~ 109.5, for square planar (1/6) * (4*90 + 2*180) = 120
            angle = 0.0;
            for (auto n = 0; n < i->nBonds(); ++n)
            {
                h = i->bond(n).partner(i);
                for (auto m = n + 1; m < i->nBonds(); ++m)
                {
                    j = i->bond(m).partner(i);
                    angle += NonPeriodicBox::literalAngleInDegrees(h->r(), i->r(), j->r());
                }
            }
            angle /= 6.0;
            if ((angle > 100.0) && (angle < 115.0))
                result = Forcefield::TetrahedralGeometry;
            else
                result = Forcefield::SquarePlanarGeometry;
            break;
    }

    return result;
}

// Return whether the specified SpeciesAtom exists in the specified geometry
bool Forcefield::isAtomGeometry(SpeciesAtom *i, AtomGeometry geom) const { return (geometryOfAtom(i) == geom); }

// Return whether supplied bonding pattern around the SpeciesAtom matches *exactly*
bool Forcefield::isBondPattern(const SpeciesAtom *i, const int nSingle, const int nDouble, const int nTriple,
                               const int nQuadruple, const int nAromatic) const
{
    auto actualNSingle = 0, actualNDouble = 0, actualNTriple = 0, actualNQuadruple = 0, actualNAromatic = 0;
    for (const SpeciesBond &bond : i->bonds())
    {
        switch (bond.bondType())
        {
            case (SpeciesBond::SingleBond):
                if (nSingle == actualNSingle)
                    return false;
                ++actualNSingle;
                break;
            case (SpeciesBond::DoubleBond):
                if (nDouble == actualNDouble)
                    return false;
                ++actualNDouble;
                break;
            case (SpeciesBond::TripleBond):
                if (nTriple == actualNTriple)
                    return false;
                ++actualNTriple;
                break;
            case (SpeciesBond::QuadrupleBond):
                if (nQuadruple == actualNQuadruple)
                    return false;
                ++actualNQuadruple;
                break;
            case (SpeciesBond::AromaticBond):
                if (nAromatic == actualNAromatic)
                    return false;
                ++actualNAromatic;
                break;
            default:
                Messenger::error("Bond has unassigned bond type.\n");
                return false;
        }
    }

    // Check sums - a supplied value of -1 means 'any number'
    if ((nSingle != -1) && (nSingle != actualNSingle))
        return false;
    if ((nDouble != -1) && (nDouble != actualNDouble))
        return false;
    if ((nTriple != -1) && (nTriple != actualNTriple))
        return false;
    if ((nQuadruple != -1) && (nQuadruple != actualNQuadruple))
        return false;
    if ((nAromatic != -1) && (nAromatic != actualNAromatic))
        return false;

    return true;
}

// Return whether the specified atom is bound to a specific element (and count thereof)
bool Forcefield::isBoundTo(const SpeciesAtom *i, Elements::Element Z, const int count, bool allowMoreThanCount) const
{
    auto found = std::count_if(i->bonds().begin(), i->bonds().end(),
                               [i, Z](const SpeciesBond &bond) { return bond.partner(i)->Z() == Z; });

    return (found < count ? false : (found == count ? true : allowMoreThanCount));
}

// Guess and return oxidation state for the specified SpeciesAtom
int Forcefield::guessOxidationState(const SpeciesAtom *i) const
{
    /*
     * Approximate the OS of the supplied atom by considering its local environment.
     * We won't consider the whole molecule, and will assume the following rules in addition to the standard:
     *   - A singly-bound Oxygen is considered to be -1 (which effectively includes it's 'R' group
     *   - An R-group is considered to be +1
     */
    auto osBound = 0;

    // Keep track of the number of bound elements that are the same as our own, as a crude check for elemental environments
    // (OS == 0)
    auto nSameElement = 0;

    const auto &bonds = i->bonds();
    for (const SpeciesBond &bond : bonds)
    {
        auto Z = bond.partner(i)->Z();
        switch (Z)
        {
            // Group 1A - Alkali earth metals (includes Hydrogen)
            case (Elements::H):
            case (Elements::Li):
            case (Elements::Na):
            case (Elements::K):
            case (Elements::Rb):
            case (Elements::Cs):
            case (Elements::Fr):
                osBound += 1;
                break;
            // Group 2A - Alkaline earth metals
            case (Elements::Be):
            case (Elements::Mg):
            case (Elements::Ca):
            case (Elements::Sr):
            case (Elements::Ba):
            case (Elements::Ra):
                osBound += 1;
                break;
            // Oxygen
            case (Elements::O):
                if (bond.bondType() == SpeciesBond::DoubleBond)
                    osBound -= 2;
                else
                    osBound -= 1;
                break;
            // Halogens (F, Cl, Br, I)
            case (Elements::F):
            case (Elements::Cl):
            case (Elements::Br):
            case (Elements::I):
                osBound -= 1;
                break;
            default:
                break;
        }

        // Check for same element
        if (Z == i->Z())
            ++nSameElement;
    }

    // Return the negative of the bound OS, or zero if we were only bound to the same element as ourselves
    return (nSameElement == i->nBonds() ? 0 : -osBound);
}
