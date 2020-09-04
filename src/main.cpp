/*
    *** Dissolve Main
    *** src/main.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "base/messenger.h"
#include "base/processpool.h"
#include "main/dissolve.h"
#include "main/version.h"
#include <ctime>
#include <stdlib.h>
#include <time.h>

int main(int args, char **argv)
{
#ifdef PARALLEL
    // Initialise parallel communication
    ProcessPool::initialiseMPI(&args, &argv);
#endif

    // Instantiate main classes
    CoreData coreData;
    Dissolve dissolve(coreData);

    // Parse CLI options...
    auto n = 1;
    std::string inputFile, redirectFileName, restartDataFile, outputInputFile;
    auto nIterations = 5;
    auto ignoreRestart = false;
    while (n < args)
    {
        if (argv[n][0] == '-')
        {
            // Command-line switch
            switch (argv[n][1])
            {
                case ('h'):
#ifdef PARALLEL
                    Messenger::print("Dissolve PARALLEL version {}, Copyright (C) 2012-2020 T. Youngs.\n\n", Version::info());
#else
                    Messenger::print("Dissolve SERIAL version {}, Copyright (C) 2012-2020 T. Youngs.\n\n", Version::info());
#endif
                    Messenger::print("Recognised CLI options are:\n\n");
                    Messenger::print("\t-c\t\tCheck input and set-up only - don't perform any main-loop "
                                     "iterations\n");
                    Messenger::print("\t-h\t\tPrint what you're reading now\n");
                    Messenger::print("\t-f <file>\tRedirect output from all process to 'file.N', where N is the "
                                     "process "
                                     "rank\n");
                    Messenger::print("\t-i\t\tIgnore restart file\n");
                    Messenger::print("\t-m\t\tRestrict output to be from the master process alone (parallel code "
                                     "only)\n");
                    Messenger::print("\t-n <iterations>\tRun for the specified number of main loop iterations, then "
                                     "stop\n");
                    Messenger::print("\t-q\t\tQuiet mode - print no output\n");
                    Messenger::print("\t-r <N>\tSet restart file frequency (default = 10)\n");
                    Messenger::print("\t-s\t\tPerform single main loop iteration and then quit\n");
                    Messenger::print("\t-t <file>\tLoad restart data from specified file (but still write to "
                                     "standard "
                                     "restart file)\n");
                    Messenger::print("\t-v\t\tVerbose mode - be a little more descriptive throughout\n");
                    Messenger::print("\t-w <file>\tWrite input to specified file after reading it, and then quit\n");
                    Messenger::print("\t-x\t\tDon't write restart or heartbeat files (but still read in the restart "
                                     "file if "
                                     "present)\n");
                    ProcessPool::finalise();
                    Messenger::ceaseRedirect();
                    return 1;
                    break;
                case ('c'):
                    nIterations = 0;
                    Messenger::print("System input and set-up will be checked, then Dissolve will exit.\n");
                    break;
                case ('f'):
                    // Next argument is filename
                    ++n;
                    if (n == args)
                    {
                        Messenger::error("Expected redirection filename.\n");
                        Messenger::ceaseRedirect();
                        return 1;
                    }
                    redirectFileName = fmt::format("{}.{}", argv[n], ProcessPool::worldRank());
                    Messenger::enableRedirect(redirectFileName);
                    break;
                case ('i'):
                    Messenger::print("Restart file (if it exists) will be ignored.\n");
                    ignoreRestart = true;
                    break;
                case ('m'):
                    Messenger::setMasterOnly(true);
                    break;
                case ('n'):
                    ++n;
                    if (n == args)
                    {
                        Messenger::error("Expected number of iterations.\n");
                        Messenger::ceaseRedirect();
                        return 1;
                    }
                    nIterations = atoi(argv[n]);
                    Messenger::print("{} main-loop iterations will be performed, then Dissolve will exit.\n", nIterations);
                    break;
                case ('q'):
                    Messenger::setQuiet(true);
                    break;
                case ('r'):
                    // Next argument is integer restart file frequency
                    ++n;
                    if (n == args)
                    {
                        Messenger::error("Expected restart file frequency.\n");
                        Messenger::ceaseRedirect();
                        return 1;
                    }
                    dissolve.setRestartFileFrequency(atoi(argv[n]));
                    if (dissolve.restartFileFrequency() <= 0)
                        Messenger::print("Restart file will not be written.\n");
                    else if (dissolve.restartFileFrequency() == 1)
                        Messenger::print("Restart file will be written after every iteration.\n",
                                         dissolve.restartFileFrequency());
                    else
                        Messenger::print("Restart file will be written after every {} iterations.\n",
                                         dissolve.restartFileFrequency());
                    break;
                case ('s'):
                    Messenger::print("Single main-loop iteration will be performed, then Dissolve will exit.\n");
                    nIterations = 1;
                    break;
                case ('t'):
                    // Next argument is filename
                    ++n;
                    if (n == args)
                    {
                        Messenger::error("Expected restart data filename.\n");
                        Messenger::ceaseRedirect();
                        return 1;
                    }
                    restartDataFile = argv[n];
                    Messenger::print("Restart data will be loaded from '{}'.\n", restartDataFile);
                    break;
                case ('v'):
                    Messenger::setVerbose(true);
                    Messenger::printVerbose("Verbose mode enabled.\n");
                    break;
                case ('w'):
                    // Next argument is filename
                    ++n;
                    if (n == args)
                    {
                        Messenger::error("Expected input filename to write.\n");
                        Messenger::ceaseRedirect();
                        return 1;
                    }
                    outputInputFile = argv[n];
                    Messenger::print("Input file will be written to '{}' once read.\n", outputInputFile);
                    break;
                case ('x'):
                    dissolve.setRestartFileFrequency(0);
                    dissolve.setWriteHeartBeat(false);
                    Messenger::print("No restart or heartbeat files will be written.\n");
                    break;
                default:
                    Messenger::print("Unrecognised command-line switch '{}'.\n", argv[n]);
                    Messenger::print("Run with -h to see available switches.\n");
                    Messenger::ceaseRedirect();
                    ProcessPool::finalise();
                    return 1;
                    break;
            }
        }
        else
        {
            // Input filename?
            if (inputFile.empty())
                inputFile = argv[n];
            else
            {
                Messenger::error("Please specify exactly one input file.");
                ProcessPool::finalise();
                Messenger::ceaseRedirect();
                return 1;
            }
        }

        ++n;
    }

    // Print GPL license information
#ifdef PARALLEL
    Messenger::print("Dissolve PARALLEL version {}, Copyright (C) 2012-2020 T. Youngs.\n", Version::info());
#else
    Messenger::print("Dissolve SERIAL version {}, Copyright (C) 2012-2020 T. Youngs.\n", Version::info());
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

    // Load input file - if no input file was provided, exit here
    Messenger::banner("Parse Input File");
    if (inputFile.empty())
    {
        Messenger::print("No input file provided. Nothing more to do.\n");
        ProcessPool::finalise();
        Messenger::ceaseRedirect();
        return 1;
    }
    if (!dissolve.loadInput(inputFile))
    {
        ProcessPool::finalise();
        Messenger::ceaseRedirect();
        return 1;
    }

    // Save input file to new output filename and quit?
    if (!outputInputFile.empty())
    {
        Messenger::print("Saving input file to '{}'...\n", outputInputFile);
        bool result;
        if (dissolve.worldPool().isMaster())
        {
            result = dissolve.saveInput(outputInputFile);
            if (result)
                dissolve.worldPool().decideTrue();
            else
                dissolve.worldPool().decideFalse();
        }
        else
            result = dissolve.worldPool().decision();
        if (!result)
            Messenger::error("Failed to save input file to '{}'.\n", outputInputFile);
        ProcessPool::finalise();
        Messenger::ceaseRedirect();
        return result ? 0 : 1;
    }

    // Load restart file if it exists
    Messenger::banner("Parse Restart File");
    if (!ignoreRestart)
    {
        // We may have been provided the name of a restart file to read in...
        std::string restartFile = restartDataFile.empty() ? fmt::format("{}.restart", inputFile) : restartDataFile;

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
            dissolve.setRestartFilename(fmt::format("{}.restart", inputFile));
        }
        else
            Messenger::print("Restart file '{}' does not exist.\n", restartFile);
    }
    else
        Messenger::print("Restart file (if it exists) will be ignored.\n");

    // Prepare for run
    if (!dissolve.prepare())
    {
        ProcessPool::finalise();
        Messenger::ceaseRedirect();
        return 1;
    }

#ifdef PARALLEL
    Messenger::print("This is process rank {} of {} processes total.\n", ProcessPool::worldRank(),
                     ProcessPool::nWorldProcesses());
#endif

    // Run main simulation
    auto result = dissolve.iterate(nIterations);

    // Print timing information
    dissolve.printTiming();

    // Clear all data
    dissolve.clear();

    if (result)
        Messenger::print("Dissolve is done.\n");
    else
        Messenger::print("Dissolve is done, but with errors.\n");

    // End parallel communication
    ProcessPool::finalise();

    // Stop redirecting
    Messenger::ceaseRedirect();

    // Done.
    return (result ? 0 : 1);
}
