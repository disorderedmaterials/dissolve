// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/parameterLink.h"

ParameterLink::ParameterLink(ParameterBase &sourceOutput, ParameterBase &targetInput)
    : sourceOutput_(sourceOutput), targetInput_(targetInput)
{
}

std::optional<ParameterLink> ParameterLink::link(ParameterBase &sourceOutput, ParameterBase &targetInput)
{
    // Check that types are compatible
    if (sourceOutput.type() != targetInput.type())
        return {};

    return ParameterLink(sourceOutput, targetInput);
}

bool ParameterLink::updateSource()
{
    // TODO
    //    auto update = sourceOutput_.runUpdate();
    //    if (!update)
    //        return false;
    return targetInput_.assign(&sourceOutput_);
}

const ParameterBase &ParameterLink::targetInput() const { return targetInput_; }

const ParameterBase &ParameterLink::sourceOutput() const { return sourceOutput_; }
