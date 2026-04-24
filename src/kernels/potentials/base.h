// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/interactionPotential.h"
#include "kernels/potentials/types.h"
#include "keywords/store.h"

// Forward Declarations
class ConfigurationAtom;
class Box;
class LineParser;

// Extended Potential Base Class
class ExternalPotential
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
    std::vector<const AtomType *> targetAtomTypes_;
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
    void setTargetAtomTypes(const std::vector<const AtomType *> &targets);
    // Add target atom type
    void addTargetAtomType(const AtomType *target);
    // Return atom types targeted by the potential
    const std::vector<const AtomType *> &targetAtomTypes() const;
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
    virtual double energy(const ConfigurationAtom &i, const Box *box) const;
    // Calculate force on specified atom, summing in to supplied vector
    virtual void force(const ConfigurationAtom &i, const Box *box, Vector3 &f) const;

    /*
     * Read / Write
     */
    public:
    // Read data from specified LineParser
    bool deserialise(LineParser &parser, const CoreData &coreData);
    // Write data to specified LineParser
    bool serialise(LineParser &parser, std::string_view prefix) const;
};
