// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/atomType.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "math/mathFunc.h"
#include <memory>
#include <numeric>

/*
 * Private Functions
 */

// Add new Atom to Configuration, with Molecule parent specified
ConfigurationAtom &Configuration::addAtom(const SpeciesAtom *sourceAtom, const std::shared_ptr<Molecule> &molecule, Vector3 r)
{
    // Create new Atom object and set its source pointer
    auto &newAtom = atoms_.emplace_back();
    newAtom.setSpeciesAtom(sourceAtom);

    // Register the Atom in the specified Molecule (this will also set the Molecule pointer in the Atom)
    molecule->addAtom(&newAtom);

    // Set the position
    newAtom.setCoordinates(r);

    // Set configuration type index for pair potential lookup
    // TODO This can be removed once the Dissolve1 unit tests have been ported over to Dissolve2
    newAtom.setConfigurationTypeIndex(sourceAtom->atomType()->index());

    return newAtom;
}

/*
 * Public Functions
 */

// Clear contents of Configuration, leaving other definitions intact
void Configuration::empty()
{
    molecules_.clear();
    atoms_.clear();
    appliedSizeFactor_ = std::nullopt;
    speciesPopulations_.clear();
    globalPotentials_.clear();
    targetedPotentials_.clear();
    cells_.clear();

    ++version_;
    typeIndicesValid_ = false;
}

// Return Species populations within the Configuration
const KeyedVector<const Species *, int> &Configuration::speciesPopulations() const { return speciesPopulations_; }

// Return atom type populations for this Configuration
KeyedVector<const AtomType *, int> Configuration::atomTypePopulations() const
{
    KeyedVector<const AtomType *, int> populations;
    for (const auto &[sp, speciesPopulation] : speciesPopulations_)
    {
        for (const auto &[atomType, atomPopulation] : sp->atomTypePopulations())
            populations[atomType] += speciesPopulation * atomPopulation;
    }
    return populations;
}

// Return atom type index map
std::map<const AtomType *, int> Configuration::atomTypeIndexMap() const
{
    auto populations = atomTypePopulations();

    std::map<const AtomType *, int> typeMap;
    for (auto n = 0; n < populations.size(); ++n)
        typeMap[populations.key(n)] = n;

    return typeMap;
}

// Return used atom type vector
std::vector<const AtomType *> Configuration::atomTypeVector() const
{
    auto populations = atomTypePopulations();
    std::vector<const AtomType *> result(populations.size());
    std::transform(populations.vector().begin(), populations.vector().end(), result.begin(),
                   [](const auto &pop) { return pop.first; });
    return result;
}

// Return the total charge of the Configuration
double Configuration::totalCharge(bool ppIncludeCoulomb) const
{
    return std::accumulate(speciesPopulations_.begin(), speciesPopulations_.end(), 0.0, [&](const auto &acc, auto &spPop)
                           { return acc + spPop.first->totalCharge(ppIncludeCoulomb) * spPop.second; });
}

// Return the total atomic mass present in the Configuration
double Configuration::atomicMass() const
{
    auto mass = 0.0;

    // Get total molar mass in configuration
    for (const auto &[sp, population] : speciesPopulations_)
        mass += sp->mass() * population;

    // Convert to absolute mass
    return mass / DissolveMath::Avogadro;
}

// Return the atomic density of the Configuration
std::optional<double> Configuration::atomicDensity() const
{
    if (nAtoms() == 0)
        return {};

    return nAtoms(SpeciesAtom::Presence::Physical) / box_->volume();
}

// Return the chemical density (g/cm3) of the Configuration
std::optional<double> Configuration::chemicalDensity() const
{
    if (nAtoms() == 0)
        return {};

    return atomicMass() / (box_->volume() / 1.0E24);
}

// Return version (atomic positions and composition)
int Configuration::version() const { return version_; }

// Increment version of current contents
void Configuration::notifyAtomicPositionsChanged() { ++version_; }

