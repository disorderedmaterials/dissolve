// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/atomVector.h"
#include "classes/atom.h"

AtomVector::AtomVector(int size) : rs_(size), localTypeIndex_(size), masterTypeIndex_(size), speciesAtom_(size), molecule_(size), cell_(size) {}
std::size_t AtomVector::size() const { return rs_.size(); }

Atom AtomVector::begin() { return Atom(0, *this); }
const Atom AtomVector::begin() const { return Atom(0, *this); }
Atom AtomVector::end() { return Atom(rs_.size(), *this); }
const Atom AtomVector::end() const { return Atom(rs_.size(), *this); }

Atom AtomVector::front() { return Atom(0, *this); }
Atom AtomVector::back() { return Atom(rs_.size() - 1, *this); }
Atom AtomVector::operator[](std::size_t index) { return Atom(index, *this); }
const Atom AtomVector::operator[](std::size_t index) const { return Atom(index, *this); }

// Modifiers
void AtomVector::clear()
{
    rs_.clear();
    localTypeIndex_.clear();
    masterTypeIndex_.clear();
    speciesAtom_.clear();
    molecule_.clear();
    cell_.clear();
    targetedPotentials_.clear();
}
void AtomVector::reserve(std::size_t newCapacity)
{
    rs_.reserve(newCapacity);
    localTypeIndex_.reserve(newCapacity);
    masterTypeIndex_.reserve(newCapacity);
    speciesAtom_.reserve(newCapacity);
    molecule_.reserve(newCapacity);
    cell_.reserve(newCapacity);
    targetedPotentials_.reserve(newCapacity);
}

Atom AtomVector::erase(const Atom pos)
{
    localTypeIndex_.erase(localTypeIndex_.begin() + pos.globalAtomIndex());
    masterTypeIndex_.erase(masterTypeIndex_.begin() + pos.globalAtomIndex());
    speciesAtom_.erase(speciesAtom_.begin() + pos.globalAtomIndex());
    molecule_.erase(molecule_.begin() + pos.globalAtomIndex());
    cell_.erase(cell_.begin() + pos.globalAtomIndex());
    targetedPotentials_.erase(targetedPotentials_.begin() + pos.globalAtomIndex());
    auto result = rs_.erase(rs_.begin() + pos.globalAtomIndex());
    return Atom(result - rs_.begin(), *this);
}
Atom AtomVector::erase(const Atom first, const Atom last)
{
    localTypeIndex_.erase(localTypeIndex_.begin() + first.globalAtomIndex(), localTypeIndex_.begin() + last.globalAtomIndex());
    masterTypeIndex_.erase(masterTypeIndex_.begin() + first.globalAtomIndex(), masterTypeIndex_.begin() + last.globalAtomIndex());
    speciesAtom_.erase(speciesAtom_.begin() + first.globalAtomIndex(), speciesAtom_.begin() + last.globalAtomIndex());
    molecule_.erase(molecule_.begin() + first.globalAtomIndex(), molecule_.begin() + last.globalAtomIndex());
    cell_.erase(cell_.begin() + first.globalAtomIndex(), cell_.begin() + last.globalAtomIndex());
    targetedPotentials_.erase(targetedPotentials_.begin() + first.globalAtomIndex(), targetedPotentials_.begin() + last.globalAtomIndex());
    auto result = rs_.erase(rs_.begin() + first.globalAtomIndex(), rs_.begin() + last.globalAtomIndex());
    return Atom(result - rs_.begin(), *this);
}
Atom AtomVector::emplace_back()
{
    localTypeIndex_.emplace_back();
    masterTypeIndex_.emplace_back();
    speciesAtom_.emplace_back();
    molecule_.emplace_back();
    cell_.emplace_back();
    rs_.emplace_back();
    return Atom(rs_.size() - 1, *this);
}

std::vector<Vec3<double>> &AtomVector::rs() {return rs_;}

std::vector<int> &AtomVector::localTypeIndices() {return localTypeIndex_;}
