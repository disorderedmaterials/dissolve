// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/sysFunc.h"
#include "keywords/module.h"
#include "main/dissolve.h"
#include "math/error.h"
#include "math/ft.h"
#include "module/context.h"
#include "module/group.h"
#include "modules/drivenMD/drivenMD.h"
#include "modules/gr/gr.h"
#include <functional>

// Run main processing
Module::ExecutionResult DrivenMDModule::process(ModuleContext &moduleContext)
{
    auto &processingData = moduleContext.dissolve().processingModuleData();

    /*
     * 1) Loop over a site
     * 2) Move in the x, y and z direction
     * 3) Calculate gr at each step
     * 4) FT gr-1 to F(Q) and weight by scattering length
     * 5) Take the negative gradient of the difference between the gr and the F(Q) which corresponds to the derived force
     * that we then apply to MD simulation
     */

    std::vector<Vec3<double>> forces;

    // Does a PartialSet already exist for this Configuration?
    auto originalGRObject = processingData.realiseIf<PartialSet>(
        std::format("{}//OriginalGR", targetConfiguration_->niceName()), name_, GenericItem::InRestartFileFlag);
    auto &originalgr = originalGRObject.first;
    auto rdfRange = targetConfiguration_->box()->inscribedSphereRadius();
    auto binWidth = 0.025;
    rdfRange = int(rdfRange / binWidth) * binWidth;
    if (originalGRObject.second == GenericItem::ItemStatus::Created)
        originalgr.setUp(targetConfiguration_->atomTypePopulations(), rdfRange, binWidth);

    originalgr.setUpHistograms(rdfRange, binWidth);
    originalgr.reset();

    double delta{2};
    auto atoms = targetConfiguration_->atoms();
    forces.reserve(atoms.size());

    for (auto *module : targets_)
    {
        // Retrieve the ReferenceData
        if (!processingData.contains("ReferenceData", module->name()))
        {
            Messenger::error("Reference data not found for target '{}'.\n", module->name());
            return ExecutionResult::Failed;
        }
        const auto &originalReferenceData = processingData.value<Data1D>("ReferenceData", module->name());

        for (auto &i : atoms)
        {
            std::format("X coord = {}, Y coord = {}, Z coord = {}\n", i.x(), i.y(), i.z());
            auto &f = forces.emplace_back(Vec3<double>(0.0, 0.0, 0.0));
            std::format("Atom {}\n", i.globalIndex());
            for (auto n = 0; n < 3; ++n)
            {
                switch (n)
                {
                    // Move x
                    case 0:
                        // Get position, change via delta then set
                        std::format("X coord = {}\n", i.x());
                        i.translateCoordinates(-delta, 0, 0);
                        std::format("X coord = {}\n", i.x());
                        // Reset GR
                        originalgr.reset();
                        // Calculate GR
                        calculateGRTestSerial(targetConfiguration_, originalgr);
                        // FT to structure factor
                        Fourier::sineFT(originalgr.total(), 1.0 / (2.0 * PI * PI * 1.39), 0.05, 0.05, 30.0,
                                        WindowFunction::Form::Lorch0);
                        // Store the error in a vector
                        f.x = Error::rmse(originalgr.total(), originalReferenceData).error;
                        std::format("X+ = {}\n", Error::rmse(originalgr.total(), originalReferenceData).error);
                        // Set new position (needs 2x to get +x from original)
                        i.translateCoordinates(2 * delta, 0, 0);
                        std::format("X coord = {}\n", i.x());
                        originalgr.reset();
                        calculateGRTestSerial(targetConfiguration_, originalgr);
                        Fourier::sineFT(originalgr.total(), 1.0 / (2.0 * PI * PI * 1.39), 0.05, 0.05, 30.0,
                                        WindowFunction::Form::Lorch0);
                        // Calculate new error
                        f.x -= Error::rmse(originalgr.total(), originalReferenceData).error;
                        std::format("X+ = {}\n", Error::rmse(originalgr.total(), originalReferenceData).error);
                        // Reset position
                        i.translateCoordinates(-delta, 0, 0);
                        std::format("X coord = {}\n", i.x());
                        break;
                    // Move y and repeat
                    case 1:
                        std::format("Y coord = {}\n", i.y());
                        i.translateCoordinates(0, -delta, 0);
                        std::format("Y coord = {}\n", i.y());
                        originalgr.reset();
                        calculateGRTestSerial(targetConfiguration_, originalgr);
                        Fourier::sineFT(originalgr.total(), 1.0 / (2.0 * PI * PI * 1.39), 0.05, 0.05, 30.0,
                                        WindowFunction::Form::Lorch0);
                        f.y = Error::rmse(originalgr.total(), originalReferenceData).error;
                        std::format("Y+ = {}\n", Error::rmse(originalgr.total(), originalReferenceData).error);
                        i.translateCoordinates(0, 2 * delta, 0);
                        std::format("Y coord = {}\n", i.y());
                        originalgr.reset();
                        calculateGRTestSerial(targetConfiguration_, originalgr);
                        Fourier::sineFT(originalgr.total(), 1.0 / (2.0 * PI * PI * 1.39), 0.05, 0.05, 30.0,
                                        WindowFunction::Form::Lorch0);
                        f.y -= Error::rmse(originalgr.total(), originalReferenceData).error;
                        std::format("Y+ = {}\n", Error::rmse(originalgr.total(), originalReferenceData).error);
                        i.translateCoordinates(0, -delta, 0);
                        std::format("Y coord = {}\n", i.y());
                        break;
                    // Move z
                    case 2:
                        std::format("Z coord = {}\n", i.z());
                        i.translateCoordinates(0, 0, -delta);
                        std::format("Z coord = {}\n", i.z());
                        originalgr.reset();
                        calculateGRTestSerial(targetConfiguration_, originalgr);
                        Fourier::sineFT(originalgr.total(), 1.0 / (2.0 * PI * PI * 1.39), 0.05, 0.05, 30.0,
                                        WindowFunction::Form::Lorch0);
                        f.z = Error::rmse(originalgr.total(), originalReferenceData).error;
                        std::format("Z+ = {}\n", Error::rmse(originalgr.total(), originalReferenceData).error);
                        i.translateCoordinates(0, 0, 2 * delta);
                        std::format("Z coord = {}\n", i.z());
                        originalgr.reset();
                        calculateGRTestSerial(targetConfiguration_, originalgr);
                        Fourier::sineFT(originalgr.total(), 1.0 / (2.0 * PI * PI * 1.39), 0.05, 0.05, 30.0,
                                        WindowFunction::Form::Lorch0);
                        f.z -= Error::rmse(originalgr.total(), originalReferenceData).error;
                        std::format("Z+ = {}\n", Error::rmse(originalgr.total(), originalReferenceData).error);
                        i.translateCoordinates(0, 0, -delta);
                        std::format("Z coord = {}\n", i.z());
                        break;
                }
            }
        }
    }

    for (auto &&[i, f] : zip(atoms, forces))
    {
        std::format("X coord = {}, Y coord = {}, Z coord = {}\n", i.x(), i.y(), i.z());
        i.translateCoordinates(f);
        f.print();
        std::format("We've moved\n");
        std::format("X coord = {}, Y coord = {}, Z coord = {}\n", i.x(), i.y(), i.z());
    }

    targetConfiguration_->incrementContentsVersion();
    targetConfiguration_->updateObjectRelationships();

    return ExecutionResult::Success;
}