// Add Molecule to Configuration based on the supplied Species
std::shared_ptr<Molecule> Configuration::addMolecule(const Species *sp,
                                                     OptionalReferenceWrapper<const std::vector<Vector3>> sourceCoordinates)
{
    // Create the new Molecule object and set its Species pointer
    auto newMolecule = std::make_shared<Molecule>();
    newMolecule->setArrayIndex(molecules_.size());
    molecules_.push_back(newMolecule);
    newMolecule->setSpecies(sp);

    // Update the relevant Species population
    ++speciesPopulations_[sp];

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

    typeIndicesValid_ = false;

    return newMolecule;
}

// Copy molecule
std::shared_ptr<Molecule> Configuration::copyMolecule(const Molecule &sourceMolecule)
{
    auto newMolecule = std::make_shared<Molecule>();
    newMolecule->setArrayIndex(molecules_.size());
    molecules_.push_back(newMolecule);
    auto *sp = sourceMolecule.species();
    newMolecule->setSpecies(sp);

    // Update the relevant Species population
    ++speciesPopulations_[sp];

    // Copy the source molecule's coordinates
    for (const auto *atom : sourceMolecule.atoms())
        addAtom(atom->speciesAtom(), newMolecule, atom->r());

    typeIndicesValid_ = false;

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
                                            --speciesPopulations_[mol->species()];
                                            return true;
                                        }
                                        else
                                            return false;
                                    }),
                     molecules_.end());

    // Now remove any atoms which have no molecule parent
    atoms_.erase(std::remove_if(atoms_.begin(), atoms_.end(), [](const auto &atom) { return !atom.molecule(); }), atoms_.end());

    typeIndicesValid_ = false;

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
                                            --speciesPopulations_[mol->species()];
                                            return true;
                                        }
                                        else
                                            return false;
                                    }),
                     molecules_.end());

    // Now remove any atoms which have no molecule parent
    atoms_.erase(std::remove_if(atoms_.begin(), atoms_.end(), [](const auto &atom) { return !atom.molecule(); }), atoms_.end());

    typeIndicesValid_ = false;

    updateObjectRelationships();
}

// Return number of Molecules in Configuration
int Configuration::nMolecules() const { return molecules_.size(); }

// Return Molecule vector
std::vector<std::shared_ptr<Molecule>> &Configuration::molecules() { return molecules_; }
const std::vector<std::shared_ptr<Molecule>> &Configuration::molecules() const { return molecules_; }

// Return nth Molecule
std::shared_ptr<Molecule> Configuration::molecule(int n) { return molecules_[n]; }

// Return the number of atoms in the configuration (or only those with the specified presence)
int Configuration::nAtoms(SpeciesAtom::Presence withPresence) const
{
    if (withPresence == SpeciesAtom::Presence::Any)
    {
        return atoms_.size();
    }
    return std::count_if(atoms_.begin(), atoms_.end(), [withPresence](const auto &i) { return i.isPresence(withPresence); });
}

// Return Atom array
std::vector<ConfigurationAtom> &Configuration::atoms() { return atoms_; }

const std::vector<ConfigurationAtom> &Configuration::atoms() const { return atoms_; }

// Return nth atom
ConfigurationAtom &Configuration::atom(int n)
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
void Configuration::scaleContents(Vector3 scaleFactors)
{
    // Un-fold all molecules so we can determine true centres of geometry
    unFoldMolecules();

    // For each molecule, set its new centre of geometry
    Vector3 oldCog, newCog, r;
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

    ++version_;
}

// Energy stable flag
void Configuration::setEnergyStable(bool stable) { energyIsStable_ = stable; }

bool Configuration::energyIsStable() const { return energyIsStable_; }

// Energy gradient
void Configuration::setEnergyGradient(double grad) { energyGradient_ = grad; }

double Configuration::getEnergyGradient() const { return energyGradient_; }

// Update type indices per Atom
void Configuration::updateTypeIndexing()
{
    // Are we currently up-to-date
    if (typeIndicesValid_)
        return;

    // Get the atom type index map
    auto typeMap = atomTypeIndexMap();

    // Loop over atoms
    for (auto &atom : atoms_)
    {
        if (atom.speciesAtom()->isPresence(SpeciesAtom::Presence::Physical))
            atom.setConfigurationTypeIndex(typeMap[atom.speciesAtom()->atomType()]);
        else
            atom.setConfigurationTypeIndex(AtomType::Ignore);
    }

    typeIndicesValid_ = true;
}
