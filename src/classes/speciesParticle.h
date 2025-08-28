// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include "math/vector3.h"
#include "templates/optionalRef.h"

// Forward Declarations
class CoreData;
class SpeciesAngle;
class SpeciesBond;
class SpeciesImproper;
class SpeciesTorsion;

class SpeciesParticle : public Serialisable<CoreData &>
{
    public:
    SpeciesParticle() = default;
    ~SpeciesParticle() = default;
    SpeciesParticle(SpeciesParticle &) = delete;
    SpeciesParticle(SpeciesParticle &&source) noexcept;
    SpeciesParticle &operator=(const SpeciesParticle &) = delete;
    SpeciesParticle &operator=(SpeciesParticle &&source) noexcept;

    protected:
    void move(SpeciesParticle &);

    public:
    // Set index (0->[N-1])
    void setIndex(int id);
    // Return index (0->[N-1])
    int index() const;
    // Return 'user' index (1->N)
    int userIndex() const;
    // Set whether the atom is currently selected
    void setSelected(bool selected);
    // Return whether the atom is currently selected
    bool isSelected() const;

    /*
     * Coordinate Manipulation
     */
    // Return coordinates (read-only)
    const Vector3 &r() const;
    // Set coordinate
    void setCoordinate(int index, double value);
    // Set coordinates
    void setCoordinates(double x, double y, double z);
    // Set coordinates (from Vec3)
    void setCoordinates(const Vector3 &newr);
    // Translate coordinates
    void translateCoordinates(const Vector3 &delta);

    /*
     * SpeciesIntra
     */
    // Add bond reference
    void addBond(SpeciesBond &b);
    // Remove bond reference
    void removeBond(SpeciesBond &b);
    // Return number of bonds
    int nBonds() const;
    // Return specified bond
    SpeciesBond &bond(int index);
    // Return bonds list
    const std::vector<std::reference_wrapper<SpeciesBond>> &bonds() const;
    // Return whether bond to specified atom exists
    OptionalReferenceWrapper<SpeciesBond> getBond(const SpeciesParticle *j);
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
    void addTorsion(SpeciesTorsion &torsion);
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

    protected:
    // Index in Species
    int index_{-1};
    // Whether the SpeciesParticle is currently selected
    bool selected_{false};

    // TODO: Make these private
    // Vector of bonds which this atom participates in
    std::vector<std::reference_wrapper<SpeciesBond>> bonds_;
    // Vector of angles which this atom participates in
    std::vector<std::reference_wrapper<SpeciesAngle>> angles_;
    // Vector of torsions which this atom participates in
    std::vector<std::reference_wrapper<SpeciesTorsion>> torsions_;
    // Vector of torsions which this atom participates in
    std::vector<std::reference_wrapper<SpeciesImproper>> impropers_;

    private:
    // Coordinates
    Vector3 r_{0.0, 0.0, 0.0};
};