// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/outputHandler.h"
#include <fmt/format.h>
#include <functional>

// Forward Declarations
class LineParser;

// Global messaging handler
class Messenger
{
    /*
     * General Print Routines
     */
    private:
    // Quiet mode
    static bool quiet_;
    // Mute mode (temporary quietness if not already)
    static bool muted_;
    // Verbose mode
    static bool verbose_;
    // Master-only mode
    static bool masterOnly_;
    // Number of errors and warnings accrued in output
    static int nErrors_, nWarnings_;

    private:
    // Split supplied text into lines (delimited by '\n') and send for output
    static void splitAndPrint(std::string_view text);

    public:
    // Set status of quiet mode
    static void setQuiet(bool b);
    // Return status of quiet mode
    static bool isQuiet();
    // Temporarily mute output
    static void mute();
    // Unmute output
    static void unMute();
    // Set status of verbose mode
    static void setVerbose(bool b);
    // Return status of verbose mode
    static bool isVerbose();
    // Set status of master-only mode
    static void setMasterOnly(bool b);
    // Clear error and warning counts
    static void clearErrorCounts();
    // Return number of warnings
    static int nWarnings();
    // Return number of errors
    static int nErrors();
    // Print normal message (no formatters)
    static void print(std::string_view s);
    // Print normal message
    template <typename... Args> static void print(std::string_view format, Args... args)
    {
        if (quiet_ || muted_)
            return;

        splitAndPrint(fmt::format(format, args...));
    }
    // Print verbose message
    template <typename... Args> static void printVerbose(std::string_view format, Args... args)
    {
        if (quiet_ || muted_ || (!verbose_))
            return;

        splitAndPrint(fmt::format(format, args...));
    }
    // Print error message
    template <typename... Args> static bool error(std::string_view format, Args... args)
    {
        if (quiet_ || muted_)
            return false;

        outputBlank();
        if (outputHandler_)
            outputHandler_->styleForError();
        setOutputPrefix("***  ERROR");
        outputBlank();
        splitAndPrint(fmt::format(format, args...));
        outputBlank();
        clearOutputPrefix();
        if (outputHandler_)
            outputHandler_->resetStyling();
        outputBlank();

        ++nErrors_;

        return false;
    }
    // Print warn message
    template <typename... Args> static void warn(std::string_view format, Args... args)
    {
        if (quiet_ || muted_)
            return;

        if (outputHandler_)
            outputHandler_->styleForWarning();
        setOutputPrefix("***  WARN ");
        outputBlank();
        splitAndPrint(fmt::format(format, args...));
        outputBlank();
        clearOutputPrefix();
        if (outputHandler_)
            outputHandler_->resetStyling();

        ++nWarnings_;
    }
    // Print banner message
    template <typename... Args> static void banner(std::string_view format, Args... args)
    {
        if (quiet_ || muted_)
            return;

        const auto bannerWidth = 80;
        static std::string bannerBorder(bannerWidth, '=');

        std::string bannerText = fmt::format(format, args...);

        outputBlank();
        outputText(bannerBorder);
        outputText(fmt::format("*{:^78}*", bannerText));
        outputText(bannerBorder);
        outputBlank();
    }
    // Print heading message
    template <typename... Args> static void heading(std::string_view format, Args... args)
    {
        if (quiet_ || muted_)
            return;

        const auto headingWidth = 80;
        static std::string headingBorder(headingWidth, '-');

        std::string headingText = fmt::format(format, args...);

        outputBlank();
        outputText(fmt::format("{:^80}", headingText));
        outputText(headingBorder);
        outputBlank();
    }

    /*
     * Text Output
     */
    private:
    // Output handler (if any)
    static OutputHandler *outputHandler_;
    // Text to prefix to all output (if any)
    static std::string outputPrefix_;

    private:
    // Set prefix text
    static void setOutputPrefix(std::string_view prefix);
    // Clear prefix text
    static void clearOutputPrefix();
    // Output text to relevant handler
    static void outputText(std::string_view s);
    // Output blank line (with prefix if set) to relevant handler
    static void outputBlank();

    public:
    // Set output handler
    static void setOutputHandler(OutputHandler *outputHandler);

    /*
     * File Redirection
     */
    private:
    // Whether file redirection is enabled
    static bool redirect_;
    // Local LineParser for file redirection
    static LineParser parser_;

    public:
    // Enable redirection of all messaging to specified file
    static bool enableRedirect(std::string_view filename);
    // Cease redirection of messaging to file
    static void ceaseRedirect();
};
