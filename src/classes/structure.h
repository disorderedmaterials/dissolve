// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include "classes/atom.h"
#include "classes/bond.h"
#include "classes/box.h"
#include <vector>

// StructureAtom
class StructureAtom : public Atom<Bond<StructureAtom>>
{
    private:
    // Identifying name string
    std::string name_;

    public:
    // Copy the specified atom's data
    void copy(const StructureAtom &other)
    {
        name_ = other.name_;
        Z_ = other.Z_;
        r_ = other.r_;
        q_ = other.q_;
        index_ = other.index_;
    }
    // Set with name, and assume no element for now
    void set(const std::string &name, Vector3 r, double q = 0.0)
    {
        name_ = name;
        Atom::set(Elements::Unknown, r, q);
    }
};

// Structure
class Structure : public Serialisable<>
{
    public:
    Structure();
    virtual ~Structure() = default;
    Structure(const Structure &source);
    Structure &operator=(const Structure &source);
    // Clear Data
    void clear();

    /*
     * Atom
     */
    private:
    // Atoms in the structure
    std::vector<std::unique_ptr<StructureAtom>> atoms_;

    private:
    // Renumber atoms so they are sequential in the vector
    void renumberAtoms() const;

    public:
    // Add a new atom
    StructureAtom *addAtom(Elements::Element Z, Vector3 r, double q = 0.0);
    StructureAtom *addAtom(const std::string &name, Vector3 r, double q = 0.0);
    // Remove the specified atom
    void removeAtom(const StructureAtom *atom);
    void removeAtom(int index);
    // Remove a number of atoms
    void removeAtoms(const std::vector<const StructureAtom *> &atoms);
    // Return the number of atoms
    int nAtoms(AtomConstants::Presence withPresence = AtomConstants::Presence::Any) const;
    // Return atom at index provided
    StructureAtom *atom(int i);
    // Return atoms
    const std::vector<std::unique_ptr<StructureAtom>> &atoms() const;
    std::vector<std::unique_ptr<StructureAtom>> &atoms();

    /*
     * Connectivity
     */
    private:
    // Connectivity within the structure
    std::vector<std::unique_ptr<Bond<StructureAtom>>> bonds_;

    public:
    // Add new bond between specified atoms
    Bond<StructureAtom> *addBond(int i, int j);
    Bond<StructureAtom> *addBond(StructureAtom *i, StructureAtom *j);
    // Remove bond
    void removeBond(StructureAtom *i, StructureAtom *j);
    void removeBond(Bond<StructureAtom> *bond);
    // Return vector of defined bonds
    std::vector<std::unique_ptr<Bond<StructureAtom>>> &bonds();
    const std::vector<std::unique_ptr<Bond<StructureAtom>>> &bonds() const;
    // Return whether bond between atoms exists
    bool hasBond(const StructureAtom *i, const StructureAtom *j) const;
    // Return the bond between the specified Atoms
    Bond<StructureAtom> *getBond(StructureAtom *i, StructureAtom *j);
    Bond<StructureAtom> *getBond(const StructureAtom *i, const StructureAtom *j) const;
    // Clear bonds
    void clearBonds();

    /*
     * Box Definition
     */
    private:
    // Periodic Box
    std::unique_ptr<Box> box_;

    public:
    // Return periodic box
    const Box *box() const;
    // Remove Box definition and revert to single image
    void removeBox();
    // Create Box definition with specified lengths and angles
    void createBox(const Vector3 lengths, const Vector3 angles, bool nonPeriodic = false);
    // Create Box definition from axes matrix
    void createBox(const Matrix3 &axes);

    /*
     * Serialisation
     */
    public:
    // Express as a serialisable value
    void serialise(std::string tag, SerialisedValue &target) const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node);
};
