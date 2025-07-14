// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "base/messenger.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"

// Static Members
bool Messenger::quiet_ = false;
bool Messenger::muted_ = false;
bool Messenger::verbose_ = false;
int Messenger::nErrors_ = 0;
int Messenger::nWarnings_ = 0;
OutputHandler *Messenger::outputHandler_ = nullptr;
std::string Messenger::outputPrefix_;

/*
 * General Print Routines (Private)
 */

// Split supplied text into lines (delimited by '\n') and send for output
void Messenger::splitAndPrint(std::string_view s)
{
    auto lines = DissolveSys::splitString(s, "\n");
    for (const auto line : lines)
        outputText(line);
}

/*
 * General Print Routines
 */

// Set status of quiet mode
void Messenger::setQuiet(bool b) { quiet_ = b; }

// Return status of quiet mode
bool Messenger::isQuiet() { return quiet_; }

// Temporarily mute output
void Messenger::mute() { muted_ = true; }

// Unmute output
void Messenger::unMute() { muted_ = false; }

// Set status of verbose mode
void Messenger::setVerbose(bool b) { verbose_ = b; }

// Return status of verbose mode
bool Messenger::isVerbose() { return verbose_; }

// Clear error and warning counts
void Messenger::clearErrorCounts()
{
    nErrors_ = 0;
    nWarnings_ = 0;
}

// Return number of warnings
int Messenger::nWarnings() { return nWarnings_; }

// Return number of errors
int Messenger::nErrors() { return nErrors_; }

// Print normal message (no formatters)
void Messenger::print(std::string_view s)
{
    if (quiet_ || muted_)
        return;

    splitAndPrint(s);
}

/*
 * Text Output Routine
 */

// Set prefix text
void Messenger::setOutputPrefix(std::string_view prefix) { outputPrefix_ = prefix; }

// Clear prefix text
void Messenger::clearOutputPrefix() { outputPrefix_.clear(); }

// Output text to relevant handler
void Messenger::outputText(std::string_view s)
{

    if (outputPrefix_.empty())
    {
        if (outputHandler_)
            outputHandler_->outputText(std::string(s));
        else
            std::cout << s << std::endl;
    }
    else
    {
        if (outputHandler_)
            outputHandler_->outputText(std::format("{} {}", outputPrefix_, s));
        else
            std::cout << outputPrefix_ << " " << s << std::endl;
    }
}

// Output blank line (with prefix if set) to relevant handler
void Messenger::outputBlank()
{
    if (outputPrefix_.empty())
    {
        if (outputHandler_)
            outputHandler_->outputText("");
        else
            std::cout << std::endl;
    }
    else
    {
        if (outputHandler_)
            outputHandler_->outputText(outputPrefix_);
        else
            std::cout << outputPrefix_ << std::endl;
    }
}

// Set output handler
void Messenger::setOutputHandler(OutputHandler *outputHandler) { outputHandler_ = outputHandler; }
