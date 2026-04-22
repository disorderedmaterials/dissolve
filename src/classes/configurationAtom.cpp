// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/configurationAtom.h"
#include "classes/atomType.h"
#include "classes/molecule.h"
#include "classes/speciesAtom.h"
#include <utility>

// Set coordinates
void ConfigurationAtom::set(const Vector3 r) { r_ = r; }

// Set coordinates
void ConfigurationAtom::set(double rx, double ry, double rz) { r_.set(rx, ry, rz); }

// Return coordinates
const Vector3 &ConfigurationAtom::r() const { return r_; }

// Return x-coordinate
double ConfigurationAtom::x() const { return r_.x; }

// Return y-coordinate
double ConfigurationAtom::y() const { return r_.y; }

// Return z-coordinate
double ConfigurationAtom::z() const { return r_.z; }

// Set AtomType index in parent Configuration
void ConfigurationAtom::setConfigurationTypeIndex(int id) { configurationTypeIndex_ = id; }

// Return AtomType index in parent Configuration
int ConfigurationAtom::configurationTypeIndex() const { return configurationTypeIndex_; }

// Return global index of the atom
int ConfigurationAtom::globalIndex() const
{
    assert(molecule_);
    return molecule_->globalAtomIndex(this);
}

/*
 * Location
 */

// Set SpeciesAtom that this Atom represents
void ConfigurationAtom::setSpeciesAtom(const SpeciesAtom *spAtom) { speciesAtom_ = spAtom; }

// Return SpeciesAtom that this Atom represents
const SpeciesAtom *ConfigurationAtom::speciesAtom() const { return speciesAtom_; }

// Return whether the atom's presence is that specified
bool ConfigurationAtom::isPresence(SpeciesAtom::Presence presence) const { return speciesAtom_->isPresence(presence); }

// Set Molecule in which this Atom exists
void ConfigurationAtom::setMolecule(std::shared_ptr<Molecule> mol) { molecule_ = std::move(mol); }

// Return Molecule in which this Atom exists
const std::shared_ptr<Molecule> &ConfigurationAtom::molecule() const { return molecule_; }

// Set cell in which the atom exists
void ConfigurationAtom::setCell(Cell *cell) { cell_ = cell; }

// Return cell in which the atom exists
Cell *ConfigurationAtom::cell() const { return cell_; }

/*
 * Coordinate Manipulation
 */

// Set coordinates
void ConfigurationAtom::setCoordinates(const Vector3 &newr) { r_ = newr; }

// Set coordinates
void ConfigurationAtom::setCoordinates(double dx, double dy, double dz) { setCoordinates(Vector3(dx, dy, dz)); }

// Translate coordinates
void ConfigurationAtom::translateCoordinates(const Vector3 &delta) { setCoordinates(r_ + delta); }

// Translate coordinates
void ConfigurationAtom::translateCoordinates(double dx, double dy, double dz) { setCoordinates(r_ + Vector3(dx, dy, dz)); }

/*
 * Intramolecular Information
 */

// Return scaling type and factors (electrostatic, van der Waals) to employ with specified Atom
SpeciesAtom::ScaledInteractionDefinition ConfigurationAtom::scaling(const ConfigurationAtom *j) const
{
    assert(speciesAtom_ != nullptr);
    assert(j != nullptr);
    assert(j->speciesAtom() != nullptr);

    return speciesAtom_->scaling(j->speciesAtom());
}

/*
 * Targeted Potentials
 */

// Add targeted potential to this atom
void ConfigurationAtom::addTargetedPotential(const ExternalPotential *potential)
{
    targetedPotentials_.emplace_back(potential);
}

// Clear all targeted potentials from this Atom
void ConfigurationAtom::clearTargetedPotentials() { targetedPotentials_.clear(); }

// Return list of targeted potentials for this atom
const std::vector<const ExternalPotential *> &ConfigurationAtom::targetedPotentials() const { return targetedPotentials_; }
