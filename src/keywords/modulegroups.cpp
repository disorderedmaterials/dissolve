// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/modulegroups.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/coredata.h"
#include "module/group.h"
#include "module/groups.h"
#include "module/module.h"

ModuleGroupsKeyword::ModuleGroupsKeyword(ModuleGroups &groups) : KeywordData<ModuleGroups &>(groups) {}

/*
 * Arguments
 */

// Return maximum number of arguments accepted
int ModuleGroupsKeyword::maxArguments() const
{
    // Module name plus group name
    return 2;
}

// Deserialise from supplied LineParser, starting at given argument offset
bool ModuleGroupsKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    // Find specified Module by its unique name
    Module *module = coreData.findModule(parser.argsv(startArg));
    if (!module)
    {
        Messenger::error("No Module named '{}' exists.\n", parser.argsv(startArg));
        return false;
    }

    // Check the module's type
    if (!data_.moduleTypeIsAllowed(module->type()))
    {
        std::string allowedTypes;
        for (const auto &s : data_.allowedModuleTypes())
            allowedTypes += allowedTypes.empty() ? s : ", " + s;
        Messenger::error("Module '{}' is of type '{}', and is not permitted in these groups (allowed types = {}).\n",
                         parser.argsv(startArg), module->type(), allowedTypes);
        return false;
    }

    // If a second argument was given, this is the name of the group we should add the Module to. Otherwise, use the default
    data_.addModule(module, parser.hasArg(startArg + 1) ? parser.argsv(startArg + 1) : "Default");

    set_ = true;

    return true;
}

// Serialise data to specified LineParser
bool ModuleGroupsKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    for (auto &group : data_.groups())
    {
        for (auto *module : group->modules())
        {
            if (!parser.writeLineF("{}{}  '{}'  '{}'\n", prefix, keywordName, module->uniqueName(), group->name()))
                return false;
        }
    }

    return true;
}

/*
 * Object Management
 */

// Prune any references to the supplied Module in the contained data
void ModuleGroupsKeyword::removeReferencesTo(Module *module)
{
    // Loop over defined groups
    for (auto &group : data_.groups())
    {
        if (group->contains(module))
            group->remove(module);
    }
}
