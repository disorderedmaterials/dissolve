// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/speciesatom.h"
#include "base/processpool.h"
#include "classes/atomtype.h"
#include "classes/species.h"
#include "classes/speciesbond.h"
#include "data/elements.h"
#include <algorithm>

SpeciesAtom::SpeciesAtom() : Z_(Elements::Unknown), charge_(0.0), atomType_(nullptr), index_(-1), selected_(false)
{
    r_.zero();
}

// Set Species parent
void SpeciesAtom::setSpecies(Species *sp) { parent_ = sp; }

// Return Species parent
const Species *SpeciesAtom::species() const { return parent_; }

// Set basic SpeciesAtom properties
void SpeciesAtom::set(Elements::Element Z, double rx, double ry, double rz, double q)
{
    Z_ = Z;
    r_.set(rx, ry, rz);
    charge_ = q;
}

// Set basic SpeciesAtom properties
void SpeciesAtom::set(Elements::Element Z, const Vec3<double> r, double q)
{
    Z_ = Z;
    r_ = r;
    charge_ = q;
}

// Set atomic element
void SpeciesAtom::setZ(Elements::Element Z) { Z_ = Z; }

// Return atomic element
Elements::Element SpeciesAtom::Z() const { return Z_; }

// Return coordinates
const Vec3<double> &SpeciesAtom::r() const { return r_; }

// Set charge of SpeciesAtom
void SpeciesAtom::setCharge(double charge) { charge_ = charge; }

// Return charge of SpeciesAtom
double SpeciesAtom::charge() const { return charge_; }

// Set AtomType of SpeciesAtom
void SpeciesAtom::setAtomType(std::shared_ptr<AtomType> at)
{
    // Check elements
    if (at && (at->Z() != Z_))
    {
        Messenger::warn("Refused to assign AtomType '{}' to an atom of element {}, since the element of the AtomType is {}.\n",
                        at->name(), Elements::symbol(Z_), Elements::symbol(at->Z()));
        return;
    }

    atomType_ = at;
    if (parent_)
        parent_->updateUsedAtomTypes();
}

// Return SpeciesAtomType of SpeciesAtom
std::shared_ptr<AtomType> SpeciesAtom::atomType() const { return atomType_; }

// Set index (0->[N-1])
void SpeciesAtom::setIndex(int id) { index_ = id; }

// Return index (0->[N-1])
int SpeciesAtom::index() const { return index_; }

// Return 'user' index (1->N)
int SpeciesAtom::userIndex() const { return index_ + 1; }

// Set whether the atom is currently selected
void SpeciesAtom::setSelected(bool selected) { selected_ = selected; }

// Return whether the atom is currently selected
bool SpeciesAtom::isSelected() const { return selected_; }

/*
 * Bond Information
 */

// Add Bond reference
void SpeciesAtom::addBond(SpeciesBond &bond)
{
    if (find_if(bonds_.begin(), bonds_.end(), [&bond](const SpeciesBond &b) { return &b == &bond; }) == bonds_.end())
        bonds_.push_back(bond);
}

// Remove Bond reference
void SpeciesAtom::removeBond(SpeciesBond &b)
{
    bonds_.erase(find_if(bonds_.begin(), bonds_.end(), [&b](const SpeciesBond &bond) { return &b == &bond; }));
}

// Clear all Bond references
void SpeciesAtom::clearBonds() { bonds_.clear(); }

// Return number of Bond references
int SpeciesAtom::nBonds() const { return bonds_.size(); }

// Return specified bond
SpeciesBond &SpeciesAtom::bond(int index) { return bonds_.at(index); }

// Return bonds list
const std::vector<std::reference_wrapper<SpeciesBond>> &SpeciesAtom::bonds() const { return bonds_; }

// Return whether Bond to specified Atom exists
OptionalReferenceWrapper<SpeciesBond> SpeciesAtom::hasBond(SpeciesAtom *partner)
{
    auto result = find_if(bonds_.begin(), bonds_.end(), [&](const SpeciesBond &bond) { return bond.partner(this) == partner; });
    if (result == bonds_.end())
        return std::nullopt;
    return *result;
}

// Add specified SpeciesAngle to Atom
void SpeciesAtom::addAngle(SpeciesAngle &angle)
{
    angles_.push_back(angle);

    // Insert the pointers to the other Atoms into the exclusions_ list
    if (angle.i() != this)
        exclusions_.emplace_back(angle.i(), 0.0);
    if (angle.j() != this)
        exclusions_.emplace_back(angle.j(), 0.0);
    if (angle.k() != this)
        exclusions_.emplace_back(angle.k(), 0.0);
}

