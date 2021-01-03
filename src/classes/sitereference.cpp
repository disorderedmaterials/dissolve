// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/sitereference.h"

SiteReference::SiteReference(SiteStack &siteStack, std::string_view name)
    : ListItem<SiteReference>(), siteStack_(siteStack), name_{name}
{
}

// Return referenced stack of sites
SiteStack &SiteReference::siteStack() const { return siteStack_; }

// Return name of this site stack (in the context of an Analyser)
std::string_view SiteReference::name() const { return name_; }
