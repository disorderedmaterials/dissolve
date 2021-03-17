// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/atom.h"
#include "templates/vector3.h"
#include <set>
#include <vector>

// Forward Declarations
class Box;
class CellNeighbour;

/*
 * Cell Definition
 */
class Cell
{
    public:
    Cell(int index, Vec3<int> gridReference, Vec3<double> centre);
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
    // Set grid reference
    void setGridReference(int x, int y, int z);
    // Return grid reference
    const Vec3<int> &gridReference() const;
    // Set unique index
    void setIndex(int id);
    // Return unique index
    int index() const;
    // Set real-space Cell centre
    void setCentre(Vec3<double> r);
    // Return real-space Cell centre
    const Vec3<double> &centre() const;

    /*
     * Contents
     */
    private:
    // Array of Atoms contained in this Cell
    std::vector<std::shared_ptr<Atom>> atoms_;

    public:
    // Return array of contained Atoms
    std::vector<std::shared_ptr<Atom>> &atoms();
    const std::vector<std::shared_ptr<Atom>> &atoms() const;
    // Return number of Atoms in array
    int nAtoms() const;
    // Add atom to Cell
    void addAtom(const std::shared_ptr<Atom> &atom);
    // Remove Atom from Cell
    void removeAtom(const std::shared_ptr<Atom> &atom);

    /*
     * Neighbours
     */
    private:
    // Arrays of neighbouring cells, within the defined potential cutoff (from anywhere in the Cell)
    std::vector<Cell *> cellNeighbours_, mimCellNeighbours_;
    // Array of all neighbouring cells
    std::vector<Cell *> allCellNeighbours_;

    public:
    // Add Cell neighbours
    void addCellNeighbours(std::vector<Cell *> &nearNeighbours, std::vector<Cell *> &mimNeighbours);
    // Return adjacent Cell neighbour list
    const std::vector<Cell *> &cellNeighbours() const;
    // Return list of Cell neighbours requiring minimum image calculation
    const std::vector<Cell *> &mimCellNeighbours() const;
    // Return if the specified Cell requires minimum image calculation
    bool mimRequired(const Cell *otherCell) const;
    // Return list of all Cell neighbours
    const std::vector<Cell *> &allCellNeighbours() const;
};
