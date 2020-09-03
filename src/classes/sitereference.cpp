/*
    *** Site Reference
    *** src/classes/sitereference.cpp
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

#include "classes/sitereference.h"

SiteReference::SiteReference(SiteStack &siteStack, std::string_view name)
    : ListItem<SiteReference>(), siteStack_(siteStack), name_{name}
{
}

// Return referenced stack of sites
SiteStack &SiteReference::siteStack() const { return siteStack_; }

// Return name of this site stack (in the context of an Analyser)
std::string_view SiteReference::name() const { return name_; }
