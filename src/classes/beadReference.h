// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <string>

// Forward Declarations
class BeadStack;

// Bead Reference
class BeadReference
{
    public:
    BeadReference(BeadStack &beadStack, std::string_view name);

    private:
    // Referenced stack of beads
    BeadStack &beadStack_;
    // Name of this bead stack (in the context of an Analyser)
    std::string name_;

    public:
    // Return referenced stack of beads
    BeadStack &beadStack() const;
    // Return name of this bead stack (in the context of an Analyser)
    std::string_view name() const;
};
