/*
    *** Site Reference
    *** src/classes/sitereference.h
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

#include "templates/listitem.h"
#include <string>

// Forward Declarations
class SiteStack;

// Site Reference
class SiteReference : public ListItem<SiteReference>
{
    public:
    SiteReference(SiteStack &siteStack, std::string_view name);

    private:
    // Referenced stack of sites
    SiteStack &siteStack_;
    // Name of this site stack (in the context of an Analyser)
    std::string name_;

    public:
    // Return referenced stack of sites
    SiteStack &siteStack() const;
    // Return name of this site stack (in the context of an Analyser)
    std::string_view name() const;
};
