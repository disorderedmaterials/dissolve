// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/messenger.h"
#include "base/processpool.h"
#include "main/cli.h"
#include "main/dissolve.h"
#include "main/version.h"

int main(int args, char **argv)
{
#ifdef PARALLEL
    // Initialise parallel communication
    ProcessPool::initialiseMPI(&args, &argv);
#endif

    // Instantiate main classes
    CoreData coreData;
    Dissolve dissolve(coreData);

    // Parse CLI options
    CLIOptions options;
#ifdef PARALLEL
    if (options.parse(args, argv, false, true) != CLIOptions::Success)
        return 1;
#else
    if (options.parse(args, argv) != CLIOptions::Success)
        return 1;
#endif

    // Enable redirect if requested
    if (options.redirectionBasename())
        Messenger::enableRedirect(fmt::format("{}.{}", options.redirectionBasename().value(), ProcessPool::worldRank()));

#ifdef PARALLEL
    Messenger::print("Dissolve PARALLEL version {}, Copyright (C) 2021 Team Dissolve and contributors.\n", Version::info());
#else
    Messenger::print("Dissolve SERIAL version {}, Copyright (C) 2021 Team Dissolve and contributors.\n", Version::info());
#endif
    Messenger::print("Source repository: {}.\n", Version::repoUrl());
    Messenger::print("Dissolve comes with ABSOLUTELY NO WARRANTY.\n");
    Messenger::print("This is free software, and you are welcome to redistribute it under certain conditions.\n");
    Messenger::print("For more details read the GPL at <http://www.gnu.org/copyleft/gpl.html>.\n");

    // Check module registration
    Messenger::banner("Available Modules");
    if (!dissolve.registerMasterModules())
    {
        ProcessPool::finalise();
        Messenger::ceaseRedirect();
        return 1;
    }

    // Load input file
    Messenger::banner("Parse Input File");
    if (!dissolve.loadInput(options.inputFile().value()))
    {
        ProcessPool::finalise();
        Messenger::ceaseRedirect();
        return 1;
    }

    // Save input file to new output filename and quit?
    if (options.writeInputFilename())
    {
        Messenger::print("Saving input file to '{}'...\n", options.writeInputFilename().value());
        bool result;
        if (dissolve.worldPool().isMaster())
        {
            result = dissolve.saveInput(options.writeInputFilename().value());
            if (result)
                dissolve.worldPool().decideTrue();
            else
                dissolve.worldPool().decideFalse();
        }
        else
            result = dissolve.worldPool().decision();
        if (!result)
            Messenger::error("Failed to save input file to '{}'.\n", options.writeInputFilename().value());
        ProcessPool::finalise();
        Messenger::ceaseRedirect();
        return result ? 0 : 1;
    }

    // Load restart file if it exists
    Messenger::banner("Parse Restart File");
    if (options.ignoreRestartFile())
        Messenger::print("Restart file (if it exists) will be ignored.\n");
    else
    {
        // We may have been provided the name of a restart file to read in...
        std::string restartFile = options.restartFilename().value_or(fmt::format("{}.restart", options.inputFile().value()));

        if (DissolveSys::fileExists(restartFile))
        {
            Messenger::print("Restart file '{}' exists and will be loaded.\n", restartFile);
            if (!dissolve.loadRestart(restartFile))
            {
                Messenger::error("Restart file contained errors.\n");
                ProcessPool::finalise();
                Messenger::ceaseRedirect();
                return 1;
            }

            // Reset the restart filename to be the standard one
            dissolve.setRestartFilename(fmt::format("{}.restart", options.inputFile().value()));
        }
        else
            Messenger::print("Restart file '{}' does not exist.\n", restartFile);
    }

    // If were just checking the input and restart files, exit now
    if (!options.nIterations())
    {
        ProcessPool::finalise();
        Messenger::ceaseRedirect();
        return 0;
    }

    // Prepare for run
    if (!dissolve.prepare())
    {
        ProcessPool::finalise();
        Messenger::ceaseRedirect();
        return 1;
    }

    // Set restart file frequency and whether to write heartbeat file
    if (options.writeNoFiles())
    {
        dissolve.setRestartFileFrequency(0);
        dissolve.setWriteHeartBeat(false);
    }
    else
        dissolve.setRestartFileFrequency(options.restartFileFrequency());

    if (dissolve.restartFileFrequency() <= 0)
        Messenger::print("Restart file will not be written.\n");
    else if (dissolve.restartFileFrequency() == 1)
        Messenger::print("Restart file will be written after every iteration.\n", dissolve.restartFileFrequency());
    else
        Messenger::print("Restart file will be written after every {} iterations.\n", dissolve.restartFileFrequency());

#ifdef PARALLEL
    Messenger::print("This is process rank {} of {} processes total.\n", ProcessPool::worldRank(),
                     ProcessPool::nWorldProcesses());
#endif

    // Run main simulation?
    auto result = dissolve.iterate(options.nIterations());

    // Print timing information
    dissolve.printTiming();

    // Clear all data
    dissolve.clear();

    if (result)
        Messenger::print("Dissolve is done.\n");
    else
        Messenger::print("Dissolve is done, but with errors.\n");

    // Stop redirecting
    Messenger::ceaseRedirect();

    // End parallel communication
    ProcessPool::finalise();

    // Done.
    return (result ? 0 : 1);
}
