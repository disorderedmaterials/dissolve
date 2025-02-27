// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "module/module.h"
#include "nodes/parameter.h"

// A link between two parameters
class ParameterLink
{

    private:
    // The constructor is private because it can only be constructed by the factory method
    ParameterLink(ParameterBase &source, ParameterBase &sink);
    ParameterBase &source_;
    ParameterBase &sink_;

    public:
    static std::optional<ParameterLink> link(ParameterBase &source, ParameterBase &sink);
    bool updateSource();
};
