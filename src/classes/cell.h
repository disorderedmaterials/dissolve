// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/atom.h"
#include "classes/atomVector.h"
#include "templates/vector3.h"
#include <set>
#include <vector>

// Forward Declarations
class Box;

/*
 * Cell Definition
 */
class Cell
{
    public:
    Cell(int index = 0, Vec3<int> gridReference = Vec3<int>(), Vec3<double> centre = Vec3<double>());
    ~Cell() = default;

    /*
     * Identity
     */
    private:
    // Grid reference
    Vec3<int> gridReference_;
    // Unique index
    int index_;
    // Real-space coordinates at the centre of this cell
    Vec3<double> centre_;

    public:
    // Return grid reference
    const Vec3<int> &gridReference() const;
    // Set unique index
    void setIndex(int id);
    // Return unique index
    int index() const;
    // Return real-space Cell centre
    const Vec3<double> &centre() const;

    /*
     * Contents
     */
    private:
    // Vector of Atoms contained in this Cell
    std::vector<AtomRef> atoms_;

    public:
    // Return array of contained Atoms
    std::vector<AtomRef> &atoms();
    const std::vector<AtomRef> &atoms() const;
    // Return number of Atoms in array
    int nAtoms() const;
    // Add atom to Cell
    void addAtom(AtomRef atom);
    // Remove Atom from Cell
    void removeAtom(AtomRef atom);
    // Clear all atoms from cell
    void clearAtoms();
};
