// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "base/processpool.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "modules/import/import.h"
#include <memory>

// Clear contents of Configuration, leaving other definitions intact
void Configuration::empty()
{
    molecules_.clear();
    atoms_.clear();
    usedAtomTypes_.clear();
    if (box_ != nullptr)
        delete box_;
    box_ = new CubicBox(1.0);
    cells_.clear();

    // Clear used species
    usedSpecies_.clear();

    ++contentsVersion_;
}

// Return specified used type
std::shared_ptr<AtomType> Configuration::usedAtomType(int index) { return usedAtomTypes_.atomType(index); }

// Return specified used type data
AtomTypeData &Configuration::usedAtomTypeData(int index) { return usedAtomTypes_[index]; }

// Return first AtomTypeData for this Configuration
const AtomTypeData &Configuration::usedAtomTypes() const { return usedAtomTypes_.first(); }

// Return AtomTypeList for this Configuration
const AtomTypeList &Configuration::usedAtomTypesList() const { return usedAtomTypes_; }

// Return number of atom types used in this Configuration
int Configuration::nUsedAtomTypes() const { return usedAtomTypes_.nItems(); }

// Add Species to list of those used by the Configuration, setting/adding the population specified
SpeciesInfo *Configuration::addUsedSpecies(Species *sp, int population)
{
    // Check if we have an existing info for this Species
    SpeciesInfo *spInfo = usedSpeciesInfo(sp);
    if (!spInfo)
    {
        spInfo = usedSpecies_.add();
        spInfo->setSpecies(sp);
    }

    // Increase the population
    spInfo->addPopulation(population);

    return spInfo;
}

// Return SpeciesInfo for specified Species
SpeciesInfo *Configuration::usedSpeciesInfo(Species *sp)
{
    for (auto *spInfo = usedSpecies_.first(); spInfo != nullptr; spInfo = spInfo->next())
        if (spInfo->species() == sp)
            return spInfo;

    return nullptr;
}

// Return list of SpeciesInfo for the Configuration
List<SpeciesInfo> &Configuration::usedSpecies() { return usedSpecies_; }

// Return if the specified Species is present in the usedSpecies list
bool Configuration::hasUsedSpecies(Species *sp)
{
    for (auto *spInfo = usedSpecies_.first(); spInfo != nullptr; spInfo = spInfo->next())
        if (spInfo->species() == sp)
            return true;

    return false;
}

// Return the total atomic mass present in the Configuration
double Configuration::atomicMass() const
{
    double mass = 0.0;

    // Get total molar mass in configuration
    ListIterator<SpeciesInfo> speciesIterator(usedSpecies_);
    while (SpeciesInfo *spInfo = speciesIterator.iterate())
        mass += spInfo->species()->mass() * spInfo->population();

    // Convert to absolute mass
    return mass / AVOGADRO;
}

// Return the atomic density of the Configuration
double Configuration::atomicDensity() const { return nAtoms() / box_->volume(); }

// Return the chemical density (g/cm3) of the Configuration
double Configuration::chemicalDensity() const { return atomicMass() / (box_->volume() / 1.0E24); }

// Return version of current contents
int Configuration::contentsVersion() const { return contentsVersion_; }

// Increment version of current contents
void Configuration::incrementContentsVersion() { ++contentsVersion_; }

// Add Molecule to Configuration based on the supplied Species
std::shared_ptr<Molecule> Configuration::addMolecule(Species *sp, CoordinateSet *sourceCoordinates)
{
    // Create the new Molecule object and set its Species pointer
    std::shared_ptr<Molecule> newMolecule = std::make_shared<Molecule>();
    newMolecule->setArrayIndex(molecules_.size());
    molecules_.push_back(newMolecule);
    newMolecule->setSpecies(sp);

    // Update the relevant SpeciesInfo population
    addUsedSpecies(sp, 1);

    // Add Atoms from Species to the Molecule, using either species coordinates or those from the source CoordinateSet
    SpeciesAtom *spi = sp->firstAtom();
    if (sourceCoordinates)
        for (auto n = 0; n < sp->nAtoms(); ++n, spi = spi->next())
            addAtom(spi, newMolecule, sourceCoordinates->r(n));
    else
        for (auto n = 0; n < sp->nAtoms(); ++n, spi = spi->next())
            addAtom(spi, newMolecule, spi->r());

    return newMolecule;
}

// Return number of Molecules in Configuration
int Configuration::nMolecules() const { return molecules_.size(); }

// Return array of Molecules
std::deque<std::shared_ptr<Molecule>> &Configuration::molecules() { return molecules_; }

// Return nth Molecule
std::shared_ptr<Molecule> Configuration::molecule(int n) { return molecules_[n]; }

// Add new Atom to Configuration, with Molecule parent specified
std::shared_ptr<Atom> Configuration::addAtom(const SpeciesAtom *sourceAtom, std::shared_ptr<Molecule> molecule, Vec3<double> r)
{
    // Create new Atom object and set its source pointer
    auto newAtom = std::make_shared<Atom>();
    newAtom->setArrayIndex(atoms_.size());
    atoms_.push_back(newAtom);
    newAtom->setSpeciesAtom(sourceAtom);

    // Register the Atom in the specified Molecule (this will also set the Molecule pointer in the Atom)
    molecule->addAtom(newAtom);

    // Set the position
    newAtom->setCoordinates(r);

    // Update our typeIndex (non-isotopic) and set local and master type indices
    AtomTypeData &atd = usedAtomTypes_.add(sourceAtom->atomType(), 1);
    newAtom->setLocalTypeIndex(atd.listIndex());
    newAtom->setMasterTypeIndex(sourceAtom->atomType()->index());

    return newAtom;
}

// Return number of Atoms in Configuration
int Configuration::nAtoms() const { return atoms_.size(); }

// Return Atom array
std::vector<std::shared_ptr<Atom>> &Configuration::atoms() { return atoms_; }

// Return Atom array (const)
const std::vector<std::shared_ptr<Atom>> &Configuration::constAtoms() const { return atoms_; }

// Return nth atom
std::shared_ptr<Atom> Configuration::atom(int n)
{
#ifdef CHECKS
    if ((n < 0) || (n >= atoms_.size()))
    {
        Messenger::print("OUT_OF_RANGE - Atom index {} passed to Configuration::atom() is out of range (nAtoms = {}).\n", n,
                         atoms_.size());
        return nullptr;
    }
#endif
    return atoms_[n];
}

// Scale geometric centres of molecules within box
void Configuration::scaleMoleculeCentres(double factor)
{
    Vec3<double> oldCog, newCog, newPos;
    for (auto mol : molecules_)
    {
        // First, work out the centre of geometry of the Molecule, and fold it into the Box
        oldCog = box()->fold(mol->centreOfGeometry(box()));

        // Scale centre of geometry by supplied factor
        newCog = oldCog * factor;

        // Loop over Atoms in Molecule, setting new coordinates as we go
        for (auto m = 0; m < mol->nAtoms(); ++m)
        {
            // Get Atom pointer
            auto i = mol->atom(m);

            // Calculate and set new position
            newPos = newCog + box()->minimumVector(i->r(), oldCog);
            i->setCoordinates(newPos);
        }
    }
}
