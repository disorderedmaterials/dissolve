// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "keywords/data.h"
#include "templates/list.h"

// Forward Declarations
class SpeciesSite;

// Keyword with Site Data
class SpeciesSiteKeyword : public KeywordData<SpeciesSite *>
{
    public:
    SpeciesSiteKeyword(SpeciesSite *site = nullptr, bool axesRequired = false);
    ~SpeciesSiteKeyword();

    /*
     * Specification
     */
    private:
    // Whether sites in the list must have a defined orientation
    bool axesRequired_;

    public:
    // Return whether axes are required for the site
    bool axesRequired() const;

    /*
     * Arguments
     */
    public:
    // Return minimum number of arguments accepted
    int minArguments() const override;
    // Return maximum number of arguments accepted
    int maxArguments() const override;
    // Parse arguments from supplied LineParser, starting at given argument offset
    bool read(LineParser &parser, int startArg, const CoreData &coreData) override;
    // Write keyword data to specified LineParser
    bool write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override;

    /*
     * Object Management
     */
    protected:
    // Prune any references to the supplied Species in the contained data
    void removeReferencesTo(Species *sp);
    // Prune any references to the supplied SpeciesSite in the contained data
    void removeReferencesTo(SpeciesSite *spSite);
};
