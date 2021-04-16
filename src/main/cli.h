// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include <optional>
#include <string>

// CLI Options Parser
class CLIOptions
{
    public:
    CLIOptions();
    ~CLIOptions() = default;

    /*
     * Options
     */
    private:
    // Input file to load
    std::optional<std::string> inputFile_;
    // Number of iterations to perform
    int nIterations_;
    // Frequency at which to write restart file
    int restartFileFrequency_;
    // Redirection basename (for per-process output)
    std::optional<std::string> redirectionBasename_;
    // Restart file to load, overriding default
    std::optional<std::string> restartFilename_;
    // New input file to write (after reading supplied file)
    std::optional<std::string> writeInputFilename_;
    // Whether to ignore restart file (if it exists)
    bool ignoreRestartFile_;
    // Whether to ignore GUI state file (if it exists)
    bool ignoreStateFile_;
    // Whether to prevent writing of all output files
    bool writeNoFiles_;

    public:
    // Parse Result enum
    enum ParseResult
    {
        Success = 1
    };
    // Parse supplied options
    int parse(int args, char **argv, bool isGUI = false, bool isParallel = false);
    // Return input file to load
    std::optional<std::string> inputFile() const;
    // Return number of iterations to perform
    int nIterations() const;
    // Return frequency at which to write restart file
    int restartFileFrequency() const;
    // Return redirection basename (for per-process output)
    std::optional<std::string> redirectionBasename() const;
    // Return restart file to load, overriding default
    std::optional<std::string> restartFilename() const;
    // Return new input file to write (after reading supplied file)
    std::optional<std::string> writeInputFilename() const;
    // Return whether to ignore restart file if it exists
    bool ignoreRestartFile() const;
    // Return whether to ignore GUI state file (if it exists)
    bool ignoreStateFile() const;
    // Return whether to prevent writing of all output files
    bool writeNoFiles() const;
};
