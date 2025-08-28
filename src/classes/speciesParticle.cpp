// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "classes/speciesParticle.h"
#include "classes/coreData.h"
#include "classes/speciesAngle.h"
#include "classes/speciesBond.h"
#include "classes/speciesImproper.h"
#include "classes/speciesTorsion.h"

SpeciesParticle::SpeciesParticle(SpeciesParticle &&source) noexcept { move(source); }

SpeciesParticle &SpeciesParticle::operator=(SpeciesParticle &&source) noexcept
{
    move(source);
    return *this;
}

void SpeciesParticle::move(SpeciesParticle &source)
{
    setCoordinates(source.r());
    selected_ = source.selected_;
    index_ = source.index_;

    bonds_ = std::move(source.bonds_);
    angles_ = std::move(source.angles_);
    torsions_ = std::move(source.torsions_);
    impropers_ = std::move(source.impropers_);

    // Rewrite pointers in intramolecular terms
    for (auto &bond : bonds_)
        bond.get().switchAtom(&source, this);
    for (auto &angle : angles_)
        angle.get().switchAtom(&source, this);
    for (auto &torsion : torsions_)
        torsion.get().switchAtom(&source, this);
    for (auto &improper : impropers_)
        improper.get().switchAtom(&source, this);

    // Tidy old data
    source.setCoordinates({});
    source.selected_ = false;
    source.index_ = -1;
    source.bonds_.clear();
    source.angles_.clear();
    source.torsions_.clear();
    source.impropers_.clear();
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

/*
 * Bond Information
 */

// Add Bond reference
void SpeciesParticle::addBond(SpeciesBond &bond)
{
    if (find_if(bonds_.begin(), bonds_.end(), [&bond](const SpeciesBond &b) { return &b == &bond; }) == bonds_.end())
        bonds_.emplace_back(bond);
}

// Remove Bond reference
void SpeciesParticle::removeBond(SpeciesBond &b)
{
    bonds_.erase(find_if(bonds_.begin(), bonds_.end(), [&b](const SpeciesBond &bond) { return &b == &bond; }));
}

// Return number of Bond references
int SpeciesParticle::nBonds() const { return bonds_.size(); }

// Return specified bond
SpeciesBond &SpeciesParticle::bond(int index) { return bonds_.at(index); }

// Return bonds list
const std::vector<std::reference_wrapper<SpeciesBond>> &SpeciesParticle::bonds() const { return bonds_; }

// Return whether Bond to specified Atom exists
OptionalReferenceWrapper<SpeciesBond> SpeciesParticle::getBond(const SpeciesParticle *partner)
{
    auto result = find_if(bonds_.begin(), bonds_.end(), [&](const SpeciesBond &bond) { return bond.partner(this) == partner; });
    if (result == bonds_.end())
        return std::nullopt;
    return *result;
}

/*
 * Angle Information
 */

// Add specified SpeciesAngle to Atom
void SpeciesParticle::addAngle(SpeciesAngle &angle) { angles_.emplace_back(angle); }

// Remove angle reference
void SpeciesParticle::removeAngle(SpeciesAngle &angle)
{
    angles_.erase(find_if(angles_.begin(), angles_.end(), [&angle](const SpeciesAngle &a) { return &a == &angle; }));
}

// Return the number of Angles in which the Atom is involved
int SpeciesParticle::nAngles() const { return angles_.size(); }

// Return specified angle
SpeciesAngle &SpeciesParticle::angle(int index) { return angles_.at(index); }

// Return array of Angles in which the Atom is involved
const std::vector<std::reference_wrapper<SpeciesAngle>> &SpeciesParticle::angles() const { return angles_; }

/*
 * Torsion Information
 */

// Add specified SpeciesTorsion to Atom
void SpeciesParticle::addTorsion(SpeciesTorsion &torsion) { torsions_.emplace_back(torsion); }

// Remove torsion reference
void SpeciesParticle::removeTorsion(SpeciesTorsion &torsion)
{
    torsions_.erase(
        find_if(torsions_.begin(), torsions_.end(), [&torsion](const SpeciesTorsion &t) { return &t == &torsion; }));
}

// Return the number of Torsions in which the Atom is involved
int SpeciesParticle::nTorsions() const { return torsions_.size(); }

// Return specified torsion
SpeciesTorsion &SpeciesParticle::torsion(int index) { return torsions_.at(index); }

// Return array of Torsions in which the Atom is involved
const std::vector<std::reference_wrapper<SpeciesTorsion>> &SpeciesParticle::torsions() const { return torsions_; }

/*
 * Improper Information
 */

// Add specified SpeciesImproper to Atom
void SpeciesParticle::addImproper(SpeciesImproper &improper) { impropers_.emplace_back(improper); }

// Remove improper reference
void SpeciesParticle::removeImproper(SpeciesImproper &improper)
{
    impropers_.erase(
        find_if(impropers_.begin(), impropers_.end(), [&improper](const SpeciesImproper &i) { return &i == &improper; }));
}

// Return the number of Impropers in which the Atom is involved
int SpeciesParticle::nImpropers() const { return impropers_.size(); }

// Return specified improper
SpeciesImproper &SpeciesParticle::improper(int index) { return impropers_.at(index); }

// Return array of Impropers in which the Atom is involved
const std::vector<std::reference_wrapper<SpeciesImproper>> &SpeciesParticle::impropers() const { return impropers_; }