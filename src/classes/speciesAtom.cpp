// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/speciesAtom.h"
#include "classes/atomType.h"
#include "classes/box.h"
#include "classes/coreData.h"
#include "classes/species.h"
#include "classes/speciesBond.h"
#include <algorithm>

SpeciesAtom::SpeciesAtom(SpeciesAtom &&source) noexcept { move(source); }

SpeciesAtom &SpeciesAtom::operator=(SpeciesAtom &&source) noexcept
{
    move(source);

    return *this;
}

// Move all data from source to this
void SpeciesAtom::move(SpeciesAtom &source)
{
    Z_ = source.Z_;
    r_ = source.r_;
    charge_ = source.charge_;
    atomType_ = source.atomType_;
    selected_ = source.selected_;
    index_ = source.index_;
    presence_ = source.presence_;

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
    source.Z_ = Elements::Unknown;
    source.r_ = {};
    source.charge_ = 0.0;
    source.atomType_ = nullptr;
    source.selected_ = false;
    source.index_ = -1;
    source.bonds_.clear();
    source.angles_.clear();
    source.torsions_.clear();
    source.impropers_.clear();
}

/*
 * Properties
 */

// Set basic properties
void SpeciesAtom::set(Elements::Element Z, double rx, double ry, double rz, double q)
{
    set(Z, {rx, ry, rz}, q);
}
void SpeciesAtom::set(Elements::Element Z, const Vec3<double> &r, double q)
{
    Z_ = Z;
    r_ = r;
    charge_ = q;

    presence_ = Z_ == Elements::Phantom ? Presence::Phantom : Presence::Physical;
}

// Set atomic element
void SpeciesAtom::setZ(Elements::Element Z) { Z_ = Z; }

// Return atomic element
Elements::Element SpeciesAtom::Z() const { return Z_; }

// Return whether the atom is of the presence specified
bool SpeciesAtom::isPresence(SpeciesAtom::Presence presence) const
{
    return presence == SpeciesAtom::Presence::Any || presence_ == presence;
}

// Return coordinates
const Vec3<double> &SpeciesAtom::r() const { return r_; }

// Set charge of SpeciesAtom
void SpeciesAtom::setCharge(double charge) { charge_ = charge; }

// Return charge of SpeciesAtom
double SpeciesAtom::charge() const { return charge_; }

