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

bool ParameterLink::updateSource()
{
    auto update = source_.runUpdate();
    if (!update)
        return false;
    return sink_.assign(&source_);
}

const ParameterBase &ParameterLink::sink() const { return sink_; }
const ParameterBase &ParameterLink::source() const { return source_; }
