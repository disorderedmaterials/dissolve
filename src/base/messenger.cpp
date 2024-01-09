// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/messenger.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"

// Static Members
bool Messenger::quiet_ = false;
bool Messenger::muted_ = false;
bool Messenger::verbose_ = false;
bool Messenger::redirect_ = false;
bool Messenger::masterOnly_ = true;
int Messenger::nErrors_ = 0;
int Messenger::nWarnings_ = 0;
LineParser Messenger::parser_;
OutputHandler *Messenger::outputHandler_ = nullptr;
std::string Messenger::outputPrefix_;

/*
 * General Print Routines (Private)
 */

// Split supplied text into lines (delimited by '\n') and send for output
void Messenger::splitAndPrint(std::string_view s)
{
    // The created text may be over multiple lines (separated by '\n') so split it, prepending the prefix and/or process id to
    // each line as necessary
    size_t pos = s.find('\n');
    do
    {
        // Output the current line (up to the next newline delimiter)
        outputText(s.substr(0, pos));

        // Adjust our view of the working string and find the next newline (if any)
        s = s.substr(pos + 1);
        pos = s.find('\n');
    } while (pos != std::string::npos);
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

// Set status of master-only mode
void Messenger::setMasterOnly(bool b) { masterOnly_ = b; }

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
#ifdef PARALLEL
    // Only print on master thread
    if (masterOnly_ && !ProcessPool::isWorldMaster())
        return;
#endif
    if (outputPrefix_.empty())
    {
        // If we are redirecting to files, use the parser_
        if (redirect_)
            parser_.writeLineF("{}\n", std::string(s));
        else
        {
            // Not redirecting - has an OutputHandler been defined?
            if (outputHandler_)
                outputHandler_->outputText(std::string(s));
            else
                std::cout << s << std::endl;
        }
    }
    else
    {
        // If we are redirecting to files, use the parser_
        if (redirect_)
            parser_.writeLineF("{} {}\n", outputPrefix_, s);
        else
        {
            // Not redirecting - has an OutputHandler been defined?
            if (outputHandler_)
                outputHandler_->outputText(fmt::format("{} {}", outputPrefix_, s));
            else
                std::cout << outputPrefix_ << " " << s << std::endl;
        }
    }
}

// Output blank line (with prefix if set) to relevant handler
void Messenger::outputBlank()
{
#ifdef PARALLEL
    // Only print on master thread
    if (masterOnly_ && !ProcessPool::isWorldMaster())
        return;
#endif
    if (outputPrefix_.empty())
    {
        // If we are redirecting to files, use the parser_
        if (redirect_)
            parser_.writeLine("");
        else
        {
            // Not redirecting - has an OutputHandler been defined?
            if (outputHandler_)
                outputHandler_->outputText("");
            else
                std::cout << std::endl;
        }
    }
    else
    {
        // If we are redirecting to files, use the parser_
        if (redirect_)
            parser_.writeLineF("{}\n", outputPrefix_);
        else
        {
            // Not redirecting - has an OutputHandler been defined?
            if (outputHandler_)
                outputHandler_->outputText(outputPrefix_);
            else
                std::cout << outputPrefix_ << std::endl;
        }
    }
}

// Set output handler
void Messenger::setOutputHandler(OutputHandler *outputHandler) { outputHandler_ = outputHandler; }

/*
 * File Redirection
 */

// Enable redirection of all messaging to specified file
bool Messenger::enableRedirect(std::string_view filename)
{
    parser_.openOutput(filename, true);
    if (!parser_.isFileGoodForWriting())
    {
        Messenger::print("Couldn't open output file '{}' for writing.\n", filename);
        return false;
    }

    redirect_ = true;
    return true;
}

// Cease redirection of messaging to file
void Messenger::ceaseRedirect()
{
    parser_.closeFiles();
    redirect_ = false;
}
