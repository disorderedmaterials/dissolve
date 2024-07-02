// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/messenger.h"
#include "base/processPool.h"
#include "main/cli.h"
#include "main/dissolve.h"
#include "main/version.h"

int main(int args, char **argv)
{
    // Initialise random seed before we do anything else - it might get re-initialised if a specific seed is provided
    srand((unsigned)time(nullptr));

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

    // Re-initialise random seed
    if (options.randomSeed())
        srand(*options.randomSeed());

    // Enable redirect if requested
    if (options.redirectionBasename())
        Messenger::enableRedirect(fmt::format("{}.{}", options.redirectionBasename().value(), ProcessPool::worldRank()));

    Messenger::print("Dissolve-{} version {}, Copyright (C) 2024 Team Dissolve and contributors.\n", Version::appType(),
                     Version::info());
    Messenger::print("Source repository: {}.\n", Version::repoUrl());
    Messenger::print("Dissolve comes with ABSOLUTELY NO WARRANTY.\n");
    Messenger::print("This is free software, and you are welcome to redistribute it under certain conditions.\n");
    Messenger::print("For more details read the GPL at <http://www.gnu.org/copyleft/gpl.html>.\n");

    Messenger::banner("This is a test run for CG bead implementation");
    // Load input file
    Messenger::banner("Parse Input File");
    if (!dissolve.loadInput(options.inputFile().value()))
    {
        ProcessPool::finalise();
        Messenger::ceaseRedirect();
        return 1;
    }

    // Save input file to new output filename and quit?
    if (options.writeInputFilename() || options.toTomlFile())
    {
        // This should be options.writeInputFilename().or_else(options.totomlFile.value())
        // but that will require C++23
        std::string filename =
            options.writeInputFilename() ? options.writeInputFilename().value() : options.toTomlFile().value();
        Messenger::print("Saving input file to '{}'...\n", filename);
        bool result;
        if (dissolve.worldPool().isMaster())
        {
            if (options.writeInputFilename())
            {
                result = dissolve.saveInput(options.writeInputFilename().value());
                if (result)
                    dissolve.worldPool().decideTrue();
                else
                    dissolve.worldPool().decideFalse();
            }
            else
            {
                auto toml = dissolve.serialise();
                std::ofstream outfile(options.toTomlFile().value());
                outfile << toml;
                outfile.close();
                result = true;
            }
        }
        else
            result = dissolve.worldPool().decision();
        if (!result)
            Messenger::error("Failed to save input file to '{}'.\n", filename);

        // Reload the written file and continue?
        if (options.writeInputAndReload())
        {
            dissolve.clear();
            Messenger::banner("Reload Input File");
            if (!dissolve.loadInput(options.writeInputFilename().value()))
            {
                ProcessPool::finalise();
                Messenger::ceaseRedirect();
                return 1;
            }
        }
        else
        {
            ProcessPool::finalise();
            Messenger::ceaseRedirect();
            return result ? 0 : 1;
        }
    }

    // Load restart file if it exists
    Messenger::banner("Parse Restart File");
    if (options.ignoreRestartFile())
        Messenger::print("Restart file (if it exists) will be ignored.\n");
    else
    {
        // We may have been provided the name of a restart file to read in...
        auto restartFile{options.restartFilename().value_or(std::string(dissolve.restartFilename()))};

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
        }
        else
        {
            // If a restart file was specifically provided, raise an error. Otherwise just print a message
            if (options.restartFilename())
            {
                Messenger::error("Specified restart file '{}' does not exist.\n", restartFile);
                return 1;
            }

            Messenger::print("Default restart file '{}' does not exist.\n", restartFile);
        }
    }

    // If we're just checking the input and restart files, exit now
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

    // Set restart file frequency
    dissolve.setRestartFileFrequency(options.noRestartFile() ? 0 : options.restartFileFrequency());

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

    // Run main simulation
    auto result = true;
    if (options.nIterations() > 0)
    {
        result = dissolve.iterate(options.nIterations());

        dissolve.printTiming();
    }

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
