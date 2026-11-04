// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "math/vector3.h"
#include "math/vector3i.h"
#include <type_traits>
#include <vector>

// Forward Declarations
class AtomBase;
class Box;

/*
 * Cell Definition
 */
template <typename T> class Cell
{
    public:
    Cell(int index = 0, Vector3i gridReference = Vector3i(), Vector3 centre = Vector3())
        : gridReference_(gridReference), index_(index), centre_(centre)
    {
    }
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
    const Vector3i &gridReference() const { return gridReference_; }
    // Set unique index
    void setIndex(int id)
    {
        assert(index_ == -1);
        index_ = id;
    }
    // Return unique index
    int index() const { return index_; }
    // Return real-space Cell centre
    const Vector3 &centre() const { return centre_; }

    /*
     * Contents
     */
    private:
    // Vector of Atoms contained in this Cell
    std::vector<T *> atoms_;

    public:
    // Return array of contained Atoms
    std::vector<T *> &atoms() { return atoms_; }
    const std::vector<T *> &atoms() const { return atoms_; }
    // Return number of Atoms in array
    int nAtoms() const { return atoms_.size(); }
    // Add atom to Cell
    void addAtom(T *atom)
    {
        assert(atom);
        assert(!atom->cell());

        atoms_.push_back(atom);
        atom->setCell(this);
    }
    // Remove Atom from Cell
    void removeAtom(T *atom)
    {
        auto it = std::find(atoms_.begin(), atoms_.end(), atom);
        assert(it != atoms_.end());
        (*it)->setCell(nullptr);
        atoms_.erase(it);
    }

    // Clear all atoms from cell
    void clearAtoms() { atoms_.clear(); }

    Cell<AtomBase> *toBase()
        requires std::is_base_of_v<AtomBase, T>
    {
        return (Cell<AtomBase> *)(this);
    }

    // Lower contents to AtomBase pointers
    const Cell<AtomBase> *toBase() const
        requires std::is_base_of_v<AtomBase, T>
    {
        return (Cell<AtomBase> *)(this);
    }
};
