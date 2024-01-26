// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/atomVector.h"
#include "classes/atom.h"

AtomVector::AtomVector(int size) : atoms_(size) {}
std::size_t AtomVector::size() const { return atoms_.size(); }

AtomRef AtomVector::begin() { return AtomRef(atoms_[0], *this); }
const AtomRef AtomVector::begin() const { return AtomRef(atoms_[0], *this); }
AtomRef AtomVector::end() { return AtomRef(*atoms_.end(), *this); }
const AtomRef AtomVector::end() const { return AtomRef(*atoms_.end(), *this); }

AtomRef AtomVector::front() { return AtomRef(atoms_.front(), *this); }
AtomRef AtomVector::back() { return AtomRef(atoms_.back(), *this); }
AtomRef AtomVector::operator[](std::size_t index) { return AtomRef(atoms_[index], *this); }
const AtomRef AtomVector::operator[](std::size_t index) const { return AtomRef(atoms_[index], *this); }

// Modifiers
void AtomVector::clear() { atoms_.clear(); }
void AtomVector::reserve(std::size_t newCapacity) { atoms_.reserve(newCapacity); }

AtomRef AtomVector::erase(const AtomRef pos) { return AtomRef(*atoms_.erase(atoms_.begin() + pos.globalAtomIndex()), *this); }
AtomRef AtomVector::erase(const AtomRef first, const AtomRef last)
{
    return AtomRef(*atoms_.erase(atoms_.begin() + first.globalAtomIndex(), atoms_.begin() + last.globalAtomIndex()), *this);
}
AtomRef AtomVector::emplace_back() { return AtomRef(atoms_.emplace_back(), *this); }

/*
 * Atom Ref Implementation
 */

AtomRef::AtomRef() : ref_(nullptr), origin_(nullptr) {}
AtomRef::AtomRef(const AtomRef &ref) : ref_(ref.ref_), origin_(ref.origin_) {}
AtomRef::AtomRef(const Atom &ref, const AtomVector &source)
    : ref_(const_cast<Atom *>(&ref)), origin_(const_cast<AtomVector *>(&source))
{
}

// AtomRef::operator Atom*() const { return ref_; }
Atom &AtomRef::operator*() { return *ref_; }
const Atom &AtomRef::operator*() const { return *ref_; }
Atom *AtomRef::operator->() { return ref_; }
const Atom *AtomRef::operator->() const { return ref_; }
AtomRef AtomRef::operator[](std::size_t offset) { return AtomRef(*(ref_ + offset), *origin_); }

bool AtomRef::operator==(const AtomRef &other) const { return ref_ == other.ref_; }
bool AtomRef::operator!=(const AtomRef &other) const { return ref_ != other.ref_ || origin_ != other.origin_; }
bool AtomRef::operator==(Atom *other) const { return ref_ == other; }

AtomRef &AtomRef::operator++()
{
    ++ref_;
    return *this;
}

AtomRef AtomRef::operator++(int)
{
    auto original = ref_++;
    return AtomRef(*original, *origin_);
}

std::size_t AtomRef::operator-(const AtomRef &other) const { return ref_ - other.ref_; }

int AtomRef::globalAtomIndex() const { return ref_ - &origin_->atoms_.front(); }

// Set SpeciesAtom that this Atom represents
void AtomRef::setSpeciesAtom(const SpeciesAtom *spAtom) { ref_->setSpeciesAtom(spAtom); }
// Set Molecule in which this Atom exists
void AtomRef::setMolecule(std::shared_ptr<Molecule> mol) { ref_->setMolecule(mol); }
// Set local AtomType index
void AtomRef::setLocalTypeIndex(int id) { ref_->setLocalTypeIndex(id); }
// Return local AtomType index
int AtomRef::localTypeIndex() const { return ref_->localTypeIndex(); }
// Set master AtomType index
void AtomRef::setMasterTypeIndex(int id) { ref_->setMasterTypeIndex(id); }
// Add targeted potential to this atom
void AtomRef::addTargetedPotential(const ExternalPotential *potential) { ref_->addTargetedPotential(potential); }
// Clear all targeted potentials from this Atom
void AtomRef::clearTargetedPotentials() { ref_->clearTargetedPotentials(); }
// Return list of targeted potentials for this atom
const std::vector<const ExternalPotential *> &AtomRef::targetedPotentials() const { return ref_->targetedPotentials(); }
// Return coordinates
const Vec3<double> &AtomRef::r() const { return ref_->r(); }

// Set coordinates
void AtomRef::setCoordinates(const Vec3<double> &newr) { ref_->setCoordinates(newr); }