// Remove angle reference
void SpeciesAtom::removeAngle(SpeciesAngle &angle)
{
    angles_.erase(find_if(angles_.begin(), angles_.end(), [&angle](const SpeciesAngle &a) { return &a == &angle; }));
}

// Return the number of Angles in which the Atom is involved
int SpeciesAtom::nAngles() const { return angles_.size(); }

// Return specified angle
SpeciesAngle &SpeciesAtom::angle(int index) { return angles_.at(index); }

// Return array of Angles in which the Atom is involved
const std::vector<std::reference_wrapper<SpeciesAngle>> &SpeciesAtom::angles() const { return angles_; }

// Add specified SpeciesTorsion to Atom
void SpeciesAtom::addTorsion(SpeciesTorsion &torsion, double scaling14)
{
    torsions_.push_back(torsion);

    // Insert the pointers to the other Atoms into the exclusions_ list
    if (torsion.i() == this)
    {
        exclusions_.emplace_back(torsion.j(), 0.0);
        exclusions_.emplace_back(torsion.k(), 0.0);
        exclusions_.emplace_back(torsion.l(), scaling14);
    }
    else if (torsion.l() == this)
    {
        exclusions_.emplace_back(torsion.i(), scaling14);
        exclusions_.emplace_back(torsion.j(), 0.0);
        exclusions_.emplace_back(torsion.k(), 0.0);
    }
    else
    {
        exclusions_.emplace_back(torsion.i(), 0.0);
        exclusions_.emplace_back(torsion.l(), 0.0);
        if (torsion.j() != this)
            exclusions_.emplace_back(torsion.j(), 0.0);
        if (torsion.k() != this)
            exclusions_.emplace_back(torsion.k(), 0.0);
    }
}

// Remove torsion reference
void SpeciesAtom::removeTorsion(SpeciesTorsion &torsion)
{
    torsions_.erase(
        find_if(torsions_.begin(), torsions_.end(), [&torsion](const SpeciesTorsion &t) { return &t == &torsion; }));
}

// Return the number of Torsions in which the Atom is involved
int SpeciesAtom::nTorsions() const { return torsions_.size(); }

// Return specified torsion
SpeciesTorsion &SpeciesAtom::torsion(int index) { return torsions_.at(index); }

// Return array of Torsions in which the Atom is involved
const std::vector<std::reference_wrapper<SpeciesTorsion>> &SpeciesAtom::torsions() const { return torsions_; }

// Add specified SpeciesImproper to Atom
void SpeciesAtom::addImproper(SpeciesImproper &improper) { impropers_.push_back(improper); }

// Remove improper reference
void SpeciesAtom::removeImproper(SpeciesImproper &improper)
{
    impropers_.erase(
        find_if(impropers_.begin(), impropers_.end(), [&improper](const SpeciesImproper &i) { return &i == &improper; }));
}

// Return the number of Impropers in which the Atom is involved
int SpeciesAtom::nImpropers() const { return impropers_.size(); }

// Return specified improper
SpeciesImproper &SpeciesAtom::improper(int index) { return impropers_.at(index); }

// Return array of Impropers in which the Atom is involved
const std::vector<std::reference_wrapper<SpeciesImproper>> &SpeciesAtom::impropers() const { return impropers_; }

// Return scaling factor to employ with specified Atom
double SpeciesAtom::scaling(const SpeciesAtom *j) const
{
    auto it = std::find_if(exclusions_.begin(), exclusions_.end(), [j](const auto &p) { return p.first == j; });
    if (it != exclusions_.end())
    {
        return it->second;
    }
    else
    {
        return 1.0;
    }
}

/*
 * Coordinate Manipulation
 */

// Set coordinate
void SpeciesAtom::setCoordinate(int index, double value) { r_.set(index, value); }

// Set coordinates
void SpeciesAtom::setCoordinates(double x, double y, double z)
{
    r_.x = x;
    r_.y = y;
    r_.z = z;
}

// Set coordinates (from Vec3)
void SpeciesAtom::setCoordinates(const Vec3<double> &newr) { r_ = newr; }

// Translate coordinates of atom
void SpeciesAtom::translateCoordinates(const Vec3<double> &delta) { r_ += delta; }
