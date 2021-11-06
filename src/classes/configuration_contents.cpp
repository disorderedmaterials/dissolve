// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/atomtype.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include <memory>

// Clear contents of Configuration, leaving other definitions intact
void Configuration::empty()
{
    molecules_.clear();
    atoms_.clear();
    usedAtomTypes_.clear();
    box_ = std::make_unique<CubicBox>(1.0);
    cells_.clear();
    appliedSizeFactor_ = 1.0;
    speciesPopulations_.clear();

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

// Increase population of specified Species in the Configuration
void Configuration::increaseSpeciesPopulation(const Species *sp, int population)
{
    auto it = std::find_if(speciesPopulations_.begin(), speciesPopulations_.end(),
                           [sp](const auto &data) { return data.first == sp; });
    if (it == speciesPopulations_.end())
        speciesPopulations_.emplace_back(sp, population);
    else
        it->second += population;
}

// Return Species populations within the Configuration
const std::vector<std::pair<const Species *, int>> &Configuration::speciesPopulations() const { return speciesPopulations_; }

// Return if the specified Species is present in the Configuration
bool Configuration::containsSpecies(const Species *sp)
{
    return std::find_if(speciesPopulations_.begin(), speciesPopulations_.end(),
                        [sp](const auto &data) { return data.first == sp; }) != speciesPopulations_.end();
}

// Return the total atomic mass present in the Configuration
double Configuration::atomicMass() const
{
    double mass = 0.0;

    // Get total molar mass in configuration
    for (const auto &spPop : speciesPopulations_)
        mass += spPop.first->mass() * spPop.second;

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
std::shared_ptr<Molecule>
Configuration::addMolecule(const Species *sp, OptionalReferenceWrapper<const std::vector<Vec3<double>>> sourceCoordinates)
{
    // Create the new Molecule object and set its Species pointer
    std::shared_ptr<Molecule> newMolecule = std::make_shared<Molecule>();
    newMolecule->setArrayIndex(molecules_.size());
    molecules_.push_back(newMolecule);
    newMolecule->setSpecies(sp);

    // Update the relevant Species population
    increaseSpeciesPopulation(sp, 1);

    // Add Atoms from Species to the Molecule, using either species coordinates or those from the source CoordinateSet
    if (sourceCoordinates)
    {
        auto r = sourceCoordinates->get();
        for (auto n = 0; n < sp->nAtoms(); ++n)
            addAtom(&sp->atom(n), newMolecule, r[n]);
    }
    else
    {
        for (auto n = 0; n < sp->nAtoms(); ++n)
            addAtom(&sp->atom(n), newMolecule, sp->atom(n).r());
    }
    return newMolecule;
}

// Return number of Molecules in Configuration
int Configuration::nMolecules() const { return molecules_.size(); }

// Return array of Molecules
std::deque<std::shared_ptr<Molecule>> &Configuration::molecules() { return molecules_; }
const std::deque<std::shared_ptr<Molecule>> &Configuration::molecules() const { return molecules_; }

// Return nth Molecule
std::shared_ptr<Molecule> Configuration::molecule(int n) { return molecules_[n]; }

// Add new Atom to Configuration, with Molecule parent specified
std::shared_ptr<Atom> Configuration::addAtom(const SpeciesAtom *sourceAtom, const std::shared_ptr<Molecule> &molecule,
                                             Vec3<double> r)
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

const std::vector<std::shared_ptr<Atom>> &Configuration::atoms() const { return atoms_; }

// Return nth atom
std::shared_ptr<Atom> Configuration::atom(int n)
{
    assert(n >= 0 && n < atoms_.size());
    return atoms_[n];
}

// Unfold molecule coordinates
void Configuration::unFoldMolecules()
{
    for (auto &mol : molecules_)
        mol->unFold(box_.get());
}

// Scale contents of the box by the specified factor
void Configuration::scaleContents(double factor)
{
    // Un-fold all molecules so we can determine true centres of geometry
    unFoldMolecules();

    // For each molecule, set its new centre of geometry
    Vec3<double> oldCog, newCog;
    for (auto &mol : molecules_)
    {
        // If the related species has a periodic box, scale atom positions rather than COG position
        if (mol->species()->box()->type() != Box::BoxType::NonPeriodic)
        {
            for (auto &i : mol->atoms())
                i->setCoordinates(i->r() * factor);
        }
        else
        {
            // First, work out the centre of geometry of the Molecule from the now un-folded coordinates
            oldCog = 0.0;
            for (const auto &i : mol->atoms())
                oldCog += i->r();
            oldCog /= mol->nAtoms();

            // Scale centre of geometry by supplied factor
            newCog = box()->fold(oldCog) * factor;

            // Can now just translate the molecule
            mol->translate(newCog - oldCog);
        }
    }
}
