// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "main/cli.h"
#include "base/messenger.h"
#include "main/version.h"
#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>

CLIOptions::CLIOptions()
    : nIterations_(0), restartFileFrequency_(10), ignoreRestartFile_(false), ignoreStateFile_(false), writeNoFiles_(false)
{
}

// Parse CLI options
int CLIOptions::parse(const int args, char **argv, bool isGUI, bool isParallel)
{
    // Create CLI object
#ifdef PARALLEL
    CLI::App app{fmt::format("Dissolve PARALLEL version {}, 2021 Team Dissolve and contributors.", Version::info())};
#else
    CLI::App app{fmt::format("Dissolve SERIAL version {}, 2021 Team Dissolve and contributors.", Version::info())};
#endif

    // Add positionals
    auto inputFileOption = app.add_option("inputFile", inputFile_, "Input file to load");

    // Basic Control
    app.add_option("-n,--iterations", nIterations_, "Number of iterations to run (default = 0)")->group("Basic Control");
    app.add_flag_callback("-q,--quiet", []() { Messenger::setQuiet(true); },
                          "Be quiet - don't output any messages whatsoever (output files are still written)")
        ->group("Basic Control");
    app.add_flag_callback("-v,--verbose", []() { Messenger::setVerbose(true); },
                          "Print lots of additional output, useful for debugging")
        ->group("Basic Control");

    // Input Files
    app.add_flag("-i,--ignore-restart", ignoreRestartFile_, "Ignore restart file (if it exists)")->group("Input Files");
    if (!isGUI)
        app.add_option("-w,--write-input", writeInputFilename_,
                       "Write out the current simulation input to the file specified and then quit")
            ->group("Input Files");

    // Output Files
    app.add_option("-f,--frequency", restartFileFrequency_, "Frequency at which to write restart file (default = 10)")
        ->group("Output Files");
    app.add_option("--restart", restartFilename_,
                   "Read restart file specified instead of the default one (but still write to the default one)")
        ->group("Output Files");
    app.add_flag("-x,--no-files", writeNoFiles_, "Don't write restart or heartbeat files while running")->group("Output Files");

    // Add GUI-specific options - if this is not the GUI, make the input file a required parameter
    if (isGUI)
        app.add_flag("-I,--ignore-state", ignoreStateFile_, "Ignore GUI state file (if it exists)")->group("GUI Options");
    else
        inputFileOption->required();

    // Add parallel-specific options
    if (isParallel)
    {
        app.add_flag_callback("-a,--all", []() { Messenger::setMasterOnly(false); },
                              "Write output from all processes, not just master")
            ->group("Parallel Code Options");
        app.add_option("--redirect", redirectionBasename_,
                       "Redirect output from individual processes to files based on the supplied name")
            ->group("Parallel Code Options");
    }

    // Tweak formatting
    app.get_formatter()->label("TEXT", "<filename>");
    app.get_formatter()->label("INT", "<n>");

    // Parse the supplied options.
    // The CLI11_PARSE macro expands to a simple try/catch block, returning non-zero on error.
    // However, early exits from -h or --help return zero, and a successful parse does not return at all.
    // So, we return 1 on a "non-help" success to inform the calling function that it should proceed.
    CLI11_PARSE(app, args, argv);

    return CLIOptions::Success;
}

// Return input file to load
std::optional<std::string> CLIOptions::inputFile() const { return inputFile_; }

// Return number of iterations to perform
int CLIOptions::nIterations() const { return nIterations_; }

// Return frequency at which to write restart file
int CLIOptions::restartFileFrequency() const { return restartFileFrequency_; }

// Return redirection basename (for per-process output)
std::optional<std::string> CLIOptions::redirectionBasename() const { return redirectionBasename_; }

// Return restart file to load, overriding default
std::optional<std::string> CLIOptions::restartFilename() const { return restartFilename_; }

// Return new input file to write (after reading supplied file)
std::optional<std::string> CLIOptions::writeInputFilename() const { return writeInputFilename_; }

// Return whether to ignore restart file if it exists
bool CLIOptions::ignoreRestartFile() const { return ignoreRestartFile_; }

// Return whether to ignore GUI state file (if it exists)
bool CLIOptions::ignoreStateFile() const { return ignoreStateFile_; }

// Return whether to prevent writing of all output files
bool CLIOptions::writeNoFiles() const { return writeNoFiles_; };
