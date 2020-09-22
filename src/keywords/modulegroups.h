// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "keywords/data.h"
#include "templates/reflist.h"

// Forward Declarations
class ModuleGroups;

// Keyword with ModuleGroups data
class ModuleGroupsKeyword : public KeywordData<ModuleGroups &>
{
    public:
    ModuleGroupsKeyword(ModuleGroups &groups);
    ~ModuleGroupsKeyword() = default;

    /*
     * Arguments
     */
    public:
    // Return minimum number of arguments accepted
    int minArguments() const;
    // Return maximum number of arguments accepted
    int maxArguments() const;
    // Parse arguments from supplied LineParser, starting at given argument offset
    bool read(LineParser &parser, int startArg, CoreData &coreData);
    // Write keyword data to specified LineParser
    bool write(LineParser &parser, std::string_view keywordName, std::string_view prefix);

    /*
     * Object Management
     */
    protected:
    // Prune any references to the supplied Module in the contained data
    void removeReferencesTo(Module *module);
};
