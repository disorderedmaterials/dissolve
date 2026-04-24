// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/configurationAtom.h"
#include "math/vector3.h"
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
    Cell(int index = 0, Vector3i gridReference = Vector3i(), Vector3 centre = Vector3());
    ~Cell() = default;

    /*
     * Identity
     */
    private:
    // Grid reference
    Vector3i gridReference_;
    // Unique index
    int index_;
    // Real-space coordinates at the centre of this cell
    Vector3 centre_;

    public:
    // Return grid reference
    const Vector3i &gridReference() const;
    // Set unique index
    void setIndex(int id);
    // Return unique index
    int index() const;
    // Return real-space Cell centre
    const Vector3 &centre() const;

    /*
     * Contents
     */
    private:
    // Vector of Atoms contained in this Cell
    std::vector<ConfigurationAtom *> atoms_;

    public:
    // Return array of contained Atoms
    std::vector<ConfigurationAtom *> &atoms();
    const std::vector<ConfigurationAtom *> &atoms() const;
    // Return number of Atoms in array
    int nAtoms() const;
    // Add atom to Cell
    void addAtom(ConfigurationAtom *atom);
    // Remove Atom from Cell
    void removeAtom(ConfigurationAtom *atom);
    // Clear all atoms from cell
    void clearAtoms();
};
