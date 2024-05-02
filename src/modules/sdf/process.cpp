// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "analyser/dataExporter.h"
#include "analyser/dataOperator3D.h"
#include "analyser/siteSelector.h"
#include "base/sysFunc.h"
#include "main/dissolve.h"
#include "math/histogram3D.h"
#include "module/context.h"
#include "modules/sdf/sdf.h"

// Run main processing
Module::ExecutionResult SDFModule::process(ModuleContext &moduleContext)
{
    // Check for Configuration target
    if (!targetConfiguration_)
    {
        Messenger::error("No configuration target set for module '{}'.\n", name());
        return ExecutionResult::Failed;
    }

    auto &processingData = moduleContext.dissolve().processingModuleData();

    // Select site A
    SiteSelector a(targetConfiguration_, a_);

    // Select site B
    SiteSelector b(targetConfiguration_, b_);

    // SDF histogram
    auto [hist, status] = processingData.realiseIf<Histogram3D>("Histo", name(), GenericItem::InRestartFileFlag);
    if (status == GenericItem::ItemStatus::Created)
        hist.initialise(rangeX_.x, rangeX_.y, rangeX_.z, rangeY_.x, rangeY_.y, rangeY_.z, rangeZ_.x, rangeZ_.y, rangeZ_.z);
    hist.zeroBins();

    for (const auto &[siteA, indexA] : a.sites())
    {
        for (const auto &[siteB, indexB] : b.sites())
        {
            if (excludeSameMolecule_ && siteB->molecule() == siteA->molecule())
                continue;
            if (siteB == siteA)
                continue;
            auto vBA = targetConfiguration_->box()->minimumVector(siteA->origin(), siteB->origin());
            vBA = siteA->axes().transposeMultiply(vBA);
            hist.bin(vBA);
        }
    }

    // Accumulate histogram
    hist.accumulate();

    // SDF
    auto &dataSDF = processingData.realise<Data3D>("SDF", name(), GenericItem::InRestartFileFlag);
    dataSDF = hist.accumulatedData();

    // Normalise
    DataOperator3D normaliserSDF(dataSDF);
    // Normalise by A site population
    normaliserSDF.divide(double(a.sites().size()));
    // Normalise by grid
    normaliserSDF.normaliseByGrid();

    // Save SDF data?
    if (!DataExporter<Data3D, Data3DExportFileFormat>::exportData(dataSDF, sdfFileAndFormat_, moduleContext.processPool()))
        return ExecutionResult::Failed;

    return ExecutionResult::Success;
}
