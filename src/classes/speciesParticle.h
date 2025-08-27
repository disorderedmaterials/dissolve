// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once 

#include "base/serialiser.h"
#include "math/vector3.h"

// Forward Declarations 
class CoreData; 

class SpeciesParticle : public Serialisable<CoreData&>
{
    public:
    SpeciesParticle() = default;
    ~SpeciesParticle() = default;
    SpeciesParticle(SpeciesParticle&) = delete;
    SpeciesParticle(SpeciesParticle &&source) noexcept;
    SpeciesParticle &operator=(const SpeciesParticle &) = delete;
    SpeciesParticle &operator=(SpeciesParticle &&source) noexcept;

    void move(SpeciesParticle &);

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

    protected:
    // Index in Species 
    int index_{-1};
    // Whether the SpeciesParticle is currently selected 
    bool selected_{false};      

    private:
    // Coordinates 
    Vector3 r_ { 0.0, 0.0, 0.0 };
};