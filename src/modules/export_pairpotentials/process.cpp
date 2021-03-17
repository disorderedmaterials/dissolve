// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/atom.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "main/dissolve.h"
#include "modules/export_pairpotentials/exportpp.h"

// Run main processing
bool ExportPairPotentialsModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    if (!pairPotentialFormat_.hasValidFileAndFormat())
        return Messenger::error("No valid file/format set for pair potential export.\n");

    // Only the pool master saves the data
    if (procPool.isMaster())
    {
        // Store the current (root) pair potential filename
        std::string rootPPName{pairPotentialFormat_.filename()};

        for (auto *pp = dissolve.pairPotentials().first(); pp != nullptr; pp = pp->next())
        {
            Messenger::print("Export: Writing pair potential file ({}) for {}-{}...\n", pairPotentialFormat_.description(),
                             pp->atomTypeNameI(), pp->atomTypeNameJ());

            // Generate filename
            pairPotentialFormat_.setFilename(fmt::format("{}-{}-{}.pp", rootPPName, pp->atomTypeNameI(), pp->atomTypeNameJ()));

            // Append pair potential
            if (!pairPotentialFormat_.exportData(pp))
            {
                Messenger::print("Export: Failed to export pair potential file '{}'.\n", pairPotentialFormat_.filename());
                pairPotentialFormat_.setFilename(rootPPName);
                procPool.decideFalse();
                return false;
            }

            procPool.decideTrue();
        }

        // Revert root name in FileAndFormat
        pairPotentialFormat_.setFilename(rootPPName);
    }
    else if (!procPool.decision())
        return false;

    return true;
}
