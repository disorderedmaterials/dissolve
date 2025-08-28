// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include "base/serialiser.h"
#include "classes/angleFunctions.h"
#include "classes/speciesIntra.h"
#include <map>
#include <vector>

// Forward Declarations
class SpeciesParticle;
class Species;
class CoreData;

// SpeciesAngle Definition
class SpeciesAngle : public SpeciesIntra<SpeciesAngle, AngleFunctions>
{
    public:
    SpeciesAngle();
    SpeciesAngle(SpeciesParticle *i, SpeciesParticle *j, SpeciesParticle *k);
    ~SpeciesAngle() override = default;
    SpeciesAngle(SpeciesAngle &source);
    SpeciesAngle(SpeciesAngle &&source) noexcept;
    SpeciesAngle &operator=(const SpeciesAngle &source);
    SpeciesAngle &operator=(SpeciesAngle &&source) noexcept;

    /*
     * SpeciesParticle Information
     */
    private:
    // First SpeciesParticle in interaction
    SpeciesParticle *i_{nullptr};
    // Second (central) SpeciesParticle in interaction
    SpeciesParticle *j_{nullptr};
    // Third SpeciesParticle in interaction
    SpeciesParticle *k_{nullptr};

    private:
    // Detach from current atoms
    void detach();

    public:
    // Assign the three atoms in the angle
    void assign(SpeciesParticle *i, SpeciesParticle *j, SpeciesParticle *k);
    // Rewrite SpeciesParticle pointer
    void switchAtom(const SpeciesParticle *oldPtr, SpeciesParticle *newPtr);
    // Return first SpeciesParticle
    SpeciesParticle *i() const;
    // Return second (central) SpeciesParticle
    SpeciesParticle *j() const;
    // Return third SpeciesParticle
    SpeciesParticle *k() const;
    // Return vector of involved atoms
    std::vector<const SpeciesParticle *> atoms() const override;
    // Return index (in parent Species) of first SpeciesParticle
    int indexI() const;
    // Return index (in parent Species) of second (central) SpeciesParticle
    int indexJ() const;
    // Return index (in parent Species) of third SpeciesParticle
    int indexK() const;
    // Return index (in parent Species) of nth SpeciesParticle
    int index(int n) const;
    // Return whether SpeciesParticle match those specified
    bool matches(const SpeciesParticle *i, const SpeciesParticle *j, const SpeciesParticle *k) const;
    // Return whether all atoms in the interaction are currently selected
    bool isSelected() const;

    /*
     * Interaction Parameters
     */
    public:
    // Return energy for specified angle theta (in radians)
    double energy(double theta) const;
    // Return force multiplier for specified angle theta (in radians)
    double force(double theta) const;

    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, CoreData &coreData);
};

// MasterAngle Definition
class MasterAngle : public SpeciesAngle
{
    public:
    explicit MasterAngle(std::string_view name) : SpeciesAngle(), name_{name} {};
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
    std::string_view name() const override { return name_; };
};
