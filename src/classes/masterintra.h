// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/speciesintra.h"
#include "templates/array2d.h"

/*
 * MasterIntra Definition
 */
class MasterIntra : public SpeciesIntra
{
    public:
    MasterIntra(const std::string_view, const SpeciesIntra::InteractionType);
    MasterIntra(const MasterIntra &) = default;
    ~MasterIntra() override = default;

    /*
     * Basic Data
     */
    protected:
    // Name for interaction
    std::string name_;
    // Type of this interaction
    SpeciesIntra::InteractionType type_;

    public:
    // Set name for interaction (if relevant)
    void setName(std::string_view name);
    // Return name for interaction
    std::string_view name() const;
    // Set type of interaction
    void setType(SpeciesIntra::InteractionType type);

    /*
     * SpeciesIntra Virtuals
     */
    public:
    // Set up any necessary parameters
    void setUp() override;
    // Calculate and return fundamental frequency for the interaction
    double fundamentalFrequency(double reducedMass) const override;
    // Return type of this interaction
    SpeciesIntra::InteractionType type() const override;
};
