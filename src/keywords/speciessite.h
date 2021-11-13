// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"

// Forward Declarations
class SpeciesSite;

// Keyword with Site Data
class SpeciesSiteKeyword : public KeywordBase
{
    public:
    explicit SpeciesSiteKeyword(const SpeciesSite *&data, bool axesRequired = false);
    ~SpeciesSiteKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data
    const SpeciesSite *&data_;
    // Whether sites in the list must have a defined orientation
    bool axesRequired_;

    public:
    // Return reference to data
    const SpeciesSite *&data();
    const SpeciesSite *&data() const;
    // Return whether axes are required for the site
    bool axesRequired() const;

    /*
     * Arguments
     */
    public:
    // Return minimum number of arguments accepted
    int minArguments() const override;
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
    // Prune any references to the supplied SpeciesSite in the contained data
    void removeReferencesTo(SpeciesSite *spSite) override;
};
