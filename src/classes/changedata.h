// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "templates/list.h"
#include "templates/vector3.h"

// Forward Declarations
class Atom;
class Cell;

// Change Data
class ChangeData
{
    public:
    ChangeData();
    ~ChangeData() = default;

    /*
     * Target Data
     */
    private:
    // Atom
    std::shared_ptr<Atom> atom_;
    // Flag indicating whether Atom has moved
    bool moved_;
    // Stored coordinates of Atom
    Vec3<double> r_;
    // Stored Cell of Atom
    Cell *cell_;

    public:
    // Set target Atom
    void setAtom(std::shared_ptr<Atom> i);
    // Return target Atom
    std::shared_ptr<Atom> atom();
    // Return array index of stored Atom
    int atomArrayIndex() const;
    // Update stored position, and flag as moved
    void updatePosition();
    // Revert Atom to stored position
    void revertPosition();
    // Return whether Atom has moved
    bool hasMoved();
    // Return position vector
    Vec3<double> r() const;
};
