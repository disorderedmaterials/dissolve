// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "base/enumoptions.h"
#include "neta/connection.h"
#include "neta/root.h"

// Forward Declarations
class Forcefield;
class SpeciesAtom;

// NETA Definition
class NETADefinition
{
    public:
    explicit NETADefinition(std::string_view definition = "");
    NETADefinition(const SpeciesAtom *i, int maxDepth = 1);
    ~NETADefinition() = default;

    /*
     * Data
     */
    private:
    // Root node of the definition
    std::shared_ptr<NETARootNode> rootNode_;
    // Original definition string
    std::string definitionString_;
    // Whether the current definition is valid
    bool valid_;

    public:
    // Return root node pointer
    std::shared_ptr<NETARootNode> rootNode();
    // Create definition from stored string
    bool create(const Forcefield *associatedFF = nullptr);
    // Set definition string and create definition
    bool create(std::string_view definition, const Forcefield *associatedFF = nullptr);
    // Create from specified atom and its connectivity
    bool create(const SpeciesAtom *i, int maxDepth = 1);
    // Set generating string
    void setDefinitionString(std::string_view definition);
    // Return original generating string
    std::string_view definitionString() const;
    // Return whether the definition is valid
    bool isValid() const;

    /*
     * Matching
     */
    public:
    // Return score of supplied atom for the definition
    int score(const SpeciesAtom *i) const;
    // Return whether the supplied atom matches the definition
    bool matches(const SpeciesAtom *i) const;
};
