/*
    *** Keyword - Configuration RefList
    *** src/keywords/configurationreflist.cpp
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

#include "keywords/configurationreflist.h"
#include "base/lineparser.h"
#include "classes/configuration.h"
#include "classes/coredata.h"

// Constructor
ConfigurationRefListKeyword::ConfigurationRefListKeyword(RefList<Configuration> &references, int maxListSize)
    : KeywordData<RefList<Configuration> &>(KeywordBase::ConfigurationRefListData, references)
{
    maxListSize_ = maxListSize;
}

// Destructor
ConfigurationRefListKeyword::~ConfigurationRefListKeyword() {}

/*
 * Data
 */

// Determine whether current data is 'empty', and should be considered as 'not set'
bool ConfigurationRefListKeyword::isDataEmpty() const { return data_.nItems() == 0; }

// Return maximum number of Configurations to allow in the list
int ConfigurationRefListKeyword::maxListSize() const { return maxListSize_; }

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int ConfigurationRefListKeyword::minArguments() const { return 1; }

// Return maximum number of arguments accepted
int ConfigurationRefListKeyword::maxArguments() const { return 99; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool ConfigurationRefListKeyword::read(LineParser &parser, int startArg, const CoreData &coreData)
{
    // Each argument is the name of a Configuration that we will add to our list
    for (int n = startArg; n < parser.nArgs(); ++n)
    {
        Configuration *cfg = coreData.findConfiguration(parser.argsv(n));
        if (!cfg)
            return Messenger::error("Error defining Configuration targets - no Configuration named '{}' exists.\n",
                                    parser.argsv(n));

        // Check maximum size of list
        if ((maxListSize_ != -1) && (data_.nItems() >= maxListSize_))
            return Messenger::error("Too many configurations given to keyword. Maximum allowed is {}.\n", maxListSize_);

        // Check that the configuration isn't already in the list
        if (data_.contains(cfg))
            return Messenger::error("Configuration '{}' is already present in the list.\n", cfg->name());

        data_.append(cfg);
    }

    set_ = true;

    return true;
}

// Write keyword data to cfgecified LineParser
bool ConfigurationRefListKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix)
{
    // Loop over list of Configuration
    std::string configurationString;
    for (Configuration *cfg : data_)
        configurationString += fmt::format("  '{}'", cfg->name());

    if (!parser.writeLineF("{}{}  {}\n", prefix, keywordName, configurationString))
        return false;

    return true;
}

/*
 * Object Management
 */

// Prune any references to the supplied Configuration in the contained data
void ConfigurationRefListKeyword::removeReferencesTo(Configuration *cfg) { data_.remove(cfg); }
