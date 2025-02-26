// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/parameterLink.h"

ParameterLink::ParameterLink(ParameterBase &source, ParameterBase &sink) : source_(source), sink_(sink) {}

std::optional<ParameterLink> ParameterLink::link(ParameterBase &source, ParameterBase &sink)
{
    if (source.type() != sink.type())
        return {};
    return ParameterLink(source, sink);
}
