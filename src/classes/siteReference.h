// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <string>

// Forward Declarations
class SiteStack;

// Site Reference
class SiteReference
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
