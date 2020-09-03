/*
    *** Keyword - Module RefList
    *** src/keywords/modulereflist.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "keywords/modulereflist.h"
#include "base/lineparser.h"
#include "classes/coredata.h"
#include "module/list.h"
#include "module/module.h"

ModuleRefListKeyword::ModuleRefListKeyword(RefList<Module> &references, int maxModules)
    : KeywordData<RefList<Module> &>(KeywordBase::ModuleRefListData, references)
{
    maxModules_ = maxModules;
}

ModuleRefListKeyword::ModuleRefListKeyword(RefList<Module> &references, const std::vector<std::string> &allowedModuleTypes,
                                           int maxModules)
    : KeywordData<RefList<Module> &>(KeywordBase::ModuleRefListData, references)
{
    moduleTypes_ = allowedModuleTypes;
    maxModules_ = maxModules;
}

ModuleRefListKeyword::~ModuleRefListKeyword() {}

/*
 * Data
 */

// Determine whether current data is 'empty', and should be considered as 'not set'
bool ModuleRefListKeyword::isDataEmpty() const { return data_.nItems() > 0; }

// Return the Module type(s) to allow
const std::vector<std::string> &ModuleRefListKeyword::moduleTypes() const { return moduleTypes_; }

// Return maximum number of Modules to allow in the list
int ModuleRefListKeyword::maxModules() const { return maxModules_; }

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int ModuleRefListKeyword::minArguments() const { return 1; }

// Return maximum number of arguments accepted
int ModuleRefListKeyword::maxArguments() const { return (maxModules_ == -1 ? 99 : maxModules_); }

// Parse arguments from supplied LineParser, starting at given argument offset
bool ModuleRefListKeyword::read(LineParser &parser, int startArg, CoreData &coreData)
{
    // Loop over arguments provided to the keyword
    for (int n = startArg; n < parser.nArgs(); ++n)
    {
        // Find specified Module by its unique name
        Module *module = coreData.findModule(parser.argsv(n));
        if (!module)
        {
            Messenger::error("No Module named '{}' exists.\n", parser.argsv(n));
            return false;
        }

        // Check the module's type (if a list has been specified)
        if ((moduleTypes_.size() > 0) && (std::find_if(moduleTypes_.cbegin(), moduleTypes_.cend(), [module](const auto &s) {
                                              return s == module->type();
                                          }) == moduleTypes_.cend()))
        {
            std::string allowedTypes;
            for (const auto &s : moduleTypes_)
                allowedTypes += allowedTypes.empty() ? s : ", " + s;

            Messenger::error("Module '{}' is of type '{}', and is not permitted in this list (allowed types = {}).\n",
                             parser.argsv(n), module->type(), allowedTypes);
            return false;
        }

        data_.append(module);
    }

    set_ = true;

    return true;
}

// Write keyword data to specified LineParser
bool ModuleRefListKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix)
{
    // Loop over list of referenced Modules
    for (Module *module : data_)
    {
        if (!parser.writeLineF("{}{}  '{}'\n", prefix, keywordName, module->uniqueName()))
            return false;
    }

    return true;
}

/*
 * Object Management
 */

// Prune any references to the supplied Module in the contained data
void ModuleRefListKeyword::removeReferencesTo(Module *module) { data_.remove(module); }
