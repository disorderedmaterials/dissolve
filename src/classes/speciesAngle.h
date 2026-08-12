// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include "classes/angleFunctions.h"
#include "classes/speciesIntra.h"
#include <vector>

// Forward Declarations
class SpeciesAtom;
class Species;

// SpeciesAngle Definition
class SpeciesAngle : public SpeciesIntra<SpeciesAngle, AngleFunctions>
{
    public:
    SpeciesAngle();
    SpeciesAngle(Species *parent, SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k);
    ~SpeciesAngle() override;

    /*
     * SpeciesAtom Information
     */
    private:
    // First SpeciesAtom in interaction
    SpeciesAtom *i_{nullptr};
    // Second (central) SpeciesAtom in interaction
    SpeciesAtom *j_{nullptr};
    // Third SpeciesAtom in interaction
    SpeciesAtom *k_{nullptr};

    public:
    // Return first SpeciesAtom
    SpeciesAtom *i() const;
    // Return second (central) SpeciesAtom
    SpeciesAtom *j() const;
    // Return third SpeciesAtom
    SpeciesAtom *k() const;
    // Return vector of involved atoms
    std::vector<const SpeciesAtom *> atoms() const override;
    // Return whether SpeciesAtom match those specified
    bool matches(const SpeciesAtom *i, const SpeciesAtom *j, const SpeciesAtom *k) const;

    /*
     * Interaction Parameters
     */
    public:
    // Return energy for specified angle theta (in radians)
    double energy(double theta) const;
    // Return force multiplier for specified angle theta (in radians)
    double force(double theta) const;

    /*
     * Serialisation
     */
    public:
    // Express as a serialisable value
    void serialise(std::string tag, SerialisedValue &target) const;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node);
};

// CommonAngle Definition
class CommonAngle : public SpeciesAngle
{
    public:
    explicit CommonAngle(std::string_view name) : SpeciesAngle(), name_{name} {};
    using SpeciesIntra<SpeciesAngle, AngleFunctions>::deserialise;

    /*
     * Identifying Name
     */
    private:
    // Identifying name
    std::string name_;

    public:
    // Set identifying name
    void setName(std::string_view name) override { name_ = name; }
    // Return identifying name
    std::string_view name() const override { return name_; }
};
