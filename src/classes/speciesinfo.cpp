// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/speciesinfo.h"
#include "base/sysfunc.h"

SpeciesInfo::SpeciesInfo()
{
    species_ = nullptr;
    population_ = 0;
}

SpeciesInfo::~SpeciesInfo() {}

/*
 * Species Target
 */

// Set target Species
void SpeciesInfo::setSpecies(Species *sp) { species_ = sp; }

// Return target Species
Species *SpeciesInfo::species() { return species_; }

// Zero the population of the Species
void SpeciesInfo::zeroPopulation() { population_ = 0; }

// Add to the population of the Species
void SpeciesInfo::addPopulation(int pop) { population_ += pop; }

// Return relative population of the Species
int SpeciesInfo::population() const { return population_; }
