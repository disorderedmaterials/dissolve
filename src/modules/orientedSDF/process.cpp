// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "analyser/dataNormaliser3D.h"
#include "analyser/siteSelector.h"
#include "main/dissolve.h"
#include "math/histogram3D.h"
#include "module/context.h"
#include "modules/orientedSDF/orientedSDF.h"

// Run main processing
Module::ExecutionResult OrientedSDFModule::process(ModuleContext &moduleContext)
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

    // Oriented SDF histogram
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

            auto axisAngle = Box::angleInDegrees(siteA->axes().columnAsVec3(axisA_), siteB->axes().columnAsVec3(axisB_));
            if (symmetric_ && axisAngle > 90.0)
                axisAngle = 180.0 - axisAngle;
            if (axisAngleRange_.contains(axisAngle))
            {
                auto vBA = targetConfiguration_->box()->minimumVector(siteA->origin(), siteB->origin());
                vBA = siteA->axes().transposeMultiply(vBA);
                hist.bin(vBA);
            }
        }
    }

    // Accumulate histogram
    hist.accumulate();

    // Oriented SDF
    auto &dataOrientedSDF = processingData.realise<Data3D>("SDF", name(), GenericItem::InRestartFileFlag);
    dataOrientedSDF = hist.accumulatedData();

    // Normalise
    DataNormaliser3D normaliserOrientedSDF(dataOrientedSDF);
    // Normalise by A site population
    normaliserOrientedSDF.normaliseBySitePopulation(double(a.sites().size()));
    // Normalise by grid
    normaliserOrientedSDF.normaliseByGrid();

    // Save data?
    if (sdfFileAndFormat_.hasFilename())
    {
        if (moduleContext.processPool().isMaster())
        {
            if (sdfFileAndFormat_.exportData(dataOrientedSDF))
                moduleContext.processPool().decideTrue();
            else
            {
                moduleContext.processPool().decideFalse();
                return ExecutionResult::Failed;
            }
        }
        else if (!moduleContext.processPool().decision())
            return ExecutionResult::Failed;
    }

    return ExecutionResult::Success;
}
