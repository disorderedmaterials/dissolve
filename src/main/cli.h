// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include <filesystem>
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
    std::optional<std::filesystem::path> inputFile_;
    // Node to run
    std::optional<std::string> node_;
    // Number of iterations to perform
    int nIterations_{0};
    // Frequency at which to write restart file
    int restartFileFrequency_{10};
    // Seed for random number generator
    std::optional<int> randomSeed_;
    // Restart file to load, overriding default
    std::optional<std::filesystem::path> restartFilename_;
    // New input file to write (after reading supplied file)
    std::optional<std::filesystem::path> writeInputFilename_;
    // Whether to reload the file written to writeInputFilename_ and continue
    bool writeInputAndReload_{false};
    // Whether to ignore restart file (if it exists)
    bool ignoreRestartFile_{false};
    // Whether to prevent writing of the restart file
    bool noRestartFile_{false};
    // Whether to simply export the nodes in mermaid form
    bool exportMermaid_{false};

    public:
    // Parse Result enum
    enum ParseResult
    {
        Success = 1
    };
    // Parse supplied options
    int parse(int args, char **argv, bool isGUI = false);
    // Return input file to load
    std::optional<std::filesystem::path> inputFile() const;
    // Return the node to run
    std::optional<std::string> node() const;
    // Return number of iterations to perform
    int nIterations() const;
    // Return frequency at which to write restart file
    int restartFileFrequency() const;
    // Return seed for random number generator
    std::optional<int> randomSeed() const;
    // Return restart file to load, overriding default
    std::optional<std::filesystem::path> restartFilename() const;
    // Return new input file to write (after reading supplied file)
    std::optional<std::filesystem::path> writeInputFilename() const;
    // Return whether to reload the file written to writeInputFilename_ and continue
    bool writeInputAndReload() const;
    // Return whether to ignore restart file if it exists
    bool ignoreRestartFile() const;
    // Return whether to prevent writing of the restart file
    bool noRestartFile() const;
    // Return whether to simply export the graph as mermaid and quit
    bool exportMermaid() const;
};
