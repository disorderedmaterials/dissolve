// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/interactionPotential.h"
#include "kernels/potentials/types.h"
#include "keywords/store.h"
#include "templates/dataModelItem.h"

// Forward Declarations
class Atom;
class Box;
class LineParser;

// External Potential Base Class
class ExternalPotential : public DataModel::Modelable<ExternalPotential>
{
    public:
    explicit ExternalPotential(ExternalPotentialTypes::ExternalPotentialType type);
    virtual ~ExternalPotential() = default;
    // Create and return a copy of this potential
    virtual std::unique_ptr<ExternalPotential> duplicate() const;

    /*
     * Type
     */
    protected:
    // Additional potential type
    ExternalPotentialTypes::ExternalPotentialType type_;

    /*
     * Target Information
     */
    protected:
    // Atom indices targeted by the potential
    std::vector<int> targetAtomIndices_;
    // Atom types targeted by the potential
    std::vector<std::shared_ptr<AtomType>> targetAtomTypes_;
    // Species targeted by the potential
    std::vector<const Species *> targetSpecies_;

    public:
    ExternalPotentialTypes::ExternalPotentialType type() const;
    // Set atom indices targeted by the potential
    void setTargetAtomIndices(const std::vector<int> &targets);
    // Add target atom index
    void addTargetAtomIndex(int index);
    // Return atom indices targeted by the potential
    const std::vector<int> &targetAtomIndices() const;
    // Atom types targeted by the potential
    void setTargetAtomTypes(const std::vector<std::shared_ptr<AtomType>> &targets);
    // Add target atom type
    void addTargetAtomType(std::shared_ptr<AtomType> target);
    // Return atom types targeted by the potential
    const std::vector<std::shared_ptr<AtomType>> &targetAtomTypes() const;
    // Species targeted by the potential
    void setTargetSpecies(const std::vector<const Species *> &targets);
    // Add target species
    void addTargetSpecies(const Species *target);
    // Return species targeted by the potential
    const std::vector<const Species *> &targetSpecies() const;
    // Return functional form of the potential, as a string
    virtual const std::string formString() const = 0;
    // Return parameters of the potential, as a string
    virtual const std::string formParametersString() const = 0;

    /*
     * Keywords
     */
    protected:
    // Keywords for the potential
    KeywordStore keywords_;

    public:
    // Return keywords for this potential
    KeywordStore &keywords();
    const KeywordStore &keywords() const;

    /*
     * Potential Calculation
     */
    public:
    // Calculate energy on specified atom
    virtual double energy(const Atom &i, const Box *box) const;
    // Calculate force on specified atom, summing in to supplied vector
    virtual void force(const Atom &i, const Box *box, Vec3<double> &f) const;

    /*
     * I/O
     */
    public:
    // Read data from specified LineParser
    bool deserialise(LineParser &parser, const CoreData &coreData);
    // Write data to specified LineParser
    bool serialise(LineParser &parser, std::string_view prefix) const;
};
