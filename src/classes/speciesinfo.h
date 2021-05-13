// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "templates/listitem.h"

// Forward Declarations
class Species;

/*
 * Species Information
 */
class SpeciesInfo
{
    public:
    SpeciesInfo();
    ~SpeciesInfo() = default;

    /*
     * Species Target
     */
    private:
    // Target Species
    const Species *species_;
    // Population of the Species
    int population_;

    public:
    // Set target Species
    void setSpecies(const Species *sp);
    // Return target Species
    const Species *species();
    // Zero the population of the Species
    void zeroPopulation();
    // Add to population of the Species
    void addPopulation(int pop);
    // Return population of the Species
    int population() const;
};
