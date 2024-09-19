// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/sysFunc.h"
#include "keywords/module.h"
#include "main/dissolve.h"
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

    std::vector<PartialSet> forces;
    PartialSet totalGR;
    PartialSet referenceData;

    // Does a PartialSet already exist for this Configuration?
    auto originalGRObject = processingData.realiseIf<PartialSet>(
        fmt::format("{}//OriginalGR", targetConfiguration_->niceName()), name_, GenericItem::InRestartFileFlag);
    auto &originalgr = originalGRObject.first;
    double delta{0.1};
    auto atoms = targetConfiguration_->atoms();
    forces.resize(atoms.size());

    for (auto &i : atoms)
    {
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
                    // Store Error here
                    //  forces.emplace_back(referenceData.total().values() - totalGR.total().values());
                    newPosition = i.x() + (2 * delta);
                    i.set(newPosition, i.y(), i.z());
                    totalGR = calculateGRTestSerial(targetConfiguration_, originalgr);
                    Fourier::sineFT(totalGR.total(), 1.0 / (2.0 * PI * PI * 1.39), 0.05, 0.05, 30.0,
                                    WindowFunction::Form::Lorch0);
                    // Store Error here
                    //  forces.emplace_back(referenceData.total().values() - totalGR.total().values());
                    // Reset position
                    newPosition = i.x() - delta;
                    i.set(newPosition, i.y(), i.z());
                    break;
                // Move y
                case 2:
                    newPosition = i.y() - delta;
                    i.set(i.x(), newPosition, i.z());
                    totalGR = calculateGRTestSerial(targetConfiguration_, originalgr);
                    Fourier::sineFT(totalGR.total(), 1.0 / (2.0 * PI * PI * 1.39), 0.05, 0.05, 30.0,
                                    WindowFunction::Form::Lorch0);
                    //  forces.emplace_back(referenceData.total().values() - totalGR.total().values());
                    newPosition = i.y() + (2 * delta);
                    i.set(i.x(), newPosition, i.z());
                    totalGR = calculateGRTestSerial(targetConfiguration_, originalgr);
                    Fourier::sineFT(totalGR.total(), 1.0 / (2.0 * PI * PI * 1.39), 0.05, 0.05, 30.0,
                                    WindowFunction::Form::Lorch0);
                    //  forces.emplace_back(referenceData.total().values() - totalGR.total().values());
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
                    //  forces.emplace_back(referenceData.total().values() - totalGR.total().values());
                    newPosition = i.z() + (2 * delta);
                    i.set(i.x(), i.y(), newPosition);
                    totalGR = calculateGRTestSerial(targetConfiguration_, originalgr);
                    Fourier::sineFT(totalGR.total(), 1.0 / (2.0 * PI * PI * 1.39), 0.05, 0.05, 30.0,
                                    WindowFunction::Form::Lorch0);
                    //  forces.emplace_back(referenceData.total().values() - totalGR.total().values());
                    newPosition = i.z() - delta;
                    i.set(i.x(), i.y(), newPosition);
                    break;
            }
        }
    }

    return ExecutionResult::Success;
}