// Set AtomType of SpeciesAtom
void SpeciesAtom::setAtomType(const std::shared_ptr<AtomType> &at)
{
    // Check elements
    if (at && (at->Z() != Z_))
    {
        Messenger::warn("Refused to assign AtomType '{}' to an atom of element {}, since the element of the AtomType is {}.\n",
                        at->name(), Elements::symbol(Z_), Elements::symbol(at->Z()));
        return;
    }

    atomType_ = at;
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

// Return presence of atom
SpeciesAtom::Presence SpeciesAtom::presence() const { return presence_; }

/*
 * Bond Information
 */

// Add Bond reference
void SpeciesAtom::addBond(SpeciesBond &bond)
{
    if (find_if(bonds_.begin(), bonds_.end(), [&bond](const SpeciesBond &b) { return &b == &bond; }) == bonds_.end())
        bonds_.emplace_back(bond);
}

// Remove Bond reference
void SpeciesAtom::removeBond(SpeciesBond &b)
{
    bonds_.erase(find_if(bonds_.begin(), bonds_.end(), [&b](const SpeciesBond &bond) { return &b == &bond; }));
}

// Return number of Bond references
int SpeciesAtom::nBonds() const { return bonds_.size(); }

// Return specified bond
SpeciesBond &SpeciesAtom::bond(int index) { return bonds_.at(index); }

// Return bonds list
const std::vector<std::reference_wrapper<SpeciesBond>> &SpeciesAtom::bonds() const { return bonds_; }

// Return whether Bond to specified Atom exists
OptionalReferenceWrapper<SpeciesBond> SpeciesAtom::getBond(const SpeciesAtom *partner)
{
    auto result = find_if(bonds_.begin(), bonds_.end(), [&](const SpeciesBond &bond) { return bond.partner(this) == partner; });
    if (result == bonds_.end())
        return std::nullopt;
    return *result;
}

// Add specified SpeciesAngle to Atom
void SpeciesAtom::addAngle(SpeciesAngle &angle) { angles_.emplace_back(angle); }

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
void SpeciesAtom::addTorsion(SpeciesTorsion &torsion) { torsions_.emplace_back(torsion); }

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
void SpeciesAtom::addImproper(SpeciesImproper &improper) { impropers_.emplace_back(improper); }

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

// Set all scaled intramolecular interactions
void SpeciesAtom::setScaledInteractions()
{
    scaledInteractions_.clear();

    std::function<void(SpeciesAtom *, SpeciesAtom::ScaledInteraction, double, double)> addInteractionFunction =
        [&](SpeciesAtom *j, SpeciesAtom::ScaledInteraction scaledType, double elecScale, double srScale)
    {
        auto it =
            std::find_if(scaledInteractions_.begin(), scaledInteractions_.end(), [j](const auto &p) { return p.first == j; });
        if (it == scaledInteractions_.end())
            scaledInteractions_.emplace_back(j, ScaledInteractionDefinition{scaledType, elecScale, srScale});
    };

    /*
     * Add atoms to our scaledInteractions_ vector with appropriate scaling factors based on the intramolecular term in which
     * the atoms exist. We never overwrite a scaling factor from a "lower order" interaction (i.e. a torsion 1-4 can never
     * override a bond or an angle exclusion) so need to search the list for each partner atom every time.
     */

    // Bonds
    for (const auto &b : bonds_)
        addInteractionFunction(b.get().partner(this), SpeciesAtom::ScaledInteraction::Excluded, 0.0, 0.0);

    // Angles
    for (const auto &aRef : angles_)
    {
        auto &a = aRef.get();

        if (a.i() != this)
            addInteractionFunction(a.i(), ScaledInteraction::Excluded, 0.0, 0.0);
        if (a.j() != this)
            addInteractionFunction(a.j(), ScaledInteraction::Excluded, 0.0, 0.0);
        if (a.k() != this)
            addInteractionFunction(a.k(), ScaledInteraction::Excluded, 0.0, 0.0);
    }

    // Torsions
    for (const auto &tRef : torsions_)
    {
        auto &t = tRef.get();

        if (t.i() == this)
        {
            addInteractionFunction(t.j(), ScaledInteraction::Excluded, 0.0, 0.0);
            addInteractionFunction(t.k(), ScaledInteraction::Excluded, 0.0, 0.0);
            addInteractionFunction(t.l(), ScaledInteraction::Scaled, t.electrostatic14Scaling(), t.vanDerWaals14Scaling());
        }
        else if (t.l() == this)
        {
            addInteractionFunction(t.i(), ScaledInteraction::Scaled, t.electrostatic14Scaling(), t.vanDerWaals14Scaling());
            addInteractionFunction(t.j(), ScaledInteraction::Excluded, 0.0, 0.0);
            addInteractionFunction(t.k(), ScaledInteraction::Excluded, 0.0, 0.0);
        }
        else
        {
            addInteractionFunction(t.i(), ScaledInteraction::Excluded, 0.0, 0.0);
            addInteractionFunction(t.l(), ScaledInteraction::Excluded, 0.0, 0.0);
            if (t.j() != this)
                addInteractionFunction(t.j(), ScaledInteraction::Excluded, 0.0, 0.0);
            if (t.k() != this)
                addInteractionFunction(t.k(), ScaledInteraction::Excluded, 0.0, 0.0);
        }
    }
}

// Return scaling type and factors (electrostatic, van der Waals) to employ with specified Atom
SpeciesAtom::ScaledInteractionDefinition SpeciesAtom::scaling(const SpeciesAtom *j) const
{
    auto it = std::find_if(scaledInteractions_.begin(), scaledInteractions_.end(), [j](const auto &p) { return p.first == j; });
    if (it != scaledInteractions_.end())
        return it->second;
    return {SpeciesAtom::ScaledInteraction::NotScaled, 1.0, 1.0};
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

/*
 * Atom Environment Helpers
 */

EnumOptions<SpeciesAtom::AtomGeometry> SpeciesAtom::geometries()
{
    return EnumOptions<SpeciesAtom::AtomGeometry>("AtomGeometry", {
                                                                      {AtomGeometry::Unknown, "Unknown"},
                                                                      {AtomGeometry::Unbound, "Unbound"},
                                                                      {AtomGeometry::Terminal, "Terminal"},
                                                                      {AtomGeometry::Linear, "Linear"},
                                                                      {AtomGeometry::TShape, "TS"},
                                                                      {AtomGeometry::TrigonalPlanar, "TP"},
                                                                      {AtomGeometry::Tetrahedral, "Tet"},
                                                                      {AtomGeometry::SquarePlanar, "SqP"},
                                                                      {AtomGeometry::TrigonalBipyramidal, "TBP"},
                                                                      {AtomGeometry::Octahedral, "Oct"},
                                                                  });
}

// Calculate and return the geometry of this atom
SpeciesAtom::AtomGeometry SpeciesAtom::geometry() const { return geometry(this); }

// Return whether the geometry of this atom matches that specified
bool SpeciesAtom::isGeometry(SpeciesAtom::AtomGeometry geom) const { return geometry(this) == geom; }

// Calculate and return the geometry of the specified SpeciesAtom
SpeciesAtom::AtomGeometry SpeciesAtom::geometry(const SpeciesAtom *i)
{
    double angle, largest;
    SpeciesAtom *h, *j;
    const auto &bonds = i->bonds();

    // Work based on the number of bound atoms
    switch (bonds.size())
    {
        // 'Simple' cases first
        case (0):
            return AtomGeometry::Unbound;
        case (1):
            return AtomGeometry::Terminal;
        case (5):
            return AtomGeometry::TrigonalBipyramidal;
        case (6):
            return AtomGeometry::Octahedral;
            // For the remaining types, take averages of bond angles about the atom
        case (2):
            h = bonds[0].get().partner(i);
            j = bonds[1].get().partner(i);
            angle = NonPeriodicBox::literalAngleInDegrees(h->r(), i->r(), j->r());
            if (angle > 150.0)
                return AtomGeometry::Linear;
            else
                return AtomGeometry::Tetrahedral;
            break;
        case (3):
            h = bonds[0].get().partner(i);
            j = bonds[1].get().partner(i);
            angle = NonPeriodicBox::literalAngleInDegrees(h->r(), i->r(), j->r());
            largest = angle;
            j = bonds[2].get().partner(i);
            angle = NonPeriodicBox::literalAngleInDegrees(h->r(), i->r(), j->r());
            if (angle > largest)
                largest = angle;
            h = bonds[1].get().partner(i);
            angle = NonPeriodicBox::literalAngleInDegrees(h->r(), i->r(), j->r());
            if (angle > largest)
                largest = angle;
            if (largest > 150.0)
                return AtomGeometry::TShape;
            else if ((largest > 115.0) && (largest < 125.0))
                return AtomGeometry::TrigonalPlanar;
            else
                return AtomGeometry::Tetrahedral;
            break;
        case (4):
            // Two possibilities - tetrahedral or square planar. Tetrahedral will have an
            // average of all angles of ~ 109.5, for square planar (1/6) * (4*90 + 2*180) = 120
            angle = 0.0;
            for (auto n = 0; n < i->nBonds(); ++n)
            {
                h = bonds[n].get().partner(i);
                for (auto m = n + 1; m < i->nBonds(); ++m)
                {
                    j = bonds[m].get().partner(i);
                    angle += NonPeriodicBox::literalAngleInDegrees(h->r(), i->r(), j->r());
                }
            }
            angle /= 6.0;
            if ((angle > 100.0) && (angle < 115.0))
                return AtomGeometry::Tetrahedral;
            else
                return AtomGeometry::SquarePlanar;
    }

    return AtomGeometry::Unknown;
}

// Return whether the specified SpeciesAtom exists in the specified geometry
bool SpeciesAtom::isGeometry(const SpeciesAtom *i, AtomGeometry geom) { return geometry(i) == geom; }

// Guess and return oxidation state for the specified SpeciesAtom
int SpeciesAtom::guessOxidationState(const SpeciesAtom *i)
{
    /*
     * Approximate the OS of the supplied atom by considering its local environment.
     * We won't consider the whole molecule, and will assume the following rules in addition to the standard:
     *   - A singly-bound Oxygen is considered to be -1 (which effectively includes it's 'R' group
     *   - An R-group is considered to be +1
     */
    auto osBound = 0;

    // Keep track of the number of bound elements that are the same as our own, as a crude check for elemental environments
    // (OS == 0)
    auto nSameElement = 0;

    const auto &bonds = i->bonds();
    for (const SpeciesBond &bond : bonds)
    {
        auto Z = bond.partner(i)->Z();
        switch (Z)
        {
            // Group 1A - Alkali earth metals (includes Hydrogen)
            case (Elements::H):
            case (Elements::Li):
            case (Elements::Na):
            case (Elements::K):
            case (Elements::Rb):
            case (Elements::Cs):
            case (Elements::Fr):
                osBound += 1;
                break;
                // Group 2A - Alkaline earth metals
            case (Elements::Be):
            case (Elements::Mg):
            case (Elements::Ca):
            case (Elements::Sr):
            case (Elements::Ba):
            case (Elements::Ra):
                osBound += 1;
                break;
                // Oxygen
            case (Elements::O):
                if (bond.bondType() == SpeciesBond::DoubleBond)
                    osBound -= 2;
                else
                    osBound -= 1;
                break;
                // Halogens (F, Cl, Br, I)
            case (Elements::F):
            case (Elements::Cl):
            case (Elements::Br):
            case (Elements::I):
                osBound -= 1;
                break;
            default:
                break;
        }

        // Check for same element
        if (Z == i->Z())
            ++nSameElement;
    }

    // Return the negative of the bound OS, or zero if we were only bound to the same element as ourselves
    return (nSameElement == i->nBonds() ? 0 : -osBound);
}

// Express as a serialisable value
SerialisedValue SpeciesAtom::serialise() const
{
    return {{"index", userIndex()}, {"z", Z_}, {"r", r_}, {"charge", charge_}, {"type", atomType_->name().data()}};
}
void SpeciesAtom::deserialise(const SerialisedValue &node, CoreData &coreData)
{
    index_ = toml::find<int>(node, "index") - 1;
    Z_ = toml::find<Elements::Element>(node, "z");

    r_ = toml::find<Vec3<double>>(node, "r");

    charge_ = toml::find_or<double>(node, "charge", 0);

    Serialisable::optionalOn(node, "type",
                             [this, &coreData](const auto node)
                             {
                                 if (Z_ == Elements::Unknown)
                                     return;
                                 std::string name = toml::get<std::string>(node);
                                 atomType_ = coreData.findAtomType(name);
                                 if (atomType_ == nullptr)
                                 {
                                     atomType_ = coreData.addAtomType(Z_);
                                     atomType_->setName(name);
                                 }
                             });
}
