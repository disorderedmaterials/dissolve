/*
    *** Messaging Routines
    *** src/base/messenger.cpp
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
#include "base/lineparser.h"
#include "base/processpool.h"
#include "base/sysfunc.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

// Static Members
bool Messenger::quiet_ = false;
bool Messenger::muted_ = false;
bool Messenger::verbose_ = false;
bool Messenger::redirect_ = false;
bool Messenger::masterOnly_ = false;
LineParser Messenger::parser_; //= new LineParser;
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
        // If we are redirecting to files, use the parser_
        if (redirect_)
            parser_.writeLineF("{}", std::string(s));
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
            parser_.writeLineF("{} {}", outputPrefix_, s);
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
