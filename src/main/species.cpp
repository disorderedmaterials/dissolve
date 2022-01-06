// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "classes/species.h"
#include "base/sysfunc.h"
#include "classes/atomtype.h"
#include "main/dissolve.h"
#include <cstring>

// Add a new Species to the list
Species *Dissolve::addSpecies()
{
    Species *newSpecies = coreData_.addSpecies();

    return newSpecies;
}

// Remove the specified Species from the list
void Dissolve::removeSpecies(Species *sp)
{
    if (!sp)
        return;

    // Remove references to the Species itself
    removeReferencesTo(sp);

    // Now safe to remove the Species
    coreData_.removeSpecies(sp);
}

// Return number of defined Species
int Dissolve::nSpecies() const { return coreData_.nSpecies(); }

// Return Species list
std::vector<std::unique_ptr<Species>> &Dissolve::species() { return coreData_.species(); }

// Search for Species by name
Species *Dissolve::findSpecies(std::string_view name) const { return coreData_.findSpecies(name); }

// Copy AtomType, creating a new one if necessary
void Dissolve::copyAtomType(const SpeciesAtom *sourceAtom, SpeciesAtom *destAtom)
{
    // Check for no AtomType being set
    if (!sourceAtom->atomType())
    {
        destAtom->setAtomType(nullptr);
        return;
    }

    // Search for the existing atom's AtomType by name, and create it if it doesn't exist
    auto at = findAtomType(sourceAtom->atomType()->name());
    if (!at)
    {
        at = addAtomType(sourceAtom->Z());
        at->setName(sourceAtom->atomType()->name());
        at->interactionPotential() = sourceAtom->atomType()->interactionPotential();
    }

    destAtom->setAtomType(at);
}

// Copy intramolecular interaction parameters, adding master term if necessary
void Dissolve::copySpeciesBond(const SpeciesBond &source, SpeciesBond &dest)
{
    if (source.masterTerm())
    {
        auto master = coreData_.getMasterBond(source.masterTerm()->name());
        if (!master)
        {
            master = coreData_.addMasterBond(source.masterTerm()->name());
            master->get().setInteractionFormAndParameters(source.interactionForm(), source.interactionParameters());
        }
        dest.setMasterTerm(&master->get());
    }
    else
        dest.setInteractionFormAndParameters(source.interactionForm(), source.interactionParameters());
}
void Dissolve::copySpeciesAngle(const SpeciesAngle &source, SpeciesAngle &dest)
{
    if (source.masterTerm())
    {
        auto master = coreData_.getMasterAngle(source.masterTerm()->name());
        if (!master)
        {
            master = coreData_.addMasterAngle(source.masterTerm()->name());
            master->get().setInteractionFormAndParameters(source.interactionForm(), source.interactionParameters());
        }
        dest.setMasterTerm(&master->get());
    }
    else
        dest.setInteractionFormAndParameters(source.interactionForm(), source.interactionParameters());
}
void Dissolve::copySpeciesTorsion(const SpeciesTorsion &source, SpeciesTorsion &dest)
{
    if (source.masterTerm())
    {
        auto master = coreData_.getMasterTorsion(source.masterTerm()->name());
        if (!master)
        {
            master = coreData_.addMasterTorsion(source.masterTerm()->name());
            master->get().setInteractionFormAndParameters(source.interactionForm(), source.interactionParameters());
        }
        dest.setMasterTerm(&master->get());
    }
    else
        dest.setInteractionFormAndParameters(source.interactionForm(), source.interactionParameters());
}
void Dissolve::copySpeciesImproper(const SpeciesImproper &source, SpeciesImproper &dest)
{
    if (source.masterTerm())
    {
        auto master = coreData_.getMasterImproper(source.masterTerm()->name());
        if (!master)
        {
            master = coreData_.addMasterImproper(source.masterTerm()->name());
            master->get().setInteractionFormAndParameters(source.interactionForm(), source.interactionParameters());
        }
        dest.setMasterTerm(&master->get());
    }
    else
        dest.setInteractionFormAndParameters(source.interactionForm(), source.interactionParameters());
}

// Copy Species from supplied instance
Species *Dissolve::copySpecies(const Species *species)
{
    // Create our new Species
    Species *newSpecies = addSpecies();
    newSpecies->setName(coreData_.uniqueSpeciesName(species->name()));

    // Copy Box definition if one exists
    if (species->box()->type() != Box::BoxType::NonPeriodic)
        newSpecies->createBox(species->box()->axisLengths(), species->box()->axisAngles());

    // Duplicate atoms
    for (auto &i : species->atoms())
    {
        // Create the Atom in our new Species
        auto id = newSpecies->addAtom(i.Z(), i.r(), i.charge());
        if (i.isSelected())
            newSpecies->selectAtom(id);

        // Search for the existing atom's AtomType by name, and create it if it doesn't exist
        copyAtomType(&i, &newSpecies->atom(id));
    }

    // Duplicate bonds
    for (const auto &bond : species->bonds())
    {
        // Create the bond in the new Species
        auto &newBond = newSpecies->addBond(bond.indexI(), bond.indexJ());
        copySpeciesBond(bond, newBond);
    }

    // Duplicate angles
    for (const auto &angle : species->angles())
    {
        // Create the angle in the new Species
        auto &newAngle = newSpecies->addAngle(angle.indexI(), angle.indexJ(), angle.indexK());
        copySpeciesAngle(angle, newAngle);
    }

    // Duplicate torsions
    for (const auto &torsion : species->torsions())
    {
        // Create the torsion in the new Species
        auto &newTorsion = newSpecies->addTorsion(torsion.indexI(), torsion.indexJ(), torsion.indexK(), torsion.indexL());
        copySpeciesTorsion(torsion, newTorsion);
    }

    // Duplicate impropers
    for (auto &t : species->impropers())
    {
        // Create the improper in the new Species
        auto &newImproper = newSpecies->addImproper(t.indexI(), t.indexJ(), t.indexK(), t.indexL());
        copySpeciesImproper(t, newImproper);
    }

    return newSpecies;
}
