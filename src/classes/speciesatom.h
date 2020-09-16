/*
    *** Species Atom Definition
    *** src/classes/speciesatom.h
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

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
class Element;
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
    ~SpeciesAtom();

    /*
     * Properties
     */
    private:
    // Parent Species
    Species *parent_;
    // Atomic Element
    Element *element_;
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
    void set(Element *element, double rx, double ry, double rz, double q = 0.0);
    // Set basic atom properties
    void set(Element *element, const Vec3<double> r, double q = 0.0);
    // Set atomic element
    void setElement(Element *el);
    // Return atomic element
    Element *element() const;
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
