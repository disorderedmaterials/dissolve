// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/atomType.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include <memory>
#include <numeric>

// Clear contents of Configuration, leaving other definitions intact
void Configuration::empty()
{
    molecules_.clear();
    atoms_.clear();
    atomTypePopulations_.clear();
    appliedSizeFactor_ = std::nullopt;
    speciesPopulations_.clear();
    globalPotentials_.clear();
    targetedPotentials_.clear();
    cells_.clear();

    ++contentsVersion_;
}

// Return atom type populations for this Configuration
const AtomTypeMix &Configuration::atomTypePopulations() const { return atomTypePopulations_; }

// Adjust population of specified Species in the Configuration
void Configuration::adjustSpeciesPopulation(const Species *sp, int delta)
{
    auto it = std::find_if(speciesPopulations_.begin(), speciesPopulations_.end(),
                           [sp](const auto &data) { return data.first == sp; });
    if (it == speciesPopulations_.end())
    {
        if (delta < 0)
            throw(std::runtime_error(
                fmt::format("Can't decrease population of Species '{}' as it is not in the list.\n", sp->name())));
        speciesPopulations_.emplace_back(sp, delta);
    }
    else
        it->second += delta;
}

// Return Species populations within the Configuration
const std::vector<std::pair<const Species *, int>> &Configuration::speciesPopulations() const { return speciesPopulations_; }

// Return population of specified species within the Configuration
int Configuration::speciesPopulation(const Species *sp) const
{
    auto it = std::find_if(speciesPopulations_.begin(), speciesPopulations_.end(),
                           [sp](const auto &spInfo) { return spInfo.first == sp; });
    return it == speciesPopulations_.end() ? 0 : it->second;
}

// Return if the specified Species is present in the Configuration
bool Configuration::containsSpecies(const Species *sp)
{
    return std::find_if(speciesPopulations_.begin(), speciesPopulations_.end(),
                        [sp](const auto &data) { return data.first == sp; }) != speciesPopulations_.end();
}

