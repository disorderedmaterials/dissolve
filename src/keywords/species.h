// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"

// Forward Declarations
class Species;

// Keyword managing Species data
class SpeciesKeyword : public KeywordBase
{
    public:
    explicit SpeciesKeyword(const Species *&data);
    ~SpeciesKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data
    const Species *&data_;

    public:
    // Return reference to data
    const Species *&data();
    const Species *&data() const;

    /*
     * Arguments
     */
    public:
    // Deserialise from supplied LineParser, starting at given argument offset
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) override;
    // Serialise data to specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override;
    bool isDefault() const override;
    SerialisedValue serialise() const override;
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override;

    /*
     * Object Management
     */
    protected:
    // Prune any references to the supplied Species in the contained data
    void removeReferencesTo(Species *sp) override;
};
