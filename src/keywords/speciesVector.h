// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"

// Forward Declarations
class Species;

// Keyword managing vector of Species
class SpeciesVectorKeyword : public KeywordBase
{
    public:
    SpeciesVectorKeyword(std::vector<const Species *> &data);
    ~SpeciesVectorKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data vector
    std::vector<const Species *> &data_;

    public:
    // Return reference to data vector
    std::vector<const Species *> &data();
    const std::vector<const Species *> &data() const;

    /*
     * Arguments
     */
    public:
    // Return maximum number of arguments accepted
    std::optional<int> maxArguments() const override;
    // Deserialise from supplied LineParser, starting at given argument offset
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) override;
    // Serialise data to specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override;

    /*
     * Object Management
     */
    protected:
    // Prune any references to the supplied Species in the contained data
    void removeReferencesTo(Species *sp) override;

    public:
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override;
    // Has not changed from initial value
    bool isDefault() const override;
    // Express as a serialisable value
    SerialisedValue serialise() const override;
};
