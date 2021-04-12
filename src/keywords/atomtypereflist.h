// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/atomtype.h"
#include "keywords/data.h"
#include "templates/reflist.h"
#include <memory>
#include <vector>

// Forward Declarations
class Configuration;

// Keyword with AtomType RefList Data
class AtomTypeRefListKeyword : public KeywordData<std::vector<std::shared_ptr<AtomType>> &>
{
    public:
    AtomTypeRefListKeyword(std::vector<std::shared_ptr<AtomType>> &targetRefList);
    ~AtomTypeRefListKeyword();

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
    // Prune any references to the supplied AtomType in the contained data
    void removeReferencesTo(std::shared_ptr<AtomType> at);
};
