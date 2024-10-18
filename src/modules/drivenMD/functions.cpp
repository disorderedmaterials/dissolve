// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "main/dissolve.h"
#include "module/context.h"
#include "modules/drivenMD/drivenMD.h"
#include "templates/algorithms.h"

// Calculate partial g(r) in serial with simple double-loop
PartialSet DrivenMDModule::calculateGRTestSerial(Configuration *cfg, PartialSet &partialSet)
{
    // Calculate radial distribution functions with a simple double loop, in serial then return the partial set!!!!!!!
    const auto *box = cfg->box();

    dissolve::for_each_pair(
        ParallelPolicies::seq, cfg->atoms().begin(), cfg->atoms().end(),
        [box, &partialSet](auto i, auto &ii, auto j, auto &jj)
        {
            if (&ii != &jj)
                partialSet.fullHistogram(ii.localTypeIndex(), jj.localTypeIndex()).bin(box->minimumDistance(ii.r(), jj.r()));
        });

    return partialSet;
}