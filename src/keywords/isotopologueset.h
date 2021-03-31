// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/isotopologueset.h"
#include "keywords/data.h"
#include "templates/list.h"
#include "templates/reflist.h"

// Forward Declarations
/* none */

// Keyword with IsotopologueSet Data
class IsotopologueSetKeyword : public KeywordData<IsotopologueSet &>
{
    public:
    IsotopologueSetKeyword(IsotopologueSet &set);
    ~IsotopologueSetKeyword();

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
    // Prune any references to the supplied Isotopologue in the contained data
    void removeReferencesTo(Isotopologue *iso);
};