// Return the total charge of the Configuration
double Configuration::totalCharge(bool ppIncludeCoulomb) const
{
    return std::accumulate(speciesPopulations_.begin(), speciesPopulations_.end(), 0.0,
                           [&](const auto &acc, auto &spPop)
                           { return acc + spPop.first->totalCharge(ppIncludeCoulomb) * spPop.second; });
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
std::optional<double> Configuration::atomicDensity() const
{
    if (nAtoms() == 0)
        return {};

    return (nAtoms() - nArtificialAtoms()) / box_->volume();
}

// Return the chemical density (g/cm3) of the Configuration
std::optional<double> Configuration::chemicalDensity() const
{
    if (nAtoms() == 0)
        return {};

    return atomicMass() / (box_->volume() / 1.0E24);
}

// Return version of current contents
int Configuration::contentsVersion() const { return contentsVersion_; }

// Increment version of current contents
void Configuration::incrementContentsVersion() { ++contentsVersion_; }

// Add Molecule to Configuration based on the supplied Species
std::shared_ptr<Molecule>
Configuration::addMolecule(const Species *sp, OptionalReferenceWrapper<const std::vector<Vec3<double>>> sourceCoordinates)
{
    // Create the new Molecule object and set its Species pointer
    auto newMolecule = std::make_shared<Molecule>();
    newMolecule->setArrayIndex(molecules_.size());
    molecules_.push_back(newMolecule);
    newMolecule->setSpecies(sp);

    // Update the relevant Species population
    adjustSpeciesPopulation(sp, 1);

    // Add Atoms from Species to the Molecule, using either species coordinates or those from the source CoordinateSet
    auto previousNAtoms = atoms_.size();
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

    newMolecule->updateAtoms(atoms_, previousNAtoms);

    return newMolecule;
}

// Copy molecule
std::shared_ptr<Molecule> Configuration::copyMolecule(const std::shared_ptr<Molecule> &sourceMolecule)
{
    std::shared_ptr<Molecule> newMolecule = std::make_shared<Molecule>();
    newMolecule->setArrayIndex(molecules_.size());
    molecules_.push_back(newMolecule);
    auto *sp = sourceMolecule->species();
    newMolecule->setSpecies(sp);

    // Update the relevant Species population
    adjustSpeciesPopulation(sp, 1);

    // Copy the source molecule's coordinates
    for (const auto *atom : sourceMolecule->atoms())
        addAtom(atom->speciesAtom(), newMolecule, atom->r());

    return newMolecule;
}

// Remove all Molecules of the target Species from the Configuration
void Configuration::removeMolecules(const Species *sp)
{
    molecules_.erase(std::remove_if(molecules_.begin(), molecules_.end(),
                                    [&, sp](auto &mol)
                                    {
                                        if (mol->species() == sp)
                                        {
                                            auto offset = mol->globalAtomOffset();
                                            for (auto i = 0; i < mol->nAtoms(); ++i)
                                                atoms_[offset + i].setMolecule(nullptr);
                                            adjustSpeciesPopulation(mol->species(), -1);
                                            return true;
                                        }
                                        else
                                            return false;
                                    }),
                     molecules_.end());

    // Now remove any atoms which have no molecule parent
    atoms_.erase(std::remove_if(atoms_.begin(), atoms_.end(), [](const auto &atom) { return !atom.molecule(); }), atoms_.end());

    updateObjectRelationships();
}

// Remove specified Molecules from the Configuration
void Configuration::removeMolecules(const std::vector<std::shared_ptr<Molecule>> &molecules)
{
    molecules_.erase(std::remove_if(molecules_.begin(), molecules_.end(),
                                    [&, molecules](const auto &mol)
                                    {
                                        if (std::find(molecules.begin(), molecules.end(), mol) != molecules.end())
                                        {
                                            auto offset = mol->globalAtomOffset();
                                            for (auto i = 0; i < mol->nAtoms(); ++i)
                                                atoms_[offset + i].setMolecule(nullptr);
                                            adjustSpeciesPopulation(mol->species(), -1);
                                            return true;
                                        }
                                        else
                                            return false;
                                    }),
                     molecules_.end());

    // Now remove any atoms which have no molecule parent
    atoms_.erase(std::remove_if(atoms_.begin(), atoms_.end(), [](const auto &atom) { return !atom.molecule(); }), atoms_.end());

    updateObjectRelationships();
}

// Return number of Molecules in Configuration
int Configuration::nMolecules() const { return molecules_.size(); }

// Return Molecule vector
std::vector<std::shared_ptr<Molecule>> &Configuration::molecules() { return molecules_; }
const std::vector<std::shared_ptr<Molecule>> &Configuration::molecules() const { return molecules_; }

// Return nth Molecule
std::shared_ptr<Molecule> Configuration::molecule(int n) { return molecules_[n]; }

// Add new Atom to Configuration, with Molecule parent specified
Atom &Configuration::addAtom(const SpeciesAtom *sourceAtom, const std::shared_ptr<Molecule> &molecule, Vec3<double> r)
{
    // Create new Atom object and set its source pointer
    auto &newAtom = atoms_.emplace_back();
    newAtom.setSpeciesAtom(sourceAtom);

    // Register the Atom in the specified Molecule (this will also set the Molecule pointer in the Atom)
    molecule->addAtom(&newAtom);

    // Set the position
    newAtom.setCoordinates(r);

    // Update atom type population and set local type index
    if (sourceAtom->isArtificial())
        newAtom.setLocalTypeIndex(Atom::Artificial);
    else
    {
        auto &&[atd, atdIndex] = atomTypePopulations_.add(sourceAtom->atomType(), 1);
        newAtom.setLocalTypeIndex(atdIndex);
    }

    // Set master index for pair potential lookup
    newAtom.setMasterTypeIndex(sourceAtom->atomType()->index());

    return newAtom;
}

// Return number of Atoms in Configuration
int Configuration::nAtoms() const { return atoms_.size(); }

// Return the number of artificial atoms in the Configuration
int Configuration::nArtificialAtoms() const
{
    return std::count_if(atoms_.begin(), atoms_.end(), [](const auto &i) { return i.isArtificial(); });
}

// Return Atom array
std::vector<Atom> &Configuration::atoms() { return atoms_; }

const std::vector<Atom> &Configuration::atoms() const { return atoms_; }

// Return nth atom
Atom &Configuration::atom(int n)
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

// Scale contents of the box by the specified factors along each axis
void Configuration::scaleContents(Vec3<double> scaleFactors)
{
    // Un-fold all molecules so we can determine true centres of geometry
    unFoldMolecules();

    // For each molecule, set its new centre of geometry
    Vec3<double> oldCog, newCog, r;
    for (auto &mol : molecules_)
    {
        // If the related species has a periodic box, scale atom positions rather than COG position
        if (mol->species()->box()->type() != Box::BoxType::NonPeriodic)
        {
            for (auto &i : mol->atoms())
            {
                r = i->r();
                box()->toFractional(r);
                r.multiply(scaleFactors);
                box()->toReal(r);
                i->setCoordinates(r);
            }
        }
        else
        {
            // First, work out the centre of geometry of the Molecule from the now un-folded coordinates
            oldCog = 0.0;
            for (const auto &i : mol->atoms())
                oldCog += i->r();
            oldCog /= mol->nAtoms();

            // Scale centre of geometry by supplied factor
            newCog = box()->fold(oldCog);
            newCog.multiply(scaleFactors);

            // Can now just translate the molecule
            mol->translate(newCog - oldCog);
        }
    }

    ++contentsVersion_;
}
