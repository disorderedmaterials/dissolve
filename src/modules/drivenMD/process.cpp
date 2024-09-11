// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/sysFunc.h"
#include "classes/neutronWeights.h"
#include "classes/partialSet.h"
#include "classes/scatteringMatrix.h"
#include "classes/xRayWeights.h"
#include "io/export/data1D.h"
#include "keywords/module.h"
#include "main/dissolve.h"
#include "math/error.h"
#include "math/filters.h"
#include "math/ft.h"
#include "math/gaussFit.h"
#include "math/poissonFit.h"
#include "module/context.h"
#include "module/group.h"
#include "modules/drivenMD/drivenMD.h"
#include "modules/energy/energy.h"
#include "modules/gr/gr.h"
#include "modules/neutronSQ/neutronSQ.h"
#include "modules/sq/sq.h"
#include "templates/algorithms.h"
#include "templates/array3D.h"
#include <functional>

// Run set-up stage
bool DrivenMDModule::setUp(ModuleContext &moduleContext, Flags<KeywordBase::KeywordSignal> actionSignals)
{
    // Default to applying generated potentials - an associated EPSRManager may turn this off in its own setup stage
    applyPotentials_ = true;

    // Check for exactly one Configuration referenced through target modules
    targetConfiguration_ = nullptr;
    std::optional<double> rho;
    for (auto *module : targets_)
    {
        // Retrieve source SQ module, and then the related RDF module
        auto optSQModule = module->keywords().get<const SQModule *, ModuleKeyword<const SQModule>>("SourceSQs");
        const SQModule *sqModule = nullptr;
        if (optSQModule)
            sqModule = optSQModule.value();
        if (!sqModule)
            return Messenger::error(
                "[SETUP {}] Target '{}' doesn't source any S(Q) data, so it can't be used as a target for the EPSR module.",
                name_, module->name());

        auto *grModule = sqModule->sourceGR();
        if (!grModule)
            return Messenger::error(
                "[SETUP {}] Target '{}'s S(Q) module doesn't reference a GRModule, it can't be used as a target "
                "for the EPSR module.",
                name_, module->name());
        // Check for number of targets, or different target if there's only 1
        auto rdfConfigs = grModule->keywords().getVectorConfiguration("Configurations");
        if (rdfConfigs.size() != 1)
            return Messenger::error(
                "[SETUP {}] GR module '{}' targets multiple configurations, which is not permitted when using "
                "its data in the EPSR module.",
                name_, grModule->name());

        if ((targetConfiguration_ != nullptr) && (targetConfiguration_ != rdfConfigs.front()))
            return Messenger::error("[SETUP {}] GR module '{}' targets a configuration which is different from another target "
                                    "module, and which is not permitted when using its data in the EPSR module.",
                                    name_, grModule->name());

        else
            targetConfiguration_ = rdfConfigs.front();

        rho = targetConfiguration_->atomicDensity();
    }

    // Realise storage for generated S(Q), and initialise a scattering matrix, but only if we have a valid configuration
    if (targetConfiguration_)
    {
        auto &estimatedSQ = moduleContext.dissolve().processingModuleData().realise<Array2D<Data1D>>(
            "EstimatedSQ", name_, GenericItem::InRestartFileFlag);
        scatteringMatrix_.initialise(targetConfiguration_->atomTypes(), estimatedSQ);
    }

    return true;
}

// Run main processing
Module::ExecutionResult DrivenMDModule::process(ModuleContext &moduleContext)
{
    std::string testDataName;

    /*
     * Do we have targets to refine against?
     */
    if (targets_.empty())
    {
        Messenger::error("At least one Module target containing suitable data must be provided.\n");
        return ExecutionResult::Failed;
    }

    if (!targetConfiguration_)
    {
        Messenger::error("No target configuration is set.\n");
        return ExecutionResult::Failed;
    }

    if (!targetConfiguration_->atomicDensity())
    {
        Messenger::error("No density available for target configuration '{}'\n", targetConfiguration_->name());
        return ExecutionResult::Failed;
    }
    auto rho = *targetConfiguration_->atomicDensity();

    return ExecutionResult::Success;
}
