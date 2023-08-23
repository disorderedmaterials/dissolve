// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "modules/compare/compare.h"

Module::ExecutionResult CompareModule::process(ModuleContext &moduleContext)
{
    for (auto &[data1, data2] : compareData1D_)
    {
        auto error = Error::error(errorType_, data1, data2);
    }

    return ExecutionResult::Success;
}
