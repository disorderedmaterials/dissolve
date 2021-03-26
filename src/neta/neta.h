// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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
    NETADefinition();
    ~NETADefinition() = default;

    /*
     * Data
     */
    private:
    // Root node of the definition
    std::shared_ptr<NETARootNode> rootNode_;
    // Original definition string
    std::string definitionString_;

    public:
    // Return root node pointer
    std::shared_ptr<NETARootNode> rootNode();
    // Create definition from stored string
    bool create(const Forcefield *associatedFF = nullptr);
    // Set definition string and create definition
    bool create(std::string_view definition, const Forcefield *associatedFF = nullptr);
    // Set generating string
    void setDefinitionString(std::string_view definition);
    // Return original generating string
    std::string_view definitionString() const;

    /*
     * Matching
     */
    private:
    // Current SpeciesAtom being typed
    SpeciesAtom *targetAtom_;

    public:
    // Check supplied atom to see if it matches this NETA description
    int score(const SpeciesAtom *i) const;
};
