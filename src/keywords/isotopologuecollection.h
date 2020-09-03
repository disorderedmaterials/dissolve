/*
    *** Keyword - Isotopologue Collection
    *** src/keywords/isotopologuecollection.h
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

#pragma once

#include "classes/isotopologuecollection.h"
#include "keywords/data.h"
#include "templates/list.h"
#include "templates/reflist.h"

// Forward Declarations
/* none */

// Keyword with IsotopologueCollection Data
class IsotopologueCollectionKeyword : public KeywordData<IsotopologueCollection &>
{
    public:
    IsotopologueCollectionKeyword(IsotopologueCollection &collection, const RefList<Configuration> &allowedConfigurations);
    ~IsotopologueCollectionKeyword();

    /*
     * Allowed Configurations
     */
    private:
    // Allowed Configurations, to which the IsotopologueCollection may refer
    const RefList<Configuration> &allowedConfigurations_;

    public:
    // Return allowed Configurations, to which the IsotopologueCollection may refer
    const RefList<Configuration> &allowedConfigurations() const;

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
    // Prune any references to the supplied Species in the contained data
    void removeReferencesTo(Species *sp);
    // Prune any references to the supplied Isotopologue in the contained data
    void removeReferencesTo(Isotopologue *iso);
};
