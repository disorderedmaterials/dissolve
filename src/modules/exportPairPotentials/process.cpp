// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/atom.h"
#include "classes/atomType.h"
#include "classes/box.h"
#include "main/dissolve.h"
#include "modules/exportPairPotentials/exportPairPotentials.h"

// Run main processing
Module::ExecutionResult ExportPairPotentialsModule::process(const ModuleContext& moduleContext)
{
    if (!pairPotentialFormat_.hasFilename())
    {
        Messenger::error("No valid file/format set for pair potential export.\n");
        return ExecutionResult::Failed;
    }

    // Only the pool master saves the data
    if (procPool.isMaster())
    {
        // Store the current (root) pair potential filename
        std::string rootPPName{pairPotentialFormat_.filename()};

        for (auto &pp : dissolve.pairPotentials())
        {
            Messenger::print("Export: Writing pair potential file ({}) for {}-{}...\n",
                             pairPotentialFormat_.formatDescription(), pp->atomTypeNameI(), pp->atomTypeNameJ());

            // Generate filename
            pairPotentialFormat_.setFilename(fmt::format("{}-{}-{}.pp", rootPPName, pp->atomTypeNameI(), pp->atomTypeNameJ()));

            // Append pair potential
            if (!pairPotentialFormat_.exportData(pp.get()))
            {
                Messenger::print("Export: Failed to export pair potential file '{}'.\n", pairPotentialFormat_.filename());
                pairPotentialFormat_.setFilename(rootPPName);
                procPool.decideFalse();
                return ExecutionResult::Failed;
            }

            procPool.decideTrue();
        }

        // Revert root name in FileAndFormat
        pairPotentialFormat_.setFilename(rootPPName);
    }
    else if (!procPool.decision())
        return ExecutionResult::Failed;

    return ExecutionResult::Success;
}
