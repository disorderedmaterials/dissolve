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
    PartialSet totalGR;

    // Does a PartialSet already exist for this Configuration?
    auto originalGRObject = processingData.realiseIf<PartialSet>(
        fmt::format("{}//OriginalGR", targetConfiguration_->niceName()), name_, GenericItem::InRestartFileFlag);
    auto &originalgr = originalGRObject.first;
    double delta{0.1};
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
            auto &f = forces.emplace_back(i.r());
            for (auto n = 0; n < 3; ++n)
            {
                double newPosition{};
                switch (n)
                {
                    // Move x
                    case 1:
                        // Get position, change via delta then set
                        newPosition = i.x() - delta;
                        i.set(newPosition, i.y(), i.z());
                        // Calculate GR
                        totalGR = calculateGRTestSerial(targetConfiguration_, originalgr);
                        // FT to structure factor
                        Fourier::sineFT(totalGR.total(), 1.0 / (2.0 * PI * PI * 1.39), 0.05, 0.05, 30.0,
                                        WindowFunction::Form::Lorch0);
                        // Store the error in a vector
                        f.x = Error::rmse(totalGR.total(), originalReferenceData).error;
                        // Set new position (needs 2x to get +x from original)
                        newPosition = i.x() + (2 * delta);
                        i.set(newPosition, i.y(), i.z());
                        totalGR = calculateGRTestSerial(targetConfiguration_, originalgr);
                        Fourier::sineFT(totalGR.total(), 1.0 / (2.0 * PI * PI * 1.39), 0.05, 0.05, 30.0,
                                        WindowFunction::Form::Lorch0);
                        // Calculate new error
                        f.x -= Error::rmse(totalGR.total(), originalReferenceData).error;
                        // Reset position
                        newPosition = i.x() - delta;
                        i.set(newPosition, i.y(), i.z());
                        break;
                    // Move y and repeat
                    case 2:
                        newPosition = i.y() - delta;
                        i.set(i.x(), newPosition, i.z());
                        totalGR = calculateGRTestSerial(targetConfiguration_, originalgr);
                        Fourier::sineFT(totalGR.total(), 1.0 / (2.0 * PI * PI * 1.39), 0.05, 0.05, 30.0,
                                        WindowFunction::Form::Lorch0);
                        f.y = Error::rmse(totalGR.total(), originalReferenceData).error;
                        newPosition = i.y() + (2 * delta);
                        i.set(i.x(), newPosition, i.z());
                        totalGR = calculateGRTestSerial(targetConfiguration_, originalgr);
                        Fourier::sineFT(totalGR.total(), 1.0 / (2.0 * PI * PI * 1.39), 0.05, 0.05, 30.0,
                                        WindowFunction::Form::Lorch0);
                        f.y -= Error::rmse(totalGR.total(), originalReferenceData).error;
                        newPosition = i.y() - delta;
                        i.set(i.x(), newPosition, i.z());
                        break;
                    // Move z
                    case 3:
                        newPosition = i.z() - delta;
                        i.set(i.x(), i.y(), newPosition);
                        totalGR = calculateGRTestSerial(targetConfiguration_, originalgr);
                        Fourier::sineFT(totalGR.total(), 1.0 / (2.0 * PI * PI * 1.39), 0.05, 0.05, 30.0,
                                        WindowFunction::Form::Lorch0);
                        f.z = Error::rmse(totalGR.total(), originalReferenceData).error;
                        newPosition = i.z() + (2 * delta);
                        i.set(i.x(), i.y(), newPosition);
                        totalGR = calculateGRTestSerial(targetConfiguration_, originalgr);
                        Fourier::sineFT(totalGR.total(), 1.0 / (2.0 * PI * PI * 1.39), 0.05, 0.05, 30.0,
                                        WindowFunction::Form::Lorch0);
                        f.z -= Error::rmse(totalGR.total(), originalReferenceData).error;
                        newPosition = i.z() - delta;
                        i.set(i.x(), i.y(), newPosition);
                        break;
                }
            }
            i.translateCoordinates(f);
            targetConfiguration_->updateAtomLocation(&i);
        }
        targetConfiguration_->updateObjectRelationships();
    }
    return ExecutionResult::Success;
}
