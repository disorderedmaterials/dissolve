// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/species.h"
#include "base/sysfunc.h"
#include "classes/atomtype.h"
#include "main/dissolve.h"
#include <string.h>

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
        at->setShortRangeParameters(sourceAtom->atomType()->shortRangeParameters());
        at->setShortRangeType(sourceAtom->atomType()->shortRangeType());
    }

    destAtom->setAtomType(at);
}

// Copy intramolecular interaction parameters, adding MasterIntra if necessary
void Dissolve::copySpeciesIntra(const SpeciesIntra &sourceIntra, SpeciesIntra &destIntra)
{
    // Remove any existing master parameters link from the destination object
    if (destIntra.masterParameters())
        destIntra.detachFromMasterIntra();

    // If sourceIntra references a MasterIntra, check for its presence in the supplied Dissolve reference, and create it if
    // necessary
    if (sourceIntra.masterParameters())
    {
        // Search for MasterIntra by the same name in our main Dissolve instance
        OptionalReferenceWrapper<MasterIntra> master = {};
        if (sourceIntra.type() == SpeciesIntra::InteractionType::Bond)
        {
            master = coreData_.getMasterBond(sourceIntra.masterParameters()->name());
            if (!master)
            {
                master = coreData_.addMasterBond(sourceIntra.masterParameters()->name());
                master->get().setParameters(sourceIntra.parameters());
            }
        }
        else if (sourceIntra.type() == SpeciesIntra::InteractionType::Angle)
        {
            master = coreData_.getMasterAngle(sourceIntra.masterParameters()->name());
            if (!master)
            {
                master = coreData_.addMasterAngle(sourceIntra.masterParameters()->name());
                master->get().setParameters(sourceIntra.parameters());
            }
        }
        else if (sourceIntra.type() == SpeciesIntra::InteractionType::Torsion)
        {
            master = coreData_.getMasterTorsion(sourceIntra.masterParameters()->name());
            if (!master)
            {
                master = coreData_.addMasterTorsion(sourceIntra.masterParameters()->name());
                master->get().setParameters(sourceIntra.parameters());
            }
        }
        else if (sourceIntra.type() == SpeciesIntra::InteractionType::Improper)
        {
            master = coreData_.getMasterImproper(sourceIntra.masterParameters()->name());
            if (!master)
            {
                master = coreData_.addMasterImproper(sourceIntra.masterParameters()->name());
                master->get().setParameters(sourceIntra.parameters());
            }
        }

        // Copy the form of the parameters
        master->get().setForm(sourceIntra.masterParameters()->form());

        // Set the master pointer in the interaction
        destIntra.setMasterParameters(&master->get());
    }
    else
    {
        // Just copy over form / parameters
        destIntra.setForm(sourceIntra.form());
        destIntra.setParameters(sourceIntra.parameters());
    }
}

// Copy Species from supplied instance
Species *Dissolve::copySpecies(const Species *species)
{
    // Create our new Species
    Species *newSpecies = addSpecies();
    newSpecies->setName(coreData_.uniqueSpeciesName(species->name()));

    // Turn off autoupdate of intramolecular terms, since we want an exact copy of the Species contents
    newSpecies->setAutoUpdateIntramolecularTerms(false);

    // Duplicate atoms
    for (auto &i : species->atoms())
    {
        // Create the Atom in our new Species
        SpeciesAtom &newAtom = newSpecies->addAtom(i.Z(), i.r(), i.charge());
        if (i.isSelected())
            newSpecies->selectAtom(&newAtom);

        // Search for the existing atom's AtomType by name, and create it if it doesn't exist
        copyAtomType(&i, &newAtom);
    }

    // Duplicate bonds
    for (const auto &bond : species->bonds())
    {
        // Create the bond in the new Species
        auto &newBond = newSpecies->addBond(bond.indexI(), bond.indexJ());

        // Copy interaction parameters, including MasterIntra if necessary
        copySpeciesIntra(bond, newBond);
    }

    // Duplicate angles
    for (const auto &angle : species->angles())
    {
        // Create the angle in the new Species
        auto &newAngle = newSpecies->addAngle(angle.indexI(), angle.indexJ(), angle.indexK());

        // Copy interaction parameters, including MasterIntra if necessary
        copySpeciesIntra(angle, newAngle);
    }

    // Duplicate torsions
    for (const auto &torsion : species->torsions())
    {
        // Create the torsion in the new Species
        SpeciesTorsion &newTorsion =
            newSpecies->addTorsion(torsion.indexI(), torsion.indexJ(), torsion.indexK(), torsion.indexL());

        // Copy interaction parameters, including MasterIntra if necessary
        copySpeciesIntra(torsion, newTorsion);
    }

    // Duplicate impropers
    for (auto &t : species->impropers())
    {
        // Create the improper in the new Species
        auto &newImproper = newSpecies->addImproper(t.indexI(), t.indexJ(), t.indexK(), t.indexL());

        // Copy interaction parameters, including MasterIntra if necessary
        copySpeciesIntra(t, newImproper);
    }

    return newSpecies;
}
