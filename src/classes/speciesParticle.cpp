// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "classes/coreData.h"
#include "classes/speciesParticle.h"

SpeciesParticle::SpeciesParticle(SpeciesParticle &&source) noexcept { move(source); }

SpeciesParticle &SpeciesParticle::operator=(SpeciesParticle &&source) noexcept
{
    move(source);
    return *this;
}

void SpeciesParticle::move(SpeciesParticle &source)
{
    setCoordinates(source.r());
    source.setCoordinates({});
}

// Set index (0->[N-1])
void SpeciesParticle::setIndex(int id) { index_ = id; }

// Return index (0->[N-1])
int SpeciesParticle::index() const { return index_; }

// Return 'user' index (1->N)
int SpeciesParticle::userIndex() const { return index_ + 1; }

// Set whether the atom is currently selected
void SpeciesParticle::setSelected(bool selected) { selected_ = selected; }

// Return whether the atom is currently selected
bool SpeciesParticle::isSelected() const { return selected_; }

// Return coordinates
const Vector3 &SpeciesParticle::r() const { return r_; }

/*
 * Coordinate Manipulation
 */

// Set coordinate
void SpeciesParticle::setCoordinate(int index, double value) { r_.set(index, value); }

// Set coordinates
void SpeciesParticle::setCoordinates(double x, double y, double z)
{
    r_.x = x;
    r_.y = y;
    r_.z = z;
}

// Set coordinates (from Vec3)
void SpeciesParticle::setCoordinates(const Vector3 &newr) { r_ = newr; }

// Translate coordinates of atom
void SpeciesParticle::translateCoordinates(const Vector3 &delta) { r_ += delta; }