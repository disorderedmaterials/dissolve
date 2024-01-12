// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "base/lineParser.h"
#include "base/randomBuffer.h"
#include "base/timer.h"
#include "data/atomicMasses.h"
#include "main/dissolve.h"
#include "module/context.h"
#include "modules/energy/energy.h"
#include "modules/forces/forces.h"
#include "modules/dlPoly/dlPoly.h"

// Return EnumOptions for TrajectoryKey
EnumOptions<DlPolyModule::TrajectoryKey> DlPolyModule::trajectoryKey()
{
    return EnumOptions<DlPolyModule::TrajectoryKey>(
        "TrajectoryKey",
        {{TrajectoryKey::pos, "pos"}, {TrajectoryKey::pos_vel, "pos-vel"}, {TrajectoryKey::pos_vel_force, "pos-vel-force"}});
}

// Return EnumOptions for CoulMethod
EnumOptions<DlPolyModule::CoulMethod> DlPolyModule::coulMethod()
{
    return EnumOptions<DlPolyModule::CoulMethod>(
        "CoulMethod",
        {{CoulMethod::off, "off"}, {CoulMethod::spme, "spme"}, {CoulMethod::dddp, "dddp"}, {CoulMethod::pairwise, "pairwise"}, {CoulMethod::reaction_field, "reaction_field"}, {CoulMethod::force_shifted,   "force_shifted"}});
}

// Return EnumOptions for Ensemble
EnumOptions<DlPolyModule::Ensemble> DlPolyModule::ensemble()
{
    return EnumOptions<DlPolyModule::Ensemble>(
        "ensemble",
        {{Ensemble::NVE, "NVE"}, {Ensemble::PMF, "PMF"}, {Ensemble::NVT, "NVT"}, {Ensemble::NPT, "NPT"}, {Ensemble::NST, "NST"}});
}

// Return EnumOptions for EnsembleMethod
EnumOptions<DlPolyModule::EnsembleMethod> DlPolyModule::ensembleMethod()
{
    return EnumOptions<DlPolyModule::EnsembleMethod>(
        "ensembleMethod",
        {{EnsembleMethod::Evans, "Evans"}, {EnsembleMethod::Langevin, "Langevin"}, {EnsembleMethod::Andersen, "Andersen"}, {EnsembleMethod::Berendsen, "Berendsen"}, {EnsembleMethod::Hoover, "Hoover"}, {EnsembleMethod::gentle, "gentle"}, {EnsembleMethod::ttm, "ttm"}, {EnsembleMethod::dpds1, "dpds1"}, {EnsembleMethod::dpds2, "dpds2"}, {EnsembleMethod::MTK, "MTK"}});
}

// Return EnumOptions for MinimisationCriterion
EnumOptions<DlPolyModule::MinimisationCriterion> DlPolyModule::minimisationCriterion()
{
    return EnumOptions<DlPolyModule::MinimisationCriterion>(
        "minimisationCriterion",
        {{MinimisationCriterion::Off, "Off"}, {MinimisationCriterion::Force, "Force"}, {MinimisationCriterion::Energy, "Energy"}, {MinimisationCriterion::Distance, "Distance"}});
}

// Execute command on cmd (MOVE TO FUNCTIONS FILE?)
std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

// Run main processing
Module::ExecutionResult DlPolyModule::process(ModuleContext &moduleContext)
{   
    // Check for zero Configuration targets
    if (!targetConfiguration_)
    {
        Messenger::error("No configuration target set for module '{}'.\n", name());
        return ExecutionResult::Failed;
    }

    // Save DlPoly CONTROL/FIELD file with props
    if (moduleContext.processPool().isMaster())
    {
        Messenger::print("Export: Writing TESTTEST file ({}) for Configuration '{}'...\n",
                         dlPolyControlFormat_.formatDescription(), targetConfiguration_->name());
        
        // Set CONTROL filename and format
        dlPolyControlFormat_.setFilename("CONTROL");
        dlPolyControlFormat_.setFormatByIndex(0);
        
        // Save CONTROL file with respective properties
        if (!dlPolyControlFormat_.exportData(targetConfiguration_,
                                             capForces_,
                                             capForcesAt_,
                                             cutoffDistance_,
                                             padding_,
                                             ensemble().keyword(ensemble_),
                                             ensembleMethod().keyword(ensembleMethod_),
                                             ensembleThermostatCoupling_,
                                             timestepVariable_,
                                             fixedTimestep_,
                                             energyFrequency_,
                                             nSteps_,
                                             outputFrequency_,
                                             randomVelocities_,
                                             trajectoryFrequency_,
                                             trajectoryKey().keyword(trajectoryKey_),
                                             coulMethod().keyword(coulMethod_),
                                             coulPrecision_,
                                             minimisationCriterion().keyword(minimisationCriterion_),
                                             minimisationTolerance_,
                                             minimisationFrequency_))
        {
            Messenger::print("Export: Failed to export CONTROL file '{}'.\n", dlPolyControlFormat_.filename());
            moduleContext.processPool().decideFalse();
            return ExecutionResult::Failed;
        }
        
        // Set FIELD filename and format
        dlPolyFieldFormat_.setFilename("FIELD");
        dlPolyFieldFormat_.setFormatByIndex(0);
        
        // Save FIELD file
        if (!dlPolyFieldFormat_.exportData(targetConfiguration_))
        {
            Messenger::print("Export: Failed to export FIELD file '{}'.\n", dlPolyFieldFormat_.filename());
            moduleContext.processPool().decideFalse();
            return ExecutionResult::Failed;
        }
        
        // Set CONFIG filename and format
        coordinatesFormat_.setFilename("CONFIG");
        coordinatesFormat_.setFormatByIndex(1);
        
        // Save CONFIG file
        if (!coordinatesFormat_.exportData(targetConfiguration_))
        {
            Messenger::print("Export: Failed to export CONFIG file '{}'.\n", coordinatesFormat_.filename());
            moduleContext.processPool().decideFalse();
            return ExecutionResult::Failed;
        }
        
        // Run DLPOLY
        char charArr[dlPolyPath_.length() + 1];
        auto run = exec(strcpy(charArr, dlPolyPath_.c_str()));

        moduleContext.processPool().decideTrue();
    }
    else if (!moduleContext.processPool().decision())
        return ExecutionResult::Failed;



    /*
     * Calculation End
     */

    return ExecutionResult::Success;
}
