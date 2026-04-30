// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include "classes/atom.h"
#include "classes/box.h"
#include <vector>

// Forward Declarations
class StructureBond;

// StructureAtom
class StructureAtom : public Atom
{
    private:
    // Bonds to this atom
    std::vector<StructureBond *> bonds_;

    public:
    // Return bonds to this atom
    const std::vector<StructureBond *> &bonds() const { return bonds_; }
    // Add a bond to this atom
    void addBond(StructureBond *bond) { bonds_.push_back(bond); }
    // Remove bond from atom
    void removeBond(StructureBond *bond) { bonds_.erase(std::remove(bonds_.begin(), bonds_.end(), bond)); }
};

// StructureBond
class StructureBond : public Serialisable<>
{
    public:
    StructureBond(StructureAtom *i, StructureAtom *j) : i_(i), j_(j)
    {
        i_->addBond(this);
        j_->addBond(this);
    }
    virtual ~StructureBond() = default;

    private:
    StructureAtom *i_{nullptr}, *j_{nullptr};

    public:
    // Return the involved atoms
    StructureAtom *i() const { return i_; }
    StructureAtom *j() const { return j_; }
    // Return the 'other' atom
    StructureAtom *partner(const StructureAtom *atom) const { return (atom == i_ ? j_ : i_); }
    // Return whether the bond's atoms match those provided
    bool isBetween(const StructureAtom *i, const StructureAtom *j) const
    {
        return (i == i_ && j == j_) || (i == j_ && j == i_);
    }

    /*
     * Serialisation
     */
    public:
    // Express as a serialisable value
    void serialise(std::string tag, SerialisedValue &target) const override
    {
        target[tag] = {{"i", i_->index()}, {"j", j_->index()}};
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
    // Remove the specified atom
    void removeAtom(const StructureAtom *atom);
    // Remove a number of atoms
    void removeAtoms(const std::vector<const StructureAtom *> &atoms);
    // Return the number of atoms
    int nAtoms(Atom::Presence withPresence = Atom::Presence::Any) const;
    // Return atoms
    const std::vector<std::unique_ptr<StructureAtom>> &atoms() const;
    std::vector<std::unique_ptr<StructureAtom>> &atoms();

    /*
     * Connectivity
     */
    private:
    // Connectivity within the structure
    std::vector<std::unique_ptr<StructureBond>> bonds_;

    public:
    // Add new bond between specified atoms
    StructureBond *addBond(int i, int j);
    StructureBond *addBond(StructureAtom *i, StructureAtom *j);
    // Remove bond
    void removeBond(StructureAtom *i, StructureAtom *j);
    void removeBond(StructureBond *bond);
    // Return vector of defined bonds
    std::vector<std::unique_ptr<StructureBond>> &bonds();
    const std::vector<std::unique_ptr<StructureBond>> &bonds() const;
    // Return whether bond between atoms exists
    bool hasBond(const StructureAtom *i, const StructureAtom *j) const;
    // Return the bond between the specified Atoms
    StructureBond *getBond(StructureAtom *i, StructureAtom *j);
    StructureBond *getBond(const StructureAtom *i, const StructureAtom *j) const;

    /*
     * Operations
     */
    private:
    // Recursively add atoms along any path from the specified one, ignoring the bond(s) provided
    void getIndicesRecursive(std::vector<int> &indices, int index, StructureBond *exclude,
                             StructureBond *excludeToo = nullptr) const;

    public:
    // Return the fragment (vector of indices) containing the specified atom, optionally ignoring paths along the bond(s)
    // provided
    std::vector<int> fragment(int startIndex, StructureBond *exclude = nullptr, StructureBond *excludeToo = nullptr) const;

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
