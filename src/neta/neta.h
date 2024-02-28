// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include "neta/connection.h"
#include "neta/matchedGroup.h"
#include "templates/flags.h"

// Forward Declarations
class Forcefield;
class SpeciesAtom;

// NETA Definition
class NETADefinition
{
    public:
    // NETA Definition Creation Flags
    enum NETACreationFlags
    {
        ExplicitHydrogens, /* Explicitly include Hydrogen atoms in NETA definition string */
        IncludeRootElement /* Include the root element in NETA definition string */
    };
    explicit NETADefinition(std::string_view definition = "");
    NETADefinition(const SpeciesAtom *i, const std::optional<int> maxDepth = 1, const Flags<NETACreationFlags> &flags = {});
    ~NETADefinition() = default;

    /*
     * Data
     */
    private:
    // Root node of the definition
    std::shared_ptr<NETANode> rootNode_;
    // Original definition string
    std::string definitionString_;
    // Whether the current definition is valid
    bool valid_;
    // Identifiers (#)
    std::set<std::string> identifiers_;

    public:
    // Return root node pointer
    std::shared_ptr<NETANode> rootNode();
    // Create definition from stored string
    bool create(const Forcefield *associatedFF = nullptr);
    // Set definition string and create definition
    bool create(std::string_view definition, const Forcefield *associatedFF = nullptr);
    // Create from specified atom and its connectivity
    bool create(const SpeciesAtom *i, const std::optional<int> maxDepth = 1, const Flags<NETACreationFlags> &flags = {});
    // Set generating string
    void setDefinitionString(std::string_view definition);
    // Return original generating string
    std::string_view definitionString() const;
    // Return whether the definition is valid
    bool isValid() const;
    // Add identifiers
    void addIdentifier(std::string identifier);
    // Return identifiers from definition
    const std::set<std::string> &identifiers() const;

    /*
     * Matching
     */
    public:
    // Return score of supplied atom for the definition
    int score(const SpeciesAtom *i) const;
    // Return whether the supplied atom matches the definition
    bool matches(const SpeciesAtom *i) const;
    // Return the path of matched atoms, including the target atom, if the definition matches
    NETAMatchedGroup matchedPath(const SpeciesAtom *i) const;

    /*
     * Axes
     */
    public:
    // Return whether axes are defined or not
    bool hasAxes() const;
};
