// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "main/dissolve.h"
#include "modules/refine/refine.h"
#include "templates/algorithms.h"

// Create / update delta S(Q) information
void RefineModule::updateDeltaSQ(GenericList &processingData, OptionalReferenceWrapper<const Array2D<Data1D>> optCalculatedSQ,
                               OptionalReferenceWrapper<const Array2D<Data1D>> optEstimatedSQ)
{
    // Find the relevant data if we were not provided them
    if (!optCalculatedSQ)
        optCalculatedSQ = processingData.valueIf<Array2D<Data1D>>("UnweightedSQ", name_);
    if (!optCalculatedSQ)
        return;
    if (!optEstimatedSQ)
        optEstimatedSQ = processingData.valueIf<Array2D<Data1D>>("EstimatedSQ", name_);
    if (!optEstimatedSQ)
        return;

    const auto &calculatedSQ = optCalculatedSQ->get();
    const auto &estimatedSQ = optEstimatedSQ->get();
    assert(calculatedSQ.nRows() == estimatedSQ.nRows() && calculatedSQ.nColumns() == estimatedSQ.nColumns());

    // Realise the DeltaSQ array
    auto [deltaSQ, status] = processingData.realiseIf<Array2D<Data1D>>("DeltaSQ", name_, GenericItem::ItemFlag::NoFlags);
    if (status == GenericItem::ItemStatus::Created)
        deltaSQ.initialise(calculatedSQ.nRows(), calculatedSQ.nRows(), true);

    // Copy the tags from the calculated data (so we avoid requiring the source AtomTypeList) and create the data
    for (auto &&[delta, calc, est] : zip(deltaSQ, calculatedSQ, estimatedSQ))
    {
        delta.setTag(calc.tag());
        delta = est;
        Interpolator::addInterpolated(calc, delta, -1.0);
    }
}

// Truncate the supplied data
void RefineModule::truncate(Data1D &data, double rMin, double rMax)
{
    // Replicates the Refine25 truncate(xx,rminpt,rmaxpt) function applied over a whole dataset.
    double x;
    auto &y = data.values();
    const auto decay = rMax - rMin;
    for (auto n = 0; n < data.nValues(); ++n)
    {
        x = data.xAxis(n);

        if (x >= rMax)
            y[n] = 0.0;
        else if (x > rMin)
            y[n] *= 0.5 * (1.0 + cos(((x - rMin) * PI) / decay));
    }
}
