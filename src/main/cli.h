// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

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
    int nIterations_{0};
    // Frequency at which to write restart file
    int restartFileFrequency_{10};
    // Seed for random number generator
    std::optional<int> randomSeed_;
    // Redirection basename (for per-process output)
    std::optional<std::string> redirectionBasename_;
    // Restart file to load, overriding default
    std::optional<std::string> restartFilename_;
    // New input file to write (after reading supplied file)
    std::optional<std::string> writeInputFilename_;
    // Whether to reload the file written to writeInputFilename_ and continue
    bool writeInputAndReload_{false};
    // Whether to ignore restart file (if it exists)
    bool ignoreRestartFile_{false};
    // Whether to prevent writing of the restart file
    bool noRestartFile_{false};
    // File for TOML conversion
    std::optional<std::string> toTomlFile_;

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
    // Return seed for random number generator
    std::optional<int> randomSeed() const;
    // Return redirection basename (for per-process output)
    std::optional<std::string> redirectionBasename() const;
    // Return restart file to load, overriding default
    std::optional<std::string> restartFilename() const;
    // Return new input file to write (after reading supplied file)
    std::optional<std::string> writeInputFilename() const;
    // Return whether to reload the file written to writeInputFilename_ and continue
    bool writeInputAndReload() const;
    // Return whether to ignore restart file if it exists
    bool ignoreRestartFile() const;
    // Return whether to prevent writing of the restart file
    bool noRestartFile() const;
    // Return output destination for TOML conversion
    std::optional<std::string> toTomlFile() const;
};
