// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "data/elements.h"
#include "templates/list.h"
#include "templates/listitem.h"
#include "templates/optionalref.h"
#include "templates/orderedpointerdataarray.h"
#include "templates/reflist.h"
#include "templates/vector3.h"
#include <memory>
#include <vector>

// Forward Declarations
class AtomType;
class Species;
class SpeciesAngle;
class SpeciesBond;
class SpeciesImproper;
class SpeciesTorsion;
class ProcessPool;

// SpeciesAtom Definition
class SpeciesAtom : public ListItem<SpeciesAtom>
{
    public:
    SpeciesAtom();
    ~SpeciesAtom() = default;

    /*
     * Properties
     */
    private:
    // Parent Species
    Species *parent_;
    // Atomic element
    Elements::Element Z_;
    // Coordinates
    Vec3<double> r_;
    // Charge (if contained in file)
    double charge_;
    // Assigned AtomType
    std::shared_ptr<AtomType> atomType_;
    // Index in Species
    int index_;
    // Whether the atom is currently selected
    bool selected_;

    public:
    // Set Species parent
    void setSpecies(Species *sp);
    // Return species parent
    const Species *species() const;
    // Set basic atom properties
    void set(Elements::Element Z, double rx, double ry, double rz, double q = 0.0);
    // Set basic atom properties
    void set(Elements::Element Z, const Vec3<double> r, double q = 0.0);
    // Set atomic element
    void setZ(Elements::Element Z);
    // Return atomic element
    Elements::Element Z() const;
    // Return coordinates (read-only)
    const Vec3<double> &r() const;
    // Set charge of Atom
    void setCharge(double charge);
    // Return charge of Atom
    double charge() const;
    // Set AtomType of Atom
    void setAtomType(std::shared_ptr<AtomType> at);
    // Return AtomType of Atom
    std::shared_ptr<AtomType> atomType() const;
    // Set List index (0->[N-1])
    void setIndex(int id);
    // Return List index (0->[N-1])
    int index() const;
    // Return 'user' index (1->N)
    int userIndex() const;
    // Set whether the atom is currently selected
    void setSelected(bool selected);
    // Return whether the atom is currently selected
    bool isSelected() const;
    // Copy properties from supplied Atom
    void copyProperties(const SpeciesAtom *source);

    /*
     * Intramolecular Information
     */
    private:
    // List of bonds which this atom participates in
    std::vector<std::reference_wrapper<SpeciesBond>> bonds_;
    // List of angles which this atom participates in
    std::vector<std::reference_wrapper<SpeciesAngle>> angles_;
    // List of torsions which this atom participates in
    std::vector<std::reference_wrapper<SpeciesTorsion>> torsions_;
    // List of torsions which this atom participates in
    std::vector<std::reference_wrapper<SpeciesImproper>> impropers_;
    // Ordered list of Atoms with scaled or excluded interactions
    OrderedPointerDataArray<SpeciesAtom, double> exclusions_;

    public:
    // Add bond reference
    void addBond(SpeciesBond &b);
    // Remove bond reference
    void removeBond(SpeciesBond &b);
    // Clear all bond references
    void clearBonds();
    // Return number of bonds
    int nBonds() const;
    // Return specified bond
    SpeciesBond &bond(int index);
    // Return bonds list
    const std::vector<std::reference_wrapper<SpeciesBond>> &bonds() const;
    // Return whether bond to specified atom exists
    OptionalReferenceWrapper<SpeciesBond> hasBond(SpeciesAtom *j);
    // Add specified Angle to Atom
    void addAngle(SpeciesAngle &angle);
    // Remove angle reference
    void removeAngle(SpeciesAngle &a);
    // Return the number of SpeciesAngles in which the Atom is involved
    int nAngles() const;
    // Return specified angle
    SpeciesAngle &angle(int index);
    // Return array of Angles in which the Atom is involved
    const std::vector<std::reference_wrapper<SpeciesAngle>> &angles() const;
    // Add specified SpeciesTorsion to Atom
    void addTorsion(SpeciesTorsion &torsion, double scaling14);
    // Remove torsion reference
    void removeTorsion(SpeciesTorsion &t);
    // Return the number of SpeciesTorsions in which the Atom is involved
    int nTorsions() const;
    // Return specified torsion
    SpeciesTorsion &torsion(int index);
    // Return array of Torsions in which the Atom is involved
    const std::vector<std::reference_wrapper<SpeciesTorsion>> &torsions() const;
    // Add specified SpeciesImproper to Atom
    void addImproper(SpeciesImproper &improper);
    // Remove improper reference
    void removeImproper(SpeciesImproper &t);
    // Return the number of SpeciesImpropers in which the Atom is involved
    int nImpropers() const;
    // Return specified improper
    SpeciesImproper &improper(int index);
    // Return array of Impropers in which the Atom is involved
    const std::vector<std::reference_wrapper<SpeciesImproper>> &impropers() const;
    // Return scaling factor to employ with specified Atom
    double scaling(const SpeciesAtom *j) const;

    /*
     * Coordinate Manipulation
     */
    public:
    // Set coordinate
    void setCoordinate(int index, double value);
    // Set coordinates
    void setCoordinates(double x, double y, double z);
    // Set coordinates (from Vec3)
    void setCoordinates(const Vec3<double> &newr);
    // Translate coordinates
    void translateCoordinates(const Vec3<double> &delta);
};
