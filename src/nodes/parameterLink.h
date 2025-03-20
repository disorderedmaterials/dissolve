// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "module/module.h"
#include "nodes/parameter.h"

// A link between two parameters
class ParameterLink
{

    // This class uses references instead of pointers for two reasons:
    // 1) Neither end of the link should EVER be null
    // 2) The link itself is immutable.  You can create links and
    // destroy links, but *never mutate links*.
    //
    // The immutability is important because the factory ensures that
    // all links are valid at creation time.  By keeping the links
    // immutable, that means that all links are valid at all times.
    // This enables us to skip a great deal of error checking when
    // processing a chain of links.

    private:
    // The constructor is private because it can only be constructed
    // by the factory method
    ParameterLink(ParameterBase &source, ParameterBase &sink);
    ParameterBase &source_;
    ParameterBase &sink_;

    public:
    // A factory method to create a link between two parameters.
    // Returns the null option if the link cannot be created.
    static std::optional<ParameterLink> link(ParameterBase &source, ParameterBase &sink);
    const ParameterBase &sink() const;
    const ParameterBase &source() const;
    bool updateSource();
};
