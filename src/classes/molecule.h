// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "templates/dynamicarrayobject.h"
#include "templates/vector3.h"
#include <memory>
#include <vector>

// Forward Declarations
class Atom;
class Box;
class Matrix3;
class Species;

// Molecule Definition
class Molecule : public DynamicArrayObject<Molecule>, public std::enable_shared_from_this<Molecule>
{
    public:
    Molecule();
    ~Molecule();

    /*
     * DynamicArrayObject Virtuals
     */
    public:
    // Clear object, ready for re-use
    void clear();

    /*
     * Contents
     */
    private:
    // Species that this Molecule represents
    const Species *species_;
    // Array of pointers to Atoms that belong to this Molecule (stored in Configuration)
    std::vector<std::shared_ptr<Atom>> atoms_;

    public:
    // Set Species that this Molecule represents
    void setSpecies(const Species *sp);
    // Return Species that this Molecule represents
    const Species *species() const;
    // Add Atom to Molecule
    void addAtom(std::shared_ptr<Atom> i);
    // Return size of Atom array
    int nAtoms() const;
    // Return Atoms array
    std::vector<std::shared_ptr<Atom>> &atoms();
    // Return Atoms array
    const std::vector<std::shared_ptr<Atom>> &atoms() const;
    // Return nth Atom pointer
    std::shared_ptr<Atom> atom(int n) const;

    /*
     * Manipulations
     */
    public:
    // Set centre of geometry
    void setCentreOfGeometry(const Box *box, const Vec3<double> newCentre);
    // Calculate and return centre of geometry
    Vec3<double> centreOfGeometry(const Box *box) const;
    // Transform molecule with supplied matrix, using centre of geometry as the origin
    void transform(const Box *box, const Matrix3 &transformationMatrix);
    // Transform selected atoms with supplied matrix, around specified origin
    void transform(const Box *box, const Matrix3 &transformationMatrix, const Vec3<double> &origin,
                   const std::vector<int> &targetAtoms);
    // Translate whole molecule by the delta specified
    void translate(const Vec3<double> delta);
    // Translate specified atoms by the delta specified
    void translate(const Vec3<double> &delta, const std::vector<int> &targetAtoms);
};
